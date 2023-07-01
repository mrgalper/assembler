/*******************************************************************************
*   Orginal Name : assembler_metadata.h                                        *
*   Name: Mark Galperin                                                        *
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
*      Get the assembly intermediate representation from the assembler metadata.
* PARAM:
*     md - Pointer to the assembler metadata.
* 
* RETURN:
*     assembly_IR_t* - Pointer to the assembly IR.
*************************************************************/
assembly_IR_t *GetAssemblyIR(as_metadata_t *md);

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
*     Get the filename associated with the assembler metadata.
* 
* PARAM:
*     md - Pointer to the assembler metadata.
* 
* RETURN:
*     char* - Pointer to the filename string.
*************************************************************/
const char *GetFilename(as_metadata_t *md);

#endif /*__ASSEMBLER_METADATA_H__*/
