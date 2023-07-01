/*******************************************************************************
*   Original Name: assembler_helper.h                                          *
*   Name: Mark Galperin                                                        *
*   Date: 2.7.23                                                               *
*   Info: This contains all the macros for the asembly used to avoid double    *
*   defenitions.                                                               *
*******************************************************************************/

#ifndef __ASSEMBLER_HELPER_H__
#define __ASSEMBLER_HELPER_H__

#define MAX_INSTRUCTION_LENGTH 81 /* max_line + '\0' */
#define UNUSED(x) (void)x
#define DEAD_BEEF (void *)0xDEADBEEF 

#endif /* __ASSEMBLER_HELPER_H__ */
