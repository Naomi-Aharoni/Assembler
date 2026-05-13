#include "preAssembler.h"
#include "utils.h"

/*
 * This is do the pre assembler pass, find macros and expand them. Creates the .am file.
 * We know the file name is good.
 * this is work like:
 * -Open the .as file for read and create .am file for write.
 * -Read the file line by line.
 * -If we see "mcro", we change state to IN_MACRO and save the macro.
 * -If we are IN_MACRO, we add the lines to the macro content.
 * -If we see "mcroend", we change state to OUT_MACRO to stop recording.
 * -If we are OUT_MACRO and see a macro name, we write the macro content to the .am file instead of the name.
 * -If we find errors, we turn on error_flag and don't save the bad file.
 */
int process_macros(char *filename, Macro **out_macro_list)
{
  FILE *fp_as, *fp_am; 
  char as_name[MAX_FILENAME]; /* source file name */
  char am_name[MAX_FILENAME]; /* output file name */
  char line[MAX_LINE_LEN]; /* the current line from the file */
  char first_word[MAX_LINE_LEN]; /* first word we read in the line */
  int state = OUT_MACRO; /* to know if we are inside a macro or not */
  Macro *macro_list = NULL; /* pointer to the start of the macro list */
  Macro *curr_macro = NULL; /* pointer to the macro we are working on now */
  Macro *found_macro = NULL; /* pointer to save a macro if we find it in the list */
  char m_name[MAX_LINE_LEN]; /* save the name of the new macro */
  char extra[MAX_LINE_LEN]; /* check for garbage text at the end */
  char *p; /* pointer to run on the line */
  int line_num = 0; /* counter for the lines to print in errors */
  int error_flag = FALSE; /* remember if we had errors */
  /* make the full names for the files with .as and .am */
  create_filename(as_name, filename, AS_EXT);
  create_filename(am_name, filename, AM_EXT);
  /* try to open the original source file for reading */
  fp_as = fopen(as_name, READ_MODE);
  if(fp_as == NULL) 
  {
    printf("Failed to open file %s.\n", as_name);
    return OPEN_FAILED; 
  }
  /* try to open the new file for writing the macro expansion */
  fp_am = fopen(am_name, WRITE_MODE); 
  if(fp_am == NULL) 
  {
    printf("Failed to open file %s.\n", am_name);
    fclose(fp_as);
    return OPEN_FAILED; 
  }
  /* run on the file line by line until the end */
  while (fgets(line, MAX_LINE_LEN, fp_as) != NULL) 
  {
    line_num++;
    if (strchr(line, '\n') == NULL && !feof(fp_as)) /* check if the line is too long (more than 80 chars) */ 
    {
      printf("Error in file %s, line %d, line is too long.\n", as_name, line_num);
      while (fgets(line, MAX_LINE_LEN, fp_as) != NULL && strchr(line, '\n') == NULL);
      error_flag = TRUE;
    }
    else 
    {
      if (is_empty(line)) /* if the line is empty or has only spaces */
      {
        if (state == OUT_MACRO) 
        {
          if (error_flag == FALSE) /* we are outside macro, copy empty lines to output only if no errors yet */
          {
            fputs(line, fp_am); 
          }
        }
        else
        {
          if (add_line_to_macro(curr_macro, line) == ERROR) /* we are inside a macro, save the empty line to the macro content */ 
          {
            error_flag = TRUE;
          }
        }     
      }
      else
      {
        p = get_next_word(line, first_word); /* line is not empty, read the first word */
        if (state == IN_MACRO) /* if we are currently recording a macro */
        {
          if (!strcmp(first_word, "mcroend")) /* check if we reached the end of the macro definition */
          {
            p = get_next_word(line, first_word); /* skip the mcroend word */
            get_next_word(p, extra); /* check if there is garbage text after it */        
            if (extra[FIRST_CHAR] != EMPTY_CHAR)
            {
              printf("Error in file %s, line %d, extra text after mcroend.\n", as_name, line_num);
              error_flag = TRUE;
            }
            state = OUT_MACRO; /* stop recording so next lines go to regular code */
          }
          else
          {
            if (add_line_to_macro(curr_macro, line) == ERROR) /* it is a regular code line inside the macro, add it */
            {
              error_flag = TRUE;
            }
          }
        } 
        else if (state == OUT_MACRO) /* if we are looking for new macros or regular code */
        {
          if (!strcmp(first_word, "mcro")) /* we found a start of a new macro */
          {
            p = get_next_word(line, first_word); /* skip the mcro word */
            p = get_next_word(p, m_name); /* read the new macro name */        
            get_next_word(p, extra); /* check for garbage text */        
            if (m_name[FIRST_CHAR] == EMPTY_CHAR || strlen(m_name) == 0)
            {
              printf("Error in file %s, line %d, macro name is missing.\n", as_name, line_num);
              error_flag = TRUE;
            }
            else if (extra[FIRST_CHAR] != EMPTY_CHAR)
            {
              printf("Error in file %s, line %d, extra text after macro name.\n", as_name, line_num);
              error_flag = TRUE; 
            }
            else if (is_good_name_macro(m_name, line_num) == ERROR)
            {
              error_flag = TRUE; 
            }
            else
            {
              state = IN_MACRO; /* name is good, change state to start recording */
              curr_macro = handle_new_macro(&macro_list, m_name);
              if (curr_macro == NULL) 
              {
                error_flag = TRUE; 
              }
            }
          }
          else if ((found_macro = find_macro(macro_list, first_word)) != NULL) /* check if the word is an existing macro name */
          {
            if (found_macro->content != NULL && error_flag == FALSE) /* we found a macro call, paste its content instead of the name */
            {
              fputs(found_macro->content, fp_am); 
            }
          }
          else if (is_label(first_word)) /* maybe it is a label before a macro call */
          {
            get_next_word(p, extra);    
            if ((found_macro = find_macro(macro_list, extra)) != NULL) /* check if the word after the label is a macro */
            {
              if (error_flag == FALSE) 
                fprintf(fp_am, "%s ", first_word); 
              if (found_macro->content != NULL && error_flag == FALSE) /* print the macro content next to the label */
                fputs(found_macro->content, fp_am);
            }
            else
            {
              if (error_flag == FALSE) /* it is a regular label with regular code, only print it */
                fputs(line, fp_am); 
            }
          }
          else 
          {
            if (error_flag == FALSE) fputs(line, fp_am); /* regular code line, only print to the am file */ 
          }
        }
      } 
    }
  }
  fclose(fp_as);
  fclose(fp_am); 
  if (error_flag == TRUE) /* if we had errors anywhere so delete the bad file and return error */
  {
    free_macros(macro_list);
    remove(am_name); /* delete the am file because we found errors */
    *out_macro_list = NULL;
    return ERROR;
  }
  *out_macro_list = macro_list; 
  return SUCCESS; 
}

/*
 * It creates a new macro and puts it in the list.
 * The memory allocation works.
 * this is work like:
 * -Malloc a new node for the macro.
 * -If it fails so print error and free everything and exit.
 * -Copy the name and set content to NULL.
 * -Put it at the head of the macro list.
 */
Macro *handle_new_macro(Macro **head, char *name)
{
  Macro *new_node = (Macro *)malloc(sizeof(Macro)); /* allocate memory for the new macro */
  if (new_node == NULL) 
  {
    printf("Malloc failed for new macro.\n"); /* if malloc failed so print error and free all macros and stop the program */
    free_macros(*head); 
    exit(EXIT_FAILURE); 
  } 
  strcpy(new_node -> name, name); /* copy the name to the new node and set content to empty */  
  new_node -> content = NULL; 
  new_node -> next = *head; /* put the new node at the start (head) of the macro list */ 
  *head = new_node; 
  return new_node;
}

/*
 * It adds a new line of text to the macro content.
 * The macro exists and is not NULL.
 * this is work like:
 * -Get the length of the new line and the old content.
 * -Use realloc to make the content array bigger to fit both.
 * -If realloc fails, free the content and return error.
 * -Strcat the new line to the end of the content.
 */
int add_line_to_macro(Macro *curr, char *line) 
{
  char *new_content; /* pointer for the new bigger string */
  size_t new_len; /* length of the new line we add */
  size_t old_len; /* length of the old content we already have */
   
  if (curr == NULL) 
  {
    return ERROR; 
  }
  new_len = strlen(line);
  old_len = 0;
  if (curr -> content != NULL) /* check if we already have some content inside */
  {
    old_len = strlen(curr -> content);
  }  
  new_content = (char *)realloc(curr->content, old_len + new_len + 1); /* make the array bigger to add the new line, +1 is for the null terminator ('\0') at the end of the string */
  if (new_content == NULL) 
  {
    printf("Realloc failed for macro content.\n");
    free(curr -> content);
    curr -> content = NULL;
    return ERROR; 
  }
  curr -> content = new_content;
  if (old_len == 0) /* if this is the first line, make sure the string starts empty */
  {
    curr -> content[FIRST_CHAR] = EMPTY_CHAR;
  }
  strcat(curr -> content, line); /* add the new line to the end of the content */
  return SUCCESS; 
}

/*
 * It searches for a macro by name in the list.
 * The list is initialized.
 * this is work like:
 * -Run on the list with pointer.
 * -Use strcmp to check the name.
 * -If we find it, return the pointer. Else return NULL.
 */
Macro *find_macro(Macro *head, char *name) 
{
  Macro *p = head;
  while (p != NULL) /* run on all the macros in the list */
  {
    if (strcmp(p->name, name) == 0)
    {
      return p; /* found it */
    }
    p = p->next;
  }
  return NULL; /* not found */
}

/*
 * It frees all macros memory at the end.
 * this is work like:
 * -Run on the list with pointer.
 * -Save current node to temp.
 * -Free the content if it exists.
 * -Free the temp node itself.
 */
void free_macros(Macro *head) 
{
  Macro *temp;
  while (head != NULL) /* run on the list and free every node and its content */
  {
    temp = head;
    head = head->next;
    if (temp -> content) /* free the lines string before we free the macro */
      free(temp->content);
    free(temp); 
  }
}

/*
 * It checks if the macro name is legal.
 * this is work like:
 * -Check if it is missing or too long.
 * -Check if it is opcode, directive or register name.
 * -Return SUCCESS if it is ok and ERROR if not.
 */
int is_good_name_macro(char *word, int line_num)
{
  if (word == NULL || strlen(word) == 0) /* check if empty */
  {
   printf("Error in line %d, macro name is missing.\n", line_num);
   return ERROR;
  }
  if(strlen(word) > MACRONAME-1) /* check length limit */
  {
   printf("Error in line %d, macro name is too long.\n", line_num);
   return ERROR;
  }
  if (find_opcode(word) != NULL) /* check if it is a saved command name */
  {
    printf("Error in line %d, macro name can't be an opcode.\n", line_num);
    return ERROR;
  }
  if (strcmp(word, "data") == 0 || strcmp(word, "string") == 0 || strcmp(word, "entry") == 0 || strcmp(word, "extern") == 0) 
  { /* check if it is a saved dot directive */
    printf("Error in line %d, macro name can't be a directive.\n", line_num);
   return ERROR;
  }
  if (word[FIRST_CHAR] == REG_PREFIX && isdigit(word[SECOND_CHAR]) && (word[SECOND_CHAR] - '0') >= MIN_REGISTER && (word[SECOND_CHAR] - '0') <= MAX_REGISTER && word[THIRD_CHAR] == EMPTY_CHAR)
  { /* check if it is a register name (like r0, r1...) */
   printf("Error in line %d, macro name can't be a register.\n", line_num);
    return ERROR;
  }
  return SUCCESS; /* the name is good */
}
