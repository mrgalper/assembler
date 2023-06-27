/******************************************************************************
*   Orginal Name : slist.c                                                    *
*   Name: Mark Galperin                                                       *
*   Date 23.6.23                                                              *
*   Info : Defenition of ADT Singel Linked List function                      *
******************************************************************************/
#include <stdlib.h> /*malloc, free */
#include <assert.h> /*assert */
#include "slist.h" /* NULL, size_t */

typedef struct node node_ty;

struct node
{
    void *data;
    node_ty *next;
};

#define DEAD_BEEF (node_ty *)0xDEADBEEF 

struct slist 
{
    node_ty *head;
    node_ty *tail;
};

static void UpdateTail(slist_ty *slist , node_ty *new_tail)
{
    slist->tail = new_tail;  
}

static node_ty *CreateNode(void *data, node_ty *next_node)
{
   node_ty *new_node = malloc(sizeof(node_ty));
   if (NULL == new_node)
   {
        return (NULL);
   }

   new_node->data =data;
   new_node->next = next_node;
   
   return (new_node);
}

slist_ty *SlistCreate(void)
{
    slist_ty *slist = malloc(sizeof(struct slist));
    node_ty *dummy = CreateNode(slist, DEAD_BEEF);
    if (NULL == slist)
    {
        return (NULL);
    }
    if (NULL == dummy)
    {
        free(slist);
        return (NULL);
    }
    slist->head = dummy;
    slist->tail = slist->head;

    return (slist);
}


void SlistDestroy(slist_ty *slist)
{ 
    node_ty *temp = slist->head;
    node_ty *free_ptr = temp;
    
    assert(NULL != slist);
    
    for ( ;(node_ty *)DEAD_BEEF != temp->next; )
    {
        free_ptr = temp;
        temp = temp->next;
        free(free_ptr);
    }
    free(temp);
    free(slist);
}

slist_itr_ty SlistInsertBefore(slist_itr_ty position, void *data)
{
    node_ty *new_node = CreateNode(position->data,position->next);
    if (NULL == new_node)
    {
        while (DEAD_BEEF != position->next)
        {
            position = SlistNext(position);
        }
        return (position);
    }    
    assert(NULL != position);
    
    position->next = new_node;
    position->data = data;

    if (DEAD_BEEF == new_node->next)
    {
       UpdateTail(new_node->data, new_node);
    }
    
    return (position);
}

slist_itr_ty SlistBegin(const slist_ty *slist)
{
    assert(NULL != slist);

    return (slist->head);
}

slist_itr_ty SlistEnd(const slist_ty *slist)
{
    assert(NULL != slist);
    
    return (slist->tail);
}

slist_itr_ty SlistNext(const slist_itr_ty iterator)
{
    assert(NULL != iterator);

    return (iterator->next);
}

slist_itr_ty SlistRemove(slist_itr_ty iterator) 
{
    node_ty *temp_node = NULL;
    assert(NULL != iterator);
    assert(DEAD_BEEF != iterator->next);

    temp_node = iterator->next;
    iterator->next = temp_node->next;
    iterator->data = temp_node->data;

    if (DEAD_BEEF == iterator->next)
    {
        UpdateTail(iterator->data, iterator);
    }

    free(temp_node);
    return (iterator);
}

void SlistSetData(slist_itr_ty iterator, void *data)
{
    assert(NULL != iterator);
    assert(NULL != data);
    assert(DEAD_BEEF != iterator->next);

    iterator->data = data;
}

void *SlistGetData(const slist_itr_ty iterator)
{
    assert(NULL != iterator);
    assert(DEAD_BEEF != iterator->next);
    
    return (iterator->data);
}

size_t SlistSize(const slist_ty *slist)
{
    size_t node_count = 0;
    node_ty *node = SlistBegin(slist);
    
    assert(NULL != slist);
    
    for ( ; DEAD_BEEF != node->next; ++node_count)
    {
        node = SlistNext(node);
    }

    return (node_count);
}

int SlistIsEmpty(const slist_ty *slist)
{
    assert(NULL != slist);

    return (slist->head == slist->tail);
}

int SlistIterIsEqual(const slist_itr_ty iterator1, const slist_itr_ty iterator2)
{
    assert(NULL != iterator1);
    assert(NULL != iterator2);

    return (iterator1 == iterator2);
}

int SlistForEach(slist_itr_ty from, slist_itr_ty to, action_ty action_func, void *param)
{ 
    int return_value = 0;

    assert(NULL != from);
    assert(NULL != to);

    while (from != to && 0 == return_value)
    {
        return_value = action_func(from->data, param);
        from = SlistNext(from);
    }
    
    return (return_value);
}

slist_itr_ty SlistFind(slist_itr_ty from, slist_itr_ty to, is_match_ty match_func, void *param)
{ 
    assert(NULL != from);
    assert(NULL != to);

    while ((from != to) && (0 == match_func(from->data, param)))
    {
         from = SlistNext(from);
    }
    
    return (from);
}
