/*************************************************************************                 
*   Orginal Name : logger.h                                              *
*   Name: Ido Sabach                                                     *
*   Date : 24.6.23                                                       *
*   Info : This is th looger of the project is used to log all the       *
*   error and warning from the assembler, and print it back to the user. *
*************************************************************************/

#ifndef __LOGGER_H__
#define __LOGGER_H__

#include "stddef.h" /* size_t */

typedef struct logger logger_t;

typedef enum {
    LG_SUCCESS = 0,
    LG_NO_MEMORY = 1,
    LG_FAIL = 2
    }logger_status_t;

/*******************************************
* DESCRIPTION: 
*       Creates the logger.
* PARAM:
* RETURN:
*       SUCCESS: A logger ponter.
*       FAIL: NULL (low memory).
* BUGS:
******************************************/
logger_t *CreateLogger(void);

/*******************************************
* DESCRIPTION: 
*       Destroys the logger.
* PARAM:
*       logger: A pointer to the logger.
* RETURN:
*       void
* BUGS:
*    if passed invalid pointer, result is underfined.
******************************************/
void DestroyLogger(logger_t *logger);

/*******************************************
* DESCRIPTION: 
*       check if logger is empty.
* PARAM:
*       logger: A pointer to the logger.
* RETURN:
*       1 if empty. else 0.
* BUGS:
*    if passed invalid pointer, result is underfined.
******************************************/
int LoggerIsEmpty(logger_t *logger);

/*******************************************
* DESCRIPTION: 
*       Adds the msg to the logger with line number and file name.
* PARAM:
*       logger: A pointer to the logger.
*       filename: The pointer to the file name.
*       msg: A pointer to the message.
*       line: The line nimer the error was found.
* RETURN:
*       SUCCESS: SUCCESS
*       FAIL: FAIL(low memory).
* BUGS:
*    if passed invalid pointer, result is underfined.
* NOTICE:
*    If the message is empty, nothing will be empty.
******************************************/
logger_status_t AddLog(logger_t *log, const char *filename, 
                                                const char *msg, size_t line);

/*******************************************
* DESCRIPTION: 
*       Prints the log to the console.
*       in the end the log will be empty but not destroyed!.
* PARAM:
*       logger: A pointer to the logger.
* RETURN:
*       void.
* BUGS:
*    if passed invalid pointer, result is underfined.
******************************************/
void PrintAllLogs(logger_t *logger);

#endif /* LOGGER_H */
