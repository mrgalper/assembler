#include <stdio.h> /* printf */

#include "symbol_table.h" /* symbol table api */
/* gcc test/symbol_table_test.c -L . -lutils -I include */
void testSymbolTable()
{
    s_table_t *table = CreateSymbolTable();

    /* Test SymbolTableInsert */
    printf("Testing SymbolTableInsert\n");
    SymbolTableInsert(table, "symbol1", 1);
    SymbolTableInsert(table, "symbol2", 2);
    SymbolTableInsert(table, "symbol3", 3);
    SymbolTableInsert(table, "symbol4", 4);
    SymbolTableInsert(table, "symbol5", 5);
    SymbolTableInsert(table, "symbol6", 124);
    SymbolTableInsert(table, "symbol5", 1254);
    SymbolTableInsert(table, "symbol5", 12);
    SymbolTableInsert(table, "symbol5", 592);
    SymbolTableInsert(table, "symbol5", 5125);
    SymbolTableInsert(table, "symbol5", 314);
    SymbolTableInsert(table, "symbol5",6755);
    SymbolTableInsert(table, "symbol5", 321);
    SymbolTableInsert(table, "symbol5", 14);
    SymbolTableInsert(table, "symbol5", 7);

    /* Test SymbolTableLookup */
    printf("Testing SymbolTableLookup\n");
    int64_t line = SymbolTableLookup(table, "symbol3");
    printf("Symbol3 found at line: %lld\n", line);

    /* Test SymbolTableRemove */
    printf("Testing SymbolTableRemove\n");
    SymbolTableRemove(table, "symbol4");

    /*Test SymbolTableConvertToFile*/
    printf("Testing SymbolTableConvertToFile\n");
    SymbolTableConvertToFile(table, "symbol_table.txt");

    /* Test DestroySymbolTable */
    printf("Testing DestroySymbolTable\n");
    DestroySymbolTable(table);
}

int main()
{
    testSymbolTable();
    return 0;
}
