/******************************************************************************
*   Orginal Name : assembly_IR.h                                               *
*   Name: Ido Sabach                                                          *
*   Date 30.6.23                                                              *
*   Info : This is a model that will hold all the intermidiate                *
*   representations of the the code.                                          *
******************************************************************************/

#ifndef __ASSEMBLY_IR_H__
#define __ASSEMBLY_IR_H__

#include <stddef.h> /* size_t */

typedef struct assembly_IR assembly_IR_t;
typedef struct assembly_IR_node *assembly_IR_iter_t;

typedef enum {
    A_IR_SUCCESS = 0,
    A_IR_NO_MEM  = 1,
    A_IR_CANNOT_OPEN_FILE = 2
}a_ir_status_t;

/*******************************************
* DESCRIPTION: Create a new assembly IR.
* 
* RETURN:
*     SUCCESS:A pointer to the newly created assembly IR.
*     FAILURE:NULL.
*******************************************/
assembly_IR_t *CreateAssemblyIR(void);

/*******************************************
* DESCRIPTION: 
*     Destroy an assembly IR and free its memory. 
* PARAM:
*     ir - A valid pointer to the assembly IR to be destroyed.
* RETURN: 
*      void.
* BUGS:
*      if the pointer is not valid, the result will be undefined
*******************************************/
void DestroyAssemblyIR(assembly_IR_t *ir);

/*******************************************
* DESCRIPTION: Add an instruction to the assembly IR.
* 
* PARAM:
*     ir - A valid pointer to the assembly IR.
*     instr - A pointer to the instruction to be added.
*     pc - The address of the instruction to be added.
* RETURN:
*     A_ir_status_t - The status of the operation:
*                     - A_IR_SUCCESS if successful
*                     - A_IR_NO_MEM if there is low memory
* BUGS:
*      if the pointer is not valid, the result will be undefined
*******************************************/
a_ir_status_t AssemblyIRAddInstr(assembly_IR_t *ir, const char *instr, size_t pc);

/*******************************************
* DESCRIPTION: Remove the last instruction from the assembly IR.
* 
* PARAM:
*     iterator - to be removed.
* BUGS:
*      if the pointer is not valid, the result will be undefined
*******************************************/
void AssemblyIRRemoveInstruction(assembly_IR_iter_t iter);

/*******************************************
* DESCRIPTION: Remove the last instruction from the assembly IR.
* 
* PARAM:
*     iter - a valid iterator to the node you want to change.
*     instr - A pointer to the new instruction to change thre current value.
* BUGS:
*      if the pointer is not valid, the result will be undefined
*******************************************/
void AssemblyIRChangeInstruction(assembly_IR_iter_t iter, const char *instr);

/*******************************************
* DESCRIPTION: Get an iterator pointing to the first line of the assembly IR.
* 
* PARAM:
*     ir - A valid pointer to the assembly IR.
* 
* RETURN:
*     assembly_IR_iter_t - An iterator pointing to the first line of the 
*     assembly IR.
* BUGS:
*      if the pointer is not valid, the result will be undefined
*******************************************/
assembly_IR_iter_t AssemblyIRGetFirstLine(assembly_IR_t *ir);

/*******************************************
* DESCRIPTION: Get an iterator pointing to the tail of the assembly IR.
*              the tail is used as an iterator to traverse the assembly IR.
* PARAM:
*     ir - A valid pointer to the assembly IR.
*    
* RETURN:
*     assembly_IR_iter_t - An iterator pointing to the tail of the 
*     assembly IR.
* BUGS:
*      if the pointer is not valid, the result will be undefined
*      Any operation on it that does not involve traversing to the end,
*      will result in undefined behavior.
*******************************************/
assembly_IR_iter_t AssemblyIRGetTail(assembly_IR_t *ir);

/*******************************************
* DESCRIPTION: Get the iterator pointing to the next line of the assembly IR.
*              Always check that you are not on Tail of the assembly IR.
* PARAM:
*     iter - The current iterator pointing to a line in the assembly IR.
* 
* RETURN:
*     assembly_IR_iter_t - The iterator pointing to the next line of the assembly IR.
* BUGS:
*      if the pointer is not valid, the result will be undefined
*******************************************/
assembly_IR_iter_t AssemblyIRGetNextLine(assembly_IR_iter_t iter);


/*******************************************
* DESCRIPTION: compare if two iterators are equal, use it to check 
*              if you are in the tail of the assembly IR.       
* PARAM:
*     iter1, iter - iterators to compare.
* RETURN: 
*     if matched or 0 ,respectivly.
*******************************************/
int AssemblyIRIterIsEqual(assembly_IR_iter_t iter1, assembly_IR_iter_t iter);

/*******************************************
* DESCRIPTION: Get the instruction from the iterator.
* 
* PARAM:
*     iter - The iterator pointing to a line in the assembly IR.
* RETURN:
*     SUCCESS: The instruction from the iterator.
*     FAILURE: 
* BUGS:
*      if the pointer is not valid, the result will be undefined
*******************************************/
const char *AssemblyIRGetInstr(assembly_IR_iter_t iter);

/*******************************************
* DESCRIPTION: Get the pc from the iterator.
* 
* PARAM:
*     iter - The iterator pointing to a line in the assembly IR.
* RETURN:
*     SUCCESS: The pc.
*     FAILURE: 
* BUGS:
*      if the pointer is not valid, the result will be undefined
*******************************************/
size_t AssemblyIRGetPc(assembly_IR_iter_t iter);
/*******************************************
* DESCRIPTION: Convert the assembly IR to a file with the specified filename.
* 
* PARAM:
*     ir - A valid pointer to the assembly IR.
*     filename - The name of the file to which the assembly IR will be converted.
* RETURN:
*     a_ir_status_t - The status of the operation:
*                     - A_IR_SUCCESS if successful
*                     - A_IR_NO_MEM if there is low memory
*                     - A_IR_CANNOT_OPEN_FILE if the file
* BUGS:
*      if the pointer is not valid, the result will be undefined
*******************************************/
a_ir_status_t AssemblyIRConvertToFile(assembly_IR_t *ir, const char *filename);

/*******************************************
* DESCRIPTION: Print the assembly IR.
* 
* PARAM:
*     ir - A valid pointer to the assembly IR.
* RETURN:
*       void.
* BUGS:
*      if the pointer is not valid, the result will be undefined
*******************************************/
void AssemblyIRPrintIr(assembly_IR_t *ir);

#endif  /* __ASSEMBLY_IR_H__ */
