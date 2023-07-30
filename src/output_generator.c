/*************************************************************************                 
*   Orginal Name : output_generator.c                                   *
*   Name: Ido Sabach                                                     *
*   Date : 28.7.23                                                       *
*   Info : This is the implemntation of the convertion to binary.        *
*************************************************************************/

#include "output_generator.h" /* API */
#include "assembler_helper.h" /* macros */

#include <string.h> /* memcpy */
#include <stdlib.h> /* free, malloc */
#include <stdio.h> /* FILE */

static output_status_t
    GenerateEntryExternalTable(as_metadata_t *mt, char *base_name) {
    s_table_status_t table_st = ST_SUCCESS;
    output_status_t ret = OUT_SUCCESS;
    size_t len = strlen(base_name);
    char *output_name = malloc(len + 1 + 3 /* ent/ext */);
    if (output_name == NULL) {
        return OUT_NO_MEMORY;
    }
    strncpy(output_name, base_name, len);
    strncpy(output_name + len, "ext", 4 /* ext + '\0'*/);
    table_st = SymbolTableConvertToFile(GetExternOutput(mt), output_name);
    if (table_st == ST_FAIL_TO_OPEN_FILE) {
        ret = OUT_COULD_NOT_OPEN_FILE;
    } else if (table_st == ST_FAILED) {
        ret = OUT_FAILED;
    }

    strncpy(output_name + len, "ent", 4 /* ent + '\0'*/);
    table_st = SymbolTableConvertToFile(GetEntryOutput(mt), output_name);
    if (table_st == ST_FAIL_TO_OPEN_FILE) {
        ret = OUT_COULD_NOT_OPEN_FILE;
    } else if (table_st == ST_FAILED) {
        ret = OUT_FAILED;
    }
    
    free(output_name);
    return ret;
}
static void ConvertToBase64(const char *binaryString, char *base64String) {
    static const char base64_lut[] = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int i = 0;
    uint16_t bits = 0;
    for (i = 0; i < 12; i++) {
        bits = (bits << 1) | (binaryString[i] - '0');
    }

    base64String[0] = base64_lut[bits >> 6];
    base64String[1] = base64_lut[bits & 0x3F];

    
    base64String[2] = '\0';
}

static void ConvertAssemblyIrToBase64(assembly_IR_t *ir, FILE *file)  {

    assembly_IR_iter_t tail = NULL;
    assembly_IR_iter_t iter = NULL;
    char base64String[3] = {0};
    tail = AssemblyIRGetTail(ir);
    iter = AssemblyIRGetFirstLine(ir);

    while (!AssemblyIRIterIsEqual(iter, tail)) {
        ConvertToBase64(AssemblyIRGetInstr(iter), base64String);
        fprintf(file, "%s\n", base64String);
        iter = AssemblyIRGetNextLine(iter);
    }

}
static output_status_t GenerateBase64ObjFile(as_metadata_t *mt, char *base_name) {
    FILE* file= NULL; 
    size_t len = strlen(base_name);
    char *output_name = malloc(len + 1 + 3 /* obj */);
    if (output_name == NULL) {
        return OUT_NO_MEMORY;
    }
    strncpy(output_name, base_name, len);
    strncpy(output_name + len, "obj", 4 /* obj + '\0'*/);

    file = fopen(output_name, "w");
    if (file == NULL) {
        free(output_name);
        return OUT_COULD_NOT_OPEN_FILE;
    }
    fprintf(file, "%lu %lu\n",GetIC(mt), GetDC(mt));
    ConvertAssemblyIrToBase64(GetAssemblyIRInst(mt), file);  
    ConvertAssemblyIrToBase64(GetAssemblyIRData(mt), file);  

    fclose(file);
    free(output_name);

    return OUT_SUCCESS;
}

output_status_t ConvertToBinary(as_metadata_t *mt)
{
    char *base_name =  NULL;
    output_status_t ret = OUT_SUCCESS;
    /* after the -2 it will remaine with name. */
    size_t len = strlen(GetFilename(mt)) - 2; 
    base_name = (char *)malloc(len + 1);
    if (base_name == NULL) {
        return OUT_NO_MEMORY;
    }
    base_name[len] = '\0';
    strncpy(base_name, GetFilename(mt), len);
    ret = GenerateEntryExternalTable(mt, base_name);
    if (ret != OUT_SUCCESS) {
        free(base_name);
        return ret;
    }
    ret = GenerateBase64ObjFile(mt, base_name);
    free(base_name);
    return ret;
}
