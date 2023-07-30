/*******************************************************************************
*   Original Name: assembler_helper.h                                          *
*   Name: Ido Sabach                                                           *
*   Date: 2.7.23                                                               *
*   Info: This contains all the macros for the asembly used to avoid double    *
*   defenitions.                                                               *
*******************************************************************************/

#ifndef __ASSEMBLER_HELPER_H__
#define __ASSEMBLER_HELPER_H__

#define MAX_INSTRUCTION_LENGTH 83 /* max_line + '\n' + '\0' + 1 
                extra we use it as safty for strtok and string manipulation */
#define UNUSED(x) (void)x
#define DEAD_BEEF (void *)0xDEADBEEF 
#define OP_SIZE 13 /* 12 + '\0' */
#define MAX_INT 4095 /* 2^12 -1     in bits 011111111111*/
#define MIN_INT -4096 /* 2^12 * -1  in bits 111111111111*/
#define MAX_INT_WITH_ARE 1023 /* 2^10 - 1*/
#define MIN_INT_WITH_ARE -1024 /* 2^10 * -1*/
/* The first 2 bits are used as ARE*/
#define MAX_LANE 1023 /* 2^10 - 1*/
#define MIN_LANE -1024 /* 2^10 * -1*/

#endif /* __ASSEMBLER_HELPER_H__ */

