/*******************************************************************************                 
*   Orginal Name :queue.c                                                      *
*   Name: Ido Sabach                                                           *
*   Reviewer : Eitan the shark                                                 *
*   Date 10.6.22                                                               *
*   Info : Defenition of ADT queue functions, implemented with SLL functions.  *
*******************************************************************************/
#include <assert.h> /*assert.h */
#include <stdlib.h> /*malloc ,free */

#include "queue.h" /*queue API */
#include "slist.h" /*slist API */

struct queue
{
    slist_t *slist;
};

queue_t *QueueCreate(void)
{
    queue_t *queue = malloc(sizeof(queue_t));
    if (NULL == queue)
    {
        return (NULL);
    }
    queue->slist  = SlistCreate();
    
    return (queue);
}

void QueueDestroy(queue_t *queue)
{
    assert(NULL != queue);

    SlistDestroy(queue->slist);
    queue->slist = NULL;
    free(queue);
}

int QueueEnqueue(queue_t *queue, void  *data)
{
    slist_itr_t iter = NULL;
    slist_itr_t iter1 =NULL;

    assert(NULL != queue);
    assert(NULL != data);
    
    iter1 = SlistEnd(queue->slist);
    iter = SlistInsertBefore(SlistEnd(queue->slist) , data);

    return (SlistIterIsEqual(iter1, iter));
}

void QueueDequeue(queue_t *queue)
{
    assert(NULL != queue);

    SlistRemove(SlistBegin(queue->slist));
}

void *QueuePeek(const queue_t *queue)
{
    assert(NULL != queue);
    
    return (SlistGetData(SlistBegin(queue->slist)));
}

size_t QueueSize(const queue_t *queue)
{
    assert(NULL != queue);
    
    return (SlistSize(queue->slist));
}

int QueueIsEmpty(const queue_t *queue)
{
    assert(NULL != queue);
   
    return (SlistIsEmpty(queue->slist));
}
