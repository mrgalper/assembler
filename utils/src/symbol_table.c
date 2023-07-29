/********************************************************************************                 
*   Orginal Name : symbol_table.c                                               *
*   Name: Ido Sabach                                                            *
*   Date 24.6.23                                                                *
*   Info : This is an api for the symbol table ,                                * 
*          it will use avl for balanced utilizion of lookup and insertion.      *
*********************************************************************************/
#include "symbol_table.h" /*symbol table api */
#include "avl.h" /* avl api */

#include <assert.h> /* assert */
#include <stdio.h>  /* FILE, fopen */
#include <stdlib.h> /* malloc */
#include <string.h> /* string function*/


struct symbol_table {
    avl_t *tree;
};

typedef struct symbol_entry {
    char *symbol;
    size_t line;
} symbol_entry_t;

typedef struct metadata {
    void *ptr;
    action_func_st ac;
}metadata_t;

/*******************************************
* DESCRIPTION: 
*       Compare two symbol entries based on their line numbers.
* PARAM:
*       a - Pointer to the first symbol entry.
*       b - Pointer to the second symbol entry.
* RETURN:
*       Returns an integer value less than, equal to, or greater than zero if
*       the line number of a is lower, equal to, or greater than the line number of b, respectively.
* BUGS:
*       None.
*******************************************/
static int compareEntries(const void *a, const void *b, void * params) {
    const symbol_entry_t *entry_a = (const symbol_entry_t *)a;
    const symbol_entry_t *entry_b = (const symbol_entry_t *)b;
    (void)params;
    if (entry_a->line < entry_b->line)
        return -1;
    else if (entry_a->line > entry_b->line)
        return 1;
    else
        return 0;
}
/* used to find the line of previously mentioned entry of the same symbol*/
static int FindEntry(void *data, void *params) {
    symbol_entry_t *entry = (symbol_entry_t *)data;
    symbol_entry_t *params_entry = (symbol_entry_t *)params;

    size_t len = strlen(params_entry->symbol) + 1; 

    if (strncmp(entry->symbol, params_entry->symbol, len) == 0) {
        params_entry->line = entry->line; 
        return -1;
    } else {
        return 0;
    }
}

static void freeSymbolEntry(void *entry) {
    symbol_entry_t *symbol_entry = (symbol_entry_t *)entry;
    free(symbol_entry->symbol);
}

static int WriteEntryToFile(void *data, void *params) {
    symbol_entry_t *entry = (symbol_entry_t *)data;
    FILE *file = (FILE *)params;
    char buffer[MAX_SYMBOL_LENGTH] = {0};
    memset(buffer, (int)' ', MAX_SYMBOL_LENGTH - 1);
    strncpy(buffer, entry->symbol, strlen(entry->symbol));
    fprintf(file, "%s %lu\n", buffer, entry->line);

    return 0;
}

s_table_t *CreateSymbolTable(void) {
    s_table_t *table = (s_table_t *)malloc(sizeof(s_table_t));
    if (table == NULL)
        return NULL;

    table->tree = AvlCreate(compareEntries, NULL);
    if (table->tree == NULL) {
        free(table);
        return NULL;
    }

    return table;
}

void DestroySymbolTable(s_table_t *table) {
    if (table == NULL)
        return;

    AvlDestroy(table->tree, freeSymbolEntry);
    free(table);
}

s_table_status_t SymbolTableInsert(s_table_t *table, const char *symbol, size_t line) 
{
    symbol_entry_t entry;
    size_t len = strlen(symbol) + 1;

    assert(table != NULL);
    assert(symbol != NULL);

    entry.symbol = (void *)malloc(len * (sizeof(char)));
    entry.symbol = strncpy(entry.symbol, symbol, len);
    entry.line = line;

    if (AVL_SUCCESS != AvlInsert(table->tree, &entry, sizeof(entry))) {
        free(entry.symbol);
        return ST_FAILED;
    }

    return ST_SUCCESS;
}

void SymbolTableRemove(s_table_t *table, char *symbol) {
    assert(table != NULL);
    assert(symbol != NULL);

    AvlRemove(table->tree, symbol, freeSymbolEntry);
}

int64_t SymbolTableLookup(s_table_t *table, char *symbol) {
    symbol_entry_t entry;

    assert(table != NULL);
    assert(symbol != NULL);

    entry.symbol = symbol;
    entry.line = -1;

    if (AVL_FAIL == AvlForEach(table->tree, FindEntry, (void *)&entry, INORDER)) {
        return entry.line;
    }

    return (int64_t)(entry.line); /* -1 */
}

static int UserFunctionWrapper(void *data, void *extra_params) {
    metadata_t *user_params = (metadata_t *)extra_params;
    for_each_data_t dt;
    symbol_entry_t *entry = (symbol_entry_t *)data;
    strncpy(dt.label, entry->symbol, sizeof(entry->symbol) + 1);
    dt.line = entry->line;
    return (user_params->ac(&dt, user_params->ptr));
}

s_table_status_t SymbolTableForEach(s_table_t *table, action_func_st ac, 
                                                                void *param) {
    metadata_t extra_params;
    extra_params.ac = ac;
    extra_params.ptr = param;
    if (AVL_FAIL == AvlForEach(table->tree, UserFunctionWrapper,
                                            (void *)&extra_params, INORDER)) {
        return ST_FAILED;
    }
    
    return ST_SUCCESS;   
}


s_table_status_t SymbolTableConvertToFile(s_table_t *table, 
                                                const char *filename) {                                            
    FILE *file = NULL;
    assert(table != NULL);
    assert(filename != NULL);
    if (AvlIsEmpty(table->tree)) {
        return ST_SUCCESS;
    }
    
    file = fopen(filename, "w");
    if (file == NULL) {
        return ST_FAIL_TO_OPEN_FILE;
    }


    if (AVL_FAIL == AvlForEach(table->tree, WriteEntryToFile, 
                                                (void *)file, INORDER)) {
        return ST_FAILED;
    }

    fclose(file); 
    
    return ST_SUCCESS;   
}

