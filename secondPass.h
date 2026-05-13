#ifndef SECONDPASS_H
#define SECONDPASS_H

#include "globals.h"

/*
 * Do the second pass. Complete missing label words and handle .entry directives.
 * Input: fp- file pointer. filename- the source file name. symbol_table- symbol table. code_img- code array. IC- instruction counter. ext_list- externals list. error_found- error flag.
 * Output: None (void).
 */
void second_pass(FILE *fp, char *filename, Symbol *symbol_table, MachineWord *code_img, int *IC, ext_node **ext_list, int *error_found);

/*
 * Read the operands of the command and send them to get their missing machine words.
 * Input: op- pointer to the opcode struct. line_ptr- string of the operands. symbol_table- symbol table. code_img- code array. IC- instruction counter. ext_list- externals list. error_found- error flag. line_num- current line number.
 * Output: None (void).
 */
void complete_instruction_words(const opcode *op, char *line_ptr, Symbol *symbol_table, MachineWord *code_img, int *IC, ext_node **ext_list, int *error_found, int line_num);

/*
 * Complete the missing 12 bit word for label operands.
 * Input: operand- the operand string. mode- addressing mode. symbol_table- symbol table. code_img- code array. IC- instruction counter. ext_list- externals list. error_found- error flag. line_num- current line number.
 * Output: None (void).
 */
void fill_operand_word(char *operand, int mode, Symbol *symbol_table, MachineWord *code_img, int *IC, ext_node **ext_list, int *error_found, int line_num);

#endif
