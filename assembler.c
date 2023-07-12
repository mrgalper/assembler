#include "assembler.h"
#include "assembler_metadata.h"
#include "first_pass.h"
#include <stdio.h>


int assembler(const char *filename);

int main(int argc, char *argv[]) {

    if (argc <= 1) {
        printf("No files have been given");
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        printf("starting assembly for file %s\n", argv[i]);
        assembler_status_t st = assembler(argv[i]);
        if (st != AS_SUCCESS) {
            printf("Error in file: %s\n", argv[i]);
            /* break ? */
        } 
        printf("finished assembly for file %s\n", argv[i]);
    }

    return 0;
}

int assembler(const char *filename) { 
    as_metadata_t *meta = NULL;
    meta = CreateAssemblerMetadata(filename);
    if (NULL == meta)
    {
        return (FAILED_TO_INITIALIZE_META_DATA);
    }
    first_pass_status_t st1 = firstPass(meta);
    if (FS_SUCCESS != st1)
    {
        PrintAllLogs(GetLogger(meta));
        
        DestroyAssemblerMetadata(meta);
        if (st1 == FS_NO_MEMORY) {
            return AS_NO_MEMORY;
        }
        return (FAILED_FIRST_PASS);
    }
/*
    second_pass_statuts_t st2 = secondPass(meta);
    if (AS_SUCCESS != st2)
    {
        DestroyAssemblerMetadata(meta);
        return (FAILED_SECOND_PASS);
    }

    output_status_t st3 = convertToBinary(meta);
    if (AS_SUCCESS != st3)
    {
        DestroyAssemblerMetadata(meta);
        return (FAILED_CONVERT_TO_BINARY);
    }
*/
    DestroyAssemblerMetadata(meta);
    return (AS_SUCCESS);
}

