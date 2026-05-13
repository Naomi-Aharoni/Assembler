#include "utils.h"

/*
 * It checks if the line has only spaces or is completely empty.
 * The input line is a valid string.
 * this is work like:
 * -Run on the string with a pointer.
 * -If we see space characters, skip them.
 * -If we reach the end of the string ('\0'), return 1 (true). Else return 0 (false).
 */
int is_empty(char *line) 
{
  char *pointer = line; /* pointer to run on the characters of the line */ 
  while (*pointer && isspace(*pointer))
    pointer++;
  return *pointer == EMPTY_CHAR;
}

/*
 * It checks if the word is a label definition.
 * A label definition ends with a colon ':'.
 * this is work like:
 * -Get the length of the word.
 * -Check if the last character is ':'. return 1 if true, 0 if false.
 */
int is_label(char *word)
{
  int len = strlen(word); /* save the length of the word */
  if (len > 0 && word[len - 1] == ':') 
    return TRUE; 
  return FALSE; 
}

/*
 * It checks if the line is a comment line.
 * A comment line starts with a semicolon ';' (can have spaces before).
 * this is work like:
 * -Skip spaces at the beginning of the line.
 * -Check if the first character after spaces is ';'. Return 1 if true.
 */
int is_comment(char *line)
{
  char *pointer = line;
  while (*pointer && isspace(*pointer)) /* skip spaces before the ';' sign */
    pointer++; 
  return (*pointer == ';') ? TRUE : FALSE;
}

/*
 * It checks if the word is a directive (like .data, .string).
 * Directives always start with a dot '.'.
 * this is work like:
 * Check if the first character of the word is '.'.
 */
int is_directive(char *word)
{
  return (word[FIRST_CHAR] == '.') ? TRUE : FALSE; /* check if the word starts with a dot */
}

/*
 * It checks if the word is exactly the ".string" directive.
 * The input string is valid.
 * this is work like:
 * Use strcmp to compare the string to ".string".
 */
int is_string(char *string)
{
return (strcmp(string, ".string") == 0) ? TRUE : FALSE; /* check if the string matches exactly */
}

/*
 * It checks if the word is exactly the ".data" directive.
 * The input string is valid.
 * this is work like:
 * Use strcmp to compare the string to ".data".
 */
int is_data(char *string)
{
  return (strcmp(string, ".data") == 0) ? TRUE : FALSE; /* check if the string matches exactly */
}

/*
 * It checks if the word is exactly the ".entry" directive.
 * The input string is valid.
 * this is work like:
 * Use strcmp to compare the string to ".entry".
 */
int is_entry(char *string)
{
return (strcmp(string, ".entry") == 0) ? TRUE : FALSE; /* check if the string matches exactly */
}

/*
 * It checks if the word is exactly the ".extern" directive.
 * The input string is valid.
 * this is work like:
 * Use strcmp to compare the string to ".extern".
 */
int is_extern(char *string) 
{
  return (strcmp(string, ".extern") == 0) ? TRUE : FALSE; /* check if the string matches exactly */
}

/*
 * It searches for a command name in the opcodes table.
 * The table has all the valid commands and ends with עעa NULL name.
 * this is work like:
 * -Create a static array of structs with all commands details.
 * -Run with a while loop on the table.
 * -Use strcmp to find the command. if this is found, return pointer to it.
 * -If we reach NULL, return NULL (not found).
 */
const opcode* find_opcode(char *cmd_name)
{ /* array to hold the machine commands info */
  static const opcode opcodes_table[] = 
  { 
    {"mov", OP_MOV, FUNCT_DEFAULT, TWO_OPERANDS},
    {"cmp", OP_CMP, FUNCT_DEFAULT, TWO_OPERANDS},
    {"add", OP_ADD, FUNCT_ADD, TWO_OPERANDS},
    {"sub", OP_SUB, FUNCT_SUB, TWO_OPERANDS},
    {"lea", OP_LEA, FUNCT_DEFAULT, TWO_OPERANDS},
    {"clr", OP_CLR, FUNCT_CLR, ONE_OPERAND},
    {"not", OP_NOT, FUNCT_NOT, ONE_OPERAND},
    {"inc", OP_INC, FUNCT_INC, ONE_OPERAND},
    {"dec", OP_DEC, FUNCT_DEC, ONE_OPERAND},
    {"jmp", OP_JMP, FUNCT_JMP, ONE_OPERAND},
    {"bne", OP_BNE, FUNCT_BNE, ONE_OPERAND},
    {"red", OP_RED, FUNCT_DEFAULT, ONE_OPERAND},
    {"prn", OP_PRN, FUNCT_DEFAULT, ONE_OPERAND},
    {"jsr", OP_JSR, FUNCT_JSR, ONE_OPERAND},
    {"rts", OP_RTS, FUNCT_DEFAULT, NO_OPERANDS},
    {"stop", OP_STOP, FUNCT_DEFAULT, NO_OPERANDS},
    {NULL, NO_MODE, NO_MODE, NO_MODE} /* end of table marker */
  };
  int i = 0;
  while (opcodes_table[i].name != NULL) 
  {
    if (strcmp(cmd_name, opcodes_table[i].name) == 0) 
      return &opcodes_table[i]; 
    i++;
  }
  return NULL; 
}

/*
 * It finds the addressing mode of the operand.
 * The operand string is not empty.
 * this is work like:
 * -If it starts with '#', it is immediate mode (0).
 * -If it starts with '%', it is relative mode (2).
 * -If it starts with 'r' and has number 0 to 7, it is register mode (3).
 * -If nothing of this, it is direct mode (1).
 */
int get_addressing_mode(char *operand)
{
  if (operand == NULL || strlen(operand) == 0)
    return NO_MODE;
  if (operand[FIRST_CHAR] == '#') /* check the first char to know the mode */
    return MODE_IMMEDIATE;
  if (operand[FIRST_CHAR] == '%') 
    return MODE_RELATIVE;
if (operand[FIRST_CHAR] == REG_PREFIX && strlen(operand) == REG_LEN && (operand[SECOND_CHAR] - '0') >= MIN_REGISTER && (operand[SECOND_CHAR] - '0') <= MAX_REGISTER) 
{
  return MODE_REGISTER;
}
  return MODE_DIRECT; 
}

/*
 * It removes the colon ':' at the end of the label.
 * The string has a colon at the end.
 * this is work like:
 * -Get the length of the string.
 * -If the last char is ':', change it to '\0' to close the string.
 */
void remove_colon(char *word)
{
  int len = strlen(word); /* save the length of the string */
  if (len > 0 && word[len - 1] == ':') 
  {
    word[len - 1] = EMPTY_CHAR;
  }
}

/*
 * It gets the next word from the string and skip spaces.
 * The words have spaces or commas between them.
 * this is work like:
 * -Run with pointer and skip spaces at the start.
 * -Copy the characters to the word array until we see space, comma or end.
 * -Put '\0' at the end of the word.
 * -Return the pointer to know where we stopped.
 */
char *get_next_word(char *str, char *word) 
{
  char *pointer = str; /* pointer to run on the string */
  int i = 0;  
  while (*pointer && isspace(*pointer)) 
    pointer++;
  while (*pointer && !isspace(*pointer) && *pointer != ',') /* copy characters until we hit space comma or the end of the string */ 
  {
    word[i] = *pointer;
    i++;
    pointer++;
  }
  word[i] = EMPTY_CHAR; 
  return pointer;
}

/*
 * It makes a full file name with the extension.
 * The dest array is big enough for the name and extension.
 * this is work like:
 * -Copy the source name to dest array.
 * -Add the extension string to the end.
 */
void create_filename(char *dest, char *source, char *ext)
{ /* put the source name first, then add the extension */
  strcpy(dest, source);
  strcat(dest, ext);
}

/*
 * It checks if the string is a good integer number and fits in 12 bits.
 * The number can have '+' or '-' at the start.
 * this is work like:
 * -Skip the sign '+' or '-' if we have it.
 * -If the string is empty now, it is error.
 * -Run on the string. If we see char that is not a digit, return error.
 * -Change the string to int. If it is smaller than -2048 or bigger than 2047, return error.
 */
int is_valid_integer(char *str)
{
  int num; /* save the converted integer number */
  char *temp = str; /* pointer to run on the string characters */   
  if (*temp == '+' || *temp == '-') 
    temp++;       
  if (*temp == EMPTY_CHAR) 
    return ERROR;
        
  while (*temp)
  {
    if (!isdigit(*temp)) 
      return ERROR;
    temp++;
  }    
  num = atoi(str); /* string is valid number format, now convert and check 12 bit */
  if (num < MIN_12_BIT || num > MAX_12_BIT)
  {
    return ERROR; 
  }    
  return SUCCESS;
}
