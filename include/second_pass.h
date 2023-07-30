/*************************************************************************                 
*   Orginal Name : second_pass.h                                         *
*   Name: Ido Sabach                                                     *
*   Date : 22.7.23                                                       *
*   Info : This header containts all the second pass error and the the   *
*   function .                                                           *
*************************************************************************/

#ifndef __SECOND_PASS_H__
#define __SECOND_PASS_H__

#include "assembler_metadata.h" 

typedef enum second_pass_error {
    SC_SUCCESS = 0,
    SC_NO_MEMORY = 1,
    SC_FAIL
}second_pass_status_t;


/***********************************************************************
*   Function: second_pass
*   Description: This function is used to perform the second pass of the 
*                assembler.
*   Input: metadata - the metadata of the assembly.
*   Return: Fail if not memory or errors else SC_SUCCESS 
***********************************************************************/
second_pass_status_t SecondPass(as_metadata_t *metadata);


#endif /* __SECOND_PASS_H__ */
