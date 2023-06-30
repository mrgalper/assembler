/******************************************************************************
*   Orginal Name : assembly_IR.c                                              *
*   Name: Mark Galperin                                                       *
*   Date 30.6.23                                                              *
*   Info : Implemntation of the IR.                                           *
******************************************************************************/
#include "assembly_IR.h" /* aseembly IR api */
#include <stdio.h> /* printf ,file operations*/
#include <stdlib.h> /* malloc, free  */
#include <string.h> /* string operation */
#include "assert.h" /* assert */

typedef struct assembly_IR_node {
    char instruction[81]; /* max instruction length + 1 */
    struct assembly_IR_node *next;
} IR_node_t;

struct assembly_IR {
    IR_node_t *head;
    IR_node_t *tail;
};

#define DEAD_BEEF (IR_node_t *)0xDEADBEEF 

static void UpdateTail(assembly_IR_t *slist_ir , IR_node_t *new_tail)
{
    slist_ir->tail = new_tail;  
}

static IR_node_t *CreateNode(const char *data, 
                                              IR_node_t *next_node) {
    size_t len = strlen(data) + 1;
    IR_node_t *new_node = malloc(sizeof(IR_node_t));
    if (new_node == NULL) {
        return NULL;
    }
    
    strncpy(new_node->instruction, data, len);
    new_node->next = next_node;
   
    return new_node;
}

assembly_IR_t *CreateAssemblyIR(void) {
    assembly_IR_t *ir = malloc(sizeof(assembly_IR_t));
    IR_node_t *dummy = NULL;
    if (ir == NULL) {
        return NULL;
    }
    dummy = CreateNode("LAST", DEAD_BEEF);
    if (NULL == dummy)
    {
        free(ir);
        return (NULL);
    }
    ir->head = dummy;
    ir->tail = ir->head;
    
    return ir;
}

void DestroyAssemblyIR(assembly_IR_t *ir) {
    IR_node_t *current = NULL;

    assert(ir != NULL);
    
    current = ir->head;
    while (current->next != DEAD_BEEF) {
        IR_node_t *next = current->next;
        free(current);
        current = next;
    }
    free(current);
    free(ir);
}


a_ir_status_t AssemblyIRAddInstr(assembly_IR_t *ir, const char *instr) {
    IR_node_t *tail; 
    IR_node_t *new_node;
    size_t len = 0;

    assert(ir != NULL);
    assert(instr != NULL);
    
    tail = AssemblyIRGetTail(ir);
    new_node = CreateNode(tail->instruction, tail->next);
    if (new_node == NULL) {
        return A_IR_NO_MEM;
    }

    len = strlen(instr) + 1;
    strncpy(tail->instruction, instr, len);
    tail->next = new_node;
    

    if (DEAD_BEEF == new_node->next)
    {
       UpdateTail(ir, new_node);
    }

    return A_IR_SUCCESS;
}

void AssemblyIRRemoveInstruction(assembly_IR_iter_t iter) {
    IR_node_t *current = iter;
    IR_node_t *next = NULL;
    
    assert(iter != NULL);

    next = current->next;
    memcpy(current, next, sizeof(IR_node_t));
    free(next);
}

void AssemblyIRChangeInstruction(assembly_IR_iter_t iter, const char *instr) {
    size_t len = 0;
    assert(iter != NULL);
    assert(instr != NULL);

    len = strlen(instr) + 1;
    strncpy(iter->instruction, instr, len);
}

assembly_IR_iter_t AssemblyIRGetFirstLine(assembly_IR_t *ir) {
    assert(ir != NULL);
    return ir->head;
}

assembly_IR_iter_t AssemblyIRGetTail(assembly_IR_t *ir) {
    assert(ir != NULL);
    return ir->tail;
}

assembly_IR_iter_t AssemblyIRGetNextLine(assembly_IR_iter_t iter) {
    assert(iter != NULL);
    return iter->next;
}

const char *AssemblyIRGetInstr(assembly_IR_iter_t iter) {
    assert(iter != NULL);
    return iter->instruction;
}

int AssemblyIRIterIsEqual(assembly_IR_iter_t iter1, assembly_IR_iter_t iter2) {
    return (iter1 == iter2);
}

a_ir_status_t AssemblyIRConvertToFile(assembly_IR_t *ir, const char *filename) {
    FILE *file = NULL;
    IR_node_t *tail = NULL;
    IR_node_t *iter = NULL;
    
    assert(ir != NULL);
    assert(filename != NULL);

    file = fopen(filename, "w");
    if (file == NULL) {
        return A_IR_CANNOT_OPEN_FILE;
    }

    tail = AssemblyIRGetTail(ir);
    iter = AssemblyIRGetFirstLine(ir);

    while (!AssemblyIRIterIsEqual(iter, tail)) {
        fprintf(file, "%s\n", AssemblyIRGetInstr(iter));
        iter = AssemblyIRGetNextLine(iter);
    }

    fclose(file);
    return A_IR_SUCCESS;
}

void AssemblyIRPrintIr(assembly_IR_t *ir) {
    IR_node_t *tail = NULL;
    IR_node_t *iter = NULL;

    assert(ir != NULL);

    printf("Starting To Print Assembly IR\n");

    tail = AssemblyIRGetTail(ir);
    iter = AssemblyIRGetFirstLine(ir);

    while (!AssemblyIRIterIsEqual(iter, tail)) {
        printf("%s\n", AssemblyIRGetInstr(iter));
        iter = AssemblyIRGetNextLine(iter);
    }

    printf("Ending To Print Assembly IR\n");
}
