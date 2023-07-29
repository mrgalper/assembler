/********************************************************************************                 
*   Orginal Name : symbol_table.h                                               *
*   Name: Ido Sabach                                                            *
*   Date 24.6.23                                                                *
*   Info : This is an api for the symbol table ,                                * 
*          it will use avl for balanced utilizion of lookup and insertion.      *
*********************************************************************************/

#ifndef __SYMBOL_TABLE_H__
#define __SYMBOL_TABLE_H__

#include "stddef.h" /* size_t */
#include "stdint.h" /* int64_t */

#define MAX_SYMBOL_LENGTH 32 /* 31 + 1*/

typedef struct symbol_table s_table_t;

typedef enum symbol_table_status {
    ST_SUCCESS = 0,
    ST_FAIL_TO_OPEN_FILE = 1,
    ST_FAILED = 2
} s_table_status_t;

typedef struct for_each_data {
    char label[MAX_SYMBOL_LENGTH];
    int64_t line;
}for_each_data_t;

/*
    this is a function of you're implemintation , and it return 0 on AVL_SUCCESS
    else will return a fail.
    you will get data from the function and params that you pass
*/
typedef int(*action_func_st)(for_each_data_t *data, void *params);

/*******************************************
* DESCRIPTION: 
*       Create a new symbol table.
* PARAM:
*    
* RETURN:
*       SUCCESS: Returns a pointer to the created symbol table.
*       FAIL: NULL no memory.
* BUGS:
*       None.
*******************************************/
s_table_t *CreateSymbolTable(void);

/*******************************************
* DESCRIPTION: 
*       Destroy a symbol table and free its resources.
* PARAM:
*       table - A pointer to the symbol table.
* RETURN:
*       None.
* BUGS:
*       If the provided table pointer is invalid (NULL), the behavior is undefined.
*******************************************/
void DestroySymbolTable(s_table_t *table);

/*******************************************
* DESCRIPTION: 
*       Insert a symbol into the symbol table.
* PARAM:
*       table - A pointer to the symbol table.
*       symbol - The symbol to insert.
*       line - The line associated with the symbol.
* RETURN:
*       Returns SUCCESS if the insertion is successful, FAILED otherwise.
* BUGS:
*       If the provided table pointer or symbol pointer is invalid (NULL), the behavior is undefined.
*******************************************/
s_table_status_t SymbolTableInsert(s_table_t *table, const char *symbol, size_t line);

/*******************************************
* DESCRIPTION: 
*       Remove a symbol from the symbol table.
* PARAM:
*       table - A pointer to the symbol table.
*       symbol - The symbol to remove.
* RETURN:
*       None.
* BUGS:
*       If the provided table pointer or symbol pointer is invalid (NULL), the behavior is undefined.
*******************************************/
void SymbolTableRemove(s_table_t *table, char *symbol);

/*******************************************
* DESCRIPTION: 
*       Lookup a symbol in the symbol table and retrieve the associated line number.
* PARAM:
*       table - A pointer to the symbol table.
*       symbol - The symbol to lookup.
* RETURN:
*       Returns the line number associated with the symbol if found, or -1 if not found.
* BUGS:
*       If the provided table pointer or symbol pointer is invalid (NULL), the behavior is undefined.
*******************************************/
int64_t SymbolTableLookup(s_table_t *table, char *symbol);

/*******************************************
* DESCRIPTION: 
*       Function that will iterate on all values in the entring order and will
*       active action function with param.
* PARAM:
*       table - A pointer to the symbol table.
*       ac - the action function.
*       param - optional param to pass to ac can be NULL
* RETURN:
*       If the action function returns value other 1 return 
*       ST_FAIL otherwise ST_SUCCESS.
* BUGS:
*       If the provided table pointer or ac is invalid (NULL), the behavior is undefined.
*******************************************/
s_table_status_t SymbolTableForEach(s_table_t *table, action_func_st ac, 
                                                                void *param);
/*******************************************
* DESCRIPTION: 
*       Print the symbol table into the file in a the following format:
*       space<symbol>space<line number>'\n' 
*       if symbol table is empty, it will not open the file.
*       The file will be created , if it exists it will override the data.
* PARAM:
*       table - A pointer to the symbol table.
*       file - The file name to create.
*RETURN:
*     SUCCESS: Returns ST_SUCCESS else fail that represents the error.
* BUGS:
*       If the provided table pointer is invalid (NULL), the behavior is undefined.
*******************************************/
s_table_status_t SymbolTableConvertToFile(s_table_t *table, const char *filename);

#endif /* __SYMBOL_TABLE_H__ */
