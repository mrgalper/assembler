/*************************************************************************                 
*   Orginal Name : first_pass.h                                          *
*   Name: Mark Galperin                                                  *
*   Date : 7.7.23                                                        *
*   Info : This is the implemntation of the first pass                   *
*************************************************************************/

#include "first_pass.h" /* API */
#include "assembler_helper.h" /* macros */
#include "instruction_handler.h" /* InstructionHandler */

#include <string.h> /* isempty, strncpy */
#include <assert.h> /* assert */
#include <stdio.h> /* printf, File  */
#include <ctype.h> /* isspace */
#include <stdlib.h> /* free */


#define MAX_LABEL_LENGTH  31
typedef enum line_type {
    LINE_TYPE_COMMENT           = 0,
    LINE_TYPE_EMPTY             = 1,
    LINE_TYPE_INSTRUCTION       = 2,
    LINE_TYPE_DATA              = 3,
    LINE_TYPE_MACRO_EXPRESSION  = 4,
    LINE_TYPE_MACRO_DEFINITION  = 5,
    LINE_TYPE_ENTRY_LABEL       = 6,
    LINE_TYPE_EXTERN_LABEL      = 7,
    LINE_TYPE_AMOUNT            = 8
} line_type_t;

typedef enum {
op_undefined = -1,
op_mov = 0,
op_cmp = 1,
op_add = 2,
op_sub = 3,
op_not = 4,
op_clr = 5,
op_lea = 6,
op_inc = 7,
op_dec = 8,
op_jmp = 9,
op_bne = 10,
op_red = 11,
op_prn = 12,
op_jsr = 13,
op_rts = 14,
op_stop = 15,
opcode_amount = 16
}opcode_t;

typedef int(*handler)(as_metadata_t *md, char *line, size_t *line_length);

/******************************************************************************
            This are all the possible line types and they're checkers
******************************************************************************/
static int IsEmptyLine(char *line) {
    size_t length = strlen(line);

    for (size_t i = 0; i < length; i++) {
        if (!isspace((unsigned char)line[i])) {
            return 0;
        }
    }

    return 1;
}

static int IsComment(char *line) {
    return line[0] == ';';
}

static int IsMacroDefinition(char *line) {
    if (strstr(line, "mcro") == NULL) {
        return 0;
    }
    return 1;
}

static char *RemoveSpace(char *line) {
    while (isspace((unsigned char)*line)) {
        ++line;
    }

    return line;
}

static int IsMacroExpression(as_metadata_t *md ,char *line) {
    char *label = NULL;
    char *word2 = NULL;
    line = RemoveSpace(line);
    label = strtok(line, " \n\t\v\r\f");
    word2 = strtok(NULL, " \n\t\v\r\f");
    if (word2 == NULL) {
        if (!MacroTableIterIsEqual(
                    MacroTableFindEntry(GetMacroTable(md), label),
                    MacroTableGetLastEntry(GetMacroTable(md)))) {
                        return 1;
                    }
    } 

    return 0;
}

static int IsData(char *line) {
    if ((strstr(line, ".data")) != NULL || (strstr(line, ".string") != NULL)) {
        return 1;
    }
    return 0;
}

static int IsExternLabel(char *line) {
    if ((strstr(line, ".extern")) != NULL) {
        return 1;
    }
    return 0;
}

static int IsEntryLabel(char *line) {
    if ((strstr(line, ".entry")) != NULL) {
        return 1;
    }
    return 0;
}

static int GetLineType(as_metadata_t *md, char *instruction) {
    if (IsEmptyLine(instruction)) {
        return LINE_TYPE_EMPTY;
    } else if (IsComment(instruction)) {
        return LINE_TYPE_COMMENT;
    } else if (IsMacroDefinition(instruction)) {
        return LINE_TYPE_MACRO_DEFINITION;
    }  else if (IsMacroExpression(md, instruction)) {
        return LINE_TYPE_MACRO_EXPRESSION;
    } else if (IsData(instruction)) {
        return LINE_TYPE_DATA;
    } else if (IsEntryLabel(instruction)) {
        return LINE_TYPE_ENTRY_LABEL;
    } else if (IsExternLabel(instruction)) {
        return LINE_TYPE_EXTERN_LABEL;
    } else {
        return LINE_TYPE_INSTRUCTION; /* default */
    }
}
/******************************************************************************
            This are all the helpers functions for handles  
******************************************************************************/
/* function used to iterate forward */
static void GetToMacroEnd(as_metadata_t *md, size_t *line_number) {
    char curr_line[MAX_INSTRUCTION_LENGTH];
    fgets(curr_line, MAX_INSTRUCTION_LENGTH, GetFile(md));
    ++*line_number;
    while (strstr(curr_line, "endmcro") == NULL) {
        fgets(curr_line, MAX_INSTRUCTION_LENGTH, GetFile(md)); 
        ++*line_number;
    }
}

static opcode_t GetOp(char *op) {
    size_t len = strlen(op) + 1;
    if (strncmp("mov", op, len) == 0) {
        return op_mov;
    } else if (strncmp("ncmp", op, len) == 0) {
        return op_cmp;
    } else if (strncmp("add", op, len) == 0) {
        return op_add;
    } else if (strncmp("sub", op, len) == 0) {
        return op_sub;
    } else if (strncmp("not", op, len) == 0) {
        return op_not;
    } else if (strncmp("clr", op, len) == 0) {
        return op_clr;
    } else if (strncmp("lea", op, len) == 0) {
        return op_lea;
    } else if (strncmp("inc", op, len) == 0) {
        return op_inc;
    } else if (strncmp("dec", op, len) == 0) {
        return op_dec;
    } else if (strncmp("jmp", op, len) == 0) {
        return op_jmp;
    } else if (strncmp("bne", op, len) == 0) {
        return op_bne;
    } else if (strncmp("red", op, len) == 0) {
        return op_red;
    } else if (strncmp("prn", op, len) == 0) {
        return op_prn;
    } else if (strncmp("jsr", op, len) == 0) {
        return op_jsr;
    } else if (strncmp("rts", op, len) == 0) {
        return op_rts;
    } else if (strncmp("stop", op, len) == 0) {
        return op_stop;
    } 
    
    return op_undefined;
}

static void FreeLines(const char **lines, size_t size) {
    for (size_t i = 0; i < size; i++) {
        free((void *)lines[i]);
    }
    free((void *)lines);
}

static char **CopyLines(as_metadata_t *md, size_t *line_number, size_t *lines) {
    int START_AMOUNT = 2;
    char **lines_copy = (char **)malloc(sizeof(char *) * START_AMOUNT);
    char **temp = NULL;
    if (lines_copy == NULL) {
        return NULL;
    }
    char curr_line[MAX_INSTRUCTION_LENGTH];
    fgets(curr_line, MAX_INSTRUCTION_LENGTH, GetFile(md));
    ++*line_number;
    for (int i = 0; strstr(curr_line, "endmcro") == NULL && i < 5; i++) {
        if (i == START_AMOUNT) {
            START_AMOUNT *= 2; /* doubles the size each time like vectors*/
            temp = (char **)realloc(lines_copy, sizeof(char *) * START_AMOUNT);
            if (temp == NULL) {
                FreeLines((const char **)lines_copy, *lines);
                return NULL;
            }
            lines_copy = temp;
        }
        lines_copy[i] = (char *)malloc(sizeof(char) * MAX_INSTRUCTION_LENGTH);
        if (lines_copy[i] == NULL) {
            FreeLines((const char **)lines_copy, *lines);
            return NULL;
        }
        memcpy(lines_copy[i], curr_line, strlen(curr_line) + 1);
        ++*lines;
        fgets(curr_line, MAX_INSTRUCTION_LENGTH, GetFile(md));
        ++*line_number; 
    }
    
    return lines_copy;
}

static int AddMacro(as_metadata_t *md, const char *label , char **lines, 
                        size_t lines_size, size_t line_defined) {

    macro_status_t ret = 
        MacroTableAddEntry(GetMacroTable(md), (const char *)label, 
                                 (const char **)lines, lines_size, 
                                                    line_defined);
    if (ret == MACRO_ALREADY_EXISTS) {
        char error[200];// MAX_INSTRUCTION_LENGTH + 120 (error message)
        macro_table_iter_t it = MacroTableFindEntry(GetMacroTable(md), label);
        size_t prev_defined = MacroTableGetEntryLineDefined(it);
        snprintf(error, 200,
                "[ERROR] : Macro %s already exists, previously defined in %lu", 
                                                   label, prev_defined);                                       
        if (LG_SUCCESS != AddLog(GetLogger(md), GetFilename(md), error, 
                                                            line_defined)) {
            return FS_FAIL;
        }

        return 0;
    } else if (ret == MACRO_NO_MEM) {
        return FS_NO_MEMORY;
    } 
    
    return FS_SUCCESS;
}

static int IsLabel(char *line) {
    int is_valid = 1;
    for (size_t i = 0; line[i] && is_valid; i++) {
        if (!isupper(line[i]) && !islower(line[i])) {
            is_valid = 0;
        }
    }

    return (is_valid);
}


/* DEAD_BEEF used as return value to represent low memory */
static char *GetLabel(as_metadata_t *md, char *line, size_t *line_number) {
    char *label = strtok(RemoveSpace(line), " \n\t\v\r\f");
    size_t len = 0;
    if (label == NULL) {
        return NULL;
    }
    len = strlen(label);
    if (label[len - 1] == ':') {
        label[len - 1] = '\0'; /* remove the : as it is not part of the label */
        if (len > MAX_LABEL_LENGTH) {
            char error[114];/*  MAX_INSTRUCTION_LENGTH + 30 (error message) */
            snprintf(error, 114,"[ERROR] : Label too long :%s" , label);
            if (LG_SUCCESS != AddLog(GetLogger(md), GetFilename(md),
                         "[ERROR] : Label too long", *line_number)) {
                return DEAD_BEEF;
            }
            return NULL;
        } else {
            if (!IsLabel(label)) {
                if (LG_SUCCESS != AddLog(GetLogger(md), GetFilename(md),
                        "[ERROR] : Label can contain only upper and lower char.", 
                        *line_number)) {
                    return DEAD_BEEF;
                }
                return NULL;
            }
            return label;
        }
    }

    return (NULL);
}

static int ConvertIntToOp(as_metadata_t *md, int val, size_t *line_number) {
    char op[OP_SIZE];
    memset(op, (int)'0', OP_SIZE);
    if (val > MAX_INT || val < MIN_INT) {
        if (LG_SUCCESS!= AddLog(GetLogger(md), GetFilename(md), 
        "[WARNING] : Value out of range in .data", *line_number)) {
        val = (val > MAX_INT) ? MAX_INT : MIN_INT;
        }
    }
    /* -2 because the last one is \0 and the first one is singd bit */
    for (size_t i = 1; i < OP_SIZE - 2; ++i){
        op[OP_SIZE - i - 1 - 1] = (val & 1) ? '1' : '0';
        val >>= 1;
    }
    /* sign bit */
    op[0] = val >= 0 ? '0' : '1';
    op[OP_SIZE - 1] = '\0';

    if (A_IR_SUCCESS != AssemblyIRAddInstr(GetAssemblyIRData(md), op)) {
        return FS_NO_MEMORY;
    }
    SetDC(md, GetDC(md) + 1);
    SetPC(md, GetPC(md) + 1);

    return FS_SUCCESS;
}

static int IsVal(char *line, int *val) {
    char* end_ptr;
    long int lval = strtol(line, &end_ptr, 10);

    *val = lval > MAX_INT ? MAX_INT : lval < MIN_INT ? MIN_INT : lval;
    if (end_ptr == line || *end_ptr != '\0') {
        return 0;
    }
    if (*val == 0 && line[0] != '0' && line[1] != '\0') {
        return 0;
    }

    return 1;
}

static int AddDataStatment(as_metadata_t *md ,size_t *line_number, char *vals) {
    char *val_s = strtok(RemoveSpace(vals), ",");
    char *prev_val_s = NULL;
    int val = 0;
    if (val_s[0] == ',') {
        if (LG_SUCCESS != AddLog(GetLogger(md), GetFilename(md), 
        "[ERROR] : comma is not allowed on the first value of the .data statment", 
                                                            *line_number)) {
            return FS_NO_MEMORY;
        }
    }
    while (val_s != NULL) {
        val_s = RemoveSpace(val_s);
        if (!IsVal(val_s, &val)) {
            if (LG_SUCCESS != AddLog(GetLogger(md), GetFilename(md), 
            "[ERROR] : values of .data statment must be integer", 
                                                            *line_number)) {
                return FS_NO_MEMORY;
           }
        }
        if (FS_SUCCESS != ConvertIntToOp(md, val, line_number)) {
            return FS_NO_MEMORY;
        }
        prev_val_s = val_s;
        val_s = strtok(NULL, ",");
        if (val_s != NULL && ((val_s[0] != ',') || (val_s[0] == '\0' && RemoveSpace(val_s)[0] == ','))) {
            if (LG_SUCCESS!= AddLog(GetLogger(md), GetFilename(md), 
            "[ERROR] : A double comma or no comma is not allowed in the .data statment",
                                                           *line_number)) {
                return FS_NO_MEMORY;
            }
        } 
    }
    if (strtok(prev_val_s, ",") != NULL) {
        if (LG_SUCCESS!= AddLog(GetLogger(md), GetFilename(md), 
        "[ERROR] : A comma is not allowed in the end of  .data statment",
                                                           *line_number)) {
                return FS_NO_MEMORY;
        }
    }

    return (FS_SUCCESS);
}

static int AddStringStatement(as_metadata_t *md ,size_t *line_number) {
    char *str = strtok(NULL, " \n\t\v\r\f,");
    if (str == NULL) {
        if (LG_SUCCESS != AddLog(GetLogger(md), GetFilename(md), 
        "[ERROR] : An empty string is not allowed in the .string  statment", 
                                                            *line_number)) {
            return FS_NO_MEMORY;
        }
    }
    if (strtok(str, " \n\t\v\r\f,") != NULL) {
        if (LG_SUCCESS != AddLog(GetLogger(md), GetFilename(md), 
        "[ERROR] : Only one string is allowed in the .string statment", 
                                                            *line_number)) {
            return FS_NO_MEMORY;
        }
    }
    for (size_t i = 0; str[i] != '\0'; i++) {
        ConvertIntToOp(md, (int)str[i], line_number);
    }
    ConvertIntToOp(md, '\0', line_number);

    return FS_SUCCESS;
}

static int IsLabelExists(as_metadata_t *md, char *label, 
                                    size_t *line_number) {
    char str[114] = {0}; /* MAX_INSTRUCTION_LENGTH + MAX_LABEL_LENGTH + 1 */
    int line = SymbolTableLookup(GetSymbolTable(md), label); 
    if (line != -1) {
        snprintf(str, 114,
        "[ERROR] : double label definition of %s found, previously defined in line %d as label", 
        label, line);
        if (LG_SUCCESS != AddLog(GetLogger(md), GetFilename(md), str, 
                                                                *line_number)) {
            return FS_NO_MEMORY;
        }
        return FS_FAIL;
    }
    line = SymbolTableLookup(GetEntryTable(md), label); 
    if (line != -1) {
        snprintf(str, 114,
        "[ERROR] : double label definition of %s found, previously defined in line %d as entry", 
        label, line);
        if (LG_SUCCESS != AddLog(GetLogger(md), GetFilename(md), str, 
                                                                *line_number)) {
            return FS_NO_MEMORY;
        }
        return FS_FAIL;
    }
    line = SymbolTableLookup(GetExternTable(md), label); 
    if (line != -1) {
        snprintf(str, 114,
        "[ERROR] : double label definition of %s found, previously defined in line %d as extern", 
        label, line);
        if (LG_SUCCESS != AddLog(GetLogger(md), GetFilename(md), str, 
                                                                *line_number)) {
            return FS_NO_MEMORY;
        }
        return FS_FAIL;
    }

    return FS_SUCCESS;
}   

/******************************************************************************
            This are all the Handlers 
******************************************************************************/
static int HandleNothing(as_metadata_t *md, char *line, 
                                                        size_t *line_number) {
    
    return 0;
}/* in case of a empty line or comment */


static int HandleMacroDefinition(as_metadata_t *md, char *line, 
                                                        size_t *line_number) {
    char **lines;
    /* first strtok will return mcro second will return macro name*/
    char *macro_command = strtok(RemoveSpace(line), " \n\t\v\r\f");
    
    char *label = strtok(NULL, " \n\t\v\r\f\0");
    size_t lines_amount = 0;
    int ret = 0;
    if (label == NULL) {
        if (LG_SUCCESS != AddLog(GetLogger(md) , GetFilename(md) ,
                          "[ERROR] : macro label is empty", *line_number)) {
            return FS_NO_MEMORY;
        }
        GetToMacroEnd(md, line_number);
        return (FS_FAIL);
    }
    if (GetOp(label) != op_undefined) {
        if (LG_SUCCESS != AddLog(GetLogger(md) , GetFilename(md) ,
            "[ERROR] : macro label cannot be named same as op", *line_number))
        {
            return FS_NO_MEMORY;
        }
        GetToMacroEnd(md, line_number);
        return (FS_FAIL);
    }
    /* inital value that represnt lines amount*/
    lines = CopyLines(md, line_number, &lines_amount);
    if (lines == NULL) {
        return (FS_NO_MEMORY);
    } else if (lines == 0) {
        /* just a warning not an error */
        if (LG_SUCCESS != AddLog(GetLogger(md), GetFilename(md),
                    "[WARNING] : macro definition is empty", *line_number)) {
            return FS_NO_MEMORY;
        }
        GetToMacroEnd(md, line_number);
    } 
    ret = AddMacro(md, label, lines, lines_amount, *line_number);
    FreeLines((const char **)lines, lines_amount);

    return (ret);
}

static int HandleData(as_metadata_t *md, char *line, size_t *line_number)
{
    char copy_line[MAX_INSTRUCTION_LENGTH];
    char *label = GetLabel(md ,line, line_number);
    size_t len = 0;
    size_t pc = GetPC(md);
    int ret = 0;
    if (label == DEAD_BEEF) {
        return (FS_NO_MEMORY);
    }
    if (label != NULL) {
        ret = IsLabelExists(md, label, line_number);
        if (ret != FS_SUCCESS) {
            return ret;
        } 
        if (ST_SUCCESS != SymbolTableInsert(GetSymbolTable(md), 
                                        (const char *)label, pc)) {
            return FS_NO_MEMORY;
        }  
        label = strtok(NULL, " \n\t\v\r\f");
    }
    strncpy(copy_line, line, MAX_INSTRUCTION_LENGTH);
    if (strncmp(label, ".data", 6)) {
        int offset = (size_t)label - (size_t)line + 6;/* 6 = .data + 1*/
        ret = AddDataStatment(md, line_number, copy_line + offset); 
    } else {
        ret = AddStringStatement(md, line_number);
    }
    return (ret);
}

static int HandleExternLabel(as_metadata_t *md, char *line, 
                                                size_t *line_number) {
    char *label = NULL;
    int ret = 0;
    /* ths intial use of strtok is done inside get label*/
    if (NULL != GetLabel(md ,line, line_number)) {
        if (LG_SUCCESS != AddLog(GetLogger(md), GetFilename(md),
            "[WARNING] : label defintion before extern is meaningless", 
            *line_number)) {
            
            return (FS_NO_MEMORY);
        }
        /* if there was a label then we want to pass it and pass the .entry */
        strtok(NULL, " \n\t\v\r\f"); 
    }
    label = strtok(NULL , " \n\t\v\r\f");
    if (label == NULL) {
        if (LG_SUCCESS != AddLog(GetLogger(md), GetFilename(md),
            "[ERROR] : .extern label cannot be empty.", 
            *line_number)) {
            
            return (FS_NO_MEMORY);
        }
    } else if (strtok(NULL , " \n\t\v\r\f") != NULL) {
        if (LG_SUCCESS != AddLog(GetLogger(md), GetFilename(md),
            "[ERROR] : .extern label cannot contain more then 1 label", 
            *line_number)) {
            
            return FS_NO_MEMORY;
        }

    }
    ret = IsLabelExists(md, label, line_number);
    if (ret != FS_SUCCESS) {
        return ret;
    }
    if (ST_SUCCESS != SymbolTableInsert(GetExternTable(md), (const char *)label,    
                                                                  GetPC(md))) {
        return FS_NO_MEMORY;
    } 
    return ST_SUCCESS;
}

static int HandleEntryLabel(as_metadata_t *md, char *line, 
                                                size_t *line_number) {
    char *label = NULL;
    int ret = 0;
    /* ths intial use of strtok is done inside get label*/
    if (NULL != GetLabel(md ,line, line_number)) {
        if (LG_SUCCESS != AddLog(GetLogger(md), GetFilename(md),
            "[WARNING] : label defintion before .entry is meaningless", 
            *line_number)) {
            
            return (FS_NO_MEMORY);
        }
        /* if there was a label then we want to pass it and pass the .entry */
        strtok(NULL, " \n\t\v\r\f"); 
    }
    label = strtok(NULL , " \n\t\v\r\f");
    if (label == NULL) {
        if (LG_SUCCESS != AddLog(GetLogger(md), GetFilename(md),
            "[ERROR] : .entry label cannot be empty.", 
            *line_number)) {
            
            return (FS_NO_MEMORY);
        }
    } else if (strtok(NULL , " \n\t\v\r\f") != NULL) {
        if (LG_SUCCESS != AddLog(GetLogger(md), GetFilename(md),
            "[ERROR] : .entry label cannot contain more then 1 label", 
            *line_number)) {
            
            return FS_NO_MEMORY;
        }

    }
    ret = IsLabelExists(md, label, line_number);
    if (ret != FS_SUCCESS) {
        return ret;
    }
    if (ST_SUCCESS != SymbolTableInsert(GetEntryTable(md), (const char *)label,    
                                                                  GetPC(md))) {
        return FS_NO_MEMORY;
    } 
    return ST_SUCCESS;
}


static int HandleMacroExpension(as_metadata_t *md, char *line, size_t *line_number) {
    char *label = strtok(RemoveSpace(line), " \n\t\v\r\f");
    macro_table_iter_t it = MacroTableFindEntry(GetMacroTable(md), label);
    size_t lines = MacroTableGetEntryNumberOfLines(it);
    const char **macro_lines = MacroTableGetEntryLines(it); 
    handler handlers[LINE_TYPE_AMOUNT] = 
                                      {HandleNothing , 
                                       HandleNothing ,
                                       HandleInstruction,
                                       HandleData,
                                       HandleMacroExpension,
                                       HandleMacroDefinition,
                                       HandleEntryLabel,
                                       HandleExternLabel };
    for (size_t i = 0; i < lines; i++) {
        char line_copy[MAX_INSTRUCTION_LENGTH];
        strncpy(line_copy, macro_lines[i], MAX_INSTRUCTION_LENGTH);
        line_type_t lt = GetLineType(md, line);
        if (lt == LINE_TYPE_INSTRUCTION || lt == LINE_TYPE_COMMENT) {
            if (LG_SUCCESS != AddLog(GetLogger(md), GetFilename(md), 
                "[FATAL_ERROR] : macro expansion or defenition inside a macro expension is not allowed , ",
                MacroTableGetEntryLineDefined(it))) {
                return FS_FAIL;
            }
             
             return (FS_FAIL);
        }
        handlers[lt](md, line_copy, line_number);
    }
    
    return (FS_SUCCESS);
}
/**************************************************************************
 *                          MAIN FUNCTION
**************************************************************************/

first_pass_status_t firstPass(as_metadata_t *md) {
    char instruction[MAX_INSTRUCTION_LENGTH] = {0};
    char instr_cp[MAX_INSTRUCTION_LENGTH] = {0};
    FILE *file;
    size_t line_number = 0;
    handler handlers[LINE_TYPE_AMOUNT] = 
                                      {HandleNothing , 
                                       HandleNothing ,
                                       HandleInstruction,
                                       HandleData,
                                       HandleMacroExpension,
                                       HandleMacroDefinition,
                                       HandleEntryLabel,
                                       HandleExternLabel };
    line_type_t lt = 0;
    
    assert(md != NULL);
    file = GetFile(md);
    first_pass_status_t ret = FS_SUCCESS;
    while ((fgets(instruction, MAX_INSTRUCTION_LENGTH, file) != NULL) && 
                                                         ret != FS_NO_MEMORY) { 
        /* since we use strtok in the GetLineType we ruin the instruction
           so we pass a acopy of the orginal before it ruined */
        strncpy(instr_cp, instruction, MAX_INSTRUCTION_LENGTH);
        lt = GetLineType(md, instruction);
        if (lt != LINE_TYPE_MACRO_DEFINITION) {
            line_number += 1;
        }
        ret = handlers[lt](md, instr_cp, &line_number);
    }

    return (ret);
}