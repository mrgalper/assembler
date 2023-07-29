/*******************************************************************************
*   Orginal Name : assembler_metadata.h                                        *
*   Name: Ido Sabach                                                           *
*   Date 30.6.23                                                               *
*   Info : The assembler metadata used to store the information about the      * 
*   assmebler.                                                                 *
*******************************************************************************/
#ifndef __ASSEMBLER_METADATA_H__
#define __ASSEMBLER_METADATA_H__

#include "symbol_table.h" /* API */
#include "logger.h" /* API */
#include "assembly_IR.h" /* API */
#include "macro_table.h" /* API */
#include <stdio.h> /* FILE */

typedef struct assembler_metadata_t as_metadata_t; 

/*******************************************
* DESCRIPTION: Create an instance of assembler metadata.
* 
* PARAM:
*     filename - The name of the file associated with the metadata.
* RETURN:
*     SUCCESS: as_metadata_t* - Pointer to the created assembler metadata.
*     FAILURE: NULL.
*************************************************************/
as_metadata_t *CreateAssemblerMetadata(const char *filename);

/*******************************************
* DESCRIPTION: 
*     Destroy an instance of assembler metadata and free allocated memory.
* 
* PARAM:
*     md - Pointer to the assembler metadata to destroy.
* BUGS:
*     if the pointer is not valid , the result is undefined.
*************************************************************/
void DestroyAssemblerMetadata(as_metadata_t *md);

/*******************************************
* DESCRIPTION: 
*     Get the symbol table from the assembler metadata.
* PARAM:
*     md - Pointer to the assembler metadata. 
* RETURN:
*     s_table_t* - Pointer to the symbol table.
*************************************************************/
s_table_t *GetSymbolTable(as_metadata_t *md); 

/*******************************************
* DESCRIPTION: 
*     Get the entry table from the assembler metadata.
* PARAM:
*     md - Pointer to the assembler metadata.
* RETURN:
*     s_table_t* - Pointer to the entry table.
*************************************************************/
s_table_t *GetEntryTable(as_metadata_t *md);

/*******************************************
* DESCRIPTION: 
*     Get the extern table from the assembler metadata.
* PARAM:
*     md - Pointer to the assembler metadata.
* RETURN:
*     s_table_t* - Pointer to the extern table.
*************************************************************/
s_table_t *GetExternTable(as_metadata_t *md);

/*******************************************
* DESCRIPTION: 
*     Get the entry output from the assembler metadata.
* PARAM:
*     md - Pointer to the assembler metadata.
* RETURN:
*     s_table_t* - Pointer to the entry output.
*************************************************************/
s_table_t *GetEntryOutput(as_metadata_t *md);

/*******************************************
* DESCRIPTION: 
*     Get the extern output from the assembler metadata.
* PARAM:
*     md - Pointer to the assembler metadata.
* RETURN:
*     s_table_t* - Pointer to the extern output.
*************************************************************/
s_table_t *GetExternOutput(as_metadata_t *md);
/*******************************************
* DESCRIPTION: 
*      Get the assembly intermediate representation from the assembler metadata.
* PARAM:
*     md - Pointer to the assembler metadata.
* 
* RETURN:
*     assembly_IR_t* - Pointer to the assembly IR.
*************************************************************/
assembly_IR_t *GetAssemblyIRData(as_metadata_t *md);

/*******************************************
* DESCRIPTION: 
*      Get the assembly intermediate representation from the assembler metadata.
* PARAM:
*     md - Pointer to the assembler metadata.
* 
* RETURN:
*     assembly_IR_t* - Pointer to the assembly IR.
*************************************************************/
assembly_IR_t *GetAssemblyIRInst(as_metadata_t *md);

/*******************************************
* DESCRIPTION: 
*     Get the macro table from the assembler metadata.
* 
* PARAM:
*     md - Pointer to the assembler metadata.
* 
* RETURN:
*     macro_table_h* - Pointer to the macro table.
*************************************************************/
macro_table_t *GetMacroTable(as_metadata_t *md);

/*******************************************
* DESCRIPTION: 
*    Get the logger from the assembler metadata.
* 
* PARAM:
*     md - Pointer to the assembler metadata.
* 
* RETURN:
*     logger_t* - Pointer to the logger.
*************************************************************/
logger_t *GetLogger(as_metadata_t *md);

/*******************************************
* DESCRIPTION: 
*    Get the Warning logger from the assembler metadata.
* 
* PARAM:
*     md - Pointer to the assembler metadata.
* 
* RETURN:
*     logger_t* - Pointer to the logger.
*************************************************************/
logger_t *GetWarningLogger(as_metadata_t *md);

/*******************************************
* DESCRIPTION: 
*     Get the filename associated with the assembler metadata.
* 
* PARAM:
*     md - Pointer to the assembler metadata.
* 
* RETURN:
*     char* - Pointer to the filename string + ".as".
*************************************************************/
const char *GetFilename(as_metadata_t *md);


/*******************************************
* DESCRIPTION: 
*     Get the file descriptor.
* 
* PARAM:
*     md - Pointer to the assembler metadata.
* 
* RETURN:
*     FILE* - Pointer to the file;
*************************************************************/
FILE *GetFile(as_metadata_t *md);

/*******************************************
* DESCRIPTION: 
*     Get the Instruction counter. default is 0.
* 
* PARAM:
*     md - Pointer to the assembler metadata.
* 
* RETURN:
*     FILE* - IC values
*************************************************************/
size_t GetIC(as_metadata_t *md);

/*******************************************
* DESCRIPTION: 
*     Get the data counter. default is 0.
* 
* PARAM:
*     md - Pointer to the assembler metadata.
* 
* RETURN:
*     FILE* - DC values
*************************************************************/
size_t GetDC(as_metadata_t *md);

/*******************************************
* DESCRIPTION: 
*     Get the Program counter. default value start at 100.
* 
* PARAM:
*     md - Pointer to the assembler metadata.
* 
* RETURN:
*     FILE* - DC values
*************************************************************/
size_t GetPC(as_metadata_t *md);

/*******************************************
* DESCRIPTION: 
*     Set the Instruction counter. default is 0.
* PARAM:
*     md - Pointer to the assembler metadata.
* RETURN:
*    void 
*************************************************************/
void SetPC(as_metadata_t *md, size_t pc);

/*******************************************
* DESCRIPTION: 
*     Set the data counter.
* 
* PARAM:
*     md - Pointer to the assembler metadata.
* 
* RETURN:
*     void
*************************************************************/
void SetIC(as_metadata_t *md, size_t ic);

/*******************************************
* DESCRIPTION: 
*     Set the Instruction counter. 
* 
* PARAM:
*     md - Pointer to the assembler metadata.
* 
* RETURN:
*     void
*************************************************************/
void SetDC(as_metadata_t *md, size_t dc);


/*******************************************
* DESCRIPTION: 
*    eturn the default values that is used as the start value of the pc.
* 
* PARAM:
*     void
* 
* RETURN:
*     The default pc value
*************************************************************/
size_t GetDefautPcValue(void);

/*******************************************
* DESCRIPTION: 
*     Set the default Program counter values (default is set to 100).
* 
* PARAM:
*     new_val - the new value to set it to. 
* 
* RETURN:
*     void
*************************************************************/
void SetDefautPcValue(size_t new_val);

#endif /*__ASSEMBLER_METADATA_H__*/
