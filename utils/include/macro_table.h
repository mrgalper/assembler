/**V****************************************************************************
*   Original Name: macro_table.h                                               *
*   Name: Ido Sabach                                                          *
*   Date: 30.6.23                                                              *
*   Info: This is a model that will hold all the macros.                       *
******************************************************************************/

#ifndef __MACRO_TABLE_H__
#define __MACRO_TABLE_H__

#include <stddef.h> /* size_t */

typedef struct macro_table macro_table_t;
typedef struct macro_entry *macro_table_iter_t;

typedef enum {
    MACRO_SUCCESS = 0,
    MACRO_NO_MEM = 1,
    MACRO_ALREADY_EXISTS = 2,
    MACRO_NOT_FOUND = 3
} macro_status_t;

/*******************************************
* DESCRIPTION: Create a new macro table.
* 
* RETURN:
*     SUCCESS: A pointer to the newly created macro table.
*     FAILURE: NULL.
*******************************************/
macro_table_t *CreateMacroTable(void);

/*******************************************
* DESCRIPTION: Destroy a macro table and free its memory. 
* PARAM:
*     table - A valid pointer to the macro table to be destroyed.
* RETURN: 
*      void.
* BUGS:
*      If the pointer is not valid, the result will be undefined.
*******************************************/
void DestroyMacroTable(macro_table_t *table);

/*******************************************
* DESCRIPTION: Add a macro entry to the macro table.
* 
* PARAM:
*     table - A valid pointer to the macro table.
*     label - The name of the macro entry.
*     lines - The lines of the macro entry.
*     line_count - The number of lines in the macro entry.
*     line_defined - The number of lines defined in the file.
* RETURN:
*     macro_status_t - The status of the operation:
*                     - MACRO_SUCCESS if successful.
*                     - MACRO_NO_MEM if there is low memory.
*                     - MACRO_ALREADY_EXISTS if the macro entry already exists.
* BUGS:
*      If the pointer is not valid, the result will be undefined.
*******************************************/
macro_status_t MacroTableAddEntry(macro_table_t *table, const char *label, 
                                 const char **lines, size_t lines_count, 
                                 size_t line_defined);

/*******************************************
* DESCRIPTION: Get the first macro entry in the macro table.
* 
* PARAM:
*     table - A valid pointer to the macro table.
* 
* RETURN:
*     macro_table_iter_t - An iterator pointing to the first macro entry.
*******************************************/
macro_table_iter_t MacroTableGetFirstEntry(macro_table_t *table);

/*******************************************
* DESCRIPTION: Get the last macro entry in the macro table.
* 
* PARAM:
*     table - A valid pointer to the macro table.
* 
* RETURN:
*     macro_table_iter_t - An iterator pointing to the first macro entry.
*******************************************/
macro_table_iter_t MacroTableGetLastEntry(macro_table_t *table);

/*******************************************
* DESCRIPTION: finds the entry  
* 
* PARAM:
*     table - A valid pointer to the macro table.
* RETURN:
*     SUCCESS: A pointer to the macro entry.
*     FAIL: return the LAST macro entry.
*******************************************/
macro_table_iter_t MacroTableFindEntry(macro_table_t *table, const char *lable);

/*******************************************
* DESCRIPTION: Get the next macro entry in the macro table.
* 
* PARAM:
*     iter - A valid iterator pointing to a macro entry.
* 
* RETURN:
*     macro_table_iter_t - An iterator pointing to the next macro entry.
*******************************************/
macro_table_iter_t MacroTableGetNextEntry(macro_table_iter_t iter);

/*******************************************
* DESCRIPTION: Get the name of the lable.
* 
* PARAM:
*     iter - A valid iterator pointing to a macro entry.
* 
* RETURN:
*     const char* - The name of the macro entry.
*******************************************/
const char *MacroTableGetEntryLable(macro_table_iter_t iter);

/*******************************************
* DESCRIPTION: Get the value of the entry lines , use with Number of lines.
* 
* PARAM:
*     iter - A valid iterator pointing to a macro entry.
* 
* RETURN:
*     const char* - The value of the macro entry.
*******************************************/
const char **MacroTableGetEntryLines(macro_table_iter_t iter);

/*******************************************
* DESCRIPTION: Get the number of lines.
* 
* PARAM:
*     iter - A valid iterator pointing to a macro entry.
* RETURN:
*     const char* - The value of the macro entry.
*******************************************/
size_t MacroTableGetEntryNumberOfLines(macro_table_iter_t iter);

/*******************************************
* DESCRIPTION: Get the line the macro is defined in the original file.
* 
* PARAM:
*     iter - A valid iterator pointing to a macro entry.
* 
* RETURN:
*     const char* - The value of the macro entry.
*******************************************/
size_t MacroTableGetEntryLineDefined(macro_table_iter_t iter);

/*******************************************
* DESCRIPTION: compare if two iterators are equal, use it to check 
*              if you are in the Last entry of the macro table.
* PARAM:
*     it1, it2 - iterators to compare.
* RETURN: 
*     if matched or 0 ,respectivly.
*******************************************/
int MacroTableIterIsEqual(macro_table_iter_t it1, macro_table_iter_t it2);


#endif/* __MACRO_TABLE_H__ */
