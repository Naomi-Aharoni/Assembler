#ifndef PREASSEMBLER_H
#define PREASSEMBLER_H

#include "globals.h"

/*
 * Do the pre assembler pass, find and expand macros. Creates the .am file.
 * Input: filename- the source file name. out_macro_list- pointer to save the macros.
 * Output: Returns OK or ERROR.
 */
int process_macros(char *filename, Macro **out_macro_list);

/*
 * Create new macro and add it to the head of the macro list.
 * Input: head- pointer to the head of the macro list. name- the name of the new macro.
 * Output: Returns pointer to the new Macro or NULL if memory error.
 */
Macro *handle_new_macro(Macro **head, char *name);

/*
 * Add a new line of text to the content of the current macro.
 * Input: curr- pointer to the macro we are recording. line- the string of the line to add.
 * Output: Returns SUCCESS if good or ERROR if memory failed.
 */
int add_line_to_macro(Macro *curr, char *line);

/*
 * Search for a macro in the list by its name.
 * Input: head- head of the macro list. name- the name of the macro to find.
 * Output: Returns pointer to the Macro if we find it or NULL if not.
 */
Macro *find_macro(Macro *head, char *name);

/*
 * Free all the memory of the macros and their content at the end.
 * Input: head- head of the macro list.
 * Output: None (void).
 */
void free_macros(Macro *head);

/*
 * Check if the name for the new macro is legal (not saved word or not too long).
 * Input: word- the macro name to check. line_num- the current line number for errors.
 * Output: Returns SUCCESS if the name is good or ERROR if it is bad.
 */
int is_good_name_macro(char *word, int line_num);

#endif
