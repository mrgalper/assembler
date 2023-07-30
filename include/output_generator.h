/*************************************************************************                 
*   Orginal Name : output_generator.h                                    *
*   Name: Ido Sabach                                                     *
*   Date : 28.7.23                                                       *
*   Info : This header containts the conversion to binary function       *
*          error and the the function .                                  *
*************************************************************************/

#ifndef __OUTPUT_GENERATOR_H__
#define __OUTPUT_GENERATOR_H__

#include "assembler_metadata.h" 

typedef enum output_status {
    OUT_SUCCESS = 0,
    OUT_COULD_NOT_OPEN_FILE = 1,
    OUT_NO_MEMORY = 2,
    OUT_FAILED = 3
}output_status_t;


/***********************************************************************
*   Description: Last step in the asembly convert the output to binary
*                assembler.
*   Input: metadata - the metadata of the assembly.
*   Return: Fail if not memory or errors else SC_SUCCESS 
***********************************************************************/
output_status_t ConvertToBinary(as_metadata_t *mt);

#endif /* __OUTPUT_GENERATOR_H__ */
