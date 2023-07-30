/*************************************************************************                 
*   Orginal Name : first_pass.h                                          *
*   Name: Ido Sabach                                                     *
*   Date : 7.7.23                                                        *
*   Info : This header containts all the first pass error and the the    *
*   function .                                                           *
*************************************************************************/

#ifndef __FIRST_PASS_H__
#define __FIRST_PASS_H__

#include "assembler_metadata.h" 

typedef enum first_pass_error {
    FS_SUCCESS = 0,
    FS_NO_MEMORY = 1,
    FS_FAIL
}first_pass_status_t;


/***********************************************************************
*   Function: FirstPass
*   Description: This function is used to perform the first pass of the 
*                assembler.
*   Input: metadata - the metadata of the assembly.
*   Output: 
*   Return:
***********************************************************************/
first_pass_status_t FirstPass(as_metadata_t *metadata);


#endif /* __FIRST_PASS_H__ */
