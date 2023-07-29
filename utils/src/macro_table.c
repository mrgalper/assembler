
/******************************************************************************
*   Original Name: macro_table.c                                               *
*   Name: Ido Sabach                                                           *
*   Date: 1.7.23                                                               *
*   Info: This is the implemntation of the macro table.                        *
******************************************************************************/

#include "assembler_helper.h" /* macros*/
#include "macro_table.h" /* API */

#include <stdio.h> /* printf, file operations */
#include <stdlib.h> /* malloc, free */
#include <string.h> /* string operations */
#include <assert.h> /* assert */

typedef struct macro_entry macro_entry_t;

struct macro_entry{
    char label[MAX_INSTRUCTION_LENGTH];
    char **def;
    size_t def_lines;
    size_t line_defined;
    struct macro_entry *next;
};
struct macro_table{
    struct macro_entry *head;
    struct macro_entry *tail;
};

static void UpdateTail(macro_table_t *slist_mt,  macro_entry_t *new_tail)
{
    slist_mt->tail = new_tail;  
}

static char **CopyData(const char **data, size_t lines) {
    char **new_data = NULL;
    size_t i = 0;
    if (lines == 0) {
        return (DEAD_BEEF); /* in case of having a tail */
    }
    new_data = (char **) malloc(sizeof(char *) * lines);
    if (NULL == new_data) {
        return NULL;
    }
    for (i = 0; i < lines; i++) {
        size_t len = strlen(data[i]) + 1;
        new_data[i] = (char *)malloc(sizeof(char) * len);
        if (new_data[i] == NULL) {
            size_t j = 0;
            for (j = 0; j < i; j++) {
                free(new_data[j]);
            }
            free(new_data);

            return NULL;
        }
        strncpy(new_data[i], data[i], len);
    } 

    return new_data;
}

static macro_entry_t *CreateEntry(const char *label, const char **data,
            size_t lines, size_t line_defined, macro_entry_t *next_node) {

    macro_entry_t *new_node = malloc(sizeof(macro_entry_t));
    char **new_data = NULL;

    if (new_node == NULL) {
        
        return NULL;
    }
    
    strncpy(new_node->label, label, sizeof(label) + 1);
    new_data = CopyData(data, lines);
    if (new_data == NULL) {
        free(new_node);
        
        return NULL;
    }
    new_node->def = new_data;
    new_node->def_lines = lines;
    new_node->line_defined = line_defined;
    new_node->next = next_node;
   
    return new_node;
}

static void FreeData(macro_entry_t *entry) {
    size_t i = 0;
    for (i = 0; i < entry->def_lines; i++) {
        free(entry->def[i]);
    }
    free(entry->def);
}

macro_table_t *CreateMacroTable(void) {
    macro_table_t *ir = malloc(sizeof(macro_entry_t));
    macro_entry_t *dummy = NULL;
    if (ir == NULL) {
        return NULL;
    }
       
    dummy = CreateEntry("LAST", NULL, 0, 0, DEAD_BEEF);
    if (NULL == dummy)
    {
        free(ir);
        return (NULL);
    }
    ir->head = dummy;
    ir->tail = ir->head;
    
    return ir;
}

void DestroyMacroTable(macro_table_t *table) {
    macro_entry_t *current = table->head;
    macro_entry_t *next = NULL;
    size_t i = 0;

    assert(table != NULL);

    while (current->next != DEAD_BEEF) {
        next = current->next;
        for (i = 0; i < current->def_lines; i++) {
            free(current->def[i]);
        }
        free(current->def);
        free(current);
        current = next;
    }
    /* since the tail is a dummy node which doesnt have data we dont need to free it */
    free(current);
    free(table);
}

macro_table_iter_t MacroTableFindEntry(macro_table_t *table, const char *lable) {
    macro_entry_t *current = table->head;
    macro_entry_t *next = NULL;
    size_t i = 0;

    assert(table != NULL);
    assert(lable != NULL);

    while (current->next != DEAD_BEEF) {
        next = current->next;
        for (i = 0; i < current->def_lines; i++) {
            if (strcmp(current->label, lable) == 0) {
                return current;
            }
        }
        current = next;
    }
    return MacroTableGetLastEntry(table);
}

macro_status_t MacroTableAddEntry(macro_table_t *table, const char *label, 
                                 const char **lines, size_t lines_count, 
                                 size_t line_defined) {
    macro_entry_t *new_node = NULL;
    macro_entry_t *tail = NULL;
    char **new_data = NULL;

    assert(table != NULL);
    assert(label != NULL);
    assert(lines != NULL);

    /* check if the macro is already in the ds */
    if (!MacroTableIterIsEqual(MacroTableFindEntry(table, label), 
                                            MacroTableGetLastEntry(table))) {
        return (MACRO_ALREADY_EXISTS);
    }
    new_data = CopyData(lines, lines_count);
    if (new_data == NULL) {
        return MACRO_NO_MEM;
    }
    tail = MacroTableGetLastEntry(table);
    new_node = CreateEntry(tail->label, (const char **)tail->def , tail->def_lines, 
                                    tail->line_defined, tail->next);
    if (new_node == NULL) {
        FreeData(new_node);        
        
        return MACRO_NO_MEM;
    }

    strncpy(tail->label, label, strlen(label) + 1);
    tail->def_lines = lines_count;
    tail->def = new_data;
    tail->line_defined = line_defined;
    tail->next = new_node;

    UpdateTail(table, new_node);
    
    return (MACRO_SUCCESS);
}

macro_table_iter_t MacroTableGetFirstEntry(macro_table_t *table) {
    assert(table != NULL);

    return table->head;
}

macro_table_iter_t MacroTableGetLastEntry(macro_table_t *table) {
    assert(table != NULL);

    return table->tail;
}

macro_table_iter_t MacroTableGetNextEntry(macro_table_iter_t iter) {
    assert(iter != NULL);

    return iter->next;
}

const char *MacroTableGetEntryLable(macro_table_iter_t iter ){
    assert(iter != NULL);

    return iter->label;
}

const char **MacroTableGetEntryLines(macro_table_iter_t iter) {
    assert(iter != NULL);

    return (const char **)iter->def;
}

size_t MacroTableGetEntryNumberOfLines(macro_table_iter_t iter) {
    assert(iter != NULL);

    return iter->def_lines;
}

size_t MacroTableGetEntryLineDefined(macro_table_iter_t iter) {
    assert(iter != NULL);

    return iter->def_lines;
}


int MacroTableIterIsEqual(macro_table_iter_t it1, macro_table_iter_t it2) {
    size_t val = memcmp((void *)it1, (void *)it2, sizeof(macro_table_iter_t));

    return (val == 0);
}
