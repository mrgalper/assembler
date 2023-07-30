/******************************************************************************
*   Orginal Name : assembler_metadata.c                                       *
*   Name: Ido Sabach                                                          *
*   Date 23.6.23                                                              *
*   Info : This file contains all ds that used in the assembler               *
******************************************************************************/
#include "assembler_metadata.h" /* API*/

#include <stdlib.h> /* malloc, free */
#include <assert.h> /* assert */
#include <string.h> /* memset */
#include <stdio.h> /* printf */

static size_t default_pc_val = 100;

struct assembler_metadata_t {
    s_table_t *symbol_table;
    s_table_t *entry_table;
    s_table_t *extern_table;

    /* this ds will hold the use of a label in a ir and line of use(PC) */
    s_table_t *entry_output;
    s_table_t *extern_output;
    /* since we store the lables in the tables we can split the data and 
    instruction into 2 piece so when we want to convert into file it will be 
    easier and some operation can multithreaded like convert to base 64*/
    assembly_IR_t *assembly_IR_instruction;
    assembly_IR_t *assembly_IR_data;

    
    macro_table_t *macro_table;
    logger_t *logger;
    logger_t *warning_logger;
    char *filename;
    FILE* file;
    size_t IC; /* instruction counter */
    size_t DC; /* data counter */
    size_t PC; /* program counter */
};

as_metadata_t *CreateAssemblerMetadata(const char *filename) {
    as_metadata_t *md;
    size_t len = 0;
    
    assert(filename != NULL);

    md = calloc(1, sizeof(as_metadata_t)); /* allocate and memset*/
    if (md == NULL) {
        return NULL;
    }
    len = strlen(filename) + 1;
    md->filename = malloc(len + 1 + 3/* suffix .as*/);
    if (md->filename == NULL) {
        return NULL;
    }
    strncpy(md->filename, filename, len);
    strcat(md->filename, ".as");
    md->file = fopen(md->filename, "r");
    if (md->file == NULL) {
        free(md->filename);
        free(md);
        return NULL;
    }


    md->symbol_table = CreateSymbolTable();
    md->entry_table  = CreateSymbolTable();
    md->extern_table = CreateSymbolTable();
    md->entry_output = CreateSymbolTable();
    md->extern_output = CreateSymbolTable();
    md->assembly_IR_instruction  = CreateAssemblyIR();
    md->assembly_IR_data  = CreateAssemblyIR();
    md->macro_table  = CreateMacroTable();
    md->logger       = CreateLogger();
    md->warning_logger       = CreateLogger();
    if (md->symbol_table == NULL || md->entry_table == NULL || 
        md->extern_table == NULL || md->assembly_IR_data == NULL ||
        md->assembly_IR_instruction == NULL ||
        md->macro_table == NULL || 
        md->logger == NULL || md->warning_logger == NULL) {
        DestroyAssemblerMetadata(md);
        fclose(md->file);
        free(md->filename);
        free(md);

        return NULL;
    }
    md->IC = 0;
    md->DC = 0;
    md->PC = default_pc_val; /* default start PC counter */

    return md;    
}

void DestroyAssemblerMetadata(as_metadata_t *md) {
    if (md->symbol_table != NULL) {
        DestroySymbolTable(md->symbol_table); 
    }
    if (md->entry_table != NULL) {
        DestroySymbolTable(md->entry_table);
    }
    if (md->extern_table != NULL) {
        DestroySymbolTable(md->extern_table);
    }
    if (md->extern_output != NULL) {
        DestroySymbolTable(md->extern_output);
    }
    if (md->entry_output != NULL) {
        DestroySymbolTable(md->entry_output);
    }
    if (md->assembly_IR_data != NULL) {
        DestroyAssemblyIR(md->assembly_IR_data);
    }
    if (md->assembly_IR_instruction != NULL) {
        DestroyAssemblyIR(md->assembly_IR_instruction);
    }
    if (md->macro_table != NULL) {
        DestroyMacroTable(md->macro_table);
    }
    if (md->logger != NULL) {
        DestroyLogger(md->logger);
    }
    if (md->warning_logger != NULL) {
        DestroyLogger(md->warning_logger);
    }
    fclose(md->file);
    free(md->filename);
    free(md);
}

s_table_t *GetSymbolTable(as_metadata_t *md) {
    return md->symbol_table;
}

s_table_t *GetEntryTable(as_metadata_t *md) {
    return md->entry_table;
}

s_table_t *GetExternTable(as_metadata_t *md) {
    return md->extern_table;
}


s_table_t *GetEntryOutput(as_metadata_t *md) {
    return md->entry_output;
}

s_table_t *GetExternOutput(as_metadata_t *md) {
    return md->extern_output;
}

assembly_IR_t *GetAssemblyIRData(as_metadata_t *md) {
    return md->assembly_IR_data;
}

assembly_IR_t *GetAssemblyIRInst(as_metadata_t *md) {
    return md->assembly_IR_instruction;
}

macro_table_t *GetMacroTable(as_metadata_t *md) {
    return md->macro_table;
}

logger_t *GetLogger(as_metadata_t *md) {
    return md->logger;
}

logger_t *GetWarningLogger(as_metadata_t *md) {
    return md->warning_logger;
}

const char *GetFilename(as_metadata_t *md) {
    return md->filename;
}

FILE *GetFile(as_metadata_t *md) {
    return md->file;
}

size_t GetIC(as_metadata_t *md) {
    return md->IC;
}

size_t GetDC(as_metadata_t *md) {
    return md->DC;
}

size_t GetPC(as_metadata_t *md) {
    return md->PC;
}

void SetPC(as_metadata_t *md, size_t pc) {
    md->PC = pc;
}

void SetIC(as_metadata_t *md, size_t ic) {
    md->IC = ic;
}

void SetDC(as_metadata_t *md, size_t dc) {
    md->DC = dc;
}

size_t GetDefautPcValue(void) {
    return default_pc_val;
}

void SetDefautPcValue(size_t new_val) {
    default_pc_val = new_val;
}

