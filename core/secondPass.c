#include "secondPass.h"
#include "utils.h"
#include "table.h"

/*
 * The second pass of the assembler.
 * We know the symbol table is already full from the first pass.
 * this is work like:
 * -Rewind the file to start reading from the beginning.
 * -Start IC again from 100.
 * -Read line by line. Skip long lines, empty lines, and comments.
 * -If the first word is label, skip it.
 * -If it is .entry directive, find the label in the table and change is_entry to 1.
 * -If the entry label is extern or not in the table, print error.
 * -If it is a regular command, send it to complete the missing operand words.
 */
void second_pass(FILE *fp, char *filename, Symbol *symbol_table, MachineWord *code_img, int *IC, ext_node **ext_list, int *error_found)
{
  char line[MAX_LINE_LEN]; /* current line */
  char current_word[MAX_LINE_LEN]; /* the current word we read */
  char *p_scan; /* pointer to run on the line */
  const opcode *op; /* pointer to the command info */
  char entry_label[MAX_LINE_LEN]; /* save the .entry label name */
  Symbol *s; /* pointer to find label in the table */
  int line_num = 0; /* counter for the lines */
  
  *IC = IC_VALUE;  
  rewind(fp); /* start from the beginning of the file */
  while (fgets(line, MAX_LINE_LEN, fp) != NULL)
  {
    line_num++; 
    if (strchr(line, '\n') == NULL && !feof(fp)) /* check if line is too long, we already reported it in first pass so just skip it here */
    {
      while (fgets(line, MAX_LINE_LEN, fp) != NULL && strchr(line, '\n') == NULL);
    }
    else 
    {
      if (!is_empty(line) && !is_comment(line)) /* skip empty lines and comments */
      {
        p_scan = line;
        p_scan = get_next_word(p_scan, current_word); /* get the first word */
        if (is_label(current_word)) /* check if the first word is label if yes skip it */ 
        {
          p_scan = get_next_word(p_scan, current_word);
        }
        if (is_directive(current_word)) /* check if it is dot directive */
        {
          if (is_entry(current_word)) /* we only care about .entry in the second pass */
          {
            p_scan = get_next_word(p_scan, entry_label); 
            
            while (*p_scan && isspace(*p_scan)) /* check for garbage text after the entry label */
              p_scan++;
            if (*p_scan != EMPTY_CHAR)
            {
              printf("Error in .am file, line %d, extra text after .entry.\n", line_num);
              *error_found = TRUE;
            }
            else
            {
              s = find_symbol(symbol_table, entry_label); /* find the entry label in the symbol table */
              if (s != NULL) 
              {
                if (s->type == EXTERN_SYMBOL) /* label can not be both extern and entry */
                {
                  printf("Error in .am file, line %d, label %s can't be both extern and entry.\n", line_num, entry_label);
                  *error_found = TRUE;
                } 
                else 
                {
                  s->is_entry = TRUE; /* turn on the entry */
                }
              }
              else 
              {
                printf("Error in .am file, line %d, entry label '%s' is missing.\n", line_num, entry_label);
                *error_found = TRUE;
              }
            }
          }
        }
        else
        {
          op = find_opcode(current_word); /* it is regular command so find it in the table */
          if (op != NULL) 
          { /* complete the empty words of the operands */
            complete_instruction_words(op, p_scan, symbol_table, code_img, IC, ext_list, error_found, line_num); 
          }
        }
      }
    }    
  }
}
 
 /*
 * It reads the operands of the command and sends them to get their missing machine words.
 * We know the command is valid.
 * this is work like:
 * -Get the operands string.
 * -Check the addressing modes again.
 * -Add 1 to IC for the opcode word (we already built it in first pass).
 * -Send the operands to the fill function to complete their words.
 */
void complete_instruction_words(const opcode *op, char *line_ptr, Symbol *symbol_table, MachineWord *code_img, int *IC, ext_node **ext_list, int *error_found, int line_num)
{
  char op1[MAX_LINE_LEN] = {0}; /* the first operand */
  char op2[MAX_LINE_LEN] = {0}; /* the second operand */
  int src_mode = NO_MODE; /* save the addressing modes */
  int dest_mode = NO_MODE; /* save the addressing modes */
  
  if (op->operands_num == TWO_OPERANDS) 
  {
    line_ptr = get_next_word(line_ptr, op1); /* get source operand */     
    while (*line_ptr && isspace(*line_ptr)) /* skip the comma */
      line_ptr++;
    if (*line_ptr == ',') 
      line_ptr++; 
        
    line_ptr = get_next_word(line_ptr, op2);  /* get destination operand */        
    src_mode = get_addressing_mode(op1);
    dest_mode = get_addressing_mode(op2);
  }
  else if (op->operands_num == ONE_OPERAND) 
  {
    line_ptr = get_next_word(line_ptr, op1); /* get destination operand */
    dest_mode = get_addressing_mode(op1); 
  }
  (*IC)++; /* skip the main opcode word that we did in the first pass */
  if (op->operands_num == TWO_OPERANDS) /* send operands to get their machine words */
  {
    fill_operand_word(op1, src_mode, symbol_table, code_img, IC, ext_list, error_found, line_num);
    fill_operand_word(op2, dest_mode, symbol_table, code_img, IC, ext_list, error_found, line_num);
  }
  else if (op->operands_num == ONE_OPERAND) 
  {
    fill_operand_word(op1, dest_mode, symbol_table, code_img, IC, ext_list, error_found, line_num);
  }
}

/*
 * It completes the missing 12 bit word for label operands.
 * this is work like:
 * -If mode is 0 or 3 (immediate or register) do nothing and add IC because we did them in the first pass.
 * -If mode is 1 (direct label)- find label in table. If extern, put 01 (ARE_E) and add to ext list. Else put address and 10 (ARE_R).
 * -If mode is 2 (relative jump)- find label in table. Calculate the distance (label address minus current IC). Put distance and 00 (ARE_A).
 */
void fill_operand_word(char *operand, int mode, Symbol *symbol_table, MachineWord *code_img, int *IC, ext_node **ext_list, int *error_found, int line_num)
{
  int distance; /* save the jump distance for relative mode */
  Symbol *s; /* pointer to find the label in the table */
  
  if (mode == MODE_IMMEDIATE || mode == MODE_REGISTER) /* immediate or register mode are already done in the first pass */
  {
    (*IC)++;
  }
  else if (mode == MODE_DIRECT) /* direct label mode */
  {
    s = find_symbol(symbol_table, operand);
    if (s != NULL) {
      if (s->type == EXTERN_SYMBOL) 
      {
        code_img[*IC] = (ARE_E << ARE_START_BIT); /* bits 12-13 are 01 for extern */        
        if (add_ext_record(ext_list, operand, *IC) == ERROR) /* add to the extern list for the .ext output file */
        {
          *error_found = TRUE;
        }
      } 
      else 
      {
        code_img[*IC] = (s->address & TWELVE_BIT_MASK) | (ARE_R << ARE_START_BIT); /* bits 12-13 are 10 for local label */
      }
    } 
    else 
    {
      printf("Error in .am file, line %d, label '%s' is missing.\n", line_num, operand);
      *error_found = TRUE;
    }
    (*IC)++;
  }
  else if (mode == MODE_RELATIVE) /* when the label has % before it */
  {
    s = find_symbol(symbol_table, operand + RELATIVE_PREFIX_LEN); /* skip the '%' sign */
    if (s != NULL) 
    {
      if (s->type == EXTERN_SYMBOL) 
      {
        printf("Error in .am file, line %d, can't jump relative to extern label '%s'.\n", line_num, operand + RELATIVE_PREFIX_LEN);
        *error_found = TRUE;
      } 
      else 
      {
        distance = s->address - *IC; /* calculate the jump distance */     
        code_img[*IC] = (distance & TWELVE_BIT_MASK ) | (ARE_A << ARE_START_BIT); /* bits 12-13 are 00 for absolute */
      }
    }
    else 
    {
      printf("Error in .am file, line %d, jump label '%s' is missing.\n", line_num, operand + RELATIVE_PREFIX_LEN);
      *error_found = TRUE;
    }
    (*IC)++; 
  }
}
