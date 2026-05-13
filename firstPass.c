#include "firstPass.h"
#include "utils.h"
#include "table.h"
#include "preAssembler.h"

/*
 * The first pass of the assembler.
 * We know the .am file is open and ready.
 * this is work like:
 * -Start IC from 100 and DC from 0.
 * -Read the file line by line.
 * -Check for long lines or memory overflow (more than 4096 words).
 * -If the line has label, check if the name is good and add to the table.
 * -If it is directive (.data, .string), send it to the handler and add DC.
 * -If it is command, check the opcodes and operands, build the machine words and add IC.
 * -At the end it is update the addresses of all data labels by adding the final IC.
 */
void first_pass(FILE *fp, MachineWord *code_img, MachineWord *data_img, int *IC, int *DC, Symbol **symbol_table, int *error_found, Macro *macro_list)
{
  char line[MAX_LINE_LEN]; /* current line */
  char current_word[MAX_LINE_LEN]; /* current word we read */
  char label_name[MAX_LINE_LEN]; /* save the label name */
  char *p_scan; /* pointer to run on the line */
  int has_label; /* to know if we found label */
  const opcode *op; /* pointer to save the command info */
  int line_num = 0; /* counter for the lines */
  int data_comma_error; /* comma errors in .data */
  int memory_overflow = FALSE; /* stop if memory is full */
    
  *IC = IC_VALUE; /* memory starts at 100 */
  *DC = DC_VALUE;
  /* loop on all lines until end of file or memory is full */
  while (memory_overflow == FALSE && fgets(line, MAX_LINE_LEN, fp) != NULL) 
  {
    line_num++;  
    if ((*IC) + (*DC) >= MAX_RAM) /* check if we have enough memory */
    {
      printf("Error in .am file, line %d, program is too big for memory.\n", line_num);
      *error_found = TRUE;
      memory_overflow = TRUE; /* stop the loop */ 
    }
    else 
    {
      if (strchr(line, '\n') == NULL && !feof(fp)) /* check if line is too long */
      {
        printf("Error in .am file, line %d, line is too long.\n", line_num);
        *error_found = TRUE;
        while (fgets(line, MAX_LINE_LEN, fp) != NULL && strchr(line, '\n') == NULL); 
      }
      else if (!is_empty(line) && !is_comment(line))
      {
        p_scan = line;
        has_label = FALSE;
        label_name[FIRST_CHAR] = EMPTY_CHAR;
        p_scan = get_next_word(p_scan, current_word); /* get the first word in the line */
      if (is_label(current_word)) /* check if the first word is label */
      {
        has_label = TRUE;
        strcpy(label_name, current_word);
        remove_colon(label_name); /* remove the ':' to check the name */
        if (is_good_label(label_name, macro_list, line_num) == ERROR)
        {
          *error_found = TRUE;
          has_label = FALSE; /* ignore bad label */
        }
        else if (find_symbol(*symbol_table, label_name) != NULL)
        {
          printf("Error in .am file, line %d, label '%s' already exists.\n", line_num, label_name);
          *error_found = TRUE;
          has_label = FALSE;
        }
        p_scan = get_next_word(p_scan, current_word); /* get the next word after the label */
      }
        if (has_label && strlen(current_word) == 0) /* check for empty label */
        {
          printf("Error in .am file, line %d, empty label.\n", line_num);
          *error_found = TRUE;
        }
        else if (is_directive(current_word)) /* check if it is a dot directive */ 
        {
          data_comma_error = FALSE;
          if (has_label && (is_data(current_word) || is_string(current_word)))
          {
            add_symbol(symbol_table, label_name, *DC, DATA_SYMBOL);
          }
          if (is_data(current_word)) /* check commas syntax in .data */
          {
            if (check_data_commas(p_scan, line_num) == ERROR)
            {
              *error_found = TRUE;
              data_comma_error = TRUE; 
            }
          }
          if (data_comma_error == FALSE) /* if commas are ok so handle the directive */
          {
            handle_directive(current_word, p_scan, data_img, DC, symbol_table, line_num, error_found);
          }
        }
        else
        {
          op = find_opcode(current_word); /* it is regular command so find it in the table */
          if (op != NULL)
          {
            if (has_label)
            {
              add_symbol(symbol_table, label_name, *IC, CODE_SYMBOL); /* code labels start from IC */
            }
            handle_instruction(op, p_scan, IC, code_img, error_found, line_num);
          }
          else 
          {
            printf("Error in .am file, line %d, unknown command or directive '%s'\n", line_num, current_word);
            *error_found = TRUE;
          }
        }
      }
    }
  }
  if ((*IC) + (*DC) > MAX_RAM) /* final check for memory overflow after the file ends */
  {
    printf("Error, program is too big.\n");
    *error_found = TRUE;
  }
  update_symbol_table_addresses(*symbol_table, *IC); /* fix the addresses of the data labels by adding the final IC */
}

/*
 * It process a machine command line.
 * We know the opcode is valid.
 * this is work like:
 * -Check how many operands the command needs (0 or 1 or 2).
 * -Extract the operands and check for missing commas.
 * -Find their addressing modes.
 * -Check if the modes are legal for this command.
 * -Build the first word (opcode word) and put in code array.
 * -If the operand is immediate or register, build its word too.
 * -If it's label, leave it 0 (we will fix it in second pass).
 */
void handle_instruction(const opcode *op, char *line_ptr, int *IC, MachineWord *code_img, int *error_found, int line_num)
{
  char op1[MAX_LINE_LEN] = {0}; /* the first operand */
  char op2[MAX_LINE_LEN] = {0}; /* the second operand */
  int src_mode = NO_MODE; /* save the source addressing mode */
  int dest_mode = NO_MODE; /* save the destination addressing mode */
  char *target_op_str; /* pointer to the destination operand string */
  /* extract operands based on how many this specific command requires */
  if (op->operands_num == 2) /* if the command needs 2 operands */
  {
    line_ptr = get_next_word(line_ptr, op1); 
    if (strlen(op1) == 0) 
    {
      printf("Error in .am file, line %d, missing operands.\n", line_num);
      *error_found = TRUE;
      return;
    }
    while (*line_ptr && isspace(*line_ptr)) /* check for comma between operands */
      line_ptr++;
    if (*line_ptr != ',') 
    {
      printf("Error in .am file, line %d, missing comma.\n", line_num);
      *error_found = TRUE;
      return;
    }
    line_ptr++; 
    line_ptr = get_next_word(line_ptr, op2); 
    if (strlen(op2) == 0) 
    {
      printf("Error in .am file, line %d, missing second operand.\n", line_num);
      *error_found = TRUE;
      return;
    }       
    src_mode = get_addressing_mode(op1);
    dest_mode = get_addressing_mode(op2);
  }
  else if (op->operands_num == 1) /* if the command needs 1 operand */
  {
    line_ptr = get_next_word(line_ptr, op1);
    if (strlen(op1) == 0) 
    {
      printf("Error in .am file, line %d, missing operand.\n", line_num);
      *error_found = TRUE;
      return;
    }
    dest_mode = get_addressing_mode(op1);
  }
  while (*line_ptr && isspace(*line_ptr)) /* check for garbage at the end of the line */
    line_ptr++;
  if (*line_ptr != EMPTY_CHAR) 
  {
    printf("Error in .am file, line %d, extra text after command.\n", line_num);
    *error_found = TRUE;
    return;
  }
  if (is_valid_addressing(op->opcode, src_mode, dest_mode) == ERROR) /* check if the modes are legal for this specific command */
  {
    printf("Error in .am file, line %d, bad addressing mode for '%s'.\n", line_num, op->name);
    *error_found = TRUE;
    return; 
  }
  code_img[*IC] = build_opcode_word(op->opcode, op->funct, src_mode, dest_mode); /* build the first word of the command and add to memory */
  (*IC)++;    
  if (op->operands_num == 2) /* handle the first operand word if we have 2 */
  {
    if (src_mode == MODE_IMMEDIATE) /* immediate mode */ 
    {
      if (is_valid_integer(op1 + IMMEDIATE_PREFIX_LEN) == ERROR) 
      {
        printf("Error in .am file, line %d, bad number format '%s'.\n", line_num, op1);
        *error_found = TRUE;
      } 
      else 
      {
        code_img[*IC] = build_immediate_word(atoi(op1 + IMMEDIATE_PREFIX_LEN)); 
      }
    }
    else if (src_mode == MODE_REGISTER) /* register mode */
    {
      code_img[*IC] = build_register_word(op1[SECOND_CHAR] - '0');
    } 
    else /* label or relative mode so leave 0 for second pass */
    {
      code_img[*IC] = 0;
    }
    (*IC)++;
  }  
  if (op->operands_num > 0) /* handle the destination operand word */
  {
    target_op_str = (op->operands_num == 2) ? op2 : op1;
    if (dest_mode == MODE_IMMEDIATE) 
    {
      if (is_valid_integer(target_op_str + IMMEDIATE_PREFIX_LEN) == ERROR) 
      {
        printf("Error in .am file, line %d, bad number format '%s'.\n", line_num, target_op_str);
        *error_found = TRUE;
      } 
      else 
      {
        code_img[*IC] = build_immediate_word(atoi(target_op_str + IMMEDIATE_PREFIX_LEN));
      }
    } 
    else if (dest_mode == MODE_REGISTER) /* register mode */
    {
      code_img[*IC] = build_register_word(target_op_str[SECOND_CHAR] - '0');
    } 
    else /* label or relative mode so leave 0 for second pass */
    {
      code_img[*IC] = FALSE;             
    }
    (*IC)++;
  }
}

/*
 * It process .data lines.
 * We know the commas are already checked and good.
 * this is work like:
 * -Loop on the numbers in the string.
 * -Change them to integers.
 * -If they are good, make them 12-bit words and put in data array.
 */
void handle_data_directive(char *line_ptr, MachineWord *data_img, int *DC, Symbol **symbol_table, int line_num, int *error_found) 
{
  char temp_num[MAX_LINE_LEN]; /* save the number before changing to int */
  while (*line_ptr && (isspace(*line_ptr) || *line_ptr == ',')) /* skip spaces and commas between the numbers */
    line_ptr++;
  while (*line_ptr) 
  {
    line_ptr = get_next_word(line_ptr, temp_num);
    if (strlen(temp_num) > 0) 
    {
      if (is_valid_integer(temp_num) == ERROR) /* check if the number is legal */ 
      {
        printf("Error in .am file, line %d, bad number format '%s'.\n", line_num, temp_num);
        *error_found = TRUE;
      } 
      else 
      {
        data_img[*DC] = (MachineWord)((atoi(temp_num) & TWELVE_BIT_MASK) | (ARE_A << ARE_START_BIT)); /* encode the number and add 'A'in bits 12-13 */
        (*DC)++;
      }
    }
    while (*line_ptr && (isspace(*line_ptr) || *line_ptr == ',')) 
      line_ptr++;
  }
}

/*
 * It process .string lines.
 * We know the string have quotes.
 * this is work like:
 * -Find the first and last quote marks.
 * -Check for garbage text after the string.
 * -Loop on the characters and add their ASCII value to the data array.
 * -Add 0 ('\0') at the end of the string.
 */
void handle_string_directive(char *line_ptr, MachineWord *data_img, int *DC, Symbol **symbol_table, int line_num, int *error_found)
{
  char *start = strchr(line_ptr, '"'); /* pointer to the first quote */
  char *end = strrchr(line_ptr, '"'); /* pointer to the last quote */
  char *check_extra; /* pointer to check garbage text after the string */

  if (start == NULL || start == end) 
  {
    printf("Error in .am file, line %d, missing \" in string.\n", line_num);
    *error_found = TRUE;
    return;
  }
  check_extra = end + 1; /* check for garbage text after the closing quote */
  while (*check_extra && isspace(*check_extra)) 
    check_extra++;
  if (*check_extra != EMPTY_CHAR) 
  {
    printf("Error in .am file, line %d, extra text after string.\n", line_num);
    *error_found = TRUE;
    return;
  }
  start++; /* skip the first quote */
  while (start < end) /* copy each character from the string to the data image */
  {
    data_img[*DC] = (MachineWord)(*start); 
    (*DC)++;
    start++;
  }
  data_img[*DC] = 0; /* add null at the end */
  (*DC)++;
}

/*
 * It process .extern lines.
 * We know the label name is legal.
 * this is work like:
 * -Get the label name.
 * -Check if it is already in the table.
 * -If it is local label, print error.
 * -Else add it to the table as EXTERN_SYMBOL.
 */
void handle_extern_directive(char *line_ptr, MachineWord *data_img, int *DC, Symbol **symbol_table, int line_num, int *error_found)
{
  char label_name[MACRONAME]; /* save the extern label */
  Symbol *existing; /* pointer to check if label is already in table */
  
  line_ptr = get_next_word(line_ptr, label_name); 
  while (*line_ptr && isspace(*line_ptr)) line_ptr++;
  if (*line_ptr != EMPTY_CHAR) 
  {
    printf("Error in .am file, line %d, extra text after .extern.\n", line_num);
    *error_found = TRUE;
    return;
  }
  
  existing = find_symbol(*symbol_table, label_name);
  if (existing != NULL)
  {
    if (existing->type != EXTERN_SYMBOL) /* if it is already in the table but not extern, it is error */ 
    {
      printf("Error in .am file, line %d, label '%s' is local so it can't be extern.\n", line_num, label_name);
      *error_found = TRUE;
    }
  }
  else
  {
    add_symbol(symbol_table, label_name, 0, EXTERN_SYMBOL); /* add to symbol table with address 0 */
  }
}

/*
 * It process .entry lines.in first pass we do nothing just skip.
 * this is work like:
 * -Return empty. We handle entries in the second pass.
 */
void handle_entry_directive(char *line_ptr, MachineWord *data_img, int *DC, Symbol **symbol_table, int line_num, int *error_found)
{
  return;
}

/*
 * It sends the directive to the correct function.
 * We know the directive string is valid.
 * this is work like:
 * -Run on an array of structs to find the function pointer.
 * -Call the function that matches the string.
 */
void handle_directive(char *directive, char *line_ptr, MachineWord *data_img, int *DC, Symbol **symbol_table, int line_num, int *error_found) 
{
  int i = 0;
  const Directive directive_table[] = /* array of structs to link the string to the function */
  {
    {".data",   handle_data_directive},
    {".string", handle_string_directive},
    {".extern", handle_extern_directive},
    {".entry",  handle_entry_directive}, 
    {NULL, NULL}
  };
  while (directive_table[i].name != NULL)
  {
    if (strcmp(directive, directive_table[i].name) == 0)     
    {
      directive_table[i].func(line_ptr, data_img, DC, symbol_table, line_num, error_found); /* call the function */
      return;
    }
    i++;
  }
  printf("Error in .am file, line %d, unknown directive %s\n", line_num, directive);
  *error_found = TRUE;
}

/*
 * It checks if the label name is legal.
 * this is work like:
 * -Check length.
 * -Check if it starts with a letter.
 * -Check if it has only letters and numbers.
 * -Check if it is saved word (opcode, directive, register, macro).
 */
int is_good_label(char *word, Macro *macro_list, int line_num)
{
  int i;  
  if(strlen(word) > MACRONAME-1)
  {
    printf("Error in .am file, line %d, label name is too long.\n", line_num);
    return ERROR;
  }  
  if (!((word[FIRST_CHAR] >= 'a' && word[FIRST_CHAR] <= 'z') || (word[FIRST_CHAR] >= 'A' && word[FIRST_CHAR] <= 'Z')))
  {
    printf("Error in .am file, line %d, label must start with a letter.\n", line_num);
    return ERROR;
  }
  for (i = 1; word[i] != EMPTY_CHAR; i++)
  {
    if (!((word[i] >= 'a' && word[i] <= 'z') || (word[i] >= 'A' && word[i] <= 'Z') ||(word[i] >= '0' && word[i] <= '9')))
    {
      printf("Error in .am file, line %d, label can only contain letters and numbers.\n", line_num);
      return ERROR;
    }
  }
  if (find_opcode(word) != NULL)
  {
    printf("Error in .am file, line %d, label can't be an opcode.\n", line_num);
    return ERROR;
  }  
  if (strcmp(word, "data") == 0 || strcmp(word, "string") == 0 || strcmp(word, "entry") == 0 || strcmp(word, "extern") == 0)
  {
    printf("Error in .am file, line %d, label can't be a directive.\n", line_num);
    return ERROR;
  }  
  if (word[FIRST_CHAR] == REG_PREFIX && isdigit(word[SECOND_CHAR]) && (word[SECOND_CHAR] - '0') >= MIN_REGISTER && (word[SECOND_CHAR] - '0') <= MAX_REGISTER && word[THIRD_CHAR] == EMPTY_CHAR)
  {
    printf("Error in .am file, line %d, label can't be a register.\n", line_num);
    return ERROR;
  }
  if (find_macro(macro_list, word) != NULL)
  {
    printf("Error in .am file, line %d, label can't be a macro name.\n", line_num);
    return ERROR;
  }
  return SUCCESS;
}

/*
 * It builds the first 12 bit word of a command.
 * this is work like:
 * -Use bitwise operators (| and <<) to put the numbers in the right bits.
 * -Bits 0-1 for dest mode.
 * -Bits 2-3 for src mode.
 * -Bits 4-7 for funct.
 * -Bits 8-11 for opcode.
 * -Bits 12-13 are ARE_A.
 */
MachineWord build_opcode_word(unsigned int opcode, int funct, int src_mode, int dest_mode)
{
  MachineWord word = 0; /* the 12 bit word we build to return */
  if (src_mode == NO_MODE) 
    src_mode = MODE_IMMEDIATE;
  if (dest_mode == NO_MODE) 
    dest_mode = MODE_IMMEDIATE;
  /* use bitwise OR and shifts to put the values in the right bits */
  word |= (dest_mode & ARE_MASK); 
  word |= ((src_mode & ARE_MASK) << DEST_MODE_SHIFT); 
  word |= ((funct & FUNCT_MASK) << FUNCT_SHIFT); 
  word |= ((opcode & OP_MASK) << OPCODE_SHIFT);
  word |= (ARE_A << ARE_START_BIT); /* opcode is always Absolute */
  return word; 
}

/*
 * It builds the 12 bit word for a register operand.
 * this is work like:
 * -Put bit 1 in the place of the register number.
 * -Add ARE_A to bits 12-13.
 */
MachineWord build_register_word(int reg_num) 
{
  MachineWord word = 0; /* the 12 bit word we build to return */
  word |= (1 << reg_num);
  word |= (ARE_A << ARE_START_BIT); 
  return word;
}

/*
 * It builds the 12 bit word for an immediate number operand.
 * this is work like:
 * -Mask the value to 12 bits and add ARE_A.
 */
MachineWord build_immediate_word(int value) 
{
  MachineWord word = 0; /* the 12 bit word we build to return */
  word |= (value & TWELVE_BIT_MASK);
  word |= (ARE_A << ARE_START_BIT);
  return word;
}

/*
 * It checks if the addressing modes are legal for this specific opcode.
 * this is work like:
 * -Use simple if conditions based on the project tables.
 * -Return ERROR if it is bad or SUCCESS if it is ok.
 */
int is_valid_addressing(int opcode, int src_mode, int dest_mode) 
{
  if (opcode == OP_MOV || opcode == OP_CMP || opcode == OP_ADD) /* check source modes */
  {
    if (src_mode == MODE_RELATIVE) 
      return ERROR; 
  } 
  else if (opcode == OP_LEA) 
  {
    if (src_mode != MODE_DIRECT) 
      return ERROR; 
  } 
  else 
  {
    if (src_mode != NO_MODE) 
      return ERROR; 
  }
  if (opcode == OP_MOV || opcode == OP_ADD || opcode == OP_LEA || opcode == OP_CLR || opcode == OP_RED) /* check destination modes */
  {
    if (dest_mode == MODE_IMMEDIATE || dest_mode == MODE_RELATIVE) 
      return ERROR; 
  } 
  else if (opcode == OP_CMP || opcode == OP_PRN) 
  {
    if (dest_mode == MODE_RELATIVE) 
      return ERROR; 
  } 
  else if (opcode == OP_JMP) 
  {
    if (dest_mode == MODE_IMMEDIATE || dest_mode == MODE_REGISTER) 
      return ERROR; 
  } 
  else if (opcode == OP_RTS || opcode == OP_STOP) 
  {
    if (dest_mode != NO_MODE) 
      return ERROR; 
  }
  return SUCCESS; 
}

/*
 * It checks if the commas in a .data line are legal.
 * this is work like:
 * -Run with pointer on the string.
 * -Check for comma at the start, double commas, missing commas, or comma at the end.
 * -Use a flag (expect_comma) to know what we need to see next.
 */
int check_data_commas(char *line, int line_num) 
{
  int expect_comma = FALSE; /* to know if we need a comma right now */
  char *ptr = line; /* pointer to run on the line */
  
  while (*ptr && isspace(*ptr)) 
    ptr++;  
  if (*ptr == ',') 
  {
    printf("Error in .am file, line %d, comma before the first number.\n", line_num);
    return ERROR;
  }   
  while (*ptr) /* Use the expect_comma flag to check if we need a comma or a number next */
  {
    if (*ptr == ',') 
    {
      if (expect_comma == FALSE) 
      {
        printf("Error in .am file, line %d, too many commas.\n", line_num);
        return ERROR;
      }
      expect_comma = FALSE; /* we saw a comma, now expect a number */
      ptr++;
    } 
    else if (!isspace(*ptr)) 
    {
      if (expect_comma == TRUE) 
      {
        printf("Error in .am file, line %d, missing comma.\n", line_num);
        return ERROR;
      }
      expect_comma = TRUE; /* we saw a number so now expect a comma */
      while (*ptr && !isspace(*ptr) && *ptr != ',') /* skip the number digits */
        ptr++;
    }
    else
    {
      ptr++; 
    }
  } 
  ptr = line + strlen(line) - 1; /* check for extra comma at the end */
  while (ptr > line && isspace(*ptr)) 
    ptr--;
  if (*ptr == ',') 
  {
    printf("Error in .am file, line %d, extra comma at the end.\n", line_num);
    return ERROR;
  }   
  return SUCCESS;
}
