#ifndef TABLE_H
#define TABLE_H

#include "globals.h"

/*
 * Add a new label to the table.
 * Input: head- pointer to head of the table. name- label name. address- label address. type- label type.
 * Output: Returns pointer to the new Symbol or NULL if it is error.
 */
Symbol* add_symbol(Symbol **head, char *name, int address, SymbolType type);

/*
 * Add a new extern to the list.
 * Input: head- pointer to the head of the list. name- extern name. address- the address we used it.
 * Output: Returns 1 if good and 0 if memory error.
 */
int add_ext_record(ext_node **head, char *name, int address);

/*
 * Free the memory of the externals list.
 * Input: head- head of the list.
 * Output: None (void).
 */
void free_ext_list(ext_node *head);


/*
 * Free the memory of the table.
 * Input: head- head of the table.
 * Output: None (void).
 */
void free_symbol_table(Symbol *head);

/*
 * Update the address of DATA labels at the end of first pass.
 * Input: head- head of the table. ICF- the final IC.
 * Output: None (void).
 */
void update_symbol_table_addresses(Symbol *head, int ICF);

/*
 * Find a label in the table by name.
 * Input: head- head of the table. name- the name to find.
 * Output: Returns pointer to the Symbol or NULL if it is not found.
 */
Symbol *find_symbol(Symbol *head, char *name);

#endif
