/*************************************************************************                 
*   Orginal Name : first_pass.h                                          *
*   Name: Mark Galperin                                                  *
*   Date : 7.7.23                                                        *
*   Info : This header containts all the first pass error and the the    *
*   function .                                                           *
*************************************************************************/

#ifndef __FIRST_PASS_H__
#define __FIRST_PASS_H__

#include "aassembler_metadata.h" 

typedef enum first_pass_error {
    FS_SUCCESS = 0,
    FS_NON_MEMORY = 1,
    FS_FAIL
}first_pass_statut_t;


/***********************************************************************
*   Function: first_pass
*   Description: This function is used to perform the first pass of the 
*                assembler.
*   Input: metadata - the metadata of the assembly.
*   Output: 
*   Return:
***********************************************************************/
first_pass_statis_t firstPass(as_metadata_t *metadata);


#endif /* __FIRST_PASS_H__ */