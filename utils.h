#ifndef UTILS_H
#define UTILS_H

#include "globals.h"

/*
 * Check if the line is empty or has only spaces.
 * Input: line- string of the line.
 * Output: Returns 1 if empty and 0 if not.
 */
int is_empty(char *line);

/*
 * Check if the string is a valid label (ends with ':').
 * Input: word- string to check.
 * Output: Returns 1 if true and 0 if false.
 */
int is_label(char *word);

/*
 * Check if the line is a comment (starts with ';').
 * Input: line- string of the line.
 * Output: Returns 1 if comment and 0 if not.
 */
int is_comment(char *line);

/*
 * Check if the string is a directive (starts with '.').
 * Input: word- string to check.
 * Output: Returns 1 if true and 0 if false.
 */
int is_directive(char *word);

/*
 * Check if the word is exactly ".string".
 * Input: string- word to check.
 * Output: Returns 1 if yes and 0 if no.
 */
int is_string(char *string);

/*
 * Check if the word is exactly ".data".
 * Input: string- word to check.
 * Output: Returns 1 if yes and 0 if no.
 */
int is_data(char *string);

/*
 * Check if the word is exactly ".entry".
 * Input: string- word to check.
 * Output: Returns 1 if yes and 0 if no.
 */
int is_entry(char *string);

/*
 * Check if the word is exactly ".extern".
 * Input: string- word to check.
 * Output: Returns 1 if yes and 0 if no.
 */
int is_extern(char *string);

/*
 * Search the command in the opcode table.
 * Input: cmd_name- the command name to find.
 * Output: Returns pointer to the opcode struct or NULL if not found.
 */
const opcode* find_opcode(char *cmd_name);

/*
 * Find the addressing mode of the operand.
 * Input: operand- the string of the operand.
 * Output: Returns the mode number (0, 1, 2, 3) or -1 if it's error.
 */
int get_addressing_mode(char *operand);

/*
 * Remove the colon (:) at the end of a label.
 * Input: word- string with the label name.
 * Output: None (void).
 */
void remove_colon(char *word);
/*
 * Get the next word from the string, skip spaces.
 * Input: str- pointer to string. word- string to put the word in.
 * Output: Returns pointer to after the word.
 */
char *get_next_word(char *str, char *word);

/*
 * Make a new file name with the extension.
 * Input: dest- string for new name. source- original name. ext- extension.
 * Output: None (void).
 */
void create_filename(char *dest, char *source, char *ext);

/*
 * Check if the string is a valid integer number and fits in 12 bits.
 * Input: str- string of the number to check.
 * Output: Returns SUCCESS if good and ERROR if it is bad.
 */
int is_valid_integer(char *str);

#endif
