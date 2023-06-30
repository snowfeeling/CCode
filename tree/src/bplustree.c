
// SPDX-License-Identifier: GPL-2.0-or-later.
/*
 *  src/bplustree.c
 *
 *  Copyright (C) 2022,  Wangss(Wang Shu Song)
 *  
 * Created by Wangss on 2022-10-30 17:08
 * To study the BPlus Tree.
 * 
 */
#include "../inc/bplustree.h"

/*=============================================================================
* Global variable
*
*/
int tree_degree = DEFALUT_TREE_DEGREE;

static BPLUS_TREE bptree;
static BPT_Node *queue = NULL;

static void usage();
static size_t get_current_time( char * time_info);

static void destroy_bptree_nodes(BPT_Node *root);
static BPT_Node *destroy_bptree(BPLUS_TREE *bptree);

/*=============================================================================
* BP Tree functions.
*
*/
static BPT_Node *find_leaf_node_in_bptree(BPT_Node *const root, DATA_RECORD * drp);
static DATA_RECORD * find_leaf_data_in_bptree(BPT_Node *root, DATA_RECORD *drp, BPT_Node ** leaf_out);
static int print_bptree_leaves(BPLUS_TREE bptree);
static void print_bptree(BPT_Node *const root);
void list_bptree_leaves(BPT_Node * const root);

// 
static void find_and_print_record(BPT_Node *const root, DATA_RECORD *drp);

static DATA_RECORD *create_data_record(DATA_RECORD *drp);
static BPT_Node *create_empty_node();
static BPT_Node *create_empty_leaf();
static BPT_Node *create_new_tree(DATA_RECORD *drp);
static BPT_Node *insert_record_to_tree(BPLUS_TREE *tree, DATA_RECORD *drp);
static BPT_Node *insert_into_bptree_node(BPT_Node *np, DATA_RECORD *drp);
static BPT_Node *make_tree_from_file();

/*=============================================================================
* 有关Tree 处理程序定义
*/
static void enqueue(BPT_Node *new_node);
static BPT_Node *dequeue(void);
static int path_to_root(BPT_Node *const root, BPT_Node *child);
int init_tree(BPLUS_TREE *tree);

BPT_Node *insert_repair(BPT_Node *np);
BPT_Node *insert_into_bptree(BPT_Node *np, DATA_RECORD *drp);
BPT_Node *split_bptree(BPT_Node *np);


static void get_and_print_range(BPT_Node *const root, int key_start, int key_end);
static int get_range(BPT_Node *const root, int key_start, int key_end, int returned_keys[], void *returned_pointers[]);

static void ERROR_EXIT(char* str);

/*=============================================================================
* 有关Tree 处理程序
*/
/* Tree初始化
*/
int init_tree(BPLUS_TREE *tree)
{
    tree->max_degreee = tree_degree;
    tree->min_key_num = (tree_degree + 1 ) / 2 -1;
    tree->max_key_num = tree_degree - 1;
    tree->split_index = tree_degree /2 ;
    tree->next_index = 3;
    tree->leaf_num = 0;
    tree->tree_height = 0;
    tree->root =  NULL;

}

/*=============================================================================
* Fundation functions
* 
*/
static void usage()
{
    printf("\nWelcoming to BPlus Tree Demo.\n");
    printf("\tPlease see the README for your usage.\n");
    printf("\tThe default tree degreee is 4. You can change it when the tree is empty.\n\n\n");
}

static void ERROR_EXIT(char* str)
{
    if (bptree.root)
    {
        list_bptree_leaves(bptree.root);
        destroy_bptree(&bptree);
    }

    perror(str);
    exit(EXIT_FAILURE);
}

static size_t get_current_time( char * time_info)
{
    time_t current_time;
    struct tm *ptime;

    time(&current_time);
    ptime = localtime(&current_time);
    size_t result_num = strftime (time_info, 20,"%Y-%m-%d %H:%M:%S",ptime);
    return result_num;
}


static int path_to_root(BPT_Node *const root, BPT_Node *child)
{
    int length = 0;
    BPT_Node *c = child;
    while (c != root)
    {
        c = c->parent;
        length++;
    }
    return length;
}


static void enqueue(BPT_Node *new_node)
{
    BPT_Node *c;
    if (queue == NULL)
    {
        queue = new_node;
        queue->print_point = NULL;
    }
    else
    {
        c = queue;
        while (c->print_point != NULL)
            c = c->print_point;
        c->print_point = new_node;
        new_node->print_point = NULL;
    }
}

/* Get one node from queue.
*  
 */
static BPT_Node *dequeue(void)
{
    BPT_Node *n = queue;
    queue = queue->print_point;
    n->print_point = NULL;
    return n;
}

/* 功能：找到比Key大的叶子节点
*  输入：root, key所在的记录，显示标志
*  返回：叶子节点的指针。
 * 从root开始，找key，直到到叶子。
 * 非叶子节点上，Key的所处的叶子，是在key的右部指针上的。所以，在非叶子节点中，找到比key大的指针。
 */
static BPT_Node *find_leaf_node_in_bptree(BPT_Node *const root, DATA_RECORD * drp)
{
    if (root == NULL)
    {
        return root;
    }
    else
    {
        int i = 0;
        BPT_Node *c = root;
        while (!c->is_leaf)
        {
            for (i = 0; i < c->keys_num; i++)
            {
                //如果Key>=节点上的key，就往下找。直到找到第一个更大的key
                if (drp->key < c->keys[i])
                    break;
            }
            c = (BPT_Node *)c->pointers[i]; //走到下一层去。
        }
        return c;
    }
}

/* 功能：在Tree里面，找是否有有这个Key
*  输入参数： root 和 drp 传入的数据指针。Leaf_out: 找到的可以所在叶子节点的位置指针
*  返回参数：在叶子节点上的数据指针
*/
static DATA_RECORD * find_leaf_data_in_bptree(BPT_Node *root, DATA_RECORD *drp, BPT_Node ** leaf_out)
{
    if (root == NULL)
    {
        if (leaf_out != NULL)
            *leaf_out = NULL;
        return NULL;
    }

    int i = 0;
    BPT_Node *leaf = NULL;

    /* 如果root不为空，先找到叶子节点(包含想要查询的key的范围) 
     */
    leaf = find_leaf_node_in_bptree(root, drp);
    if (leaf_out != NULL)
        *leaf_out = leaf;
    for (i = 0; i < leaf->keys_num; i++)
        if (leaf->keys[i] ==  drp->key )
            break;
    if (i == leaf->keys_num)
        return NULL;  // 如找到最后了，说明叶子节点中没有这个值。返回NULL。
    else
        return (DATA_RECORD *)leaf->leaf[i];
}


/* 功能：从tree中找到一个key所在的记录，并打印出来
*  输入：root，要查找的值，显示标识
*  返回：无
 */
static void find_and_print_record(BPT_Node *const root, DATA_RECORD *drp)
{
    BPT_Node *leaf = NULL;
    DATA_RECORD *r = find_leaf_data_in_bptree(root, drp, NULL);

    if (r == NULL)
    {    
        printf("Not found the key %d.\n", drp->key);
    }
    else
    {
        printf("Record at %p -- key:[%d], \nID:[%s] NAME:[%s] Create Time:[%s].\n",
               r, drp->key, r->id, r->name, r->create_time);
    }
}


/* Function: To list the detail information of all leaves. 
*  Input    : The root of tree.
*  Output   : <None>
*  Return   : <void>
*/
void list_bptree_leaves(BPT_Node * const root)
{
    if (root == NULL)
    {
        printf("The tree is empty.\n");
        return;
    }
    int i;
    BPT_Node * c = root;
    while (! c->is_leaf)
        c= c->pointers[0];
    while (true)
    {
        for (i = 0; i< c->keys_num; i++)
        {
            DATA_RECORD * drp = (DATA_RECORD*) (c->leaf[i]);
            printf("[Key:%4d] [ID:%8s] [Name:%20s] [Create_Time:%20s]\n", drp->key, drp->id, drp->name, drp->create_time);
        }
        if (c->next != NULL)
        {
            c = c->next;
        }
        else
            break;
    }
    printf("\n");
}

/* Just the tree leaves from the leaf level.
*/
static int print_bptree_leaves(BPLUS_TREE bptree)
{
    BPT_Node *r = bptree.root;
    BPT_Node *tmp;
    if (r != NULL)
    {
        tmp = r;
        printf("The Leaves in the tree:\n");
        while (!tmp->is_leaf)
        {
            tmp = tmp->pointers[0];
        }
        while (tmp != NULL)
        {
            printf("[");
            for (int i = 0; i < tmp->keys_num; i++)
            {
                printf(" %d", tmp->keys[i]);
            }    
            printf(" ] ");
            tmp = tmp->next;
        }
        printf("\n");
    }
    return EXIT_SUCCESS;
}

/* Show the tree from the root to leaves.
*/
static void print_bptree(BPT_Node *const root)
{

    BPT_Node *n = NULL;
    int i = 0;
    int rank = 0;
    int new_rank = 0;

    if (root == NULL)
    {
        printf("The tree is empty.\n");
        return;
    }
    queue = NULL;
    enqueue(root);
    while (queue != NULL)
    {
        n = dequeue();
        if (n->parent != NULL && n == n->parent->pointers[0])
        {
            new_rank = path_to_root(root, n);
            if (new_rank != rank)
            {
                rank = new_rank;
                printf("\n");
            }
        }
        printf("[");
        for (i = 0; i < n->keys_num; i++)
        {
            printf("%d ", n->keys[i]);
        }
        if (!n->is_leaf)
            for (i = 0; i <= n->keys_num; i++)
                enqueue(n->pointers[i]);

        printf("] ");
    }
    printf("\n");
}


/* 功能：根据传入的记录，创建一个数据块。
* 输入： 数据指针
* 输出： 返回生成记录的指针。
 */
static DATA_RECORD *create_data_record(DATA_RECORD *drp)
{
    DATA_RECORD *new_record = (DATA_RECORD *)malloc(sizeof(DATA_RECORD));
    if (new_record == NULL)
    {
        ERROR_EXIT("Record creation.");
    }
    else
    {
        new_record->key = drp->key;
        strcpy(new_record->id, drp->id);
        strcpy (new_record->name , drp->name);
        strcpy(new_record->create_time, drp->create_time);
    }
    return new_record;
}

static BPT_Node *create_empty_node()
{
    BPT_Node *new_node;
    new_node = malloc(sizeof(BPT_Node));
    if (new_node == NULL)
        ERROR_EXIT("Node creation.");
    
    /*new_node->keys = malloc((tree_degree - 1) * sizeof(int)); // （阶数-1） 个key
    if (new_node->keys == NULL)
    {
        ERROR_EXIT("New node keys array.");
    }
    new_node->pointers = malloc((tree_degree) * sizeof(void *)); // （阶数） 个key
    if (new_node->pointers == NULL)
    {
        ERROR_EXIT("New node pointers array.");
    }
    */
    new_node->is_leaf = false;
    new_node->keys_num = 0;
    new_node->parent = NULL;
    new_node->next = NULL;
    int i;
    for ( i = 0; i < tree_degree; i++)
    {
        new_node->pointers[i] = NULL;
        new_node->leaf[i] = NULL;
    }
    new_node->pointers[i] = NULL;

    return new_node;
}

/* 创建一个叶子节点。先创立一个Node，然后设置成叶子节点的标识 */
static BPT_Node *create_empty_leaf()
{
    BPT_Node *leaf = create_empty_node();
    leaf->is_leaf = true;
    return leaf;
}

/* 如果是第一次插入，就建立一个新树root  */
static BPT_Node *create_new_tree(DATA_RECORD *drp)
{
    BPT_Node *root = create_empty_leaf();
    DATA_RECORD *lrp = create_data_record(drp);
    root->keys[0] = drp->key;
    root->pointers[0] = NULL;
    root->leaf[0] = lrp;
    root->parent = NULL;
    root->keys_num++;
    root->next = NULL;

    return root;
}

/* Insert one record to tree(from root);
*/
static BPT_Node *insert_record_to_tree(BPLUS_TREE *tree, DATA_RECORD *drp)
{
    if (tree->root == NULL)
    {
        // make a new tree.
        tree->root = create_new_tree(drp);
        return tree->root;
    }
    else
    {
        return insert_into_bptree_node(tree->root, drp);
    }
}
/* To check the node, if the node is overfow, need to repair it.
*/
BPT_Node *insert_repair(BPT_Node *np)
{
	if (np->keys_num <= bptree.max_key_num) 
	{
		return np;
	}
	else if (np->parent == NULL)
	{
		bptree.root = split_bptree(np);
		return (BPT_Node *) (bptree.root);
	}
	else
	{
		BPT_Node *newNode  = split_bptree(np);
		return insert_repair(newNode);
	}			    
}

/* 拆分一个节点
*/
BPT_Node *split_bptree(BPT_Node *np)
{
    if (np->keys_num <= bptree.max_key_num)
    {
        printf("The node is not need to repair.");
        return np;
    }
    
    BPT_Node *rightNode = create_empty_leaf();
	
	int risingNode = np->keys[bptree.split_index];
    int i, parentIndex;
	if (np->parent != NULL)
	{
		BPT_Node *currentParent = np->parent;
		for (parentIndex = 0; parentIndex < currentParent->keys_num + 1 && currentParent->pointers[parentIndex] != np; parentIndex++);
		if (parentIndex == currentParent->keys_num + 1)
		{
			ERROR_EXIT("Couldn't find which child we were!");
		}
		for (i = currentParent->keys_num; i > parentIndex; i--)
		{
			currentParent->pointers[i+1] = currentParent->pointers[i];
			currentParent->keys[i] = currentParent->keys[i-1];
            currentParent->leaf[i] = currentParent->leaf[i-1];
		}
		currentParent->keys_num++;
		currentParent->keys[parentIndex] = risingNode;
		currentParent->pointers[parentIndex+1] = rightNode;
		rightNode->parent = currentParent;
	}
	
	int rightSplit;
	
	if (np->is_leaf)
	{
		rightSplit = bptree.split_index; // If 
		rightNode->next = np->next;
		np->next = rightNode;
	}
	else
	{
		rightSplit = bptree.split_index + 1;
	}
	
	rightNode->keys_num = np->keys_num - rightSplit;

    // Pointer 是比 key_number要多一个的。
	for (int i = rightSplit; i < np->keys_num + 1; i++)
	{
        BPT_Node * tmp = (BPT_Node*) np->pointers[i];
		rightNode->pointers[i - rightSplit] = tmp;

		if (tmp != NULL)
		{
			rightNode->is_leaf = false;
			tmp->parent = rightNode;
            tmp = NULL;
		}
	}
    for (int i = rightSplit; i < np->keys_num; i++)
	{
		rightNode->keys[i - rightSplit] = np->keys[i];
        rightNode->leaf[i - rightSplit] = np->leaf[i];
	}
    // To handle the LEFT node.
	BPT_Node *leftNode = np;
	leftNode->keys_num = bptree.split_index;

	if (np->parent != NULL)
	{
		return np->parent;
	}
	else //			if (tree.parent == null)
	{
		BPT_Node *treeRoot = create_empty_node();
		treeRoot->keys[0] = risingNode;
		treeRoot->pointers[0] = leftNode;
		treeRoot->pointers[1] = rightNode;
		leftNode->parent = treeRoot;
		rightNode->parent = treeRoot;
        treeRoot->keys_num = 1;
		treeRoot->is_leaf = false;
		return treeRoot;
	}
	
}

/* insert one record to one node.
*/
static BPT_Node *insert_into_bptree_node(BPT_Node *np, DATA_RECORD *drp)
{
    BPT_Node * c = np;
    if(c->is_leaf)
    {
        c->keys_num ++;
        int insert_index = c->keys_num  - 1;
        while (insert_index > 0 )
        {
            if (c->keys[insert_index - 1] > drp->key)
            {
                c->keys[insert_index] = c->keys[insert_index  - 1];
                c->pointers[insert_index] = c->pointers[insert_index  - 1];
                c->leaf[insert_index] = c->leaf[insert_index - 1];
                insert_index --;
            }
            else
            {
                break;
            }
        }
        c->keys[insert_index] = drp->key;
        DATA_RECORD *new_leaf = create_data_record(drp);
        c->pointers[insert_index] = NULL;
        c->leaf[insert_index] = new_leaf;
        return insert_repair(c);
    }
    else
    {
        int find_index = 0;
        while ( find_index < c->keys_num )
        {
            if ( c->keys[find_index] < drp->key)
            {
                find_index ++;
            }
            else
                break;
        }
        return insert_into_bptree_node(c->pointers[find_index], drp);
    }
}

/* Make the BPlus Tree from the designated Data file, and print the leaves.
* Input: <void>
* Return: the root pointer (BPT_Node*). 
*/
static BPT_Node *make_tree_from_file()
{
    FILE *fp;
    char * input_file_name = "../data/mytree.txt";

    fp = fopen(input_file_name, "r");
    if ( fp== NULL)
    {
        ERROR_EXIT("Failure to open input file.");
    }
    else
    {
        DATA_RECORD dr;
        int i;
        int count =0;

        while (!feof(fp))
        {
            i = fscanf(fp, "%d %s %s\n", &dr.key, dr.id, dr.name);
            if (i == 3)
            {
                char str1[BUFFER_SIZE];
                get_current_time(dr.create_time);
                sprintf(str1, "[Key:%4d] [ID:%8s] [Name:%20s] [Create_Time: %20s]\n", dr.key, dr.id, dr.name, dr.create_time);
                printf(str1);
                insert_record_to_tree(&bptree, &dr);
                print_bptree(bptree.root);
                printf("\n");
                count ++;
            }
            else
            {   //输入值有问题。废弃掉。
                fclose(fp);
                ERROR_EXIT("Input data error.");
            }
        }
        fclose(fp);
        printf("\n%d records are created.\n", count);
        //print_bptree_leaves(bptree);

        return bptree.root;
    }
}

/*======================================================================================
*/
/* 功能：从tree中找一定范围的值。并显示出来
* 输入：root，开始值，结束值，显示标志
* 返回：无
*/
static void get_and_print_range(BPT_Node *const root, int key_start, int key_end)
{
    int i;
    int array_size = key_end - key_start + 1;
    int returned_keys[array_size];
    void *returned_pointers[array_size];
    int num_found = get_range(root, key_start, key_end, returned_keys, returned_pointers);
    if (!num_found)
        printf("None found.\n");
    else
    {
        for (i = 0; i < num_found; i++)
            printf("[Key: %4d] [ID:%10s] [Name:%20s] [CreateTime:%20s]\n",
                   ((DATA_RECORD *)returned_pointers[i]) ->key,
                   ((DATA_RECORD *)returned_pointers[i])->id,
                   ((DATA_RECORD *)returned_pointers[i])->name,
                   ((DATA_RECORD *)returned_pointers[i])->create_time
                   );
    }
    
}

/* 功能：找到起止key之间的叶子
*  输入：root，起止key，输出标识。找到的key数组、找到的叶子数组
*  返回：找到的数量。
 */
static int get_range(BPT_Node *const root, int key_start, int key_end, int returned_keys[], void *returned_pointers[])
{
    DATA_RECORD dr;    
    dr.key = key_start;
    BPT_Node *n = find_leaf_node_in_bptree(root, &dr);
    if (n == NULL)
        return 0;
    else
    {
        int i;
        int num_found = 0;

        for (i = 0; i < n->keys_num && (n->keys[i] <  key_start); i++)
            ;
       
        if (i == n->keys_num)
            return 0;
        while (n != NULL)
        {
            for (; i < n->keys_num && (n->keys[i] <= key_end); i++)
            {
                returned_keys[num_found] = n->keys[i];
                returned_pointers[num_found] = n->leaf[i];
                num_found++;
            }
            n = n->next;
            i = 0;
        }
        return num_found;
    }
}

/* ============================================================================*/
/*  Destory Tree functions
*/
/* Destory the BPlus noedes of BPlus tree.
*  Input    : root
*  Return   : <void>
*/
static void destroy_bptree_nodes(BPT_Node *root)
{
    int i;
    if (root->is_leaf)
        for (i = 0; i < root->keys_num; i++)
        {
            free(root->leaf[i]);
        }
    else
        for (i = 0; i < root->keys_num + 1; i++)
            destroy_bptree_nodes(root->pointers[i]);
    free(root);
}

static BPT_Node *destroy_bptree(BPLUS_TREE *bptree)
{
    if (bptree->root)
        destroy_bptree_nodes(bptree->root);
    return NULL;
}

/* ============================================================================*/
/*  Deletion functions
*/
static void debug_bptree(char *str);
static int deleteElement (int deletedValue);
static int doDelete (BPT_Node *root, int val);
static BPT_Node *mergeRight (BPT_Node *tree);
static BPT_Node *stealFromRight (BPT_Node *tree, int parentIndex) ;
static BPT_Node *repairAfterDelete (BPT_Node *tree);

static void debug_bptree(char *str)
{
    printf("\n[DEBUG Start:%s]\n", str);
    print_bptree(bptree.root);
    printf("\n[DEBUG Stop :%s]\n", str);
}
/* Function： Delete one record with the key.
*/
static int deleteElement (int deletedValue)
{

	doDelete(bptree.root, deletedValue);
    if (bptree.root)
        if (bptree.root->keys_num == 0)
        {
            bptree.root = bptree.root->pointers[0];
            bptree.root->parent = NULL;
        }
	return 0;				
}


static int doDelete (BPT_Node *root, int val)
{
    BPT_Node *tree;
    tree = root;
	if (tree != NULL)
	{
		int i;
		for (i = 0; i < tree->keys_num && tree->keys[i] < val; i++)
            ;
		if (i == tree->keys_num) /*找到本节点的最后，说明在本节点找不到，*/
		{
			if (!tree->is_leaf) /*如果在本节点找不到，且不是叶子，就找下一层*/
			{
				doDelete(tree->pointers[tree->keys_num], val);
			}
			else
			{	//如果在本节点找不到，且是leaf.说明没找到。
                printf("Not find the key.\n");
            }
		}
		else
        { 
            if (!tree->is_leaf)  // The inner node.
            {
                if ( tree->keys[i] == val)
                {
                    doDelete(tree->pointers[i+1], val);
                }
                else /* tree->keys[i] > val*/
                {
                    doDelete(tree->pointers[i], val);
                }
            }
            else //In the leaf, Tree->key[i] > val or Tree->key[i]== val
            {
                if (tree->keys[i] > val)
                {
                    printf("Not find the key.\n");
                }
                else // "Tree->key[i] == val", find the key and deleting it.
                {
                    // Release the memory.
                    free(tree->leaf[i]);
                    printf("The Key %d is deleted with the memory is released.\n", tree->keys[i]);

                    for (int j = i; j < tree->keys_num - 1; j++)
                    {
                        tree->keys[j] = tree->keys[j+1];
                        tree->leaf[j] = tree->leaf[j+1];
                    }
                    tree->keys_num--;			
                    // Bit of a hack -- if we remove the smallest element in a leaf, then find the *next* smallest element
                    //  (somewhat tricky if the leaf is now empty!), go up our parent stack, and fix index keys
                    if (i == 0 && tree->parent != NULL) // 如果是第一个值
                    {
                        int nextSmallest = 0;
                        BPT_Node *parentNode = tree->parent;
                        int parentIndex;
                        for (parentIndex = 0; parentNode->pointers[parentIndex] != tree; parentIndex++)
                            ;
                        if (tree->keys_num == 0)
                        {
                            if (parentIndex == parentNode->keys_num)
                            {
                                //nextSmallest = -999;
                                nextSmallest = val;
                            }
                            else
                            {
                                nextSmallest = parentNode->pointers[parentIndex+1]->keys[0];			
                            }
                        }
                        else
                        {
                            nextSmallest = tree->keys[0];
                        }
                        while (parentNode != NULL)
                        {
                            if (parentIndex > 0 && parentNode->keys[parentIndex - 1] == val)
                            {
                                parentNode->keys[parentIndex - 1] = nextSmallest;
                                debug_bptree("go up to parent.");
                           }
                            BPT_Node *grandParent = parentNode->parent;
                            for (parentIndex = 0; grandParent != NULL && grandParent->pointers[parentIndex] != parentNode; parentIndex++)
                                ;
                            parentNode = grandParent;                                
                        }                            
                    }
                    repairAfterDelete(tree);
                }
            }
        }            
    }
}

static BPT_Node *mergeRight (BPT_Node *tree) 
{

	BPT_Node *parentNode = tree->parent;
	int parentIndex = 0;
    int i;
    int fromParentIndex;

	for (parentIndex = 0; parentNode->pointers[parentIndex] != tree; parentIndex++)
        ;
	BPT_Node *rightSib = parentNode->pointers[parentIndex+1];

	if (tree->is_leaf)
	{
	}
	else
	{
        tree->keys[tree->keys_num] = parentNode->keys[parentIndex];
        tree->leaf[tree->keys_num] = parentNode->leaf[parentIndex];
	}
	
	fromParentIndex = tree->keys_num;

	for (i = 0; i < rightSib->keys_num; i++)
	{
		int insertIndex =  tree->keys_num + 1 + i;
		if (tree->is_leaf)
		{
			insertIndex -= 1;
		}
		tree->keys[insertIndex] = rightSib->keys[i];
        tree->leaf[insertIndex] = rightSib->leaf[i];
	}
	if (!tree->is_leaf)
	{
		for (i = 0; i <= rightSib->keys_num; i++)
		{
			tree->pointers[tree->keys_num + 1 + i] = rightSib->pointers[i];
			tree->pointers[tree->keys_num + 1 + i]->parent = tree;
		}
		tree->keys_num = tree->keys_num + rightSib->keys_num + 1;
	}
	else
	{
		tree->keys_num = tree->keys_num + rightSib->keys_num;
		tree->next = rightSib->next;
		if (rightSib->next != NULL)
		{        
            //debug_bptree("BPT-D003: Merge Right(Right Sib next != NULL).");
		}
	}
	for (i = parentIndex+1; i < parentNode->keys_num; i++)
	{
		parentNode->pointers[i] = parentNode->pointers[i+1];
		parentNode->keys[i-1] = parentNode->keys[i];
		parentNode->leaf[i-1] = parentNode->leaf[i];
	}
	parentNode->keys_num--;
    free(rightSib);

	if (!tree->is_leaf)
	{
        //debug_bptree("BPT-D004: Merge Right( node is not leaf).");
	}

	return tree;
}

static BPT_Node *stealFromRight (BPT_Node *tree, int parentIndex) 
{
	// Steal from right sibling
	BPT_Node *parentNode = tree->parent;
	BPT_Node *rightSib = parentNode->pointers[parentIndex + 1];
    int i;
	tree->keys_num++;	
	if (tree->is_leaf)
 	{
		tree->keys[tree->keys_num - 1] = rightSib->keys[0];
		tree->leaf[tree->keys_num - 1] = rightSib->leaf[0];
		parentNode->keys[parentIndex] = rightSib->keys[1];
        parentNode->leaf[parentIndex] = rightSib->leaf[1];
	}
	else
	{
		tree->keys[tree->keys_num - 1] = parentNode->keys[parentIndex];
		tree->leaf[tree->keys_num - 1] = parentNode->leaf[parentIndex];
		parentNode->keys[parentIndex] = rightSib->keys[0];
        parentNode->leaf[parentIndex] = rightSib->leaf[0];
	}
	if (!tree->is_leaf)
	{
		tree->pointers[tree->keys_num] = rightSib->pointers[0];
		tree->pointers[tree->keys_num]->parent = tree;
		
		for ( i = 1; i < rightSib->keys_num + 1; i++)
		{
			rightSib->pointers[i-1] = rightSib->pointers[i];								
		}
	}
	for (i = 1; i < rightSib->keys_num; i++)
	{
		rightSib->keys[i-1] = rightSib->keys[i];
        rightSib->leaf[i-1] = rightSib->leaf[i];
	}

	rightSib->keys_num--;

	if (tree->is_leaf)
	{
		if (rightSib->next != NULL)
		{
            //debug_bptree("BPT-D006: stealFromRight( leaf and rightSib->next != NULL).");
		}		
	}
	return tree;
}

static BPT_Node *stealFromLeft (BPT_Node *tree, int parentIndex) 
{
	BPT_Node * parentNode = tree->parent;
	int i;

    tree->keys_num++;
	if (tree->is_leaf && tree->next != NULL)
	{	}
	
	for (i = tree->keys_num - 1; i > 0; i--)
	{
		tree->keys[i] = tree->keys[i-1];
		tree->leaf[i] = tree->leaf[i-1];

	}
	BPT_Node *leftSib = parentNode->pointers[parentIndex -1];
	
	if (tree->is_leaf)
	{
		tree->keys[0] = leftSib->keys[leftSib->keys_num - 1];
		tree->leaf[0] = leftSib->leaf[leftSib->keys_num - 1];
		parentNode->keys[parentIndex-1] = leftSib->keys[leftSib->keys_num - 1];
		parentNode->leaf[parentIndex-1] = leftSib->leaf[leftSib->keys_num - 1];
	}
	else
	{
		tree->keys[0] = parentNode->keys[parentIndex - 1];
		tree->leaf[0] = parentNode->leaf[parentIndex - 1];
		parentNode->keys[parentIndex-1] = leftSib->keys[leftSib->keys_num - 1];				
		parentNode->leaf[parentIndex-1] = leftSib->leaf[leftSib->keys_num - 1];				
	}
	
	if (!tree->is_leaf)
	{
		for ( i = tree->keys_num; i > 0; i--)
		{
			tree->pointers[i] =tree->pointers[i-1];
		}
		tree->pointers[0] = leftSib->pointers[leftSib->keys_num];
		leftSib->pointers[leftSib->keys_num] = NULL;
		tree->pointers[0]->parent = tree;
	}
	
	leftSib->keys_num--;
	
	if (tree->is_leaf)
	{
        //debug_bptree("BPT-D007: stealFromLeft( leaf ).");
    }

	return tree;
}


static BPT_Node *repairAfterDelete (BPT_Node *tree)
{
	if (tree->keys_num < bptree.min_key_num)
	{
		if (tree->parent == NULL) /* Root */
		{
			if (tree->keys_num == 0) /* The root is empty.*/
			{
                BPT_Node * tmp = bptree.root;
			    bptree.root = tree->pointers[0];
				if (bptree.root != NULL)
                {   
                    free(tmp);
					bptree.root->parent = NULL;
                }
			}
		}
		else 
		{
			BPT_Node *parentNode = tree->parent;
            BPT_Node *nextNode;
            int parentIndex = 0;
			for (parentIndex = 0; parentNode->pointers[parentIndex] != tree; parentIndex++)
                ;
			if (parentIndex > 0 && parentNode->pointers[parentIndex - 1]->keys_num > bptree.min_key_num)
			{
				stealFromLeft(tree, parentIndex);				
			}
			else 
            {
                if (parentIndex < parentNode->keys_num && parentNode->pointers[parentIndex + 1]->keys_num > bptree.min_key_num)
                {
                    stealFromRight(tree,parentIndex);
                }
                else 
                {    if (parentIndex == 0)
                    {
                        // Merge with right sibling
                        nextNode = mergeRight(tree);
                        repairAfterDelete(nextNode->parent);			
                    }
                    else
                    {
                        // Merge with left sibling
                        nextNode = mergeRight(parentNode->pointers[parentIndex-1]);
                        repairAfterDelete(nextNode->parent);
                    }
                }
            }
		}
	}
	else 
        if (tree->parent != NULL)
        {	
        }
}

/*=============================================================================
* 有关操作菜单的处理程序
*/
/* 显示菜单和操作界面
*/
void show_bpt_manual(void)
{
    printf("Enter any of the following commands after the prompt > :\n"
           "\ti <key> <ID> <Name> -- Insert the value <Key> (an integer), ID (String) , Name (String).\n"
           "\tf <k>  -- Find the value under key <k>.\n"
           "\tp <k> -- Print the path from the root to key k and its associated value.\n"
           "\tr <k1> <k2> -- Print the keys and values found in the range [<k1>, <k2>\n"
           "\td <k>  -- Delete key <k> and its associated value.\n"
           "\tx -- Destroy the whole tree.  Start again with an empty tree of the same order.\n"
           "\tt -- Show the tree information and Print the B+ tree.\n"
           "\tl -- Print the keys of the leaves (bottom row of the tree).\n"
           "\tv -- Toggle output of pointer addresses (\"verbose\") in tree and leaves.\n"
           "\tq -- Quit. (Or use Ctl-D or Ctl-C.)\n"
           "\t? -- Print this help message.\n");
}

/* 功能：主操作界面
*  返回值：0
*/
static int bpt_manual()
{

    char command;
    bool input_consumed = false;
    char buffer[BUFFER_SIZE];
    DATA_RECORD dr;
    int count = 0;
    int input_key, input_key_2;

    printf("> ");
    while (scanf("%c", &command) != EOF)
    {
        input_consumed= false;
        switch (command)
        {
        case 'd':
            count = scanf("%d", &input_key);
            if (count ==1)
            {
                deleteElement(input_key);
                print_bptree(bptree.root);;
           }
            else
            {
                printf("Input error. Please press ? to get help.\n");
            }
            break;
        case 'i':
            fgets(buffer, BUFFER_SIZE, stdin);
            input_consumed = true;
            count = sscanf(buffer, "%d %s %s", &dr.key, dr.id, dr.name);;
            if (count ==3)
            {
                get_current_time(dr.create_time);
                insert_record_to_tree(&bptree, &dr);
                print_bptree(bptree.root);;
            }
            else
            {                
                printf("Input error. Please press ? to get help.\n");
            }
            break;
        case 'f':
            count = scanf("%d", &dr.key);
            if (count == 1)
            {
                get_current_time(dr.create_time);
                find_and_print_record(bptree.root, &dr);
            }
            else
            {
                printf("Input error. Please press ? to get help.\n");
            }
            break;
        case 'l':
            list_bptree_leaves(bptree.root);
            print_bptree(bptree.root);
            break;
         case 'x':
            bptree.root = destroy_bptree(&bptree);            
            printf("The tree is destoryed.\n");
            break;
        case 't':
            if (bptree.root)
            {
                //get_tree_info(root);
                //show_tree_info();
                //print_tree(root);
            }
            else
                printf("The tree is empty.\n");
            break;
        case 'R':
            bptree.root = destroy_bptree(&bptree);            
            make_tree_from_file();
            break;
        case 'o':
            if (bptree.root)
            {
                printf("The tree is active. Please press \'x\' to release or \'R\' to reload the tree.\n");
             }
            else
            {
                int input_order;
                count = scanf("%d", &input_order);
                if (count == 1)
                    bptree.max_degreee = input_order;
                else
                    printf("Input error. Please press ? to get help.\n");
            }
            break;
        case 'q':           
            if (bptree.root)
            {
                destroy_bptree(&bptree);
                printf("The tree is not empty and is cleaned. Quit safely.\n");
            }
            else
            {
                printf("The tree is empty. Quit safely.\n");
            }
            return EXIT_SUCCESS;
        case 'r':
            if (scanf("%d %d", &input_key, &input_key_2) == 2)
            {
                if (input_key > input_key_2)
                {
                    int tmp = input_key_2;
                    input_key_2 = input_key;
                    input_key = tmp;
                }
                get_and_print_range(bptree.root, input_key, input_key_2);
            }
            else
            {
                printf("Input error. Please press ? to get help.\n");
            }
            break;
        case '\n':
            input_consumed = true;
            break;
        default:
            show_bpt_manual();
            //input_consumed = true;
        }
        if (!input_consumed)
        {
            while ( getchar() != (int)'\n')
                ;
        }
        printf("> ");
    }
    printf("\n");

    return EXIT_SUCCESS;
}

/*=============================================================================
* Main Program.
*/
int test_bplus_tree()
{
    usage();
    init_tree(&bptree);
    make_tree_from_file();
    bpt_manual();
    return EXIT_SUCCESS;
}

