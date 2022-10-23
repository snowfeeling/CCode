/*
*   Mytree.c
*   Create by Wangss on 2022-10-19 14:29
*   1) BP Tree
*   2) ...
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "../inc/mytree.h"

//=============================================================================
/* Global Variable */
int tree_order = DEFAULT_ORDER;
bool verbose_output = false;
Tree_Node *root = NULL;
Tree_Node *queue = NULL;
BP_TREE tree;
//=============================================================================
/*  Base Function declarion.
*   For general use.
*/
void ERROR_EXIT(char* str);
void usage();
size_t get_current_time( char * time_info);
int compare_key(int key1, int key2);
Tree_Node *get_data_from_file();
int cut(int length);
void enqueue(Tree_Node *new_node);
Tree_Node *dequeue(void);
int path_to_root(Tree_Node *const root, Tree_Node *child);
Data_Record *make_data_record(Data_Record *drp);
Tree_Node *make_empty_node(void);
Tree_Node *make_empty_leaf(void);
void list_leaves(Tree_Node * const root);
void print_leaves(Tree_Node *const root);
void print_tree(Tree_Node *const root);

//=============================================================================
// 插入tree的函数
int get_index_in_parent(Tree_Node *parent, Tree_Node *left);
Tree_Node *insert_into_new_root(Tree_Node *left, int key, Tree_Node *right);
Tree_Node *insert_into_node(Tree_Node *root, Tree_Node *n, int left_index, int key, Tree_Node *right);
Tree_Node *insert_into_node_after_splitting(Tree_Node *root, Tree_Node *old_node, int left_index, int key, Tree_Node *right);
Tree_Node *insert_into_parent(Tree_Node *root, Tree_Node *left, int key, Tree_Node *right);
Tree_Node *insert_into_leaf_after_splitting(Tree_Node *root, Tree_Node *leaf, int key, Data_Record *pointer);
Tree_Node *insert_into_leaf(Tree_Node *leaf, int key, Data_Record *pointer);
Tree_Node *start_new_tree(Data_Record *drp, Data_Record *pointer);
Tree_Node *insert_into_tree(Tree_Node *root, Data_Record *drp);


//=============================================================================
// 释放tree的函数
void destroy_tree_nodes(Tree_Node *root);
Tree_Node *destroy_tree(Tree_Node *root);

//=============================================================================
// 查找单个Key的函数
void find_and_print_record(Tree_Node *const root, Data_Record *drp, bool verbose);
Tree_Node *find_leaf(Tree_Node *const root, Data_Record * drp, bool verbose);
Data_Record * find_key_in_tree(Tree_Node *root, Data_Record *drp, bool verbose, Tree_Node ** leaf_out);

/*============================================================================
*  删除Key的函数
*/
int get_neighbor_index(Tree_Node *n);
Tree_Node *remove_entry_from_node(Tree_Node *n, int key, Tree_Node *pointer);
Tree_Node *adjust_root(Tree_Node *root);
Tree_Node *coalesce_nodes(Tree_Node *root, Tree_Node *n, Tree_Node *neighbor, int neighbor_index, int k_prime);
Tree_Node *redistribute_nodes(Tree_Node *root, Tree_Node *n, Tree_Node *neighbor, int neighbor_index, int k_prime_index, int k_prime);
Tree_Node *delete_entry(Tree_Node *root, Tree_Node *n, int key, void *pointer);
Tree_Node *delete_key (Tree_Node *root, int key);

//=============================================================================
// 寻找两个数值的节点的函数
void get_and_print_range(Tree_Node *const root, int key_start, int key_end, bool verbose);
int get_range(Tree_Node *const root, int key_start, int key_end, bool verbose, int returned_keys[], void *returned_pointers[]);

//=============================================================================
// 关于Tree 的函数定义
int height(Tree_Node *const root);
int empty_tree();
int get_tree_order();
int get_tree_height(Tree_Node * root);
int get_tree_root(Tree_Node * root);
int calc_leaf_num(Tree_Node *const root);
int get_tree_info(Tree_Node * root);
int show_tree_info();

// 主界面的函数
void show_manual(void);
int manual();

/* the input data
19 |
7 13 | 25 31 |
3 5 | 9 11 | 15 17 | 21 23 | 27 29 | 33 35 |
1 2 | 3 4 | 5 6 | 7 8 | 9 10 | 11 12 | 13 14 | 15 16 | 17 18 | 19 20 | 21 22 | 23 24 | 25 26 | 27 28 | 29 30 | 31 32 | 33 34 | 35 36 37 |
*/

/* Funcation Definition.
*/
void ERROR_EXIT(char* str)
    {
        if (root)
        {
            list_leaves(root);
            destroy_tree(root);
        }
        perror(str);
        exit(EXIT_FAILURE);
    }

void usage()
{
    printf("This program is testing the B+ Tree of Order %d.\n", tree_order);
    printf("The program will test creation, insertion, deletion and destory of BP tree.\n"
            "To start with inputing from one file of [Primary, Name] \n"
            "and will insert the creation date & time.\n" );
}

size_t get_current_time( char * time_info)
{
    time_t current_time;
    struct tm *ptime;

    time(&current_time);
    ptime = localtime(&current_time);
    size_t result_num = strftime (time_info, 20,"%Y-%m-%d %H:%M:%S",ptime);
    return result_num;
}

int compare_key(int key1, int key2)
{
    if (key1 > key2)
        return 1;
    else if (key1 == key2)
        return 0;
    else 
        return -1;
}

/* Finds the appropriate place to *split a node that is too big into two.
 */
int cut(int length)
{
    if (length % 2 == 0)
        return length / 2;
    else
        return length / 2 + 1;
}

void enqueue(Tree_Node *new_node)
{
    Tree_Node *c;
    if (queue == NULL)
    {
        queue = new_node;
        queue->next = NULL;
    }
    else
    {
        c = queue;
        while (c->next != NULL)
            c = c->next;
        c->next = new_node;
        new_node->next = NULL;
    }
}

/* Helper function for printing the
 * tree out.  See print_tree.
 */
Tree_Node *dequeue(void)
{
    Tree_Node *n = queue;
    queue = queue->next;
    n->next = NULL;
    return n;
}

int path_to_root(Tree_Node *const root, Tree_Node *child)
{
    int length = 0;
    Tree_Node *c = child;
    while (c != root)
    {
        c = c->parent;
        length++;
    }
    return length;
}


void print_tree(Tree_Node *const root)
{

    Tree_Node *n = NULL;
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
        if (verbose_output)
            printf("(%p)", n);
        for (i = 0; i < n->num_keys; i++)
        {
            if (verbose_output)
                printf("%p ", n->pointers[i]);
            printf("%d ", n->keys[i]);
        }
        if (!n->is_leaf)
            for (i = 0; i <= n->num_keys; i++)
                enqueue(n->pointers[i]);
        if (verbose_output)
        {
            if (n->is_leaf)
                printf("%p ", n->pointers[tree_order - 1]);
            else
                printf("%p ", n->pointers[n->num_keys]);
        }
        printf("|");
    }
    printf("\n");
}
/* 功能：根据传入的记录，创建一个数据块。
* 输入： 数据指针
* 输出： 返回生成记录的指针。
 */
Data_Record *make_data_record(Data_Record *drp)
{
    Data_Record *new_record = (Data_Record *)malloc(sizeof(Data_Record));
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


/* ======================================================================================
* Insertion functions
* 
* 创建新的节点信息。
* 注意：Key的数量=阶数-1. 指针的数量=阶数
*/
Tree_Node *make_empty_node(void)
{
    Tree_Node *new_node;
    new_node = malloc(sizeof(Tree_Node));
    if (new_node == NULL)
        ERROR_EXIT("Node creation.");
    
    new_node->keys = malloc((tree_order - 1) * sizeof(int)); // （阶数-1） 个key
    if (new_node->keys == NULL)
    {
        ERROR_EXIT("New node keys array.");
    }
    new_node->pointers = malloc(tree_order * sizeof(void *)); // （阶数） 个key
    if (new_node->pointers == NULL)
    {
        ERROR_EXIT("New node pointers array.");
    }
    new_node->is_leaf = false;
    new_node->num_keys = 0;
    new_node->parent = NULL;
    new_node->next = NULL;
    return new_node;
}

/* 创建一个叶子节点。先创立一个Node，然后设置成叶子节点的标识 */
Tree_Node *make_empty_leaf(void)
{
    Tree_Node *leaf = make_empty_node();
    leaf->is_leaf = true;
    return leaf;
}


/* 功能：找到比Key大的叶子节点
*  输入：root, key所在的记录，显示标志
*  返回：叶子节点的指针。
 * 从root开始，找key，直到到叶子。
 * 非叶子节点上，Key的所处的叶子，是在key的右部指针上的。所以，在非叶子节点中，找到比key大的指针。
 */
Tree_Node *find_leaf(Tree_Node *const root, Data_Record * drp, bool verbose)
{
    if (root == NULL)
    {
        if (verbose)
            printf("Empty tree.\n");
        return root;
    }
    int i = 0;
    Tree_Node *c = root;
    while (!c->is_leaf)
    {
        if (verbose)
        {
            printf("[");
            for (i = 0; i < c->num_keys - 1; i++)
                printf("%d ", c->keys[i]);
            printf("%d] ", c->keys[i]);
        }
        i = 0;
        while (i < c->num_keys)
        {
            //if (key >= c->keys[i]) 如果Key>=节点上的key，就往下找。直到找到第一个更大的key
            if (compare_key(drp->key , c->keys[i]) >= 0)
                i++;
            else
                break;
        }
        if (verbose)
            printf("%d ->\n", i);
        c = (Tree_Node *)c->pointers[i]; //走到下一层去。
    }
    if (verbose)
    {
        printf("Leaf [");
        for (i = 0; i < c->num_keys - 1; i++)
        {    
            printf("%d ", c->keys[i]);
        }
        printf("%d] ->\n", c->keys[i]);
    }
    return c;
}

/*
* 功能：在Tree里面，找是否有有这个Key
*  输入参数： root 和 传入的数据指针。verbose：是否输出过程。Leaf_out: 找到的可以所在叶子节点的位置指针
*  返回参数：在叶子节点上的数据指针
*/
Data_Record * find_key_in_tree(Tree_Node *root, Data_Record *drp, bool verbose, Tree_Node ** leaf_out)
{
    if (root == NULL)
    {
        if (leaf_out != NULL)
            *leaf_out = NULL;
        return NULL;
    }

    int i = 0;
    Tree_Node *leaf = NULL;

    /* 如果root不为空，叶子节点必然包含有value，即使没有包含想查询的key
     * 先找到叶子节点，包含想要查询的key的范围 
     */
    leaf = find_leaf(root, drp, verbose);

    for (i = 0; i < leaf->num_keys; i++)
        if (compare_key(leaf->keys[i], drp->key) ==0 )
            break;
    if (leaf_out != NULL)
        *leaf_out = leaf;
    // 如找到最后了，说明叶子节点中没有这个值。返回NULL。
    if (i == leaf->num_keys)
        return NULL;
    else
        return (Data_Record *)leaf->pointers[i];
}


/* Creates a new root for two subtrees
 * and inserts the appropriate key into
 * the new root.
 */
Tree_Node *insert_into_new_root(Tree_Node *left, int key, Tree_Node *right)
{

    Tree_Node *root = make_empty_node();
    root->keys[0] = key;
    root->pointers[0] = left;
    root->pointers[1] = right;
    root->num_keys++;
    root->parent = NULL;
    left->parent = root;
    right->parent = root;
    return root;
}

/* 在父节点找左节点的位置*/
int get_index_in_parent(Tree_Node *parent, Tree_Node *left)
{
    int left_index = 0;
    while (left_index <= parent->num_keys && parent->pointers[left_index] != left)
        left_index++;
    return left_index;
}


/* Inserts a new key and pointer to a node
 * into a node into which these can fit
 * without violating the B+ tree properties.
 */
Tree_Node *insert_into_node(Tree_Node *root, Tree_Node *n, int left_index, int key, Tree_Node *right)
{
    int i;

    for (i = n->num_keys; i > left_index; i--)
    {
        n->pointers[i + 1] = n->pointers[i];
        n->keys[i] = n->keys[i - 1];
    }
    n->pointers[left_index + 1] = right;
    n->keys[left_index] = key;
    n->num_keys++;
    return root;
}

/* Inserts a new key and pointer to a node
 * into a node, causing the node's size to exceed
 * the order, and causing the node to split into two.
 */
Tree_Node *insert_into_node_after_splitting(Tree_Node *root, Tree_Node *old_node, int left_index, int key, Tree_Node *right)
{

    int i, j, split, k_prime;
    Tree_Node *new_node, *child;
    int *temp_keys;
    Tree_Node **temp_pointers;

    /* First create a temporary set of keys and pointers to hold everything in order, including
	 * the new key and pointer, inserted in their  correct places. 
	 * Then create a new node and copy half of the keys and pointers to the old node and the other half to the new.
	 */

    temp_pointers = malloc((tree_order + 1) * sizeof(Tree_Node *));
    if (temp_pointers == NULL)
    {
        ERROR_EXIT("Temporary pointers array for splitting nodes.");
    }
    temp_keys = malloc(tree_order * sizeof(int));
    if (temp_keys == NULL)
    {
        ERROR_EXIT("Temporary keys array for splitting nodes.");
    }

    for (i = 0, j = 0; i < old_node->num_keys + 1; i++, j++)
    {
        if (j == left_index + 1)
            j++;
        temp_pointers[j] = old_node->pointers[i];
    }

    for (i = 0, j = 0; i < old_node->num_keys; i++, j++)
    {
        if (j == left_index)
            j++;
        temp_keys[j] = old_node->keys[i];
    }

    temp_pointers[left_index + 1] = right;
    temp_keys[left_index] = key;

    // Create the new node and copy half the keys and pointers to the old and half to the new.
    split = cut(tree_order);
    new_node = make_empty_node();
    old_node->num_keys = 0;
    for (i = 0; i < split - 1; i++)
    {
        old_node->pointers[i] = temp_pointers[i];
        old_node->keys[i] = temp_keys[i];
        old_node->num_keys++;
    }
    old_node->pointers[i] = temp_pointers[i];
    k_prime = temp_keys[split - 1];
    for (++i, j = 0; i < tree_order; i++, j++)
    {
        new_node->pointers[j] = temp_pointers[i];
        new_node->keys[j] = temp_keys[i];
        new_node->num_keys++;
    }
    new_node->pointers[j] = temp_pointers[i];
    free(temp_pointers);
    free(temp_keys);
    new_node->parent = old_node->parent;
    for (i = 0; i <= new_node->num_keys; i++)
    {
        child = new_node->pointers[i];
        child->parent = new_node;
    }

    /* Insert a new key into the parent of the two
	 * nodes resulting from the split, with
	 * the old node to the left and the new to the right.
	 */

    return insert_into_parent(root, old_node, k_prime, new_node);
}

/* Inserts a new node (leaf or internal node) into the B+ tree.
 * Returns the root of the tree after insertion.
 */
Tree_Node *insert_into_parent(Tree_Node *root, Tree_Node *left, int key, Tree_Node *right)
{

    int left_index;
    Tree_Node *parent;

    parent = left->parent;

    /* Case: new root. */

    if (parent == NULL)
        return insert_into_new_root(left, key, right);

    /* Case: leaf or node. (Remainder of  function body.)   */

    /* Find the parent's pointer to the left node. */

    left_index = get_index_in_parent(parent, left);

    /* Simple case: the new key fits into the node.  */

    if (parent->num_keys < tree_order - 1)
        return insert_into_node(root, parent, left_index, key, right);

    /* Harder case:  split a node in order  to preserve the B+ tree properties. */

    return insert_into_node_after_splitting(root, parent, left_index, key, right);
}


/* 功能：把一个key和新记录插入到一个叶子，超过叶子的容量（order-1）导致叶子拆分成两个
*  输入：root,  leaf, key, data
*  返回：root
 */
Tree_Node *insert_into_leaf_after_splitting(Tree_Node *root, Tree_Node *leaf, int key, Data_Record *pointer)
{

    Tree_Node *new_leaf;
    int *temp_keys;
    void **temp_pointers;
    int insertion_index, split, new_key, i, j;

    new_leaf = make_empty_leaf();

    temp_keys = malloc(tree_order * sizeof(int));
    if (temp_keys == NULL)
    {
        ERROR_EXIT("Temporary keys array.");
    }

    temp_pointers = malloc(tree_order * sizeof(void *));
    if (temp_pointers == NULL)
    {
        ERROR_EXIT("Temporary pointers array.");
    }

    insertion_index = 0;
    //while (insertion_index < order - 1 && leaf->keys[insertion_index] < key)
    while (insertion_index < tree_order - 1 && compare_key(leaf->keys[insertion_index], key) < 0)
        insertion_index++;

    for (i = 0, j = 0; i < leaf->num_keys; i++, j++)
    {
        if (j == insertion_index)
            j++;
        temp_keys[j] = leaf->keys[i];
        temp_pointers[j] = leaf->pointers[i];
    }

    temp_keys[insertion_index] = key;
    temp_pointers[insertion_index] = pointer;

    leaf->num_keys = 0;

    //拆分的依据：按照阶数-1的一半来拆分。左边小，右边大
    split = cut(tree_order - 1);

    for (i = 0; i < split; i++)
    {
        leaf->pointers[i] = temp_pointers[i];
        leaf->keys[i] = temp_keys[i];
        leaf->num_keys++;
    }

    for (i = split, j = 0; i < tree_order; i++, j++)
    {
        new_leaf->pointers[j] = temp_pointers[i];
        new_leaf->keys[j] = temp_keys[i];
        new_leaf->num_keys++;
    }

    free(temp_pointers);
    free(temp_keys);

    new_leaf->pointers[tree_order - 1] = leaf->pointers[tree_order - 1];
    leaf->pointers[tree_order - 1] = new_leaf;

    for (i = leaf->num_keys; i < tree_order - 1; i++)
        leaf->pointers[i] = NULL;
    for (i = new_leaf->num_keys; i < tree_order - 1; i++)
        new_leaf->pointers[i] = NULL;

    new_leaf->parent = leaf->parent;
    new_key = new_leaf->keys[0];

    return insert_into_parent(root, leaf, new_key, new_leaf);
}


/* Inserts a new pointer to a record and its corresponding key into a leaf.
 * Returns the altered leaf.
 */
Tree_Node *insert_into_leaf(Tree_Node *leaf, int key, Data_Record *pointer)
{

    int i;
    int insertion_point = 0;

    while (insertion_point < leaf->num_keys && compare_key(leaf->keys[insertion_point], key)<0)
        insertion_point++;

    for (i = leaf->num_keys; i > insertion_point; i--)
    {
        leaf->keys[i] = leaf->keys[i - 1];
        leaf->pointers[i] = leaf->pointers[i - 1];
    }
    leaf->keys[insertion_point] = key;
    leaf->pointers[insertion_point] = pointer;
    leaf->num_keys++;
    return leaf;
}

/* 如果是第一次插入，就建立一个新树root  */
Tree_Node *start_new_tree(Data_Record *drp, Data_Record *pointer)
{
    Tree_Node *root = make_empty_leaf();
    root->keys[0] = drp->key;
    root->pointers[0] = pointer;
    root->pointers[tree_order - 1] = NULL;
    root->parent = NULL;
    root->num_keys++;
    return root;
}

/* 功能：把一个节点插入到树中
*  输入参数： root，节点数据指针
*  返回结果：root指针
*/
Tree_Node * insert_into_tree(Tree_Node *root, Data_Record *drp)
{
    Data_Record *lrp = NULL;
    Tree_Node *leaf = NULL;

    // 首先找是否在树中有该值
    lrp = find_key_in_tree(root, drp, false, NULL);
    if (lrp != NULL)
    {
        /* 如果树中已经有了，更新一下节点内容 */
        //rp->key = drp->key;
        strcpy(lrp->id, drp->id);
        strcpy(lrp->name, drp->name);
        strcpy(lrp->create_time, drp->create_time);
        return root;
    }

    /* 建立一个新的记录节点， 把传入记录写进去*/
    lrp = make_data_record(drp);

    /* 如果root是空，就建立一个新的树。直接返回。	 */
    if (root == NULL)
        return start_new_tree(drp, lrp);

    /* 如果树非空，找到叶子节点	 */
    leaf = find_leaf(root, drp, false);

    /* 如果叶子节点有空间，就插入到叶子节点中。判断条件是：目前叶子的key< oreder-1	 */
    if (leaf->num_keys < tree_order - 1)
    {
        leaf = insert_into_leaf(leaf, drp->key, lrp);
        return root;
    }

    // 如果叶子节点已经满了，就需要把叶子节点拆分，返回的是root指针

    return insert_into_leaf_after_splitting(root, leaf, drp->key, lrp);

}

/* 从文件里读取数据。数据格式
*   [25 20211425 LiTongHe]
*   int char[9]  char[20]   
*/

Tree_Node * get_data_from_file( )
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
        Data_Record dr;
        int i;
        int count =0;

        while (!feof(fp))
        {
            i = fscanf(fp, "%d %s %s\n", &dr.key, dr.id, dr.name);
            if (i == 3)
            {
                char str1[BUFFER_SIZE];
                get_current_time(dr.create_time);
                sprintf(str1, "[Key:%4d] [No:%8s] [Name:%30s] [Create_Time: %20s]\n", dr.key, dr.id, dr.name, dr.create_time);
                printf(str1);
                root = insert_into_tree(root, &dr);
                count ++;
            }
            else
            {   //输入值有问题。废弃掉。
                fclose(fp);
                ERROR_EXIT("Input data error.");
            }
        }
        fclose(fp);
        printf("\n%4d records are created.\n", count);

        return root;
    }
}


/* 功能：从tree中找到一个key所在的记录，并打印出来
*  输入：root，要查找的值，显示标识
*  返回：无
 */
void find_and_print_record(Tree_Node *const root, Data_Record *drp, bool verbose)
{
    Tree_Node *leaf = NULL;
    Data_Record *r = find_key_in_tree(root, drp, verbose, NULL);
    if (r == NULL)
        printf("Record not found under key %d.\n", drp->key);
    else
        printf("Record at %p -- key:[%d], \nID:[%s] NAME:[%s] Create Time:[%s].\n",
               r, drp->key, r->id, r->name, r->create_time);
}


void list_leaves(Tree_Node * const root)
{
    if (root == NULL)
    {
        printf("The tree is empty.\n");
        return;
    }
    int i;
    Tree_Node * c = root;
    while (! c->is_leaf)
        c= c->pointers[0];
    while (true)
    {
        for (i = 0; i< c->num_keys; i++)
        {
            Data_Record * drp = (Data_Record*) (c->pointers[i]);
            printf("[Key:%4d] [ID:%8s] [Name:%40s] [Create_Time:%20s]\n", drp->key, drp->id, drp->name, drp->create_time);
        }
        if (c->pointers[tree_order - 1] != NULL)
        {
            c = c->pointers[tree_order - 1];
        }
        else
            break;
    }
    printf("\n");
}

/*
* 功能：按照节点，显示叶子节点。
*/
void print_leaves(Tree_Node *const root)
{
    if (root == NULL)
    {
        printf("Empty tree.\n");
        return;
    }
    int i;
    Tree_Node *c = root;
    while (!c->is_leaf)
        c = c->pointers[0];
    while (true)
    {
        for (i = 0; i < c->num_keys; i++)
        {
            if (verbose_output)
                printf("%p ", c->pointers[i]);
            printf("%d ", ((Data_Record*)(c->pointers[i]))->key);
        }
        if (verbose_output)
            printf("%p ", c->pointers[tree_order - 1]);
        if (c->pointers[tree_order - 1] != NULL)
        {
            printf("|");
            c = c->pointers[tree_order - 1]; // 走向下一个
        }
        else
            break;
    }
    printf("|\n");
}

/*======================================================================================
* 功能：从tree中找一定范围的值。并显示出来
* 输入：root，开始值，结束值，显示标志
* 返回：无
*/

void get_and_print_range(Tree_Node *const root, int key_start, int key_end, bool verbose)
{
    int i;
    int array_size = key_end - key_start + 1;
    int returned_keys[array_size];
    void *returned_pointers[array_size];
    int num_found = get_range(root, key_start, key_end, verbose, returned_keys, returned_pointers);
    if (!num_found)
        printf("None found.\n");
    else
    {
        for (i = 0; i < num_found; i++)
            printf("Key: %d   Location: %p  Value: %d %s %s %s\n",
                   returned_keys[i],
                   returned_pointers[i],
                   ((Data_Record *) returned_pointers[i]) ->key,
                   ((Data_Record *)returned_pointers[i])->id,
                   ((Data_Record *)returned_pointers[i])->name,
                   ((Data_Record *)returned_pointers[i])->create_time
                   );
    }
    
}

/* 功能：找到起止key之间的叶子
*  输入：root，起止key，输出标识。找到的key数组、找到的叶子数组
*  返回：找到的数量。
 */
int get_range(Tree_Node *const root, int key_start, int key_end, bool verbose, int returned_keys[], void *returned_pointers[])
{
    Data_Record dr;    
    dr.key = key_start;
    Tree_Node *n = find_leaf(root, &dr, verbose);
    if (n == NULL)
        return 0;

    int i;
    int num_found = 0;

    for (i = 0; i < n->num_keys && compare_key(n->keys[i] ,  key_start) < 0; i++)
        ;
    if (i == n->num_keys)
        return 0;
    while (n != NULL)
    {
        for (; i < n->num_keys && compare_key(n->keys[i], key_end) <= 0; i++)
        {
            returned_keys[num_found] = n->keys[i];
            returned_pointers[num_found] = n->pointers[i];
            num_found++;
        }
        n = n->pointers[tree_order - 1];
        i = 0;
    }
    return num_found;
}

/* ===========================================================
*  Destory Tree functions
*/

void destroy_tree_nodes(Tree_Node *root)
{
    int i;
    if (root->is_leaf)
        for (i = 0; i < root->num_keys; i++)
            free(root->pointers[i]);
    else
        for (i = 0; i < root->num_keys + 1; i++)
            destroy_tree_nodes(root->pointers[i]);
    free(root->pointers);
    free(root->keys);
    free(root);
}

Tree_Node *destroy_tree(Tree_Node *root)
{
    if (root)
        destroy_tree_nodes(root);
    return NULL;
}


/*  ==============================
*   tree function implementation
*/
int height(Tree_Node *const root)
{
    int h = 0;
    Tree_Node *c = root;
    while (!c->is_leaf)
    {
        c = c->pointers[0];
        h++;
    }
    return h;
}
int empty_tree()
{
    tree.root = NULL;
    tree.leaf_num = 0;
    tree.tree_height = 0;
    tree.tree_order = tree_order;
    return EXIT_SUCCESS;

}

int get_tree_order()
{
    tree.tree_order = tree_order;
    return EXIT_SUCCESS;
}

int get_tree_height(Tree_Node * root)
{
    tree.tree_height = height(root) + 1;
    return EXIT_SUCCESS;
}

int get_tree_root(Tree_Node * root)
{
    tree.root= root;
    return EXIT_SUCCESS;
}

int calc_leaf_num(Tree_Node *const root)
{
    if (root == NULL)
    {
        tree.leaf_num = 0;
        return EXIT_SUCCESS;
    }

    int leaf_num = 0;
    Tree_Node *c = root;
    while (!c->is_leaf)
        c = c->pointers[0]; //找到叶子。
    do 
    {
        leaf_num +=  c->num_keys;        
        c = c->pointers[tree_order - 1]; // 走向下一个叶子节点
    } while (c != NULL);
   
    tree.leaf_num = leaf_num;
    return EXIT_SUCCESS;
}

int get_tree_info(Tree_Node * root)
{
    if (root)
    {
        get_tree_root(root);
        get_tree_height(root);
        get_tree_order();
        calc_leaf_num(root);
    }
    else
        empty_tree();
    return 0;
}
int show_tree_info()
{
    if (tree.root == NULL)
        printf("\nThe tree is empty. \n");
    else
    {
        printf("The tree is at          : [%p]\n", tree.root);
        printf("The tree order is       : [%d]\n", tree.tree_order);
        printf("the tree hight is       : [%d]\n", tree.tree_height);
        printf("The tree leaf number is : [%d]\n", tree.leaf_num);
    }
    return 0;
}

/*============================================================================
*  删除Key的函数
*/

int get_neighbor_index(Tree_Node *n)
{
    int i;
    /* Return the index of the key to the left of the pointer in the parent pointing to n.  
	 * If n is the leftmost child, this means  return -1.
	 */
    for (i = 0; i <= n->parent->num_keys; i++)
        if (n->parent->pointers[i] == n)
            return i - 1;

    // Error state.
    printf("Search for nonexistent pointer to node in parent.\n");
    printf("Node:  %#lx\n",  (unsigned long*)n);
    exit(EXIT_FAILURE);
}

Tree_Node *remove_entry_from_node(Tree_Node *n, int key, Tree_Node *pointer)
{

    int i, num_pointers;

    // Remove the key and shift other keys accordingly.
    i = 0;
    while (n->keys[i] != key)
        i++;
    for (++i; i < n->num_keys; i++)
        n->keys[i - 1] = n->keys[i];

    // Remove the pointer and shift other pointers accordingly.
    // First determine number of pointers.
    num_pointers = n->is_leaf ? n->num_keys : n->num_keys + 1;
    i = 0;
    while (n->pointers[i] != pointer)
        i++;
    for (++i; i < num_pointers; i++)
        n->pointers[i - 1] = n->pointers[i];

    // One key fewer.
    n->num_keys--;

    // Set the other pointers to NULL for tidiness.
    // A leaf uses the last pointer to point to the next leaf.
    if (n->is_leaf)
        for (i = n->num_keys; i < tree_order - 1; i++)
            n->pointers[i] = NULL;
    else
        for (i = n->num_keys + 1; i < tree_order; i++)
            n->pointers[i] = NULL;

    return n;
}

Tree_Node *adjust_root(Tree_Node *root)
{

    Tree_Node *new_root;

    /* Case: nonempty root.
	 * Key and pointer have already been deleted,
	 * so nothing to be done.
	 */

    if (root->num_keys > 0)
        return root;

    /* Case: empty root.*/

    // If it has a child, promote the first (only) child as the new root.

    if (!root->is_leaf)
    {
        new_root = root->pointers[0];
        new_root->parent = NULL;
    }

    // If it is a leaf (has no children),
    // then the whole tree is empty.

    else
        new_root = NULL;

    free(root->keys);
    free(root->pointers);
    free(root);

    return new_root;
}

/* Coalesces a node that has become
 * too small after deletion
 * with a neighboring node that
 * can accept the additional entries
 * without exceeding the maximum.
 */
Tree_Node *coalesce_nodes(Tree_Node *root, Tree_Node *n, Tree_Node *neighbor, int neighbor_index, int k_prime)
{

    int i, j, neighbor_insertion_index, n_end;
    Tree_Node *tmp;

    /* Swap neighbor with node if node is on the
	 * extreme left and neighbor is to its right.
	 */

    if (neighbor_index == -1)
    {
        tmp = n;
        n = neighbor;
        neighbor = tmp;
    }

    /* Starting point in the neighbor for copying
	 * keys and pointers from n.
	 * Recall that n and neighbor have swapped places
	 * in the special case of n being a leftmost child.
	 */

    neighbor_insertion_index = neighbor->num_keys;

    /* Case:  nonleaf node.
	 * Append k_prime and the following pointer.
	 * Append all pointers and keys from the neighbor.
	 */

    if (!n->is_leaf)
    {

        /* Append k_prime.
		 */

        neighbor->keys[neighbor_insertion_index] = k_prime;
        neighbor->num_keys++;

        n_end = n->num_keys;

        for (i = neighbor_insertion_index + 1, j = 0; j < n_end; i++, j++)
        {
            neighbor->keys[i] = n->keys[j];
            neighbor->pointers[i] = n->pointers[j];
            neighbor->num_keys++;
            n->num_keys--;
        }

        /* The number of pointers is always one more than the number of keys. */

        neighbor->pointers[i] = n->pointers[j];

        /* All children must now point up to the same parent. */

        for (i = 0; i < neighbor->num_keys + 1; i++)
        {
            tmp = (Tree_Node *)neighbor->pointers[i];
            tmp->parent = neighbor;
        }
    }

    /* In a leaf, append the keys and pointers of  n to the neighbor.
	 * Set the neighbor's last pointer to point to  what had been n's right neighbor.
	 */

    else
    {
        for (i = neighbor_insertion_index, j = 0; j < n->num_keys; i++, j++)
        {
            neighbor->keys[i] = n->keys[j];
            neighbor->pointers[i] = n->pointers[j];
            neighbor->num_keys++;
        }
        neighbor->pointers[tree_order - 1] = n->pointers[tree_order - 1];
    }

    root = delete_entry(root, n->parent, k_prime, n);
    free(n->keys);
    free(n->pointers);
    free(n);
    return root;
}

/* Redistributes entries between two nodes when
 * one has become too small after deletion
 * but its neighbor is too big to append the
 * small node's entries without exceeding the
 * maximum
 */
Tree_Node *redistribute_nodes(Tree_Node *root, Tree_Node *n, Tree_Node *neighbor, int neighbor_index,
                         int k_prime_index, int k_prime)
{

    int i;
    Tree_Node *tmp;

    /* Case: n has a neighbor to the left. 
	 * Pull the neighbor's last key-pointer pair over
	 * from the neighbor's right end to n's left end.
	 */

    if (neighbor_index != -1)
    {
        if (!n->is_leaf)
            n->pointers[n->num_keys + 1] = n->pointers[n->num_keys];
        for (i = n->num_keys; i > 0; i--)
        {
            n->keys[i] = n->keys[i - 1];
            n->pointers[i] = n->pointers[i - 1];
        }
        if (!n->is_leaf)
        {
            n->pointers[0] = neighbor->pointers[neighbor->num_keys];
            tmp = (Tree_Node *)n->pointers[0];
            tmp->parent = n;
            neighbor->pointers[neighbor->num_keys] = NULL;
            n->keys[0] = k_prime;
            n->parent->keys[k_prime_index] = neighbor->keys[neighbor->num_keys - 1];
        }
        else
        {
            n->pointers[0] = neighbor->pointers[neighbor->num_keys - 1];
            neighbor->pointers[neighbor->num_keys - 1] = NULL;
            n->keys[0] = neighbor->keys[neighbor->num_keys - 1];
            n->parent->keys[k_prime_index] = n->keys[0];
        }
    }

    /* Case: n is the leftmost child.
	 * Take a key-pointer pair from the neighbor to the right.
	 * Move the neighbor's leftmost key-pointer pair
	 * to n's rightmost position.
	 */

    else
    {
        if (n->is_leaf)
        {
            n->keys[n->num_keys] = neighbor->keys[0];
            n->pointers[n->num_keys] = neighbor->pointers[0];
            n->parent->keys[k_prime_index] = neighbor->keys[1];
        }
        else
        {
            n->keys[n->num_keys] = k_prime;
            n->pointers[n->num_keys + 1] = neighbor->pointers[0];
            tmp = (Tree_Node *)n->pointers[n->num_keys + 1];
            tmp->parent = n;
            n->parent->keys[k_prime_index] = neighbor->keys[0];
        }
        for (i = 0; i < neighbor->num_keys - 1; i++)
        {
            neighbor->keys[i] = neighbor->keys[i + 1];
            neighbor->pointers[i] = neighbor->pointers[i + 1];
        }
        if (!n->is_leaf)
            neighbor->pointers[i] = neighbor->pointers[i + 1];
    }

    /* n now has one more key and one more pointer;
	 * the neighbor has one fewer of each.
	 */

    n->num_keys++;
    neighbor->num_keys--;

    return root;
}


Tree_Node *delete_entry(Tree_Node *root, Tree_Node *n, int key, void *pointer)
{

    int min_keys;
    Tree_Node *neighbor;
    int neighbor_index;
    int k_prime_index, k_prime;
    int capacity;

    // Remove key and pointer from node.
    n = remove_entry_from_node(n, key, pointer);

    /* Case:  deletion from the root.  */
    if (n == root)
        return adjust_root(root);

    /* Case:  deletion from a node below the root. (Rest of function body.)	 */

    /* Determine minimum allowable size of node, to be preserved after deletion. */

    min_keys = n->is_leaf ? cut(tree_order - 1) : cut(tree_order) - 1;

    /* Case:  node stays at or above minimum.* (The simple case.) */

    if (n->num_keys >= min_keys)
        return root;

    /* Case:  node falls below minimum.
	 * Either coalescence or redistribution is needed.
	 */

    /* Find the appropriate neighbor node with which to coalesce.
	 * Also find the key (k_prime) in the parent between the pointer to node n and the pointer to the neighbor.
	 */

    neighbor_index = get_neighbor_index(n);
    k_prime_index = neighbor_index == -1 ? 0 : neighbor_index;
    k_prime = n->parent->keys[k_prime_index];
    neighbor = neighbor_index == -1 ? n->parent->pointers[1] : n->parent->pointers[neighbor_index];

    capacity = n->is_leaf ? tree_order : tree_order - 1;

    /* Coalescence. */
    if (neighbor->num_keys + n->num_keys < capacity)
        return coalesce_nodes(root, n, neighbor, neighbor_index, k_prime);

    /* Redistribution. */
    else
        return redistribute_nodes(root, n, neighbor, neighbor_index, k_prime_index, k_prime);

}

/* 功能：删除一个key的主程序
*  返回值：root
 */
Tree_Node *delete_key (Tree_Node *root, int key)
{

    Tree_Node *key_leaf = NULL;
    Data_Record *key_record = NULL;
    Data_Record dr;
    dr.key = key;

    //找到叶子节点中的数据块指针。并返回Key所在的叶子节点。
    key_record = find_key_in_tree(root, &dr, false, &key_leaf);

    /* 如果Ke所在的记录和叶子节点都存在，就删除，并释放空间 */
    if (key_record != NULL && key_leaf != NULL)
    {
        root = delete_entry(root, key_leaf, key, key_record);
        free(key_record);
    }
    return root;
}

/* 显示菜单和操作界面
*/
void show_manual(void)
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
int manual()
{

    char command;
    bool input_consumed = false;
    char buffer[BUFFER_SIZE];
    Data_Record dr;
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
                root = delete_key (root, input_key);
                print_tree(root);
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
                root = insert_into_tree(root, &dr);
                print_tree(root);;
            }
            else
            {                
                printf("Input error. Please press ? to get help.\n");
            }
            break;
        case 'f':
        case 'p':
            count = scanf("%d", &dr.key);
            if (count == 1)
            {
                get_current_time(dr.create_time);
                find_and_print_record(root, &dr, command == 'p');
            }
            else
            {
                printf("Input error. Please press ? to get help.\n");
            }
            break;
        case 'l':
            print_leaves(root);
            list_leaves(root);
            break;
         case 'x':
            root = destroy_tree(root);            
            printf("The tree is destoryed.\n");
            break;
        case 't':
            if (root)
            {
                get_tree_info(root);
                show_tree_info();
                print_tree(root);
            }
            else
                printf("The tree is empty.\n");
            break;
        case 'R':
            root = destroy_tree(root);
            root = get_data_from_file();    //reload the data from file.
            print_tree(root);
            break;
        case 'o':
            if (root)
            {
                printf("The tree is active. Please press \'x\' to release or \'R\' to reload the tree.\n");
             }
            else
            {
                int input_order;
                count = scanf("%d", &input_order);
                if (count == 1)
                    tree_order = input_order;
                else
                    printf("Input error. Please press ? to get help.\n");
            }
            break;
        case 'q':           
            if (root)
            {
                root = destroy_tree(root);
                printf("The tree is not empty and is cleanned. Quit safely.\n");
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
                get_and_print_range(root, input_key, input_key_2, command == 'p');
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
            show_manual();
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
//===============================================================================
// 主测试函数
int mybptree()
{
    FILE *fp;

    //usage();
    root = get_data_from_file();
    print_tree(root);

    manual();
    return EXIT_SUCCESS;
}


