#ifndef OUTPUTFILES_H
#define OUTPUTFILES_H

#include "globals.h"

/*
 * Create and write the final output files (.ob, .ent, .ext). Writes the files to the disk.
 * Input: filename- base name. code_img- finished code array. ICF- final IC. data_img- finished data array. DCF- final DC. symbol_table- table for entries. ext_list- list for externals.
 * Output: None (void).
 */
void write_output_files(char *filename, MachineWord *code_img, int ICF, MachineWord *data_img, int DCF, Symbol *symbol_table, ext_node *ext_list);

/*
 * Change the ARE number to char letter so we can print it to the object file.
 * Input: are_val- the number of ARE (0 or 1 or 2).
 * Output: Returns the char letter ('A', 'R' or 'E').
 */
char get_are_char(int are_val);

#endif
