#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h> /* for files and printing (printf, fopen) */
#include <string.h> /* for string functions (strlen, strcpy, strcmp) */
#include <stdlib.h> /* for memory (malloc, free) and atoi */
#include <ctype.h> /* to check chars (isspace, isdigit) */

/* return codes and flags */
#define OK 0 /* success return for main*/
#define OPEN_FAILED 0 /* failure return */
#define ERROR 0 /* error flag */
#define SUCCESS 1 /* success flag */
#define TRUE 1 /* true value */
#define FALSE 0 /* false value */
/* Numbers for registers */
#define MIN_REGISTER 0 /* minimum register number(r0) */
#define MAX_REGISTER 7 /* maximum register number(r7) */
#define REG_PREFIX 'r' /* the character that indicates the beginning of a register name */
/* String and char constants */
#define EMPTY_CHAR '\0' /* null terminator */
#define FIRST_CHAR 0 /* index 0 in string */
#define SECOND_CHAR 1 /* index 1 in string */
#define THIRD_CHAR 2 /* index 2 in string */
/* File extensions and read/write */
#define AS_EXT ".as" /* end of source file */
#define AM_EXT ".am" /* end of macro file */
#define OB_EXT ".ob" /* machine code output */
#define ENT_EXT ".ent" /* entries file */
#define EXT_EXT ".ext" /* externals file */
#define READ_MODE "r" /* open file to read */
#define WRITE_MODE "w" /* open file to write */
/* Max sizes */
#define MAX_FILENAME (FILENAME_MAX + 4) /* max file name size (system max + ".as" + '\0') */
#define MAX_LINE_LEN (80+2) /* max line size (80 + \n + \0) */
#define MACRONAME 32 /* max macro name size (31 chars + 1 for '\0') */
#define MAX_RAM 4096 /* max memory size */
/* A,R,E numbers */
#define ARE_A 0 /* absolute */
#define ARE_E 1 /* external */
#define ARE_R 2 /* can replace */
/* A,R,E characters for output */
#define CHAR_A 'A' /* absolute */
#define CHAR_R 'R' /* can replace*/
#define CHAR_E 'E' /* external */
/* get the ARE bits */
#define ARE_MASK 3 /* binary 11 to mask 2 bits */
#define ARE_START_BIT 12 /* ARE starts at bit 12 */
#define TWELVE_BIT_MASK 0xFFF /* keep only the first 12 bits */
/* 12 bit number limits */
#define MIN_12_BIT -2048 /* minimum for 12 bits */
#define MAX_12_BIT 2047 /* maximum for 12 bits */
/* start values for counters */
#define IC_VALUE 100 /* memory starts at 100 */
#define DC_VALUE 0 /* data starts at 0 */
/* addressing modes */
#define MODE_IMMEDIATE 0 /* for numbers with # */
#define MODE_DIRECT 1 /* for labels */
#define MODE_RELATIVE 2 /* for jumps with % */
#define MODE_REGISTER 3 /* for registers r0-r7 */
#define NO_MODE -1 /* when there is no operand */
/* Number of operands for commands */
#define NO_OPERANDS 0 /* command takes 0 operands */
#define ONE_OPERAND 1 /* command takes 1 operand */
#define TWO_OPERANDS 2 /* command takes 2 operands */
/* Bit shifts and masks for building machine words */
#define DEST_MODE_SHIFT 2 /* destination mode moves 2 bits left */
#define FUNCT_SHIFT 4 /* funct moves 4 bits left */
#define OPCODE_SHIFT 8 /* opcode moves 8 bits left */
#define OP_MASK 15    /* Mask (binary 1111) to isolate the 4 bits of the opcode */
#define FUNCT_MASK 15 /* Mask (binary 1111) to isolate the 4 bits of the funct */


/* String prefix lengths */
#define RELATIVE_PREFIX_LEN 1 /* skip the '%' sign */
#define IMMEDIATE_PREFIX_LEN 1 /* skip the '#' sign */
#define REG_LEN 2 /* length of register string like r3 */
/* Command line arguments limits */
#define MIN_ARGS 2 /* minimum arguments for main program */
/* Command opcodes */
#define OP_MOV 0 /* opcode for mov command */
#define OP_CMP 1 /* opcode for cmp command */
#define OP_ADD 2 /* opcode for add command */
#define OP_SUB 2 /* opcode for sub command */
#define OP_LEA 4 /* opcode for lea command */
#define OP_CLR 5 /* opcode for clr command */
#define OP_JMP 9 /* opcode for jmp command */
#define OP_RED 12 /* opcode for red command */
#define OP_PRN 13 /* opcode for prn command */
#define OP_RTS 14 /* opcode for rts command */
#define OP_STOP 15 /* opcode for stop command */
#define OP_NOT 5 /* opcode for not command */
#define OP_INC 5 /* opcode for inc command */
#define OP_DEC 5 /* opcode for dec command */
#define OP_BNE 9 /* opcode for bne command */
#define OP_JSR 9 /* opcode for jsr command */
/* Command funct values */
#define FUNCT_DEFAULT 0 /* default funct value */
#define FUNCT_ADD 10 /* funct for add command */
#define FUNCT_SUB 11 /* funct for sub command */
#define FUNCT_CLR 10 /* funct for clr command */
#define FUNCT_NOT 11 /* funct for not command */
#define FUNCT_INC 12 /* funct for inc command */
#define FUNCT_DEC 13 /* funct for dec command */
#define FUNCT_JMP 10 /* funct for jmp command */
#define FUNCT_BNE 11 /* funct for bne command */
#define FUNCT_JSR 12 /* funct for jsr command */

/* SymbolType- What type the label is*/
typedef enum 
{
  CODE_SYMBOL, /* Label before command */
  DATA_SYMBOL, /* Label before .data or .string */
  EXTERN_SYMBOL /* Label from .extern */
} SymbolType;

/* MachineWord- One word in the memory (12 bits) */
typedef unsigned short MachineWord;

/*Symbol- Linked list node for the symbol table.*/
typedef struct Symbol 
{
  char name[32]; /* Name of label */
  int address; /* Memory address */
  SymbolType type; /* Code, Data or Extern */
  int is_entry; /* 1 if it is entry, 0 if not */
  struct Symbol *next; /* Pointer to next label */
} Symbol;

/* Opcode- Struct for assembler command */
typedef struct 
{
  char *name; /* The name of the command */
  unsigned int opcode; /* The opcode number */
  int funct; /* The funct number */
  int operands_num; /* How many operands it needs */
} opcode;

/* State- To know if we are inside a macro or not */
typedef enum 
{
  OUT_MACRO, /* Not inside a macro */
  IN_MACRO /* Inside a macro */
} State;

/* ext_node - Linked list to save the externs for the .ext file */
typedef struct ext_node 
{
  char name[32]; /* Name of the extern */
  int address; /* The address we used it */
  struct ext_node *next; /* Pointer to next node */
} ext_node;

/* Macro - Linked list to save macros */
typedef struct Macro 
{
  char name[MACRONAME]; /* Macro name */
  char *content; /* The content that the macro replace */
  struct Macro *next; /* Pointer to next macro */
} Macro;

/* Pointer to a function that handles directives */
typedef void (*DirectiveFunc)(char *line_ptr, MachineWord *data_img, int *DC, Symbol **symbol_table, int line_num, int *error_found);

/* Directive - Connects the directive name to its function */
typedef struct 
{
  char *name; /* Directive name */
  DirectiveFunc func; /* Pointer to the function */
} Directive;

#endif
