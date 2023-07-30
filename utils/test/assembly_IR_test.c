#include "assembly_IR.h"
#include "stdio.h"
#include "stddef.h"


#define MAX_INSTRUCTIONS 20 

int main() {
    int i = 0;
    assembly_IR_t *ir = CreateAssemblyIR();
    if (ir == NULL) {
        printf("Failed to create assembly IR\n");
        return 1;
    }

    // Add instructions
    for (i = 0; i < MAX_INSTRUCTIONS; i++) {
        char instruction[20];
        snprintf(instruction, sizeof(instruction), "INSTR_%d", i);
        a_ir_status_t status = AssemblyIRAddInstr(ir, instruction, 100);
        if (status != A_IR_SUCCESS) {
            printf("Failed to add instruction\n");
            DestroyAssemblyIR(ir);
            return 1;
        }
    }

    // Print assembly IR
    printf("Printing Assembly IR:\n");
    AssemblyIRPrintIr(ir);
    printf("\n");

    // Modify every other instruction
    assembly_IR_iter_t iter = AssemblyIRGetFirstLine(ir);
    int count = 0;
    while (!AssemblyIRIterIsEqual(iter, AssemblyIRGetTail(ir))) {
        if (count % 2 == 0) {
            char instruction[20];
            snprintf(instruction, sizeof(instruction), "MODIFIED_%d", count);
            AssemblyIRChangeInstruction(iter, instruction);
        }
        iter = AssemblyIRGetNextLine(iter);
        count++;
    }

    // Print modified assembly IR
    printf("Printing Modified Assembly IR:\n");
    AssemblyIRPrintIr(ir);
    printf("\n");

    // Convert assembly IR to file
    a_ir_status_t status = AssemblyIRConvertToFile(ir, "output.txt");
    if (status != A_IR_SUCCESS) {
        printf("Failed to convert assembly IR to file\n");
        DestroyAssemblyIR(ir);
        return 1;
    }

    DestroyAssemblyIR(ir);
    printf("Assembly IR test completed successfully\n");

    return 0;
}

