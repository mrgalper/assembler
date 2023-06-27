/******************************************************************************              
*   Orginal Name : avl.c                                                      *
*   Name: Mark Galperin                                                       *
*   Date 24.6.23                                                              *
*   Info :  Implemtation of Avl.                                              *
******************************************************************************/
#include <assert.h> /* assert */
#include <stddef.h> /* size_t , NULL */
#include <stdlib.h> /* malloc, free */
#include "string.h" /* memcpy */
#ifndef  NDEBUG
#include <stdio.h> /* labs, printf */
#endif 

#include "avl.h" /* AVL API */

typedef struct node node_ty;

typedef enum child
{
    EQUAL               = -1,
    LEFT                =  0,
    RIGHT               =  1,
    NUMBER_OF_CHILDREN  =  2
}child_ty;

typedef enum
{
    LL = 0,
    RR = 1,
    LR = 2,
    RL = 3,
    BALANCED = 4
}balance_ty;

typedef status_ty(*order_func)(node_ty *node, action_func action, void *params);
typedef void(*balance_func)(node_ty *parent_node); 
typedef void(*cmp_balance_func)(balance_ty *balance, node_ty *parent);

#define UNUSED(x) (void)x

struct node
{
	void *data;
    size_t data_size;
	long height;
	node_ty *child[NUMBER_OF_CHILDREN];
};

struct avl
{
    cmp_func cmp_func;
    void *params;
    node_ty *root;
};
/*``````````````````````````````````````````
 *          Getters/Setters
 ``````````````````````````````````````````*/
static void *AvlGetData(node_ty *node);
static long AvlGetHeight(node_ty *node);
static node_ty *GetMinValue(node_ty *node);
static node_ty *AvlGetRoot(const avl_ty *avl);
static node_ty *AvlGetChildBySide(node_ty *node, child_ty side_child);
static child_ty AvlGetChildSide(const avl_ty *avl, node_ty *node, void *data);
static child_ty AvlGetSide(node_ty *node);
static void AvlSetChild(node_ty *parent, child_ty side_child, node_ty *child);
static void AvlSetRoot(avl_ty *avl, node_ty *node);
static void AvlSetHeight(node_ty *node, long height);
static void AvlSetData(node_ty *parent, void *data);
/*``````````````````````````````````````````
 *          Helpers For Main Function
 ``````````````````````````````````````````*/
static node_ty *AvlCreateNode(void *data, size_t data_size);
static node_ty *AvlRemoveHelper(avl_ty *avl, node_ty *node, void *data, 
                                                            free_func ff);
static void AvlRemoveAllNodes(avl_ty *avl, free_func ff);
static int IsLeaf(node_ty *parent);
static int HasBothChildren(node_ty *parent);
static node_ty *AvlFindNode(const avl_ty *avl, void *data);
static int SizeHelper(void *data, void *param);
static status_ty AvlInsertNode(avl_ty *avl, node_ty *node, void *data, 
                                                            size_t data_size);
static void TreePrintR(node_ty *node, int level);

/*``````````````````````````````````````````
 *          Balance And Height Helpers
 ``````````````````````````````````````````*/
static void AvlHighetCalculator(node_ty *node);
static void SwapData(node_ty *node1, node_ty *node2);
static void AvlReBalance(node_ty *parent_node);
static void AvlLRBalance(node_ty *parent_node);
static void AvlRRBalance(node_ty *parent_node);
static void AvlLLBalance(node_ty *parent_node);
static void AvlRLBalance(node_ty *parent_node);
static void AvlCmpLL(balance_ty *balance, node_ty *parent);
static void AvlCmpRR(balance_ty *balance, node_ty *parent);

/*``````````````````````````````````````````
 *          Tree Traversal order
 ``````````````````````````````````````````*/
static status_ty PreOrder(node_ty *node , action_func action, void *params);
static status_ty PostOrder(node_ty *node , action_func action, void *params);
static status_ty InOrder(node_ty *node , action_func action, void *params);

/*``````````````````````````````````````
 *         APIFUNCTION
 ``````````````````````````````````````*/
avl_ty *AvlCreate(cmp_func cmp, void *params)
{
    avl_ty *avl = NULL;

    assert(NULL != cmp);

    avl = malloc(sizeof(avl_ty));
    if (NULL == avl)
    {
        return (avl);
    }
    avl->cmp_func = cmp;
    avl->params = params;
    avl->root = NULL;

    return (avl);
}

void AvlDestroy(avl_ty *avl, free_func ff)
{
    assert(NULL != avl);
    if (FALSE == AvlIsEmpty(avl))
    {
        AvlRemoveAllNodes(avl, ff);
    }
    free(avl);
}

status_ty AvlInsert(avl_ty *avl, void *data, size_t data_size)
{
    assert(NULL != avl);
    assert(NULL != data);

    if (AvlIsEmpty(avl))
    {
        AvlSetRoot(avl,AvlCreateNode(data, data_size));
        if (NULL == AvlGetRoot(avl))
        {
            return (AVL_FAIL);
        }
        return (AVL_SUCCESS);
    } 
    return (AvlInsertNode(avl, AvlGetRoot(avl), data, data_size));
}

void AvlRemove(avl_ty *avl, void *data ,free_func ff)
{
    assert(NULL != avl);
    assert(NULL != data);

    AvlSetRoot(avl, AvlRemoveHelper(avl, AvlGetRoot(avl), data, ff));
    if (NULL != AvlGetRoot(avl))
    {
        AvlHighetCalculator(AvlGetRoot(avl));
    }
}

long AvlHeight(const avl_ty *avl)
{
    long ret_value = -1;
    
    assert(NULL != avl);
    ret_value = AvlGetHeight(AvlGetRoot(avl));

    return (ret_value);
}
	
size_t AvlSize(const avl_ty *avl)
{
    size_t size = 0;

    assert(NULL != avl);

    AvlForEach((avl_ty *)avl, SizeHelper, &size, INORDER);

    return (size);    
}

bool_ty AvlIsEmpty(const avl_ty *avl)
{
    assert(NULL != avl);

    return (NULL == avl->root);
}

void *AvlFind(const avl_ty *avl, void *data)
{
    void *node_data = NULL;
    node_ty *node = AvlFindNode(avl, data);
    if (node == NULL) {
        return NULL;
    }
    node_data = malloc(node->data_size);
    if (NULL == node_data){
        return (void *)0xDEADBEEF;
    }
    memcpy(node_data, node->data, node->data_size);
    
    return node_data;
}

status_ty AvlForEach(avl_ty *avl, action_func action, void *params, trav_ty trav)
{
    static order_func orders[3] = {InOrder, PreOrder, PostOrder};
    status_ty ret_value = AVL_FAIL;

    assert(NULL != action);
    assert(NULL != avl);
    if (!AvlIsEmpty(avl))
    {
        ret_value = orders[trav](avl->root, action, params);
    }
    return (ret_value);
}
#ifndef NDEBUG
void TreePrint(avl_ty *avl)
{
    printf("\n----------------------------TREE-----------------------------\n");
    TreePrintR(avl->root, 0);
    printf("\n-------------------------------------------------------------\n");
}
#endif
/*````````````````````````````````````````````````````````
 *
 *                   static functions
 *
 *
 ````````````````````````````````````````````````````````*/
/*``````````````````````````````````````````````````````
 *              ReBalnce And HeightBalance functions
 * ````````````````````````````````````````````````````*/
static void AvlReBalance(node_ty *parent_node)
{ 
    static balance_func rebalance[4] = {AvlLLBalance, AvlRRBalance, 
                                                AvlLRBalance, AvlRLBalance}; 
    static cmp_balance_func cmp[2] = {AvlCmpLL, AvlCmpRR};
    node_ty *r_child_node = AvlGetChildBySide(parent_node, RIGHT);
    node_ty *l_child_node = AvlGetChildBySide(parent_node, LEFT);
    balance_ty balance = LL;
    
    if (AvlGetHeight(r_child_node) > AvlGetHeight(l_child_node))
    {
        balance = RR;
    }
    cmp[balance](&balance, parent_node);
    
    rebalance[balance](parent_node);

}

static void AvlCmpLL(balance_ty *balance, node_ty *parent)
{    
    node_ty *l_child = AvlGetChildBySide(parent, LEFT);
    node_ty *l_gran_child = AvlGetChildBySide(l_child, LEFT);
    node_ty *r_gran_child = AvlGetChildBySide(l_child, RIGHT);
 
    if (AvlGetHeight(r_gran_child) > AvlGetHeight(l_gran_child))
    {
        *balance = LR;
    }
}

static void AvlCmpRR(balance_ty *balance, node_ty *parent)
{    
    node_ty *r_child = AvlGetChildBySide(parent, RIGHT);
    node_ty *l_gran_child = AvlGetChildBySide(r_child, LEFT);
    node_ty *r_gran_child = AvlGetChildBySide(r_child, RIGHT);
 
    if (AvlGetHeight(r_gran_child) < AvlGetHeight(l_gran_child))
    {
        *balance = RL;
    }
}


static void AvlLLBalance(node_ty *parent_node)
{
    node_ty *temp = AvlGetChildBySide(parent_node, RIGHT);
    node_ty *child_node = AvlGetChildBySide(parent_node, LEFT);
    node_ty *gra_left = AvlGetChildBySide(child_node, LEFT);
    node_ty *gra_right = AvlGetChildBySide(child_node, RIGHT);
    
    SwapData(parent_node, child_node);
    AvlSetChild(parent_node, LEFT , gra_left);
    AvlSetChild(parent_node, RIGHT, child_node);
    AvlSetChild(child_node, RIGHT, temp);
    AvlSetChild(child_node, LEFT, gra_right);
    
    AvlHighetCalculator(child_node);
    if (NULL != gra_left)
    {
        AvlHighetCalculator(gra_left);
    }
    AvlHighetCalculator(parent_node);
}

static void AvlRRBalance(node_ty *parent_node)
{
    node_ty *temp = AvlGetChildBySide(parent_node, LEFT);
    node_ty *child_node = AvlGetChildBySide(parent_node, RIGHT);
    node_ty *gra_right = AvlGetChildBySide(child_node, RIGHT);
    node_ty *gra_left =  AvlGetChildBySide(child_node, LEFT);
    
    SwapData(parent_node, child_node);
    AvlSetChild(parent_node, RIGHT , gra_right);
    AvlSetChild(parent_node, LEFT, child_node);
    AvlSetChild(child_node, LEFT, temp);
    AvlSetChild(child_node, RIGHT,gra_left);

    AvlHighetCalculator(child_node);
    if (NULL != gra_right)
    {
        AvlHighetCalculator(gra_right);
    }
    AvlHighetCalculator(parent_node);
    
}

static void AvlRLBalance(node_ty *parent_node)
{
    node_ty *child_node = AvlGetChildBySide(parent_node, RIGHT);
    
    AvlLLBalance(child_node);
    AvlRRBalance(parent_node);
}

static void AvlLRBalance(node_ty *parent_node)
{
    node_ty *child_node = AvlGetChildBySide(parent_node, LEFT);
    
    AvlRRBalance(child_node);
    AvlLLBalance(parent_node);
}

static void SwapData(node_ty *node1, node_ty *node2)
{
    void *data_temp = AvlGetData(node1);
    AvlSetData(node1, AvlGetData(node2));
    AvlSetData(node2, data_temp);
}

static void AvlHighetCalculator(node_ty *node)
{
    long left_child_height = 
        (NULL != AvlGetChildBySide(node, LEFT)) ? 
            AvlGetHeight(AvlGetChildBySide(node, LEFT)) : -1;
    long right_child_height = 
        (NULL != AvlGetChildBySide(node, RIGHT)) ? 
            AvlGetHeight(AvlGetChildBySide(node, RIGHT)) : -1;
    long diffrence = labs(left_child_height - right_child_height);

     AvlSetHeight(node ,
                ((left_child_height > right_child_height) 
            ? left_child_height + 1 : right_child_height + 1));

    if (2 <= diffrence)
    {
        AvlReBalance(node);
    }
}

/*``````````````````````````````````````
 *          AVL helpers
 ``````````````````````````````````````*/
static node_ty *AvlCreateNode(void *data, size_t data_size)
{
    node_ty *new_node = malloc(sizeof(node_ty));
    if(NULL == new_node)
    {
        return (NULL);
    }
    new_node->data = (void *)malloc(data_size);
    memcpy(new_node->data, data, data_size);
    new_node->data_size = data_size;
    new_node->child[LEFT] = NULL;
    new_node->child[RIGHT] = NULL;
    new_node->height = 0;

    return (new_node);
}

static int HasBothChildren(node_ty *parent)
{
    int ret_value = 0;

    if (NULL != AvlGetChildBySide(parent, LEFT) && 
            NULL != AvlGetChildBySide(parent, RIGHT))
    {
        ret_value = 1;
    }

    return (ret_value);
}

static void AvlRemoveAllNodes(avl_ty *avl, free_func ff)
{
    node_ty *current_node = NULL;
    assert(NULL != avl);

    current_node = AvlGetRoot(avl);
    if (NULL != AvlGetChildBySide(current_node, LEFT))
    {
        AvlSetRoot(avl, AvlGetChildBySide(current_node,LEFT));
        AvlRemoveAllNodes(avl, ff);
        AvlSetChild(current_node, LEFT, NULL);
    }
    if (NULL != AvlGetChildBySide(current_node, RIGHT))
    { 
        AvlSetRoot(avl, AvlGetChildBySide(current_node,RIGHT));
        AvlRemoveAllNodes(avl, ff);
        AvlSetChild(current_node, RIGHT, NULL);
    }

    ff((void *)current_node->data); /* free the internal data */
    free(current_node);     
}

static int IsLeaf(node_ty *parent)
{
    int ret_value = 0;

    if (NULL == AvlGetChildBySide(parent, LEFT) && 
            NULL == AvlGetChildBySide(parent, RIGHT))
    {
        ret_value = 1;
    }

    return (ret_value);
}

static int SizeHelper(void *data, void *param)
{
    UNUSED(data);

    *(size_t *)param += 1;
    return (0);
}

static node_ty *AvlRemoveHelper(avl_ty *avl, node_ty *node, void *data, 
                                                                free_func ff)
{
    child_ty child_side = EQUAL;
    node_ty *runner = NULL;

    if (NULL == node)
    {
        return (NULL);
    }
    child_side = AvlGetChildSide(avl, node, data);
    
    if (EQUAL != child_side)
    {
        AvlSetChild(node, child_side, 
            AvlRemoveHelper(avl, AvlGetChildBySide(node, child_side), data, ff));
    }
    else
    {
        if (IsLeaf(node))
        {
            free(node);
            ff((void *)node->data); /* free the internal data */
            free(node->data);
            node = NULL;
        }
        else if(HasBothChildren(node))
        {
            runner = GetMinValue(AvlGetChildBySide(node, RIGHT));
            AvlSetData(node, AvlGetData(runner));
            AvlSetChild(node, RIGHT, 
                    AvlRemoveHelper(avl, AvlGetChildBySide(node, RIGHT), 
                                                    AvlGetData(runner), ff));
        }
        else
        {
            runner = node;
            node = AvlGetChildBySide(node, AvlGetSide(node));
            ff((void *)runner->data); /* free the internal data */
            free(runner->data);
            free(runner);
        }
    }
    if (NULL != node)
    {
        AvlHighetCalculator(node);
    }

    return (node);
}

static node_ty *AvlFindNode(const avl_ty *avl, void *data)
{
    node_ty *parent = NULL;
    child_ty side_child = 0;
    node_ty *ret_node = NULL;

    assert(NULL != avl);
    assert(NULL != data);
    
    parent = AvlGetRoot(avl);
    if (NULL == parent)
    {
        return (NULL);
    }
    side_child = AvlGetChildSide(avl, parent, data);
    if (EQUAL == side_child)
    {
        return (parent);
    }
    AvlSetRoot((avl_ty *)avl, 
            AvlGetChildBySide(AvlGetRoot(avl), side_child)); 
    ret_node = AvlFindNode(avl, data);
    AvlSetRoot((avl_ty *)avl, parent);
    
    return (ret_node);
}

static status_ty AvlInsertNode(avl_ty *avl, node_ty *node, void *data, 
                                                            size_t data_size)
{
    child_ty child_side = AvlGetChildSide(avl, node, data);
    status_ty ret_status = AVL_SUCCESS;
    if (EQUAL == child_side)
    {
        return (AVL_FAIL);
    }
    else if (NULL == AvlGetChildBySide(node, child_side))
    {    
        AvlSetChild(node, child_side, AvlCreateNode(data, data_size));
        if (NULL == AvlGetChildBySide(node, child_side))
        {
            return (AVL_FAIL);
        }           
        AvlHighetCalculator(node);
        return (AVL_SUCCESS);
    }
    ret_status = AvlInsertNode(avl, AvlGetChildBySide(node, child_side), data, data_size);
    if (AVL_FAIL == ret_status)
    {
        return (AVL_FAIL);
    }
    AvlHighetCalculator(node);

    return (ret_status);
}
#ifndef NDEBUG
static void TreePrintR(node_ty *node, int level)
{
    int i = 0;
    if (node == NULL)
    {
        return;
    }
     
    level += MAX_HEIGHT;
 
    TreePrintR(node->child[RIGHT], level);

    for (i = MAX_HEIGHT; i < level; i++)
    {
        printf("   ");
    }
    
    printf("Num: %d H:%ld\n",  *(int *)node->data, node->height);
 
    TreePrintR(node->child[LEFT], level);
}
#endif
/*``````````````````````````````````````````````````````````````````
 *              Tree Traversa
 ````````````````````````````````````````````````````````````````````*/
static status_ty PreOrder(node_ty *node , action_func action, void *params)
{
    node_ty *left_node = AvlGetChildBySide(node, LEFT);
    node_ty *right_node = AvlGetChildBySide(node, RIGHT);
    status_ty ret_value = AVL_SUCCESS;

    if (0 != action(AvlGetData(node), params))
    {
        return (AVL_FAIL);
    }
    if (NULL != left_node)
    {
        ret_value = PreOrder(left_node, action, params);
        if (ret_value != AVL_SUCCESS) {
            return (ret_value);
        }
    }
    if (NULL != right_node)
    {
        ret_value = PreOrder(right_node, action, params);
        if (ret_value != AVL_SUCCESS) {
            return (ret_value);
        }
    }

    return (ret_value);
}


static status_ty PostOrder(node_ty *node , action_func action, void *params)
{
    node_ty *left_node = AvlGetChildBySide(node, LEFT);
    node_ty *right_node = AvlGetChildBySide(node, RIGHT);
    status_ty ret_value = AVL_SUCCESS;

    if (NULL != left_node)
    {
        ret_value = PostOrder(left_node, action, params);
        if (ret_value != AVL_SUCCESS) {
            return (ret_value);
        }
    }
    if (NULL != right_node)
    {
        ret_value = PostOrder(right_node, action, params);
        if (ret_value != AVL_SUCCESS) {
            return (ret_value);
        }
    }
    if (0 != action(AvlGetData(node), params))
    {
        return (AVL_FAIL);
    }

    return (ret_value);
}

static status_ty InOrder(node_ty *node , action_func action, void *params)
{
    node_ty *left_node = AvlGetChildBySide(node, LEFT);
    node_ty *right_node = AvlGetChildBySide(node, RIGHT);
    status_ty ret_value = AVL_SUCCESS;

    if (NULL != left_node)
    {
        ret_value = InOrder(left_node, action, params);
        if (ret_value != AVL_SUCCESS) {
            return (ret_value);
        }
    }
    if (0 != action(AvlGetData(node), params))
    {
        return (AVL_FAIL);
    }
    if (NULL != right_node)
    {
        ret_value = InOrder(right_node, action, params);
        if (ret_value != AVL_SUCCESS) {
            return (ret_value);
        }
    }

    return (ret_value);
}

/* ``````````````````````````````````````````````````````````````````````
 *                      Getters/Setters
 ````````````````````````````````````````````````````````````````````````*/
static child_ty AvlGetChildSide(const avl_ty *avl, node_ty *node, void *data)
{
    int ret_status = avl->cmp_func(AvlGetData(node), data, avl->params);
    if (0 == ret_status)
    {
        return (EQUAL);
    }

    return (0 > ret_status);
}

static node_ty *AvlGetChildBySide(node_ty *node, child_ty side_child)
{
    return (node->child[side_child]);
}

static void AvlSetRoot(avl_ty *avl, node_ty *node)
{
    avl->root = node;
}

static void AvlSetChild(node_ty *parent, child_ty side_child, node_ty *child)
{
    parent->child[side_child] = child;
}

static void AvlSetData(node_ty *parent, void *data)
{
    parent->data = data;
}

static node_ty *GetMinValue(node_ty *node)
{
    if (NULL == AvlGetChildBySide(node, LEFT))
    {
        return (node);
    }

    return (GetMinValue(AvlGetChildBySide(node, LEFT)));
}

static node_ty *AvlGetRoot(const avl_ty *avl)
{
    return (avl->root);
}

static void *AvlGetData(node_ty *node)
{
    return (node->data);
}

static child_ty AvlGetSide(node_ty *node)
{
    child_ty child_side = LEFT;

    if (NULL == AvlGetChildBySide(node, LEFT))
    {
        child_side = RIGHT;
    }

    return (child_side);
}

static long AvlGetHeight(node_ty *node)
{
    if (NULL == node)
    {
        return (-1);
    }

    return (node->height);
}

static void AvlSetHeight(node_ty *node, long height)
{
    node->height = height;
}

