#include "outputFiles.h"

/*
 * write output files, create the output files .ob, .ent, .ext.
 * We know the first pass and second pass finished with no errors. The code and data arrays are full.
 * this is work like-
 * -Open the .ob file and print the sizes of code and data.
 * -Loop on the code array and print address, hex word, and ARE char.
 * -Loop on the data array and print address, hex word, and 'A'.
 * -Run on the symbol table. If find entry, open .ent file and print the label and address.
 * -Run on the extern list. If not empty, open .ext file and print the label and address.
 */
void write_output_files(char *filename, MachineWord *code_img, int ICF, MachineWord *data_img, int DCF, Symbol *symbol_table, ext_node *ext_list)
{
  char out_filename[MAX_FILENAME]; /* save the new file name with the ext */
  FILE *fp; /* file pointer to write the output files */
  int i;
  int address; /* the memory address we print to the file */
  int val_12bit; /* the 12 bits of the word to print in hex */
  int are_val; /* the ARE number we get from the word */
  char are_char; /*  A,R,E to print */
  Symbol *curr_sym; /* pointer to run on the symbol table */
  ext_node *curr_ext; /* pointer to run on the extern list */

  sprintf(out_filename, "%s%s", filename, OB_EXT); /* create the object file (.ob) */
  fp = fopen(out_filename, WRITE_MODE);
    
  if (fp != NULL)
  {
    fprintf(fp, "%d %d\n", ICF - IC_VALUE, DCF); /* print the sizes of code and data in the top */
    /* print the code array. run from 100 to ICF */   
    for (i = IC_VALUE; i < ICF; i++)
    {
      val_12bit = code_img[i] & TWELVE_BIT_MASK;
      are_val = (code_img[i] >> ARE_START_BIT) & ARE_MASK; /* get the ARE from bits 12-13 */
      are_char = get_are_char(are_val);
      /* print address, hex code, and ARE char */
      fprintf(fp, "%04d %03X %c\n", i, val_12bit, are_char);
    }
    /* print the data array right after the code */   
    for (i = 0; i < DCF; i++)
    {
      address = ICF + i;
      val_12bit = data_img[i] & TWELVE_BIT_MASK; /* get the 12 bits */
      fprintf(fp, "%04d %03X %c\n", address, val_12bit, CHAR_A); /* data is always Absolute so we print 'A' */
    }        
    fclose(fp);
  }
  else
  {
    printf("Failed to create object file %s.\n", out_filename);
  }
  /* Create the Entries file (.ent) */
  curr_sym = symbol_table;
  fp = NULL; /* we use this like a flag to know if we opened the file */    
  while (curr_sym != NULL)
  {
    if (curr_sym->is_entry == TRUE)
    {
      /* we found entry,if file is closed, open it */
      if (fp == NULL)
      {
        sprintf(out_filename, "%s%s", filename, ENT_EXT); /* create the Entry file (.ent) */
        fp = fopen(out_filename, WRITE_MODE);
      }
      /* print the label and address */      
      if (fp != NULL)
      {
        fprintf(fp, "%s %04d\n", curr_sym->name, curr_sym->address);
      }
    }
    curr_sym = curr_sym->next;
  }   
  if (fp != NULL) /* close the file if we opened it */
  {
    fclose(fp);
  }
  /* Create the Externals file (.ext)*/
  if (ext_list != NULL)
  {
    sprintf(out_filename, "%s%s", filename, EXT_EXT); /* create the extern file (.ext) */
    fp = fopen(out_filename, WRITE_MODE);
        
    if (fp != NULL)
    {
      curr_ext = ext_list;
      while (curr_ext != NULL)
      {
        fprintf(fp, "%s %04d\n", curr_ext->name, curr_ext->address); /* print the extern name and the address we used it */
        curr_ext = curr_ext->next;
      }
      fclose(fp);
    }
  }
}

/*
 * get are char
 * This change the ARE number to char letter.
 * We know the input are_val is only 0, 1 or 2.
 * this is work like-
 * -if it is ARE_E return 'E'.
 * -if it is ARE_R return 'R'.
 * -else return 'A'.
 */
char get_are_char(int are_val) 
{
  if (are_val == ARE_E) 
    return CHAR_E; /* external */
  if (are_val == ARE_R) 
    return CHAR_R; /* can replace*/
  return CHAR_A; /* absolute */
}
