#include "table.h"

/*
 * It adds a new label to the symbol table.
 * The memory allocation works and the name is not too long.
 * this is work like:
 * -Check if the name length is bigger than max (MACRONAME - 1). If yes, print error and return NULL.
 * -Allocate memory for the new symbol.
 * -If malloc failed, free the table and exit the program.
 * -Copy the name, address, and type to the new node.
 * -Set is_entry to 0 as default (change later if we see .entry).
 * -Put the new node at the head of the list and return it.
 */
Symbol* add_symbol(Symbol **head, char *name, int address, SymbolType type)
{
  Symbol *new_symbol; /* pointer to the new label node we create */
  if (strlen(name) > MACRONAME - 1) /* check if the name is too long for the array */
  {
    printf("Label name %s is too long.\n", name);
    return NULL; /* we don't add it to the table */
  }
  new_symbol = (Symbol *)malloc(sizeof(Symbol)); /* try to allocate memory for the new label */
  if (new_symbol == NULL) /* if malloc failed, we must free everything and exit */ 
  {
    printf("Malloc failed for symbol table.\n");
    free_symbol_table(*head);
    exit(EXIT_FAILURE); 
  }
  /* copy the details to the new node */
  strcpy(new_symbol -> name, name);
  new_symbol->address = address;
  new_symbol->type = type;
  new_symbol->is_entry = FALSE; /* default- it is not entry. we change it only if we see .entry later */
  /* put the new node at the start of the list */
  new_symbol->next = *head;
  *head = new_symbol;
  return new_symbol;
}

/*
 * It adds a new extern record to the externals list.
 * We know the memory allocation works.
 * this is work like:
 * -Allocate memory for the new node.
 * -If malloc failed, free the list and exit.
 * -Copy the name and address to the new node.
 * -Put the new node at the head of the list.
 * -Return 1 for success.
 */
int add_ext_record(ext_node **head, char *name, int address)
{
  ext_node *new_node = (ext_node *)malloc(sizeof(ext_node)); /* try to allocate memory for the new extern node */
  if (new_node == NULL) /* if malloc failed, free the list and exit */
  {
    printf("Malloc failed for extern node.\n");
    free_ext_list(*head);
    exit(EXIT_FAILURE);
  }
  strcpy(new_node->name, name); /* copy the name and the address where we used it */
  new_node->address = address;
  new_node->next = *head; /* add it to the head of the list */
  *head = new_node;
  return SUCCESS; /* return 1 for success */
}

/*
 * It frees the memory of the externals list at the end of the program.
 * The head points to the start of the list or NULL.
 * this is work like:
 * -Run on the list with a pointer.
 * -Save the current node in a temp pointer.
 * -Move the current pointer to the next node.
 * -Free the temp node.
 */
void free_ext_list(ext_node *head)
{
  ext_node *current = head; /* pointer to run on the list */
  ext_node *temp; /* pointer to hold the node we want to free */
  while (current != NULL) /* run until the end of the list */
  {
    temp = current; /* save the current node to free it later */
    current = current->next; /* move to the next node before we delete this one */
    free(temp); /* free the memory */
  }
}

/*
 * It frees the memory of the symbol table.
 * The head points to the start of the table or NULL.
 * this is work like:
 * -Run on the table with a pointer.
 * -Save the current node in a temp pointer.
 * -Move the current pointer to the next node.
 * -Free the temp node.
 */
void free_symbol_table(Symbol *head) 
{
  Symbol *current = head; /* pointer to run on the table */
  Symbol *temp; /* pointer to hold the node we want to free */   
  while (current != NULL) /* run until the end of the table */
  {
    temp = current;    
    current = current->next;  
    free(temp);
  }
}

/*
 * It updates the addresses of the data labels after the first pass.
 * We know the first pass is done and we have the final ICF number.
 * this is work like:
 * -Run on the table with a pointer.
 * -If the symbol type is DATA_SYMBOL, add the ICF to its address.
 */
void update_symbol_table_addresses(Symbol *head, int ICF) 
{
  Symbol *current = head; /* pointer to run on the table */
  while (current != NULL) /* loop on all the labels in the table */ 
  { 
    if (current->type == DATA_SYMBOL) /* if it is a data label, we need to add the final ICF to its address */ 
      current->address += ICF; 
    current = current->next; /* move to the next label */
  }
}

/*
 * It searches for a label in the symbol table by its name.
 * We know the name is a valid string.
 * this is work like:
 * -Run on the table with a pointer.
 * -Compare the names using strcmp.
 * -If we find it, return the pointer to the symbol.
 * -If we reach the end and don't find it, return NULL.
 */
Symbol *find_symbol(Symbol *head, char *name)
{
  Symbol *current = head; /* pointer to run on the table to search */
  while (current != NULL) /* loop on the table to find the name */
  { 
    if (strcmp(current->name, name) == 0) /* if we found the exact name, return this node */
      return current;
    current = current->next; /* move to check the next label */
  }
  return NULL; /* not found */
}
