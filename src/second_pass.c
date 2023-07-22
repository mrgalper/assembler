
/*************************************************************************                 
*   Orginal Name : second_pass.h                                         *
*   Name: Mark Galperin                                                  *
*   Date : 22.7.23                                                       *
*   Info : This is the implemntation of the second pass                  *
*************************************************************************/

#include "second_pass.h" /* API */
#include "assembler_helper.h" /* macros */

#include <string.h> /* memcpy */
#define MAX_OP_LENGTH 13 /* 12 + '/0'*/

typedef enum label_type {
    LABEL_DONT_EXIST = -1,
    LABEL_ENTRY     = 0,
    LABEL_EXTERN    = 1,
    LABEL_SYMBOL    = 2
}label_t;

typedef struct label_mt {
    label_t label;
    int64_t line;
}label_mt_t;

static void FindLabel(as_metadata_t *md, char *label, label_mt_t *label_mt) {
    int line = SymbolTableLookup(GetSymbolTable(md), label); 
    if (line != -1) {
        label_mt->label = LABEL_SYMBOL;    
        label_mt->line = line;
        return;
    }
    line = SymbolTableLookup(GetEntryTable(md), label); 
    if (line != -1) {
        label_mt->label = LABEL_ENTRY;
        label_mt->line = line;
        return;
    }
    line = SymbolTableLookup(GetExternTable(md), label); 
    if (line != -1) {
        label_mt->label = LABEL_EXTERN;
        label_mt->line = line;
        return;
    }
    label_mt->label = LABEL_DONT_EXIST;
    label_mt->line = line;
    return;
}   

static void FillARE(char *cmd, label_t label_type) {
    static int operand_size = sizeof(char) * 2;
    static char *op_bin_str[3] = 
                {"00"/*ABSOLUTE*/, "10" /*RELOCATABLE*/, "01" /*EXTERNAL*/};
    int ARE_type = 1; /* default relocatble in case of symbol or entry */
    if (label_type == LABEL_EXTERN) {
        ARE_type = 2;
    }
    memcpy(cmd + 10, op_bin_str[ARE_type], operand_size);
}

static second_pass_status_t ConvertLineToOp(as_metadata_t *md, int val, 
                                                char *cmd, const char *label) {
    
    if (val > MAX_LANE || val < MIN_LANE) {
        char err_msg[120]= {0};
        snprintf(err_msg, 120, 
        "[WARNING] : The label %s was defined in line %d which is to big/small for a 10 bit\n",
        label, val);
        if (LG_SUCCESS!= AddLog(GetLogger(md), GetFilename(md), err_msg, -1)) {
            val = (val > MAX_LANE) ? MAX_LANE : MIN_LANE;

            return SC_NO_MEMORY;
        }
    }
    for (size_t i = 3; i < MAX_OP_LENGTH - 1; ++i){
        cmd[OP_SIZE - i - 1 ] = ((val & 1) ? '1' : '0');
        val >>= 1;
    }

    return SC_SUCCESS;
}

static void UpdateInstrOpARE(assembly_IR_iter_t it, label_t lb) {
    const char *instr = AssemblyIRGetInstr(it);
    char cp_instr[MAX_INSTRUCTION_LENGTH] = {0};
    strncpy(cp_instr, instr, MAX_INSTRUCTION_LENGTH);

    FillARE(cp_instr, lb);
    AssemblyIRChangeInstruction(it, cp_instr);
}

static second_pass_status_t ChangeLableToBin(as_metadata_t *mt, 
                    const char *label, assembly_IR_iter_t it,
                    assembly_IR_iter_t instr_it) {
    label_mt_t label_mt;
    char op[MAX_OP_LENGTH] = {0};
    memset(op, '0',  MAX_OP_LENGTH - 1);
    FindLabel(mt, (char *)label, &label_mt);
    if ( LABEL_DONT_EXIST == label_mt.label) {
        char msg_err[120];
        snprintf(msg_err, 120, 
        "[ERROR]: The following label was used %s but not defined", label);
        if (LG_SUCCESS != AddLog(GetLogger(mt), GetFilename(mt), msg_err , -1)) {
            return SC_NO_MEMORY;
        }
    }
    FillARE(op, label_mt.label);
    ConvertLineToOp(mt, label_mt.line, op, label);
    AssemblyIRChangeInstruction(it, op);
    UpdateInstrOpARE(instr_it, label_mt.label);

    return SC_SUCCESS;
}

second_pass_status_t SecondPass(as_metadata_t *meta) {
    assembly_IR_iter_t prev_prev_it; 
    assembly_IR_iter_t prev_it =  AssemblyIRGetFirstLine(GetAssemblyIRInst(meta));
    assembly_IR_iter_t it =  AssemblyIRGetNextLine(prev_it);
    assembly_IR_iter_t tail = AssemblyIRGetTail(GetAssemblyIRInst(meta));
    second_pass_status_t ret = SC_SUCCESS;

    while (!AssemblyIRIterIsEqual(it , tail) && ret != SC_NO_MEMORY) {
        const char *instr = AssemblyIRGetInstr(it);
        if (*instr == '3' || *instr == '2') {
            if (*instr == '2') {
                ret = ChangeLableToBin(meta, instr + 1 /* skip 2/3*/, it, prev_it);
            } else {
                ret = ChangeLableToBin(meta, instr + 1 /* skip 2/3*/, it, prev_prev_it);
            }
        }
        prev_prev_it = prev_it;
        prev_it = it;
        it = AssemblyIRGetNextLine(it);
    }

    if (!LoggerIsEmpty(GetLogger(meta)) && ret != SC_NO_MEMORY) {
        return SC_FAIL;
    }
    
    return SC_SUCCESS; 
}