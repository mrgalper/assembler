/*******************************************************************************                 
*   Orginal Name :slist.h                                                      *
*   Name: Ido Sabach                                                           *
*   Reviewer : Ahron Cohen                                                     *
*   Date 23.6.23                                                               *
*   Info : Decleration of ADT Singel Linked List function and function pointers*
*******************************************************************************/
#ifndef __SLINKED_LIST_H__
#define __SLINKED_LIST_H__

#include <stddef.h> /* size_t */

typedef struct slist slist_t;
typedef struct node *slist_itr_t;

/* Both function pointers in the next line will recives a param type param that we will
put in the function ,the implementation of the function is on you ,
the user should not be concernd with the void *data type */

/* A boolean function that returns 1 if matched else 0 Used in SListFind*/
typedef int (*is_match_t)(const void *data, void *params);
/*Function pointer that is used as argument to SListForEach */
typedef int (*action_t)(void *data, void *params);

/*****************************************************************************
*    DESCRIPTION: Creates a ADT Single link list 
*    PARAM: none
*    RETURN: A pointer to the LinkList
*    BUGS: if malloc fails return Null
******************************************************************************/
slist_t *SlistCreate(void);

/*****************************************************************************
*    DESCRIPTION: Destroy all alocation on the heap 
*    PARAM: SLL type that has been initialized
*    RETURN: none.
*    BUGS: If the input is of diffrent type or null the result is undefined 
******************************************************************************/
void SlistDestroy(slist_t *slist); 

/*****************************************************************************
*    DESCRIPTION: Insert the data before the position (varible) 
*    PARAM: position - is iterator that pointes the desirable location ,
*    data - is a pointer to the data we want to insert.
*    RETURN: SUCCESS:A pointer iterator to the next data
*            FAIL: return TAIL iterator.
*    BUGS: 
******************************************************************************/
slist_itr_t SlistInsertBefore(slist_itr_t position, void *data); 

/*****************************************************************************
*    DESCRIPTION: return a pointer to the start of the SLL.
*    PARAM:  a pointer to the SSL
*    RETURN: iterate to the start of SLL.
*    BUGS: If used on uninitialized list or other pointer will cause undefined behaviour.
******************************************************************************/
slist_itr_t SlistBegin(const slist_t *slist);

/*****************************************************************************
*    DESCRIPTION: return an iterator the end of the SLL, THis is used only as pararmenter 
*    in function SListForEach and Find , do not use otherwise !.
*    PARAM: a pointer to the SSL
*    RETURN: A iterator to the end 
*    BUGS: none 
******************************************************************************/
slist_itr_t SlistEnd(const slist_t *slist);

/*****************************************************************************
*    DESCRIPTION: Increase the iterator by one. 
*    PARAM: A poiter to the SSL.
*    RETURN: A iterator to the next iterator
*    BUGS:if you're pass is equal to the SlistEnd the result is undefined .
******************************************************************************/
slist_itr_t SlistNext(const slist_itr_t iterator);

/*****************************************************************************
*    DESCRIPTION: removes the current itereitor from the list 
*    PARAM: the iterator that is desired to erase.
*    RETURN: The next itaretor .
*    BUGS: if you're pass is equal to the SlistEnd the result is undefined .
******************************************************************************/
slist_itr_t SlistRemove(slist_itr_t iterator); 

/*****************************************************************************
*    DESCRIPTION:set a data at position iterator (will overate the data). 
*    PARAM:  iterator - represent the desired iteratir to change the value ,
*    data - a pointer to the desired data .
*    RETURN: none .
*    BUGS: if you're pass is equal to the SlistEnd the result is undefined . 
******************************************************************************/
void SlistSetData(slist_itr_t iterator, void *data);

/*****************************************************************************
*    DESCRIPTION:will get the data at position iterator .
*    PARAM: the iterator you want to get the data from .
*    RETURN: void pointer to the desired data.
*    BUGS:if used on last iterator the result is undefined .
******************************************************************************/
void *SlistGetData(const slist_itr_t iterator);

/*****************************************************************************
*    DESCRIPTION: RChack the size of the SLL
*    PARAM: A pointer to SLL that want to check.
*    RETURN: a positibve number of the size , if return 0 the size is 0.
*    BUGS: if passed invalid slist the reult is undefined .
******************************************************************************/
size_t SlistSize(const slist_t *slist);

/*****************************************************************************
*    DESCRIPTION: check if the SLL is empty.
*    PARAM: A pointer to the SLL.
*    RETURN: 1 if empt or 0 ,respectivly.
*    BUGS:if the SSLL is NULL the result is undefined .
******************************************************************************/
int SlistIsEmpty(const slist_t *slist);

/*****************************************************************************
*    DESCRIPTION: Will check if to iterators are equall.
*    PARAM:  two iterators that are desired to check.
*    RETURN: 1 if matched or 0 ,respectivly.
*    BUGS: if the passed iterators are NULL result is negetive (0).
******************************************************************************/
int SlistIterIsEqual(const slist_itr_t iterator1, 
                                            const slist_itr_t iterator2);

/*****************************************************************************
*    DESCRIPTION: for each operation from iterator from till to not included , 
*    will implement the 
*    action fumc on each data .
*    PARAM: 
*    iterator to and iterator from are the tart of the data till the end ,
*    action_func a pointer to a function returns 0 is successed always !,
*    param will be the paramter passed to the action func.
*    RETURN: will return the RETURN of the functioon.
*    if return 0 it wfinished successfully otherwise, it stopeed at the problem 
*    place and return the int (error).
*    BUGS: if passed invalid iterators the rsult is undefined , if the action 
*    func will return other then
*    0 on success will sop on the firts iterator.
******************************************************************************/
int SlistForEach(slist_itr_t from, slist_itr_t to, 
                                            action_t action_func, void *param);

/*****************************************************************************
*    DESCRIPTION:  will use the match func between from till to iteratorts 
*     exclude to, with the desired param.
*    PARAM: 
*    match_func - implemntation off a matching function of void param to a data 
*    in the iterator.
*    (suggestion : use type casting for no problems ),
*    iterator to and iterator from are the tart of the data till the end ,
*    param - will be the param you are passing to comper function.
*    RETURN: will return the the iterator to the wanted data if found ,
*    otherwise return iterator pointed to to .
*    BUGS: if passed invalid iterators the rsult is undefined .
******************************************************************************/
slist_itr_t SlistFind(slist_itr_t from, slist_itr_t to, 
                                         is_match_t match_func, void *param);

#endif /* __SLINKED_LIST_H__ */

