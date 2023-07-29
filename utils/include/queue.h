/*******************************************************************************                 
*   Orginal Name : ueue.h                                                      *
*   Name: Ido Sabach                                                           *
*   Date 22.6.23                                                               *
*   Info :Decleration of ADT queue functions this simple queue and was designed*
*   for minimum space usage.                                                   *
*******************************************************************************/
#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stddef.h> /* size_t */

typedef struct queue queue_t;

/*****************************************************************************
*    DESCRIPTION: Creates a queue pointer to the queue.
*    PARAM: None.
*    RETURN:  queue pointer of type queue_t, if there is not enough 
*    space will return NULL.
*    BUGS: none.
******************************************************************************/
queue_t *QueueCreate(void);

/*****************************************************************************
*    DESCRIPTION: Destroy all the queue data .
*    PARAM: queue pointer.
*    RETURN: none
*    BUGS: If the queue pointers is invalid the result is undefined.
****************************************************************************/
void QueueDestroy(queue_t *queue); 

/*****************************************************************************
*    DESCRIPTION: Enter the data pointer to the rear of the queue.
*    PARAM: void * to the data , queue pointer.
*    RETURN: return 1 on success and 0 on fail.
*    BUGS:if return 0 that means(no space in you're machine).
****************************************************************************/
int QueueEnqueue(queue_t *queue, void  *data);

/*****************************************************************************
*    DESCRIPTION: pushes out the data from the front(FIFO).
*    PARAM: queue pointer.
*    RETURN: none.
*    BUGS: if the queue is empt or invalid the result is undefined.
****************************************************************************/
void QueueDequeue(queue_t *queue);

/*****************************************************************************
*    DESCRIPTION: return the data at the front ( the data that has 
*    been inputed first).
*    PARAM: const queue pointer .
*    RETURN: void * to the data.
*    BUGS: if the list is empt will return an undefined result. 
****************************************************************************/
void *QueuePeek(const queue_t *queue);

/*****************************************************************************
*    DESCRIPTION: Return The size of the desired queue. 
*    PARAM: queue pointer.
*    RETURN: size_t number that represent the size .
*    BUGS: invalid pointer will result undefined behavior.
****************************************************************************/
size_t QueueSize(const queue_t *queue);

/*****************************************************************************
*    DESCRIPTION: checks if the queue is empty .
*    PARAM: queue pointer .    
*    RETURN: 1 if empty , vice versa .
*    BUGS: invalid pointer will result undefined behavior.
****************************************************************************/
int QueueIsEmpty(const queue_t *queue);

#endif /*__QUEUE_H__ */

