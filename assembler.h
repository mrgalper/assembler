/*************************************************************************                 
*   Orginal Name : assembler.h                                           *
*   Name: Mark Galperin                                                  *
*   Date : 24.6.23                                                       *
*   Info :                                                               *
*************************************************************************/

typedef enum { 
        AS_SUCCESS = 0,
        FAILED_TO_INITIALIZE_META_DATA = 1,
        FAILED_FIRST_PASS = 2,
        FAILED_SECOND_PASS = 3,
        FAILED_CONVERT_TO_BINARY = 4  
        }assembler_status_t;

