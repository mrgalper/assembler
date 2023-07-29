/*************************************************************************                 
*   Orginal Name : assembler.h                                           *
*   Name: Ido Sabach                                                     *
*   Date : 24.6.23                                                       *
*   Info :                                                               *
*************************************************************************/

#ifndef __ASSEMBLER_H__
#define __ASSEMBLER_H__

typedef enum { 
        AS_SUCCESS = 0,
        FAILED_TO_INITIALIZE_META_DATA = 1,
        FAILED_FIRST_PASS = 2,
        FAILED_SECOND_PASS = 3,
        FAILED_CONVERT_TO_BINARY = 4,  
        AS_NO_MEMORY = 5
        }assembler_status_t;

#endif /* __ASSEMBLER_H__ */
