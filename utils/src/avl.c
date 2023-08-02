/******************************************************************************              
*   Orginal Name : avl.c                                                      *
*   Name: Ido Sabach                                                          *
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

typedef struct node node_t;

typedef enum child
{
    EQUAL               = -1,
    LEFT                =  0,
    RIGHT               =  1,
    NUMBER_OF_CHILDREN  =  2
}child_t;

typedef enum
{
    LL = 0,
    RR = 1,
    LR = 2,
    RL = 3,
    BALANCED = 4
}balance_t;

typedef status_t(*order_func)(node_t *node, action_func action, void *params);
typedef void(*balance_func)(node_t *parent_node); 
typedef void(*cmp_balance_func)(balance_t *balance, node_t *parent);

#define UNUSED(x) (void)x

struct node
{
	void *data;
    size_t data_size;
	long height;
	node_t *child[NUMBER_OF_CHILDREN];
};

struct avl
{
    cmp_func cmp_func;
    void *params;
    node_t *root;
};
/*``````````````````````````````````````````
 *          Getters/Setters
 ``````````````````````````````````````````*/
static void *AvlGetData(node_t *node);
static long AvlGetHeight(node_t *node);
static node_t *GetMinValue(node_t *node);
static node_t *AvlGetRoot(const avl_t *avl);
static node_t *AvlGetChildBySide(node_t *node, child_t side_child);
static child_t AvlGetChildSide(const avl_t *avl, node_t *node, void *data);
static child_t AvlGetSide(node_t *node);
static void AvlSetChild(node_t *parent, child_t side_child, node_t *child);
static void AvlSetRoot(avl_t *avl, node_t *node);
static void AvlSetHeight(node_t *node, long height);
static void AvlSetData(node_t *parent, void *data);
/*``````````````````````````````````````````
 *          Helpers For Main Function
 ``````````````````````````````````````````*/
static node_t *AvlCreateNode(void *data, size_t data_size);
static node_t *AvlRemoveHelper(avl_t *avl, node_t *node, void *data, 
                                                            free_func ff);
static void AvlRemoveAllNodes(avl_t *avl, free_func ff);
static int IsLeaf(node_t *parent);
static int HasBothChildren(node_t *parent);
static node_t *AvlFindNode(const avl_t *avl, void *data);
static int SizeHelper(void *data, void *param);
static status_t AvlInsertNode(avl_t *avl, node_t *node, void *data, 
                                                            size_t data_size);
static void TreePrintR(node_t *node, int level);

/*``````````````````````````````````````````
 *          Balance And Height Helpers
 ``````````````````````````````````````````*/
static void AvlHighetCalculator(node_t *node);
static void SwapData(node_t *node1, node_t *node2);
static void AvlReBalance(node_t *parent_node);
static void AvlLRBalance(node_t *parent_node);
static void AvlRRBalance(node_t *parent_node);
static void AvlLLBalance(node_t *parent_node);
static void AvlRLBalance(node_t *parent_node);
static void AvlCmpLL(balance_t *balance, node_t *parent);
static void AvlCmpRR(balance_t *balance, node_t *parent);

/*``````````````````````````````````````````
 *          Tree Traversal order
 ``````````````````````````````````````````*/
static status_t PreOrder(node_t *node , action_func action, void *params);
static status_t PostOrder(node_t *node , action_func action, void *params);
static status_t InOrder(node_t *node , action_func action, void *params);

/*``````````````````````````````````````
 *         APIFUNCTION
 ``````````````````````````````````````*/
avl_t *AvlCreate(cmp_func cmp, void *params)
{
    avl_t *avl = NULL;

    assert(NULL != cmp);

    avl = malloc(sizeof(avl_t));
    if (NULL == avl)
    {
        return (avl);
    }
    avl->cmp_func = cmp;
    avl->params = params;
    avl->root = NULL;

    return (avl);
}

void AvlDestroy(avl_t *avl, free_func ff)
{
    assert(NULL != avl);
    if (FALSE == AvlIsEmpty(avl))
    {
        AvlRemoveAllNodes(avl, ff);
    }
    free(avl);
}

status_t AvlInsert(avl_t *avl, void *data, size_t data_size)
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

void AvlRemove(avl_t *avl, void *data ,free_func ff)
{
    assert(NULL != avl);
    assert(NULL != data);

    AvlSetRoot(avl, AvlRemoveHelper(avl, AvlGetRoot(avl), data, ff));
    if (NULL != AvlGetRoot(avl))
    {
        AvlHighetCalculator(AvlGetRoot(avl));
    }
}

long AvlHeight(const avl_t *avl)
{
    long ret_value = -1;
    
    assert(NULL != avl);
    ret_value = AvlGetHeight(AvlGetRoot(avl));

    return (ret_value);
}
	
size_t AvlSize(const avl_t *avl)
{
    size_t size = 0;

    assert(NULL != avl);

    AvlForEach((avl_t *)avl, SizeHelper, &size, INORDER);

    return (size);    
}

bool_t AvlIsEmpty(const avl_t *avl)
{
    assert(NULL != avl);

    return (NULL == avl->root);
}

void *AvlFind(const avl_t *avl, void *data)
{
    void *node_data = NULL;
    node_t *node = AvlFindNode(avl, data);
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

status_t AvlForEach(avl_t *avl, action_func action, void *params, trav_t trav)
{
    static order_func orders[3] = {InOrder, PreOrder, PostOrder};
    status_t ret_value = AVL_FAIL;

    assert(NULL != action);
    assert(NULL != avl);
    if (!AvlIsEmpty(avl))
    {
        ret_value = orders[trav](avl->root, action, params);
    }
    return (ret_value);
}
#ifndef NDEBUG
void TreePrint(avl_t *avl)
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
static void AvlReBalance(node_t *parent_node)
{ 
    static balance_func rebalance[4] = {AvlLLBalance, AvlRRBalance, 
                                                AvlLRBalance, AvlRLBalance}; 
    static cmp_balance_func cmp[2] = {AvlCmpLL, AvlCmpRR};
    node_t *r_child_node = AvlGetChildBySide(parent_node, RIGHT);
    node_t *l_child_node = AvlGetChildBySide(parent_node, LEFT);
    balance_t balance = LL;
    
    if (AvlGetHeight(r_child_node) > AvlGetHeight(l_child_node))
    {
        balance = RR;
    }
    cmp[balance](&balance, parent_node);
    
    rebalance[balance](parent_node);

}

static void AvlCmpLL(balance_t *balance, node_t *parent)
{    
    node_t *l_child = AvlGetChildBySide(parent, LEFT);
    node_t *l_gran_child = AvlGetChildBySide(l_child, LEFT);
    node_t *r_gran_child = AvlGetChildBySide(l_child, RIGHT);
 
    if (AvlGetHeight(r_gran_child) > AvlGetHeight(l_gran_child))
    {
        *balance = LR;
    }
}

static void AvlCmpRR(balance_t *balance, node_t *parent)
{    
    node_t *r_child = AvlGetChildBySide(parent, RIGHT);
    node_t *l_gran_child = AvlGetChildBySide(r_child, LEFT);
    node_t *r_gran_child = AvlGetChildBySide(r_child, RIGHT);
 
    if (AvlGetHeight(r_gran_child) < AvlGetHeight(l_gran_child))
    {
        *balance = RL;
    }
}


static void AvlLLBalance(node_t *parent_node)
{
    node_t *temp = AvlGetChildBySide(parent_node, RIGHT);
    node_t *child_node = AvlGetChildBySide(parent_node, LEFT);
    node_t *gra_left = AvlGetChildBySide(child_node, LEFT);
    node_t *gra_right = AvlGetChildBySide(child_node, RIGHT);
    
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

static void AvlRRBalance(node_t *parent_node)
{
    node_t *temp = AvlGetChildBySide(parent_node, LEFT);
    node_t *child_node = AvlGetChildBySide(parent_node, RIGHT);
    node_t *gra_right = AvlGetChildBySide(child_node, RIGHT);
    node_t *gra_left =  AvlGetChildBySide(child_node, LEFT);
    
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

static void AvlRLBalance(node_t *parent_node)
{
    node_t *child_node = AvlGetChildBySide(parent_node, RIGHT);
    
    AvlLLBalance(child_node);
    AvlRRBalance(parent_node);
}

static void AvlLRBalance(node_t *parent_node)
{
    node_t *child_node = AvlGetChildBySide(parent_node, LEFT);
    
    AvlRRBalance(child_node);
    AvlLLBalance(parent_node);
}

static void SwapData(node_t *node1, node_t *node2)
{
    void *data_temp = AvlGetData(node1);
    AvlSetData(node1, AvlGetData(node2));
    AvlSetData(node2, data_temp);
}

static void AvlHighetCalculator(node_t *node)
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
static node_t *AvlCreateNode(void *data, size_t data_size)
{
    node_t *new_node = malloc(sizeof(node_t));
    if(NULL == new_node)
    {
        return (NULL);
    }
    new_node->data = (void *)malloc(data_size);
    if (NULL == new_node->data) 
    {
        free(new_node);
        return (NULL);
    }
    memcpy(new_node->data, data, data_size);
    new_node->data_size = data_size;
    new_node->child[LEFT] = NULL;
    new_node->child[RIGHT] = NULL;
    new_node->height = 0;

    return (new_node);
}

static int HasBothChildren(node_t *parent)
{
    int ret_value = 0;

    if (NULL != AvlGetChildBySide(parent, LEFT) && 
            NULL != AvlGetChildBySide(parent, RIGHT))
    {
        ret_value = 1;
    }

    return (ret_value);
}

static void AvlRemoveAllNodes(avl_t *avl, free_func ff)
{
    node_t *current_node = NULL;
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
    free(current_node->data); /* free the node data itself */
    free(current_node);     /* free the node itself*/
}

static int IsLeaf(node_t *parent)
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

static node_t *AvlRemoveHelper(avl_t *avl, node_t *node, void *data, 
                                                                free_func ff)
{
    child_t child_side = EQUAL;
    node_t *runner = NULL;

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
            ff((void *)node->data); /* free the internal data */
            free(node->data);
            free(node);
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

static node_t *AvlFindNode(const avl_t *avl, void *data)
{
    node_t *parent = NULL;
    child_t side_child = 0;
    node_t *ret_node = NULL;

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
    AvlSetRoot((avl_t *)avl, 
            AvlGetChildBySide(AvlGetRoot(avl), side_child)); 
    ret_node = AvlFindNode(avl, data);
    AvlSetRoot((avl_t *)avl, parent);
    
    return (ret_node);
}

static status_t AvlInsertNode(avl_t *avl, node_t *node, void *data, 
                                                            size_t data_size)
{
    child_t child_side = AvlGetChildSide(avl, node, data);
    status_t ret_status = AVL_SUCCESS;
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
static void TreePrintR(node_t *node, int level)
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
static status_t PreOrder(node_t *node , action_func action, void *params)
{
    node_t *left_node = AvlGetChildBySide(node, LEFT);
    node_t *right_node = AvlGetChildBySide(node, RIGHT);
    status_t ret_value = AVL_SUCCESS;

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


static status_t PostOrder(node_t *node , action_func action, void *params)
{
    node_t *left_node = AvlGetChildBySide(node, LEFT);
    node_t *right_node = AvlGetChildBySide(node, RIGHT);
    status_t ret_value = AVL_SUCCESS;

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

static status_t InOrder(node_t *node , action_func action, void *params)
{
    node_t *left_node = AvlGetChildBySide(node, LEFT);
    node_t *right_node = AvlGetChildBySide(node, RIGHT);
    status_t ret_value = AVL_SUCCESS;

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
static child_t AvlGetChildSide(const avl_t *avl, node_t *node, void *data)
{
    int ret_status = avl->cmp_func(AvlGetData(node), data, avl->params);
    if (0 == ret_status)
    {
        return (EQUAL);
    }

    return (0 > ret_status);
}

static node_t *AvlGetChildBySide(node_t *node, child_t side_child)
{
    return (node->child[side_child]);
}

static void AvlSetRoot(avl_t *avl, node_t *node)
{
    avl->root = node;
}

static void AvlSetChild(node_t *parent, child_t side_child, node_t *child)
{
    parent->child[side_child] = child;
}

static void AvlSetData(node_t *parent, void *data)
{
    parent->data = data;
}

static node_t *GetMinValue(node_t *node)
{
    if (NULL == AvlGetChildBySide(node, LEFT))
    {
        return (node);
    }

    return (GetMinValue(AvlGetChildBySide(node, LEFT)));
}

static node_t *AvlGetRoot(const avl_t *avl)
{
    return (avl->root);
}

static void *AvlGetData(node_t *node)
{
    return (node->data);
}

static child_t AvlGetSide(node_t *node)
{
    child_t child_side = LEFT;

    if (NULL == AvlGetChildBySide(node, LEFT))
    {
        child_side = RIGHT;
    }

    return (child_side);
}

static long AvlGetHeight(node_t *node)
{
    if (NULL == node)
    {
        return (-1);
    }

    return (node->height);
}

static void AvlSetHeight(node_t *node, long height)
{
    node->height = height;
}

