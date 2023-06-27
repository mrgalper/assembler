/********************************************************************************                 
*   Orginal Name : symbol_table.c                                               *
*   Name: Mark Galperin                                                         *
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
    avl_ty *tree;
};

typedef struct symbol_entry {
    char *symbol;
    size_t line;
} symbol_entry_ty;

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
    const symbol_entry_ty *entry_a = (const symbol_entry_ty *)a;
    const symbol_entry_ty *entry_b = (const symbol_entry_ty *)b;
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
    symbol_entry_ty *entry = (symbol_entry_ty *)data;
    symbol_entry_ty *params_entry = (symbol_entry_ty *)params;

    size_t len = strlen(params_entry->symbol) + 1; 

    if (strncmp(entry->symbol, params_entry->symbol, len) == 0) {
        params_entry->line = entry->line; 
        return -1;
    } else {
        return 0;
    }
}

static void freeSymbolEntry(void *entry) {
    symbol_entry_ty *symbol_entry = (symbol_entry_ty *)entry;
    free(symbol_entry->symbol);
}

static int WriteEntryToFile(void *data, void *params) {
    symbol_entry_ty *entry = (symbol_entry_ty *)data;
    FILE *file = (FILE *)params;
    
    fprintf(file, " %s %lu\n", entry->symbol, entry->line);

    return 0;
}

s_table_ty *symbolTableCreate(void) {
    s_table_ty *table = (s_table_ty *)malloc(sizeof(s_table_ty));
    if (table == NULL)
        return NULL;

    table->tree = AvlCreate(compareEntries, NULL);
    if (table->tree == NULL) {
        free(table);
        return NULL;
    }

    return table;
}

void SymbolTableDestroy(s_table_ty *table) {
    if (table == NULL)
        return;

    AvlDestroy(table->tree, freeSymbolEntry);
    free(table);
}

s_table_status_ty SymbolTableInsert(s_table_ty *table, const char *symbol, size_t line) 
{
    symbol_entry_ty entry;
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

void SymbolTableRemove(s_table_ty *table, char *symbol) {
    assert(table != NULL);
    assert(symbol != NULL);

    AvlRemove(table->tree, symbol, freeSymbolEntry);
}

int64_t SymbolTableLookup(s_table_ty *table, char *symbol) {
    symbol_entry_ty entry;

    assert(table != NULL);
    assert(symbol != NULL);

    entry.symbol = symbol;
    entry.line = -1;

    if (AVL_FAIL == AvlForEach(table->tree, FindEntry, (void *)&entry, INORDER)) {
        return entry.line;
    }

    return (int64_t)(entry.line); /* -1 */
}


s_table_status_ty SymbolTableConvertToFile(s_table_ty *table, 
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
