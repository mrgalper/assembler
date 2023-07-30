/******************************************************************************
*   Original Name: macro_table_tests.c                                         *
*   Name: Ido Sabach                                                          *
*   Date: 30.6.23                                                              *
*   Info: Scalable tests for the macro table API.                              *
******************************************************************************/
#include "macro_table.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Helper function to print the contents of the macro table */
void PrintMacroTable(macro_table_t *table) {
    macro_table_iter_t iter;

    for (iter = MacroTableGetFirstEntry(table); iter != MacroTableGetLastEntry(table); iter = MacroTableGetNextEntry(iter)) {
        const char *label = MacroTableGetEntryLable(iter);
        const char **lines = MacroTableGetEntryLines(iter);
        size_t line_count = MacroTableGetEntryNumberOfLines(iter);
        size_t line_defined = MacroTableGetEntryLineDefined(iter);
        size_t i = 0;
        printf("Label: %s\n", label);
        printf("Lines:\n");
        for (i = 0; i < line_count; i++) {
            printf("%s\n", lines[i]);
        }
        printf("Line Defined: %zu\n", line_defined);
        printf("----------------------\n");
    }
}

int main(void) {
    macro_table_t *table = CreateMacroTable();
    if (table == NULL) {
        printf("Failed to create macro table\n");
        return 1;
    }

    /* Add macro entries */
    const char *lines1[] = {"line1", "line2", "line3"};
    int val = MacroTableAddEntry(table, "MACRO1", lines1, sizeof(lines1) / sizeof(lines1[0]), 10);
     const char *lines2[] = {"line4", "line5"};
    MacroTableAddEntry(table, "MACRO2", lines2, sizeof(lines2) / sizeof(lines2[0]), 15);

    const char *lines3[] = {"line6"};
     MacroTableAddEntry(table, "MACRO3", lines3, sizeof(lines3) / sizeof(lines3[0]), 20);

    /* Print the macro table */
    printf("Macro Table:\n");
    PrintMacroTable(table);

    /* Find a specific macro entry */
    macro_table_iter_t iter = MacroTableFindEntry(table, "MACRO2");
    if (iter != MacroTableGetLastEntry(table)) {
        printf("Found MACRO2:\n");
        const char *label = MacroTableGetEntryLable(iter);
        const char **lines = MacroTableGetEntryLines(iter);
        size_t line_count = MacroTableGetEntryNumberOfLines(iter);
        size_t line_defined = MacroTableGetEntryLineDefined(iter);
        size_t i = 0;
        printf("Label: %s\n", label);
        printf("Lines:\n");
        for (i = 0; i < line_count; i++) {
            printf("%s\n", lines[i]);
        }
        printf("Line Defined: %zu\n", line_defined);
        printf("----------------------\n");
    } else {
        printf("MACRO2 not found\n");
    }

    /* Cleanup */
    DestroyMacroTable(table);

    return 0;
}
