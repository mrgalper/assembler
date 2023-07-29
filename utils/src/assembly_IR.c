/******************************************************************************
*   Orginal Name : assembly_IR.c                                              *
*   Name: Ido Sabach                                                          *
*   Date 30.6.23                                                              *
*   Info : Implemntation of the IR.                                           *
******************************************************************************/
#include "assembly_IR.h" /* aseembly IR api */
#include "assembler_helper.h" /* macros */

#include <stdio.h> /* printf ,file operations*/
#include <stdlib.h> /* malloc, free  */
#include <string.h> /* string operation */
#include <assert.h> /* assert */

typedef struct assembly_IR_node {
    char instruction[MAX_INSTRUCTION_LENGTH]; 
    size_t pc; 
    struct assembly_IR_node *next;
} IR_node_t;

struct assembly_IR {
    IR_node_t *head;
    IR_node_t *tail;
};

static void UpdateTail(assembly_IR_t *slist_ir , IR_node_t *new_tail)
{
    slist_ir->tail = new_tail;  
}

static IR_node_t *CreateNode(const char *data, IR_node_t *next_node, size_t pc) {
    size_t len = strlen(data) + 1;
    IR_node_t *new_node = malloc(sizeof(IR_node_t));
    if (new_node == NULL) {
        return NULL;
    }
    
    strncpy(new_node->instruction, data, len);
    new_node->next = next_node;
    new_node->pc = pc;

    return new_node;
}

assembly_IR_t *CreateAssemblyIR(void) {
    assembly_IR_t *ir = malloc(sizeof(assembly_IR_t));
    IR_node_t *dummy = NULL;
    if (ir == NULL) {
        return NULL;
    }
    dummy = CreateNode("LAST", DEAD_BEEF, -1);
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
    ir->head = NULL;
    ir->tail = NULL;

    free(ir);
}

a_ir_status_t AssemblyIRAddInstr(assembly_IR_t *ir, const char *instr, size_t pc) {
    IR_node_t *tail; 
    IR_node_t *new_node;
    size_t len = 0;

    assert(ir != NULL);
    assert(instr != NULL);
    
    tail = AssemblyIRGetTail(ir);
    new_node = CreateNode(tail->instruction, tail->next, tail->pc);
    if (new_node == NULL) {
        return A_IR_NO_MEM;
    }

    len = strlen(instr) + 1;
    strncpy(tail->instruction, instr, len);
    tail->next = new_node;
    tail->pc = pc;
    

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

size_t AssemblyIRGetPc(assembly_IR_iter_t iter) {
    assert(iter != NULL);
    return iter->pc;
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
    printf(" pc:    instr:\n");
    tail = AssemblyIRGetTail(ir);
    iter = AssemblyIRGetFirstLine(ir);

    while (!AssemblyIRIterIsEqual(iter, tail)) {
        printf("%lu %s\n", AssemblyIRGetPc(iter), AssemblyIRGetInstr(iter));
        iter = AssemblyIRGetNextLine(iter);
    }

    printf("Ending To Print Assembly IR\n");
}
