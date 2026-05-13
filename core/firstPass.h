#ifndef FIRSTPASS_H
#define FIRSTPASS_H

#include "globals.h"

/*
 * Do the first pass. Build symbol table, count memory words, and encode first words.
 * Input: fp- file pointer. code_img- code array. data_img- data array. IC- instruction counter. DC- data counter. symbol_table- symbol table pointer. error_found- error flag. macro_list- macros list to check names.
 * Output: None (void).
 */
void first_pass(FILE *fp, MachineWord *code_img, MachineWord *data_img, int *IC, int *DC, Symbol **symbol_table, int *error_found, Macro *macro_list);

/*
 * Encode the machine instruction words into the code array.
 * Input: op- pointer to the opcode struct. line_ptr- string of the operands. IC- instruction counter. code_img- code array. error_found- error flag. line_num- current line number.
 * Output: None (void).
 */
void handle_instruction(const opcode *op, char *line_ptr, int *IC, MachineWord *code_img, int *error_found, int line_num);

/*
 * Find which directive it is and send it to the correct function.
 * Input: directive- the directive name. line_ptr- rest of the line. data_img- data array. DC- data counter. symbol_table- symbol table pointer. line_num- current line. error_found- error flag.
 * Output: None (void).
 */
void handle_directive(char *directive, char *line_ptr, MachineWord *data_img, int *DC, Symbol **symbol_table, int line_num, int *error_found);

/*
 * Process the data directive and save the numbers to the data array.
 * Input: line_ptr- the numbers string. data_img- data array. DC- data counter. symbol_table- symbol table. line_num- current line. error_found- error flag.
 * Output: None (void).
 */
void handle_data_directive(char *line_ptr, MachineWord *data_img, int *DC, Symbol **symbol_table, int line_num, int *error_found);

/*
 * Process the string directive and save characters to the data array.
 * Input: line_ptr- the string to save. data_img- data array. DC- data counter. symbol_table- symbol table. line_num- current line. error_found- error flag.
 * Output: None (void).
 */
void handle_string_directive(char *line_ptr, MachineWord *data_img, int *DC, Symbol **symbol_table, int line_num, int *error_found);

/*
 * Process the extern directive and add the label to the symbol table.
 * Input: line_ptr- the label name. data_img- data array. DC- data counter. symbol_table- symbol table. line_num- current line. error_found- error flag.
 * Output: None (void).
 */
void handle_extern_directive(char *line_ptr, MachineWord *data_img, int *DC, Symbol **symbol_table, int line_num, int *error_found);

/*
 * Process the entry directive. (Mainly checks syntax in the first pass).
 * Input: line_ptr- the label name. data_img- data array. DC- data counter. symbol_table- symbol table. line_num- current line. error_found- error flag.
 * Output: None (void).
 */
void handle_entry_directive(char *line_ptr, MachineWord *data_img, int *DC, Symbol **symbol_table, int line_num, int *error_found);

/*
 * Check if the label name is legal (not a saved word and/or not too long).
 * Input: word- the label name. macro_list- list of macros to check if name is used. line_num- current line number.
 * Output: Returns 1 if it's good and 0 if it's not good.
 */
int is_good_label(char *word, Macro *macro_list, int line_num);

/*
 * Create machine word for register operand.
 * Input: reg_num- the number of the register (we start from 0 to 7).
 * Output: Returns the 12 bit machine word.
 */
MachineWord build_register_word(int reg_num);

/*
 * Create machine word for immediate number operand.
 * Input: value- the integer number.
 * Output: Returns the 12 bit machine word.
 */
MachineWord build_immediate_word(int value);

/*
 * Check if the addressing modes they are legal for the specific command.
 * Input: opcode- command number. src_mode- source addressing mode. dest_mode- destination addressing mode.
 * Output: Returns 1 if valid and 0 if not valid.
 */
int is_valid_addressing(int opcode, int src_mode, int dest_mode);

/*
 * Check if the commas in data line are written correctly.
 * Input: line- the string of numbers. line_num- current line number.
 * Output: Returns 1 if good syntax and 0 if bad syntax.
 */
int check_data_commas(char *line, int line_num);

/*
 * Create the first machine word of command with opcode and modes.
 * Input: opcode- the command number. funct- the funct number. src_mode- source mode. dest_mode- destination mode.
 * Output: Returns the 12 bit machine word.
 */
MachineWord build_opcode_word(unsigned int opcode, int funct, int src_mode, int dest_mode);

#endif
