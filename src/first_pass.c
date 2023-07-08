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

typedef enum line_type {
    LINE_TYPE_COMMENT           = 0,
    LINE_TYPE_EMPTY             = 1,
    LINE_TYPE_INSTRUCTION       = 2,
    LINE_TYPE_DATA              = 3,
    LINE_TYPE_MACRO_EXPRESSION  = 4,
    LINE_TYPE_MACRO_DEFENITION  = 5,
} line_type_t;

typedef int(*handler)(as_metadata_t *md, char *line);

/******************************************************************************
            This are all the possible line types and they're checkers
******************************************************************************/
static int isEmptyLine(char *line) {
    size_t length = strlen(line);

    for (size_t i = 0; i < length; i++) {
        if (!isspace((unsigned char)line[i])) {
            return 0;
        }
    }

    return 1;
}

static int isComment(char *line) {
    return line[0] == ';';
}

static int isMacroDefinition(char *line) {
    if (strstr(line, "mcro") == NULL) {
        return false;
    }
    return true;
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

static int isMacroExpression(as_metadata_t *md ,char *line) {
    char *lable = NULL;
    char *word2 = NULL;
    line = RemoveSpace(line);
    lable = strtok(line, " \n\t\v\r\f");
    word2 = strtok(NULL, " \n\t\v\r\f");
    if (word2 == NULL) {
        if (!MacroTableIterIsEqual(
                    MacroTableFindEntry(md->GetMacroTable(), lable)),
                    MacroTableGetLastEntry(md->GetMacroTable())) {
                        return 1;
                    }
    } 

    return 0;
}

static int isData(char *line) {
    if ((strstr(line, ".data")) != NULL || (strstr(line, ".string") != NULL)) {
        return 1;
    }
    return 0;
}

static GetLineType(as_metadata_t *md, char *line) {
    if (isEmptyLine(instruction)) {
        return LINE_TYPE_EMPTY;
    } else if (isComment(instruction)) {
        return LINE_TYPE_COMMENT;
    } else if (IsMacroDefenition(md, instruction)) {
        return LINE_TYPE_MACRO_DEFENITION;
    }  else if (IsMacroExpansion(instruction)) {
        return LINE_TYPE_MACRO_EXPRESSION;
    } else if (IsData(instruction)) {
        return LINE_TYPE_DATA;
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
    fgets(curr_line, MAX_INSTRUCTION_LENGTH, md->GetFile());
    ++*line_number;
    while (strstr(curr_line, "endmcro") == NULL) {
        fgets(curr_line, MAX_INSTRUCTION_LENGTH, md->GetFile()); 
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

static void FreeLines(char **lines, size_t size) {
    for (size_t i = 0; i < size; i++) {
        free(lines[i]);
    }
    free(lines);
}

static char **CopyLines(as_metadata_t *md, size_t *line_number, size_t *lines) {
    int START_AMOUNT = 2;
    char **lines_copy = (char **)malloc(sizeof(char *) * START_AMOUNT);
    char **temp = NULL;
    if (lines_copy == NULL) {
        return NULL;
    }
    char curr_line[80];
    fgets(curr_line, MAX_INSTRUCTION_LENGTH, md->GetFile());
    ++*line_number;
    for (size_t i = 0; strstr(curr_line, "endmcro") == NULL; i++) {
        if (i == START_AMOUNT) {
            START_AMOUNT *= 2; /* doubles the size each time like vectors*/
            temp = (char **)realloc(lines_copy, sizeof(char *) * START_AMOUNT);
            if (temp == NULL) {
                FreeLines(lines_copy, lines);
                return NULL;
            }
            lines_copy = temp;
        }
        lines_copy[i] = (char *)malloc(sizeof(char) * MAX_INSTRUCTION_LENGTH);
        if (lines_copy[i] == NULL) {
            FreeLines(lines_copy, lines);
            return NULL;
        }
        memcpy(lines_copy[i], curr_line, strlen(curr_line) + 1);
        ++*lines;
        fgets(lines_copy[i], MAX_INSTRUCTION_LENGTH, md->GetFile())
        ++*line_number; 
    }
    
    return lines_copy;
}

static int AddMacro(as_metadata_t *md, const char *lable , char **lines, 
                        size_t lines_size, size_t line_defined) {

    macro_status_t ret = 
        MacroTableAddEntry(md->GetMacroTable(), lable, lines, line_size, 
                                                                line_defined);
    if (ret == MACRO_ALREADY_EXISTS) {
        const char error[200];// MAX_INSTRUCTION_LENGTH + 120 (error message)
        macro_table_iter_t it = MacroTableFindEntry(md->GetMacroTable(), lable);
        size_t prev_defined = MacroTableGetEntryLineDefined(it);
        sprintf("[ERROR] : Macro %s already exists, previously defined in %lu", 
                                                    lable, prev_defined);                                       
        AddLog(md->GetLogger(), error, lable);

        return 0;
    } else if (ret == MACRO_NON_MEM) {
        return FS_NON_MEMORY;
    } 
    
    return FS_SUCCESS;
}

static char *GetLable(as_metadata_t *md, char *line) {
    const int MAX_LABLE_LENGTH = 31;
    char *lable = strtok(RemoveSpace(line), " \n\t\v\r\f");
    size_t len = strlen(lable);
    int is_label = true;
    if (lable[len - 1] == ':') {
        if (len > MAX_LABEL_LEN) {
            char error[200];/*  MAX_INSTRUCTION_LENGTH + 1 */
            AddLog(md->GetLogger(), "[ERROR] : Label too long", lable)
        }
    }
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
    size_t lines = 0;
    int ret = 0;
    if (IsOp(lines)) {
        AdDLog(md->GetLogger() , mt->GetFileName() ,
                        "[ERROR] : macro label cannot be named same as op", *line_number);
        GetToMacroEnd(md, line, line_number);
        return (FS_FAIL);
    }
    if (lable == NULL) {
        AddLog(md->GetLogger() , mt->GetFileName() ,
                                "[ERROR] : macro label is empty", *line_number);
        GetToMacroEnd(md, line, line_number);
        return (FS_FAIL);
    }
    /* inital value that represnt lines amount*/
    lines = CopyLines(md, line_number, &lines);
    if (lines == NULL) {
        return (FS_NON_MEMORY);
    } else if (lines == 0) {
        /* just a warning not an error */
        AddLog(md->GetLogger(), mt->GetFileName(),
                        "[WARNING] : macro definition is empty", *line_number);
        GetToMacroEnd(md, line, line_number);
    } 
    ret = AddMacro(md, lable, lines, *line_number, line_defined);
    FreeLines(lines, lines);

    return (ret);
}

int handleMacroExpension(as_metadata_t *md, char *line, size_t *line) {
    char *label = strtok(RemoveSpace(line), " \n\t\v\r\f");
    macro_table_iter_t it = MacroTableFindEntry(md->GetMacroTable(), label);
    size_t lines = MacroTableGetEntryNumberOfLines(it);
    char **lines_copy = MacroTableGetEntryLines(it);
    if (lines_copy == NULL) {
        return (FS_NON_MEMORY);
    }
    for (size_t i = 0; i < lines; i++) {
        line_type_t lt = GetLineType(instruction);
        if (lt == LINE_TYPE_INSTRUCTION || lt == LINE_TYPE_COMMENT) {
            AddLog(md->GetLogger(), mt->GetFileName(), 
            "[FATAL_ERROR] : macro expansion or defenition inside a macro expension is not allowed , ",
             MacroTableGetEntryLineDefined(it));
             
             FreeLines(lines_copy, lines);
             return (FS_FAIL);
        }
        handler[lt](md, lines_copy[i], line);
    }
    FreeLines(lines_copy, lines);
    
    return (FS_SUCCESS);
}

int handleData(as_metadata_t *md, char *line, size_t *line)
{
    char *label = GetLable(line);
    if (label != NULL) {
    
    }
}

/**************************************************************************
 *                          MAIN FUNCTION
**************************************************************************/


first_pass_statis_t firstPass(as_metadata_t *md) {
    char instruction[MAX_INSTRUCTION_LENGH];
    FILE *file;
    size_t line_number = 0;
    const handler[4] = {handleNothing, handleNothing, };
    line_type_t lt = 0;
    
    assert(md != NULL);

    file = GetFile(md);
    while (fgets(instruction, MAX_INSTRUCTION_LENGH, file)!= NULL) {
        lt = GetLineType(instruction);
        handler[lt](md, instruction, &line_number);
    }
}