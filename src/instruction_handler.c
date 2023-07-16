
/*************************************************************************                 
*   Orginal Name : instruction_handler.c                                 *
*   Name: Mark Galperin                                                  *
*   Date : 15.7.23                                                       *
*   Info : This is file has the implemntation of the instruction handler.*
*          This handler is used in the first pass.                       *
*************************************************************************/

#include "instruction_handler.h" /* API */
#include "first_pass.h" /* STATUS */

#include <string.h> /* isempty, strncpy */
#include <assert.h> /* assert */
#include <stdio.h> /* printf, File  */
#include <ctype.h> /* isspace */
#include <stdlib.h> /* free */

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


#define MAX_LABEL_LENGTH  31
static char *RemoveSpace(char *line) {
    while (isspace((unsigned char)*line)) {
        ++line;
    }

    return line;
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

static int IsLabel(char *line) {
    int is_valid = 1;
    for (size_t i = 0; line[i] && is_valid; i++) {
        if (!isupper(line[i]) && !islower(line[i])) {
            is_valid = 0;
        }
    }

    return (is_valid);
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
/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/

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

int HandleInstruction(as_metadata_t *md, char *line, 
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