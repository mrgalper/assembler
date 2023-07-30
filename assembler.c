#include "assembler.h" /* assembler errors */
#include "assembler_metadata.h" /* Metadata api*/
#include "first_pass.h" /* first pass API*/
#include "second_pass.h" /* second pass API*/
#include "output_generator.h" /* The output generator */

#include <stdio.h> /* printf*/
#include <stdlib.h> /* getenv */

int assembler(const char *filename);

int main(int argc, char *argv[]) {
    int i = 0;
    assembler_status_t st;
    if (argc <= 1) {
        printf("No files have been given");
        return 0;
    }

    for (i = 1; i < argc; i++) {
        printf("starting assembly for file %s\n", argv[i]);
        st = assembler(argv[i]);
        if (st != AS_SUCCESS) {
            printf("Error in file: %s\n", argv[i]);
            /* break ? */
        } 
        printf("finished assembly for file %s\n", argv[i]);
    }

    return 0;
}

static void PrintPassOutput(as_metadata_t *mt) {
    printf("Printing data output\n");
    AssemblyIRPrintIr(GetAssemblyIRData(mt));
    printf("\n\nPrinting Instructions output\n");
    AssemblyIRPrintIr(GetAssemblyIRInst(mt));
}

int assembler(const char *filename) { 
    as_metadata_t *meta = NULL;
    first_pass_status_t st1 = FS_SUCCESS;
    second_pass_status_t st2 = SC_SUCCESS;
    output_status_t st3 = OUT_SUCCESS;
    meta = CreateAssemblerMetadata(filename);
    if (NULL == meta)
    {
        return (FAILED_TO_INITIALIZE_META_DATA);
    }
    st1 = FirstPass(meta);
    if (FS_SUCCESS != st1)
    {
        PrintAllLogs(GetLogger(meta)); 
        PrintAllLogs(GetWarningLogger(meta)); 
        DestroyAssemblerMetadata(meta);
        if (st1 == FS_NO_MEMORY) {
            return AS_NO_MEMORY;
        }
        return (FAILED_FIRST_PASS);
    }
    if (getenv("ASSEMBLY_FIRST_PASS_IR") != NULL) {
        PrintPassOutput(meta);
    }
    st2 = SecondPass(meta);
    if (SC_SUCCESS != st2)
    {
        PrintAllLogs(GetLogger(meta)); 
        PrintAllLogs(GetWarningLogger(meta)); 
        DestroyAssemblerMetadata(meta);
        return (FAILED_SECOND_PASS);
    }
    if (getenv("ASSEMBLY_SECOND_PASS_IR") != NULL) {
        PrintPassOutput(meta);
    }
    st3 = ConvertToBinary(meta);
    if (OUT_SUCCESS != st3)
    {
        PrintAllLogs(GetLogger(meta)); 
        PrintAllLogs(GetWarningLogger(meta)); 
        DestroyAssemblerMetadata(meta);
        return (FAILED_CONVERT_TO_BINARY);
    }
    /* if there were any warning print them*/
    PrintAllLogs(GetWarningLogger(meta));  
    DestroyAssemblerMetadata(meta);


    return (AS_SUCCESS);
}

