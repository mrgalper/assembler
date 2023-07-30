
/*************************************************************************                 
*   Orginal Name : second_pass.h                                         *
*   Name: Ido Sabach                                                     *
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

typedef struct status_and_meta {
    as_metadata_t *metadata;
    second_pass_status_t ret;
}status_and_meta_t;

typedef struct label_mt {
    label_t label;
    int64_t line;
}label_mt_t;

static void FindLabel(as_metadata_t *md, char *label, label_mt_t *label_mt) {
    int line = 0;
    /* first check if it is external no need to check if internal because 
    it is supposed to be in the Symbol if not we will check later */ 
    line = SymbolTableLookup(GetExternTable(md), label); 
    if (line != -1) {
        label_mt->label = LABEL_EXTERN;
        label_mt->line = line;
        return;
    }
    line = SymbolTableLookup(GetSymbolTable(md), label); 
    if (line != -1) {
        label_mt->label = LABEL_SYMBOL;    
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
    int i = 0;
    if (val > MAX_LANE || val < MIN_LANE) {
        char err_msg[120]= {0};
        snprintf(err_msg, 120, 
        "[WARNING] : The label %s was defined in line %d which is to big/small for a 10 bit\n",
        label, val);
        if (LG_SUCCESS!= AddLog(GetWarningLogger(md), GetFilename(md), err_msg, -1)) {
            val = (val > MAX_LANE) ? MAX_LANE : MIN_LANE;

            return SC_NO_MEMORY;
        }
    }
    for (i = 3; i < MAX_OP_LENGTH - 1; ++i){
        cmd[OP_SIZE - i - 1 ] = ((val & 1) ? '1' : '0');
        val >>= 1;
    }

    return SC_SUCCESS;
}

static second_pass_status_t FillExternTable(as_metadata_t *mt, 
            label_mt_t label_mt, const char *label , assembly_IR_iter_t it)
{
    if (label_mt.label == LABEL_EXTERN) {
        if (ST_SUCCESS != SymbolTableInsert(GetExternOutput(mt), label, 
                                                        AssemblyIRGetPc(it))) {
            return SC_NO_MEMORY;
        }
    } 

    return SC_SUCCESS;
}

static second_pass_status_t ChangeLableToBin(as_metadata_t *mt, 
                    const char *label, assembly_IR_iter_t it) {
    label_mt_t label_mt;
    char op[MAX_OP_LENGTH] = {0};
    memset(op, '0',  MAX_OP_LENGTH - 1);
    FindLabel(mt, (char *)label, &label_mt);
    if ( LABEL_DONT_EXIST == label_mt.label) {
        char msg_err[120];
        snprintf(msg_err, 120, 
        "[ERROR]: label was used %s but not defined", label);
        if (LG_SUCCESS != AddLog(GetLogger(mt), GetFilename(mt), msg_err , 
            AssemblyIRGetPc(it))) {
            return SC_NO_MEMORY;
        }
    }
    if (SC_SUCCESS != FillExternTable(mt, label_mt, label, it)) {
        return SC_NO_MEMORY;
    }
    FillARE(op, label_mt.label);
    if ( LABEL_EXTERN != label_mt.label) /* extern labels stay 0 since they are unkown*/
    {
        ConvertLineToOp(mt, label_mt.line, op, label);
    }
    AssemblyIRChangeInstruction(it, op);

    return SC_SUCCESS;
}

static int ActionFunc(for_each_data_t *data, void *param) {
    status_and_meta_t *st_and_mt = (status_and_meta_t *)param;
    as_metadata_t *meta = st_and_mt->metadata;
    int line = SymbolTableLookup(GetSymbolTable(meta), data->label);
    if (-1 == line) {
        char msg_err[256] = {0};
        snprintf(msg_err, sizeof(msg_err),
         "[ERROR]: symbol %s defined as entry but no entry was found.", data->label);
        if (LG_SUCCESS != AddLog(GetLogger(meta), GetFilename(meta),msg_err, 
                                                                data->line)) {
            st_and_mt->ret =  SC_NO_MEMORY;
        }
        return SC_FAIL;
    } else {
        if (ST_FAILED == 
                SymbolTableInsert(GetEntryOutput(meta), data->label, line)) {
            st_and_mt->ret = SC_NO_MEMORY;

            return SC_FAIL;
        }
    }
    
    return SC_SUCCESS;
}

static second_pass_status_t HandleEntryOutput(as_metadata_t *meta) {
    status_and_meta_t st_and_mt;
    st_and_mt.metadata = meta;
    st_and_mt.ret = SC_SUCCESS;
    SymbolTableForEach(GetEntryTable(meta), ActionFunc, (void *)&st_and_mt);
    
    return st_and_mt.ret;
}

second_pass_status_t SecondPass(as_metadata_t *meta) {
    assembly_IR_iter_t it =  AssemblyIRGetFirstLine(GetAssemblyIRInst(meta));
    assembly_IR_iter_t tail = AssemblyIRGetTail(GetAssemblyIRInst(meta));
    second_pass_status_t ret = SC_SUCCESS;

    while (!AssemblyIRIterIsEqual(it , tail) && ret != SC_NO_MEMORY) {
        const char *instr = AssemblyIRGetInstr(it);
        if (*instr != '1' && *instr != '0') {
            ret = ChangeLableToBin(meta, instr, it);
        }
        it = AssemblyIRGetNextLine(it);
    }

    if (!LoggerIsEmpty(GetLogger(meta)) && ret != SC_NO_MEMORY) {
        return SC_FAIL;
    }

    ret = HandleEntryOutput(meta);
    if (!LoggerIsEmpty(GetLogger(meta)) && ret != SC_NO_MEMORY) {
        return SC_FAIL;
    }

    return ret; 
}
