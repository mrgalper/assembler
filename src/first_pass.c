/*************************************************************************                 
*   Orginal Name : first_pass.h                                          *
*   Name: Mark Galperin                                                  *
*   Date : 7.7.23                                                        *
*   Info : This is the implemntation of the first pass                   *
*************************************************************************/

#include "first_pass.h" /* API */
#include "assembler_helper.h" /* macros */

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

typedef int(*handler)(as_metadata_t *md, char *line, size_t *line_length);

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

typedef enum operand_type{
    OP_NOT_EXIST = -1,
    OP_IMMIDIATE = 0,
    OP_DIRECT    = 1,
    OP_REGISTER  = 2
}operand_type_t;

typedef enum op_scope{
    OP_ABSOLUTE     = 0,
    OP_EXTERNAL     = 1,
    OP_RELOCATABLE  = 2
}op_scope_t;

typedef enum op_type{
    undefined_register = -1,
    r0 = 0,
    r1 = 1,
    r2 = 2,
    r3 = 3,
    r4 = 4,
    r5 = 5,
    r6 = 6,
    r7 = 7,
    register_amount = 8
}register_type_t;

typedef union  {
    char *label;
    register_type_t reg;
    int constant; 
}operand_val_t;

typedef enum operands{
    OP_SRC = 0,
    OP_DEST = 1,
    OP_TOTAL = 2
}operands_t;

typedef struct op {
    char opcode[OP_SIZE];
    /* in case there is operands*/
    char op_first[OP_SIZE]; 
    char op_second[OP_SIZE];
    char *op_bin;
    opcode_t op_type;
    op_scope_t ARE;
    operand_type_t operand[OP_TOTAL];
    operand_val_t operand_val[OP_TOTAL];
    int8_t has_label : 2;
    int8_t has_src : 2;
    int8_t has_dest : 2;
}op_t; 

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
/*                    src(if 1 op dest)     op       dest     ARE  */
/* op code is the follow 11 - 10 - 9  8 - 7 - 6 - 5  4 - 3 - 2  1 - 0*/
static void FillOpBin(op_t *op) {
    static int op_code_size = sizeof(char) * 4;
    static char *op_bin_str[opcode_amount] = 
        {"0000", "0001", "0010", "0011", "0100", "0101", "0110", "0111", 
        "1000", "1001", "1010",  "1011", "1100", "1101", "1110", "1111"};
    memcpy(op->opcode + 3, op_bin_str[op->op_type], op_code_size);
}
/* since if we have 1 operand the src is the dest so we can do a manipulation 
   here.
   op_store is the op the data saved
   op_actual - is where we actualy want to store the result*/
static void FillOperandBin(op_t *op, operands_t op_store, operands_t op_actual) {
    static int offset[OP_TOTAL] = {0/* src */, 7/* dest*/};
    static int operand_size = sizeof(char) * 3;
    static char *op_bin_str[3] = 
                {"001"/*IMMIDIATE */, "011" /*DIRECT */, "101" /*REGISTER*/};
    memcpy(op->opcode + offset[op_actual], op_bin_str[op->operand[op_store]], 
                                                                operand_size);
}

static void FillOperandARE(char *cmd, op_scope_t scope) {
    static int operand_size = sizeof(char) * 2;
    static char *op_bin_str[3] = 
                {"00"/*ABSOLUTE*/, "10" /*RELOCATABLE*/, "01" /*EXTERNAL*/};
    memcpy(cmd + 10, op_bin_str[scope], operand_size);
}

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

static void ResetOpToDefault(op_t *op) {
    memset(op, 0, sizeof(*op));
    op->op_type = op_undefined;
    op->operand[OP_SRC] = OP_NOT_EXIST;
    op->operand[OP_DEST] = OP_NOT_EXIST;
}

static first_pass_status_t FillLabel(as_metadata_t *md, char *line, 
                                size_t *line_number, op_t *op) {
    first_pass_status_t ret = FS_SUCCESS;
    char *label = GetLabel(md, line, line_number);
    if (DEAD_BEEF == label) {
        return FS_NO_MEMORY;
    } else if (label != NULL) {
        ret = IsLabelExists(md, label, line_number);    
        if (ret == FS_SUCCESS) {
            if (ST_SUCCESS != SymbolTableInsert(GetSymbolTable(md), 
                                        (const char *)label, GetPC(md))) {
                return FS_NO_MEMORY;
            }  
            op->has_label = 1;
        }   
    }
    
    return ret;
}

static void GetOperand(char *line , op_t *op, operands_t type) {
    static register_type_t register_lut[register_amount] = 
                                            {r0, r1, r2, r3, r4, r5, r6, r7};
    int val = 0;
    if (line[0] == '@') {
        if (line[1] != 'r' || (line[3] != '\0' && line[3] != ',') || 
                                            line[2] > '7' || line[2] < '0') {
            op->operand[type] = OP_NOT_EXIST;
            op->operand_val[type].reg = undefined_register;

            return; 
        } 
        op->operand_val[type].reg = register_lut[(int)(line[2] - '0')];
        op->operand[type] = OP_REGISTER;
    }else if (IsLabel(line)) {
        op->operand_val[type].label = line; 
        op->operand[type] = OP_DIRECT;
    }else if (IsVal(line, &val)) {
        op->operand_val[type].constant = val; 
        op->operand[type] = OP_IMMIDIATE;
    } else {
        op->operand[type] = OP_NOT_EXIST;
    }
}

static first_pass_status_t EmitOpTypeError(as_metadata_t *md, char *section,
                        size_t *line_number, op_t *op, operands_t op_type) {

    char err_msg[200] = {0};
    if (op->operand[op_type] == OP_NOT_EXIST 
            && op->operand_val[OP_DEST].reg == undefined_register) {
        snprintf(err_msg, sizeof(err_msg), 
                "[ERROR] undefined regsister is used %s" , section);
    } else {
        snprintf(err_msg, sizeof(err_msg),
        "[ERROR] : %s Is neither a valid label int or register",
        section);
    }
    if (LG_SUCCESS != AddLog(GetLogger(md), GetFilename(md), 
                                            err_msg, *line_number)) {
        return FS_NO_MEMORY;
    }

    return FS_FAIL;
}

/* returns the amount of operands */
static int GetOperands(as_metadata_t *md, 
                                char operands[OP_TOTAL][MAX_INSTRUCTION_LENGTH],
                                char *string_operands, size_t *line_number) {
    char *operand = RemoveSpace(string_operands);
    char *operandend = operand;
    size_t len = 0;
    if (*operand == '\0') {
        return 0;
    }
    if (*operand == ',') {
        if (LG_SUCCESS != AddLog(GetLogger(md), GetFilename(md), 
            "[ERROR] : operands list cannot start with a comma.",
                                                                *line_number)) {
            return -2 /* no memory*/;
        }
        return -1; /* general fail*/
    }
    while (*operandend && !isspace(*operandend) && *operandend != ',') {
        ++operandend;
        
    }
    len = (size_t)operandend - (size_t)operand;
    memcpy(operands[OP_SRC], operand, len);
    operand = RemoveSpace(operandend);

    if (*operand != ',') {
        if (LG_SUCCESS != AddLog(GetLogger(md), GetFilename(md), 
            "[ERROR] : between 2 operand must be a comma",
                                                                *line_number)) {
            return -2 /* no memory*/;
        }
        return -1; /* general fail*/
    }else if (*operand == '\0') {
        return 1; /* 1 operand*/
    }
    operand = RemoveSpace(operandend + 1);
    if (*operand == ',') {
        if (LG_SUCCESS != AddLog(GetLogger(md), GetFilename(md), 
            "[ERROR] : Cannot have consicutive commas between operator",
                                                                *line_number)) {
            return -2 /* no memory*/;
        }
        return -1; /* general fail*/
    }
    operandend = operand;
    while (*operandend && !isspace(*operandend) && *operandend != ',') {
        ++operandend;
    }
    len = (size_t)operandend - (size_t)operand;
    memcpy(operands[OP_DEST], operand, len);
    if (*RemoveSpace(operandend) != '\0') {

        if (LG_SUCCESS != AddLog(GetLogger(md), GetFilename(md), 
            "[ERROR] : Cannot have extra operands after the 2 opcodes.", 
                                                        *line_number)) {
            return -2 /* no memory*/;
        }
        return -1; /* general fail*/
    }

    printf("operands1 :%s\n", operands[0]);
    printf("operands2 :%s\n", operands[1]);
    return 2;
}


static first_pass_status_t FillOp(as_metadata_t *md, char *line,
                                size_t *line_number, op_t *op) {
    char *section = strtok(RemoveSpace(line), " \n\t\v\r\f"); 
    char operands[OP_TOTAL][MAX_INSTRUCTION_LENGTH];
    first_pass_status_t ret = FS_SUCCESS;
    char err_msg[200];
    int op_amount = 0;
    if (op->has_label) {
        section = strtok(NULL, " \n\t\v\r\f"); 
    }
    op->op_type = GetOp(section);
    if (op->op_type == op_undefined) {
        snprintf(err_msg, sizeof(err_msg),"[ERROR] : %s is Undefined Op.", 
                                                                      section);
        if (LG_SUCCESS != AddLog(GetLogger(md), GetFilename(md),err_msg ,
                                                        *line_number)) {
            return FS_NO_MEMORY;
        }
        ret = FS_FAIL;
    }
    section = section + strlen(section) + 1 /* \0 */;
    memset(operands, 0, MAX_INSTRUCTION_LENGTH * OP_TOTAL * sizeof(char));
    op_amount = GetOperands(md, operands, section, line_number);
    if (op_amount == -1) {
        ret = FS_FAIL;
    } else if (op_amount == -2) {
        ret = FS_NO_MEMORY;
    } else if (op_amount > 0) {
        GetOperand(operands[OP_SRC], op, OP_SRC); 
        if (op->operand[OP_SRC] == OP_NOT_EXIST) {
            ret = EmitOpTypeError(md, 
                        operands[OP_DEST], line_number, op, OP_DEST);
        }
        if (op_amount == 2) {
            GetOperand(operands[OP_DEST], op, OP_DEST); 
            if (op->operand[OP_DEST] == OP_NOT_EXIST) {
                ret = EmitOpTypeError(md, 
                            operands[OP_DEST], line_number, op, OP_DEST);
            }
        }
    }

    return ret;
}
/* is responsible to filll the extra instruction that 
    represnt the data of the p[]
    actual - is where the values are supposed to go
    stored - is where the value is stored
*/
static first_pass_status_t FillInstructionOp(as_metadata_t *md, char *line,
                                size_t *line_number, op_t *op, 
                                operands_t stored, operands_t actual,
                                char *cmd)
{
    static char *register_str[register_amount] =    
            {"000", "001", "010", "011", "100", "101", "110", "111"}; 
    static int offset[OP_TOTAL] = {0/* src */, 7/* dest*/};
    operand_type_t type = op->operand[stored];
    if (type == OP_IMMIDIATE) {
        int val = op->operand_val[stored].constant;
        if ( val > MAX_INT || val < MIN_INT) {
            if (LG_SUCCESS!= AddLog(GetLogger(md), GetFilename(md), 
            "[WARNING] : Value out of range as interger", *line_number)) {
                return FS_NO_MEMORY;
            }
            val = (val > MAX_INT) ? MAX_INT : MIN_INT;
        }
        /* -2 because the last one is \0 and the first one is singd bit */
        for (size_t i = 1; i < OP_SIZE - 2; ++i){
            cmd[OP_SIZE - i - 1 - 1] = (val & 1) ? '1' : '0';
            val >>= 1;
        }

        return FS_SUCCESS;
    } else if (type == OP_REGISTER) {
        memcpy(cmd + offset[actual], register_str[op->operand_val->reg], sizeof(char) * 3);
        FillOperandARE(cmd, OP_ABSOLUTE); 

        return FS_SUCCESS;
    }else { /* lable*/
        char *label = op->operand_val[stored].label;
        memcpy(cmd, label, sizeof(label) + 1);

        return FS_SUCCESS;
    }

}

static first_pass_status_t FillTwoOp(as_metadata_t *md, char *line,
                                size_t *line_number, op_t *op) {
    first_pass_status_t ret = FS_SUCCESS;
    if (op->operand[OP_SRC] == OP_REGISTER && 
                                    op->operand[OP_DEST] == OP_REGISTER) {
        ret = FillInstructionOp(md, line,line_number, op,
                                                OP_DEST, OP_DEST, op->op_first);
        if (ret == FS_NO_MEMORY) {
            return ret;
        } 
        ret = FillInstructionOp(md, line,line_number, op,
                                                OP_SRC, OP_SRC, op->op_first);
        if (ret == FS_NO_MEMORY) {
            return ret;
        } 
    } else {
        ret = FillInstructionOp(md, line,line_number, op,
                                                OP_SRC, OP_SRC, op->op_first);
        if (ret == FS_NO_MEMORY) {
            return ret;
        } 
        ret = FillInstructionOp(md, line,line_number, op,
                                                OP_DEST, OP_DEST, op->op_second);
        if (ret == FS_NO_MEMORY) {
            return ret;
        } 
    }

    return ret;
}

static first_pass_status_t AddOpToIR(as_metadata_t *md, char *line,
                                size_t *line_number, op_t *op) {
    static int operand_amount[opcode_amount] = 
                {2, 2, 2, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 0, 0};
    int op_amount = (int)op->has_dest + op->has_src;
    first_pass_status_t ret = FS_SUCCESS;
    if (op_amount != operand_amount[op->op_type]) {
    static char *op_str[opcode_amount] =  {
         "mov", "cmp", "add", "sub", "not", "clr","lea" ,"inc" ,"dec" ,"jmp" ,
         "bne", "red", "prn", "jsr", "rts", "stop"};
        char err_msg[200] = {0};
        snprintf(err_msg, 200, 
        "[ERROR] : for op %s expected %d operands but got %d.", 
        op_str[op->op_type], operand_amount[op->op_type], op_amount);
        if (LG_SUCCESS != AddLog(GetLogger(md), GetFilename(md), 
                                                    err_msg, *line_number)) {
            return FS_NO_MEMORY;
        }       
    }
    FillOpBin(op);
    if (op_amount == 1) {
        /* since we store always the first op in src here we will pass that we 
        store the val in src but want to put it in dest*/
        FillOperandBin(op, OP_SRC, OP_DEST); 
        ret = FillInstructionOp(md, line,line_number, op,
                                                OP_SRC, OP_DEST, op->op_first);
    } else if (op_amount == 2) {
        if (op->operand[OP_SRC] == OP_REGISTER && 
                        op->operand[OP_DEST] == OP_REGISTER) {
            
        }
        FillOperandBin(op, OP_SRC, OP_SRC); 
        FillOperandBin(op, OP_DEST,OP_DEST); 
        FillTwoOp(md, line, line_number, op);
    }
    if (A_IR_SUCCESS != AssemblyIRAddInstr(GetAssemblyIRInst(md), op->opcode)) {
        return FS_NO_MEMORY;
    }
    SetIC(md, GetIC(md) + 1);
    SetPC(md, GetPC(md) + 1);
    if ((op->operand[OP_SRC] == OP_REGISTER && 
                        op->operand[OP_DEST] == OP_REGISTER) || op_amount == 1) {
        if (A_IR_SUCCESS != AssemblyIRAddInstr(GetAssemblyIRInst(md), op->op_first)) {
            return FS_NO_MEMORY;
        }
        SetIC(md, GetIC(md) + 1);
        SetPC(md, GetPC(md) + 1);
    } else if (op_amount == 2) {
        if (A_IR_SUCCESS != AssemblyIRAddInstr(GetAssemblyIRInst(md), op->op_first)) {
            return FS_NO_MEMORY;
        }
        if (A_IR_SUCCESS != AssemblyIRAddInstr(GetAssemblyIRInst(md), op->op_second)) {
            return FS_NO_MEMORY;
        }
        SetIC(md, GetIC(md) + 2);
        SetPC(md, GetPC(md) + 2);
    }

    return (ret);
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

static int HandleInstruction(as_metadata_t *md, char *line, 
                                                size_t *line_number) {
    
    first_pass_status_t ret = FS_SUCCESS;
    op_t op; 
    char instr_cp[MAX_INSTRUCTION_LENGTH] = {0};
    strncpy(instr_cp, line, MAX_INSTRUCTION_LENGTH);

    ResetOpToDefault(&op);
    ret = FillLabel(md, line, line_number, &op); 
    if (ret != ST_SUCCESS) {
        return ret;
    }
    
    ret = FillOp(md, instr_cp, line_number, &op);
    if (ret != ST_SUCCESS) {
        return ret;
    }
    ret = AddOpToIR(md, line,line_number, &op);
    if (ret != ST_SUCCESS) {
        return ret;
    }
    return ret;
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