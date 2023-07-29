/********************************************************************************                 
*   Orginal Name : avl.h                                                        *
*   Name: Ido Sabach                                                            *
*   Date 24.6.23                                                                *
*   Info : Api for AVL tree - AVL is a self-balncing tree ,each element can move*
*   in the tree for the sake of balancing , the balancing happend in O(1) so it *
*   does not change any Complexity here, For traversing the tree you are given  *
*   the opportunity the chose the traversal.                                    *
*   Warning If you Change the Thr data in the ordering the tree is doomed!!!.   *
*   This Tree will not deal with multiply defenition of Equal value.            *
********************************************************************************/
#ifndef __AVL_TREE_H__
#define __AVL_TREE_H__

#include <stddef.h> /* size_t */

typedef enum 
{
    INORDER     = 0,
    PREORDER    = 1,
    POST_ORDER  = 2
}trav_t;

typedef enum
{
	AVL_SUCCESS = 0,
	AVL_FAIL    = 1
}status_t;

typedef enum
{
	FALSE = 0,
	TRUE    = 1
}bool_t;

typedef struct avl avl_t;



#define MAX_HEIGHT 5 /* for TreePrint */

/* 
   A compare function that will be used to determen the order of avl
   it should return 0 if both have the same priorit 
   negetive number if the avl_data is less then user_data
   a positive number if avl_data is bigger
   the param is an option if you want to use in the function 
   avl_data we provid from the tree , and the user data is you're
   data. 
*/
typedef int(*cmp_func)(const void *avl_data,
                       const void *user_data,
                       void *params);

/*
    this is a function of you're implemntation , and it return 0 on AVL_SUCCESS
    else will return a fail.
*/
typedef int(*action_func)(void *data, void *params);

/*
    this is a function of you're implemntation you will get youre data. 
    and will be used to free the data inside allocated.
*/
typedef void(*free_func)(void *data);

/*******************************************
* DESCRIPTION: 
*       Creates the avl.
* PARAM:
*       cmp - a comapraison function.
*       params - the param for cmp func (can be NULL if not used ). 
* RETURN:
*       SUCCESS: A avl ponter.
*       FAIL: NULL (low memory ).
* BUGS:
*       if cmp is NULL wiil result undefined.
* COMPLEXITY:
*   SPACE : O(1)
*   TIME : O(1)
******************************************/
avl_t *AvlCreate(cmp_func cmp, void *params);

/*******************************************
* DESCRIPTION: 
*           Destroy all the inside of the avl and avl itself,
*           do not use if afterwards.
* PARAM:
*       avl - a valid pointer to a avl.
*       ff - a function to free the data.
* RETURN:
*       SUCCESS: void.
*       FAIL: 
* BUGS:
*       if passed invalid pointer or NULL the result is undefined.
* COMPLEXITY:
*   SPACE : O(1)
*   TIME : O(N)
******************************************/
void AvlDestroy(avl_t *avl, free_func ff);

/*******************************************
* DESCRIPTION: 
*      Insert the Data to the avl and rebalance.
* PARAM:
*      avl - a valid pointer to avl.
*      data - a pointer to the data .
*      data_size - the size of the data in bytes.
* RETURN:
*       SUCCESS: AVL_SUCCESS.
*       FAIL: AVL_FAIL(low memory or the data exist) .
* BUGS:
*       if passed invalid pointer or NULL the result is undefined.
*       if passed a NULL data the result is undefined.
* COMPLEXITY:
*   SPACE : O(1)
*   TIME : O(logN)
******************************************/
status_t AvlInsert(avl_t *avl, void *data, size_t data_size);

/*******************************************
* DESCRIPTION: 
*       removes the data from the avl and rebalance if needed.
* PARAM:
*      avl - a valid pointer to avl.
*      data - a pointer to the data .
*      ff - a function to free the data.
* RETURN:
*       SUCCESS: void
*       FAIL: 
* BUGS:
*       if passed invalid pointer or NULL the result is undefined.
*       if passed a NULL data the result is undefined.
* COMPLEXITY:
*   SPACE : O(1)
*   TIME : O(logN)
******************************************/
void AvlRemove(avl_t *avl, void *data, free_func ff);

/*******************************************
* DESCRIPTION:
*           calculates the height of the deepest root.
*           if counts the number of edges till the deepest leef.
*           if the Tree is empt it will return -1.
* PARAM:
*      avl - a valid pointer to avl.
* RETURN:
*       SUCCESS: A number represnt the height.
*       FAIL: 
* BUGS:
*       if passed invalid pointer or NULL the result is undefined.
* COMPLEXITY:
*   SPACE : O(1)
*   TIME : O(1)
******************************************/
long AvlHeight(const avl_t *avl);
	
/*******************************************
* DESCRIPTION: 
*        Calculate the size of the avl.
* PARAM:
*      avl - a valid pointer to avl.
* RETURN:
*       SUCCESS: A number repersent the amount of data stored in the avl.
*       FAIL: 
* BUGS:
*       if passed invalid pointer or NULL the result is undefined.
* COMPLEXITY:
*   SPACE : O(1)
*   TIME : O(N)
******************************************/
size_t AvlSize(const avl_t *avl);

/*******************************************
* DESCRIPTION:
*       Checks if the avl is empty
* PARAM:
*      avl - a valid pointer to avl.
* RETURN:
*       SUCCESS: TRUE if empt , FALSE if not .
*       FAIL: 
* BUGS:
*       if passed invalid pointer or NULL the result is undefined.
* COMPLEXITY:
*   SPACE : O(1)
*   TIME : O(1)
******************************************/
bool_t AvlIsEmpty(const avl_t *avl);

/*******************************************
* DESCRIPTION:
*      Search for Exiests off the data via cmp func.
*     it allocates and copy to a data on the heap you mush deallocate it .
* PARAM:
*      avl - a valid pointer to avl.
*      data - a pointer to the data .      
* RETURN:
*       SUCCESS: AVL_SUCCESS if found, AVL_FAIL if not. 
*       FAIL: iF malloc failed it return 0xDEADBEEF
* BUGS:
*       if passed invalid pointer or NULL the result is undefined.
*       if passed NULL data the result is undefined.   
* COMPLEXITY:
*   SPACE : O(1)
*   TIME : O(1)
******************************************/
void *AvlFind(const avl_t *avl, void *data);

/*******************************************
* DESCRIPTION: 
*       On each element in the avl use action function with the params.
*       if the action func faild it will stop the traversing.
* PARAM: 
*       avl - a valid pointer to avl.
*       action_func - a pointer to you;re implemntation of action func.
*       params - a pram to be passed to action_func(can be NULL).
*       trav - A traversal of you're option.
* RETURN:
*       SUCCESS: AVL_SUCCESS - if finished, otherwise AVL_FAIL if action 
*       func returned value that is not 0.
*       FAIL: 
* BUGS:
*       if passed invalid pointer or NULL the result is undefined.
*       if action func is NULL the result is undefined.
* COMPLEXITY:
*   SPACE : O(1)
*   TIME : O(N)
******************************************/
status_t AvlForEach(avl_t *avl, action_func action,
						 void *params, trav_t trav);

#endif /* __AVLTREE_H__ */
