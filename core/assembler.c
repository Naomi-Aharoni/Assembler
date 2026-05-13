#include "globals.h"
#include "preAssembler.h"
#include "firstPass.h"
#include "secondPass.h"
#include "outputFiles.h"
#include "table.h"
#include "utils.h"

/*
 * This program is an assembler for a specific 12 bit machine. 
 * The goal is to translate assembly code files (.as) written by the user into machine code files (.ob), and create entry (.ent) and external (.ext) files if needed.
 * this is work like:
 * The translation works in 3 big steps:
 * -Pre Assembler: Read the source file, find macros, save their content, and replace macro calls with the text. Creates a clean .am file.
 * -First Pass: Read the .am file. Build the symbol table, count memory words using IC (Instruction Counter) and DC (Data Counter). Encode the first word of each instruction and immediate/register operands.
 * -Second Pass: Read the .am file again. Complete the missing operand words that need label addresses. 
 * If all steps finish without errors, it creates the output files.
 * Input: 
 * File names passed in the command line (argv) without the ".as" extension.
 * Output: 
 * .am (after macros), .ob (machine code), .ent (entry labels), .ext (extern labels).
 * Rules and limits:
 * -Max memory size is 4096 words (MAX_RAM).
 * -Max line length is 80 characters.
 * -Max macro or label name is 31 characters.
 */
int main(int argc, char *argv[]) 
{
  int i;
  if (argc < MIN_ARGS) /* check if the user gave at least one file name in the command line */
  {
    printf("Please enter file names like this: %s <file1> <file2> ...\n", argv[0]);
    return !OK;
  }
  for (i = 1; i < argc; i++) /* loop on all the source files the user gave */ 
  {
    int ICF = 0;
    int DCF = 0; 
    int error_found = FALSE; /* flag to know if we had errors in the passes */
    Macro *macro_list = NULL; /* pointer to the head of the macros list */       
    MachineWord code_img[MAX_RAM] = {0}; /* array to hold the code memory */
    MachineWord data_img[MAX_RAM] = {0}; /* array to hold the data memory */      
    Symbol *symbol_table = NULL; /* pointer to the head of the symbol table */
    ext_node *ext_list = NULL; /* pointer to the head of the externals list */
    char am_filename[MAX_FILENAME]; /* hold the name of the .am file */
    FILE *fp; /* file pointer to read the files */
    printf("\n");
    printf("Processing file: %s.as\n", argv[i]);
    /* Pre Assembler (find and expand macros) */
    if (process_macros(argv[i], &macro_list))
    {
      create_filename(am_filename, argv[i], AM_EXT); /* create the file name with .am extension */
      fp = fopen(am_filename, READ_MODE); /* open it */          
      if (fp != NULL) 
      { /* First Pass (build symbol table and code first words) */
        first_pass(fp, code_img, data_img, &ICF, &DCF, &symbol_table, &error_found, macro_list);
        if (error_found == FALSE) /* continue to second pass only if first pass is good without errors */ 
        { /* Second Pass (finish the empty words of labels) */
          second_pass(fp, argv[i], symbol_table, code_img, &ICF, &ext_list, &error_found);           
          if (error_found == FALSE) /* create output files only if second pass is good too */ 
          {
            write_output_files(argv[i], code_img, ICF, data_img, DCF, symbol_table, ext_list);
            printf("Finished %s successfully.\n", argv[i]);
          }
          else
          {
            printf("Errors found in second pass. Skipping output files.\n");
          }
        }
        else
        {
          printf("Errors found in first pass. Skipping output files.\n");
        }
        fclose(fp); /* close the .am file when we finish reading it */
      }
      else
      {
        printf("Failed to open file %s for reading.\n", am_filename);
      }
    }
    else
    {
      printf("Errors in macros, skipping output files.\n");
    }
    /* free all the memory at the end of the file so the next file starts clean */   
    free_symbol_table(symbol_table);
    free_ext_list(ext_list);
    free_macros(macro_list);
  } 
  printf("\n");
  printf("Assembler finished processing all files.\n");
  return OK;
}
