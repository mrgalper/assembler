/*************************************************************************                 
*   Orginal Name : instruction_handler.h                                 *
*   Name: Mark Galperin                                                  *
*   Date : 15.7.23                                                       *
*   Info : This is file has the implemntation of the instruction handler.*
*          This handler is used in the first pass.                       *
*************************************************************************/
#ifndef __ASSEMBLER_FIRST_PASS_INSTRUCTION_HANDLER__
#define __ASSEMBLER_FIRST_PASS_INSTRUCTION_HANDLER__

#include "assembler_metadata.h"
#include "stddef.h"
#include "assembler_helper.h" /* macros */

int HandleInstruction(as_metadata_t *md, char *line, 
                                                        size_t *line_number);
#endif /* __ASSEMBLER_FIRST_PASS_INSTRUCTION_HANDLER__*/