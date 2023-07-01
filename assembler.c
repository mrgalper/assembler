#include "assembler.h"


int main(int argc, char *argv[]) {

    if (argc <= 1) {
        printf("No files have been given");
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        printf("starting assembly for file %s\n", argv[i]);
        assembler_status_t st = assembler(argv[i]);
        if (st != SUCCESS) {
            printf("Error in file: %s\n", argv[i]);
            /* break ? */
        } 
        printf("finished assembly for file %s\n", argv[i]);
    }

    return 0;
}

int assembler(const char *filename) {
    
    metadata_t*meta = NULL;
    meta = initializeMetaData(filename);
    if (NULL == meta)
    {
        return (FAILED_TO_INITIALIZE_METADATA);
    }

    first_pass_statuts_t st1 = firstPass(meta);
    if (SUCCESS != st1)
    {
        CleanUpMetaData(meta);
        return (FAILED_FIRST_PASS);
    }

    second_pass_statuts_t st2 = secondPass(meta);
    if (SUCCESS != st2)
    {
        CleanUpMetaData(meta);
        return (FAILED_SECOND_PASS);
    }

    output_status_t st3 = convertToBinary(meta);
    if (SUCCESS != st3)
    {
        CleanUpMetaData(meta);
        return (FAILED_CONVERT_TO_BINARY);
    }

    CleanUpMetaData(meta);
    return (SUCCESS);
}

