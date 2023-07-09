/*******************************************************************************
*   Original Name: assembler_helper.h                                          *
*   Name: Mark Galperin                                                        *
*   Date: 2.7.23                                                               *
*   Info: This contains all the macros for the asembly used to avoid double    *
*   defenitions.                                                               *
*******************************************************************************/

#ifndef __ASSEMBLER_HELPER_H__
#define __ASSEMBLER_HELPER_H__

#define MAX_INSTRUCTION_LENGTH 82 /* max_line + '\n' + '\0' */
#define UNUSED(x) (void)x
#define DEAD_BEEF (void *)0xDEADBEEF 
#define OP_SIZE 13 /* 12 + '\0' */
#define MAX_INT 4095 /* 2^12 -1     in bits 011111111111*/
#define MIN_INT -4096 /* 2^12 * -1  in bits 111111111111*/
#endif /* __ASSEMBLER_HELPER_H__ */
