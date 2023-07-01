/******************************************************************************
*   Orginal Name : slist.c                                                    *
*   Name: Mark Galperin                                                       *
*   Date 23.6.23                                                              *
*   Info : Defenition of ADT Singel Linked List function                      *
******************************************************************************/
#include "assembler_metadata.h" /* API*/

#include <stdlib.h> /* malloc, free, getenv */
#include <assert.h> /* assert */
#include <string.h> /* memset */
#include <stdio.h> /* printf */

struct assembler_metadata_t {
    s_table_t *symbol_table;
    s_table_t *enrty_table;
    s_table_t *extern_table;
    assembly_IR_t *assembly_IR;
    macro_table_t *macro_table;
    logger_t *logger;
    char *filename;
};

as_metadata_t *CreateAssemblerMetadata(const char *filename) {
    as_metadata_t *md;
    size_t len = strlen(filename);
    assert(filename != NULL);
    
    md = calloc(1, sizeof(as_metadata_t)); /* allocate and memset*/
    if (md == NULL) {
        return NULL;
    }
    md->symbol_table = CreateSymbolTable();
    md->enrty_table  = CreateSymbolTable();
    md->extern_table = CreateSymbolTable();
    md->assembly_IR  = CreateAssemblyIR();
    md->macro_table  = CreateMacroTable();
    md->logger       = CreateLogger();
    if (md->symbol_table == NULL || md->enrty_table == NULL || 
        md->extern_table == NULL || md->assembly_IR == NULL || 
        md->macro_table == NULL || md->logger == NULL) {
        DestroyAssemblerMetadata(md);
        return NULL;
    }
    
    md->filename = malloc(len + 1 + 3/* suffix .as*/);
    strncpy(md->filename, filename, len);
    strncpy(md->filename + len, ".as", 3);
    md->filename[len + 3] = '\0';
    
    if (getenv("ASSEMBLER_DEBUG") != NULL) {
        printf("%s\n", md->filename);
    }

    return md;    
}

void DestroyAssemblerMetadata(as_metadata_t *md) {
    if (md->symbol_table != NULL) {
        DestroySymbolTable(md->symbol_table); 
    }
    if (md->enrty_table != NULL) {
        DestroySymbolTable(md->enrty_table);
    }
    if (md->extern_table != NULL) {
        DestroySymbolTable(md->extern_table);
    }
    if (md->assembly_IR != NULL) {
        DestroyAssemblyIR(md->assembly_IR);
    }
    if (md->macro_table != NULL) {
        DestroyMacroTable(md->macro_table);
    }
    if (md->logger != NULL) {
        DestroyLogger(md->logger);
    }
    free(md->filename);
    free(md);
}

s_table_t *GetSymbolTable(as_metadata_t *md) {
    return md->symbol_table;
}

s_table_t *GetEntryTable(as_metadata_t *md) {
    return md->enrty_table;
}

s_table_t *GetExternTable(as_metadata_t *md) {
    return md->extern_table;
}

assembly_IR_t *GetAssemblyIR(as_metadata_t *md) {
    return md->assembly_IR;
}

macro_table_t *GetMacroTable(as_metadata_t *md) {
    return md->macro_table;
}

logger_t *GetLogger(as_metadata_t *md) {
    return md->logger;
}

const char *GetFilename(as_metadata_t *md) {
    return md->filename;
}



