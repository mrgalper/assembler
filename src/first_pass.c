/*************************************************************************                 
*   Orginal Name : first_pass.h                                          *
*   Name: Mark Galperin                                                  *
*   Date : 7.7.23                                                        *
*   Info : This is the implemntation of the first pass                   *
*************************************************************************/

#include "first_pass.h" /* API */
#include "assembler_helper.h" /* macros */

#include <string.h> /* isempty */
#include <assert.h> /* assert */
#include <stdio.h> /* printf, File  */
#include <ctype.h> /* isspace */
#include <stdlib.h> /* free */

typedef enum line_type {
    LINE_TYPE_COMMENT           = 0,
    LINE_TYPE_EMPTY             = 1,
    LINE_TYPE_INSTRUCTION       = 2,
    LINE_TYPE_DATA              = 3,
    LINE_TYPE_MACRO_EXPRESSION  = 4,
    LINE_TYPE_MACRO_DEFENITION  = 5,
    LINE_TYPE_ENTRY_LABLE       = 6,
    LINE_TYPE_EXTERN_LABLE      = 7,
    LINE_TYPE_AMOUNT            = 8
} line_type_t;

typedef int(*handler)(as_metadata_t *md, char *line);

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
    size_t length = strlen(line);

    for (size_t i = 0; i < length; i++) {
        if (isspace((unsigned char)line[i])) {
            line[i] = '\0';
        }
    }

    return line;
}

static int IsMacroExpression(as_metadata_t *md ,char *line) {
    char *lable = NULL;
    char *word2 = NULL;
    line = RemoveSpace(line);
    lable = strtok(line, " \n\t\v\r\f");
    word2 = strtok(NULL, " \n\t\v\r\f");
    if (word2 == NULL) {
        if (!MacroTableIterIsEqual(
                    MacroTableFindEntry(GetMacroTable(md), lable),
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

static int IsExternLable(char *line) {
    if ((strstr(line, ".extern")) != NULL) {
        return 1;
    }
    return 0;
}

static int IsEntryLable(char *line) {
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
        return LINE_TYPE_MACRO_DEFENITION;
    }  else if (IsMacroExpression(md, instruction)) {
        return LINE_TYPE_MACRO_EXPRESSION;
    } else if (IsData(instruction)) {
        return LINE_TYPE_DATA;
    } else if (IsEntryLable(instruction)) {
        return LINE_TYPE_ENTRY_LABLE;
    } else if (IsExternLable(instruction)) {
        return LINE_TYPE_EXTERN_LABLE;
    } else {
        return LINE_TYPE_INSTRUCTION; /* default */
    }
}
/******************************************************************************
            This are all the helpers functions for handles  
******************************************************************************/

/* function used to iterate forward */
static void GetToMacroEnd(as_metadata_t *md, size_t *line_number) {
    char curr_line[80];
    fgets(curr_line, MAX_INSTRUCTION_LENGTH, GetFile(md));
    ++*line_number;
    while (strstr(curr_line, "endmcro") == NULL) {
        fgets(curr_line, MAX_INSTRUCTION_LENGTH, GetFile(md)); 
        ++*line_number;
    }
}

static int IsOp(char *op) {
    size_t len = strlen(op) + 1;
    if (strcmp("mov", op) == 0) {
        return (0);
    } else if (strcmp("cmp", op) == 0) {
        return 1;
    } else if (strcmp("add", op) == 0) {
        return 2;
    } else if (strcmp("sub", op) == 0) {
        return 3;
    } else if (strcmp("not", op) == 0) {
        return 4;
    } else if (strcmp("clr", op) == 0) {
        return 5;
    } else if (strcmp("lea", op) == 0) {
        return 6;
    } else if (strcmp("inc", op) == 0) {
        return 7;
    } else if (strcmp("dec", op) == 0) {
        return 8;
    } else if (strcmp("jmp", op) == 0) {
        return 9;
    } else if (strcmp("bne", op) == 0) {
        return 10;
    } else if (strcmp("red", op) == 0) {
        return 11;
    } else if (strcmp("prn", op) == 0) {
        return 12;
    } else if (strcmp("jsr", op) == 0) {
        return 13;
    } else if (strcmp("rts", op) == 0) {
        return 14;
    } else if (strcmp("stop", op) == 0) {
        return 15;
    } 
    
    return -1;
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
    char curr_line[80];
    fgets(curr_line, MAX_INSTRUCTION_LENGTH, GetFile(md));
    ++*line_number;
    for (size_t i = 0; strstr(curr_line, "endmcro") == NULL; i++) {
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
        fgets(lines_copy[i], MAX_INSTRUCTION_LENGTH, GetFile(md));
        ++*line_number; 
    }
    
    return lines_copy;
}

static int AddMacro(as_metadata_t *md, const char *lable , char **lines, 
                        size_t lines_size, size_t line_defined) {

    macro_status_t ret = 
        MacroTableAddEntry(GetMacroTable(md), (const char *)lable, 
                                 (const char **)lines, lines_size, 
                                                    line_defined);
    if (ret == MACRO_ALREADY_EXISTS) {
        const char error[200];// MAX_INSTRUCTION_LENGTH + 120 (error message)
        macro_table_iter_t it = MacroTableFindEntry(GetMacroTable(md), lable);
        size_t prev_defined = MacroTableGetEntryLineDefined(it);
        sprintf("[ERROR] : Macro %s already exists, previously defined in %lu", 
                                                    lable, prev_defined);                                       
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
/* DEAD_BEEF used as return value to represent low memory */
static char *GetLable(as_metadata_t *md, char *line, size_t *line_number) {
    const int MAX_LABLE_LENGTH = 31;
    char *lable = strtok(RemoveSpace(line), " \n\t\v\r\f");
    size_t len = strlen(lable);
    size_t pc = 0;
    if (lable[len - 1] == ':') {
        if (len > MAX_LABLE_LENGTH) {
            char error[114];/*  MAX_INSTRUCTION_LENGTH + 30 (error message) */
            sprintf(error, "[ERROR] : Label too long :%s" , lable);
            if (LG_SUCCESS != AddLog(GetLogger(md), GetFilename(md),
                         "[ERROR] : Label too long", *line_number)) {
                return DEAD_BEEF;
            }
            return NULL;
        } else {
            int is_valid = 1;
            for (size_t i = 0; i < lable[i] && is_valid; i++) {
                if (!isupper(lable[i]) && !islower(lable[i])) {
                    is_valid = 0;
                }
            }
            if (!is_valid) {
                if (LG_SUCCESS != AddLog(GetLogger(md), GetFilename(md),
                        "[ERROR] : Label can contain only upper and lower char.", 
                        *line_number)) {
                    return DEAD_BEEF;
                }
                return NULL;
            }
            return lable;
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

static int AddDataStatment(as_metadata_t *md ,size_t *line_number) {
    char *val_s = strtok(NULL, " \n\t\v\r\f");    
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
        val = atoi(val_s);
        /* atoi return 0 on fail so we need to check that the actual number 
           is not 0*/
        if (atoi == 0 && strncmp(val_s, "0", 2) != 0) {
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
        val_s = strtok(NULL, " \n\t\v\r\f");
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
    char *str = strtok(NULL, " \n\t\v\r\f");
    if (str == NULL) {
        if (LG_SUCCESS != AddLog(GetLogger(md), GetFilename(md), 
        "[ERROR] : An empty string is not allowed in the .string  statment", 
                                                            *line_number)) {
            return FS_NO_MEMORY;
        }
    }
    if (strtok(str, " \n\t\v\r\f")b!= NULL) {
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

}

static int IsLabelExists(as_metadata_t *md, const char *lable, 
                                    size_t *line_number) {
    char str[114] = {0}; /* MAX_INSTRUCTION_LENGTH + MAX_LABLE_LENGTH + 1 */
    int line = SymbolTableLookup(GetSymbolTable(md), lable); 
    if (line != -1) {
        sprintf(str, 
        "[ERROR] : double lable definition of %s found, previously defined in line %d as lable", 
        lable, line);
        if (LG_SUCCESS != AddLog(GetLogger(md), GetFilename(md), str, 
                                                                *line_number)) {
            return FS_NO_MEMORY;
        }
        return FS_FAIL;
    }
    line = SymbolTableLookup(GetEntryTable(md), lable); 
    if (line != -1) {
        sprintf(str, 
        "[ERROR] : double lable definition of %s found, previously defined in line %d as entry", 
        lable, line);
        if (LG_SUCCESS != AddLog(GetLogger(md), GetFilename(md), str, 
                                                                *line_number)) {
            return FS_NO_MEMORY;
        }
        return FS_FAIL;
    }
    line = SymbolTableLookup(GetExternTable(md), lable); 
    if (line != -1) {
        sprintf(str, 
        "[ERROR] : double lable definition of %s found, previously defined in line %d as extern", 
        lable, line);
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
static int handleNothing(as_metadata_t *md, char *line, 
                                                        size_t *line_number) {
    ++*line_number;
    
    return 0;
}/* in case of a empty line or comment */


static int handleMacroDefinition(as_metadata_t *md, char *line, 
                                                        size_t *line_number) {
    char **lines;
    /* first strtok will return mcro second will return macro name*/
    size_t line_defined = *line_number;
    char *macro_command = strtok(RemoveSpace(line), " \n\t\v\r\f");
    char *lable = strtok(NULL, " \n\t\v\r\f");
    size_t lines_amount = 0;
    int ret = 0;
    if (IsOp(lable)) {
        if (LG_SUCCESS != AddLog(GetLogger(md) , GetFilename(md) ,
            "[ERROR] : macro label cannot be named same as op", *line_number))
        {
            return FS_NO_MEMORY;
        }
        GetToMacroEnd(md, line_number);
        return (FS_FAIL);
    }
    if (lable == NULL) {
        if (LG_SUCCESS != AddLog(GetLogger(md) , GetFilename(md) ,
                          "[ERROR] : macro label is empty", *line_number)) {
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
    ret = AddMacro(md, lable, lines, lines_amount, line_defined);
    FreeLines((const char **)lines, lines_amount);

    return (ret);
}

static int handleMacroExpension(as_metadata_t *md, char *line, size_t *line_number) {
    char *label = strtok(RemoveSpace(line), " \n\t\v\r\f");
    macro_table_iter_t it = MacroTableFindEntry(GetMacroTable(md), label);
    size_t lines = MacroTableGetEntryNumberOfLines(it);
    const char **lines_copy = MacroTableGetEntryLines(it);
    if (lines_copy == NULL) {
        return (FS_NO_MEMORY);
    }
    for (size_t i = 0; i < lines; i++) {
        line_type_t lt = GetLineType(md, line);
        if (lt == LINE_TYPE_INSTRUCTION || lt == LINE_TYPE_COMMENT) {
            if (LG_SUCCESS != AddLog(GetLogger(md), GetFilename(md), 
                "[FATAL_ERROR] : macro expansion or defenition inside a macro expension is not allowed , ",
                MacroTableGetEntryLineDefined(it))) {
                return FS_FAIL;
            }
             
             FreeLines(lines_copy, lines);
             return (FS_FAIL);
        }
        handler[lt](md, lines_copy[i], line_number);
    }
    FreeLines(lines_copy, lines);
    
    return (FS_SUCCESS);
}

static int handleData(as_metadata_t *md, char *line, size_t *line_number)
{
    char *lable = GetLable(md ,line, line_number);
    size_t len = 0;
    size_t pc = GetPC(md);
    int ret = 0;
    if (lable == DEAD_BEEF) {
        return (FS_NO_MEMORY);
    }
    if (lable != NULL) {
        ret = IsLabelExists(md, lable, line_number);
        if (ret != FS_SUCCESS) {
            return ret;
        } 
        if (ST_SUCCESS != SymbolTableInsert(GetSymbolTable(md), 
                                        (const char *)lable, pc)) {
            return FS_NO_MEMORY;
        }  
        lable = strtok(NULL, " \n\t\v\r\f");
    }
    if (strncmp(lable, ".data", 6)) {
        ret = AddDataStatment(md, line_number); 
    } else {
        ret = AddStringStatement(md, line_number);
    }
    ++*line_number;
    return (ret);
}

static int handleExternLable(as_metadata_t *md, char *line, 
                                                size_t *line_number) {
    char *lable = NULL;
    int ret = 0;
    /* ths intial use of strstr is done inside get lable*/
    if (NULL != GetLable(md ,line, line_number)) {
        if (LG_SUCCESS != AddLog(GetLogger(md), GetFilename(md),
            "[WARNING] : lable defintion before extern is meaningless", 
            *line_number)) {
            
            return (FS_NO_MEMORY);
        }
        /* if there was a label then we want to pass it and pass the .entry */
        strstr(NULL, " \n\t\v\r\f"); 
    }
    lable = strstr(NULL , " \n\t\v\r\f");
    if (lable == NULL) {
        if (LG_SUCCESS != AddLog(GetLogger(md), GetFilename(md),
            "[ERROR] : .extern lable cannot be empty.", 
            *line_number)) {
            
            return (FS_NO_MEMORY);
        }
    } else if (strstr(NULL , " \n\t\v\r\f") != NULL) {
        if (LG_SUCCESS != AddLog(GetLogger(md), GetFilename(md),
            "[ERROR] : .extern lable cannot contain more then 1 lable", 
            *line_number)) {
            
            return FS_NO_MEMORY;
        }

    }
    ret = IsLabelExists(md, lable, line_number);
    if (ret != FS_SUCCESS) {
        return ret;
    }
    if (ST_SUCCESS != SymbolTableInsert(GetExternTable(md), (const char *)lable,    
                                                                  GetPC(md))) {
        return FS_NO_MEMORY;
    } 
    ++*line_number;
    return ST_SUCCESS;
}

static int handleEntryLable(as_metadata_t *md, char *line, 
                                                size_t *line_number) {
    char *lable = NULL;
    int ret = 0;
    /* ths intial use of strstr is done inside get lable*/
    if (NULL != GetLable(md ,line, line_number)) {
        if (LG_SUCCESS != AddLog(GetLogger(md), GetFilename(md),
            "[WARNING] : lable defintion before .entry is meaningless", 
            *line_number)) {
            
            return (FS_NO_MEMORY);
        }
        /* if there was a label then we want to pass it and pass the .entry */
        strstr(NULL, " \n\t\v\r\f"); 
    }
    lable = strstr(NULL , " \n\t\v\r\f");
    if (lable == NULL) {
        if (LG_SUCCESS != AddLog(GetLogger(md), GetFilename(md),
            "[ERROR] : .entry lable cannot be empty.", 
            *line_number)) {
            
            return (FS_NO_MEMORY);
        }
    } else if (strstr(NULL , " \n\t\v\r\f") != NULL) {
        if (LG_SUCCESS != AddLog(GetLogger(md), GetFilename(md),
            "[ERROR] : .entry lable cannot contain more then 1 lable", 
            *line_number)) {
            
            return FS_NO_MEMORY;
        }

    }
    ret = IsLabelExists(md, lable, line_number);
    if (ret != FS_SUCCESS) {
        return ret;
    }
    if (ST_SUCCESS != SymbolTableInsert(GetEntryTable(md), (const char *)lable,    
                                                                  GetPC(md))) {
        return FS_NO_MEMORY;
    } 
    ++*line_number;
    return ST_SUCCESS;
}

static int handleInstruction(as_metadata_t *md, char *line, 
                                                size_t *line_number) {
    
    char *lable = GetLable(md ,line, line_number);
    
}
/**************************************************************************
 *                          MAIN FUNCTION
**************************************************************************/


first_pass_status_t firstPass(as_metadata_t *md) {
    char instruction[MAX_INSTRUCTION_LENGTH];
    FILE *file;
    size_t line_number = 0;
    const handler[4] = {handleNothing, handleNothing, };
    line_type_t lt = 0;
    
    assert(md != NULL);

    file = GetFile(md);
    while (fgets(instruction, MAX_INSTRUCTION_LENGTH, file)!= NULL) {
        lt = GetLineType(instruction);
        handler[lt](md, instruction, &line_number);
    }
}