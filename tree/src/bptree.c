/*
 *
 *  bpt:  B+ Tree Implementation
 *  My B-PLus Tree.c
 *  Created by wangss on 2022.10.04
 *  Last modified: 
 *
 *  This implementation demonstrates the B+ tree data structure
 *  for educational purposes, includin insertion, deletion, search, and display
 *  of the search path, the leaves, or the whole tree.
 *  
 *  Must be compiled with a C99-compliant C compiler such as the latest GCC.
 *
 *  Usage:  bpt [order]
 *  where order is an optional argument
 *  (integer MIN_ORDER <= order <= MAX_ORDER)
 *  defined as the maximal number of pointers in any Node.
 *
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../inc/bptree.h"

// GLOBALS.

/* The order
 * Order阶数决定最大和最小的Key/Pointer数量，在每个节点林，最大有（阶数-1）个Key，至少有(阶数-1)/2个key。
 * 叶子节点中，数据Pointer数目=Key数目
 */
int order = DEFAULT_ORDER;
/* The queue 
 * 用于打印tree，按level顺序，从root开始，每层一行，到叶子结束
 */
Node *queue = NULL;

/* 
 * 决定是否使用16进制打印
 */
bool verbose_output = false;
void usage_1(void);
void usage_2(void);
int Cut(int length);
void EnQueue(Node *NewNode);
Node *DeQueue(void);
int GetHeight(Node *const root);
int PathToRoot(Node *const root, Node *child);
void PrintLeaves(Node *const root);
void PrintTree(Node *const root);
void find_and_print(Node *const root, int key, bool verbose);
void find_and_print_range(Node *const root, int range1, int range2, bool verbose);
int find_range(Node *const root, int key_start, int key_end, bool verbose,
               int returned_keys[], void *returned_pointers[]);
Node *find_leaf(Node *const root, int key, bool verbose);
Record *find(Node *root, int key, bool verbose, Node **leaf_out);

// Insertion：插入的模块

Record *MakeRecord(int value);
Node *MakeEmptyNode(void);
Node * MakeEmptyLeaf(void);
int GetLeftIndex(Node *parent, Node *left);

/* First message to the user.
 */
void usage_1(void)
{
    printf("B+ Tree of Order %d.\n", order);
    printf("To build a B+ tree of a different order, start again and enter "
           "the order using the initial data from ../data/testbptree.txt.\n ");
}

/* Second message to the user.
 */
void usage_2(void)
{
    printf("Enter any of the following commands after the prompt > :\n"
           "\ti <k>  -- Insert <k> (an integer) as both key and value).\n"
           "\ti <k> <v> -- Insert the value <v> (an integer) as the value of key <k> (an integer).\n"
           "\tf <k>  -- Find the value under key <k>.\n"
           "\tp <k> -- Print the path from the root to key k and its associated value.\n"
           "\tr <k1> <k2> -- Print the keys and values found in the range [<k1>, <k2>]\n"
           "\td <k>  -- Delete key <k> and its associated value.\n"
           "\tx -- Destroy the whole tree.  Start again with an empty tree of the same order.\n"
           "\tt -- Print the B+ tree.\n"
           "\tl -- Print the keys of the leaves (bottom row of the tree).\n"
           "\tv -- Toggle output of pointer addresses (\"verbose\") in tree and leaves.\n"
           "\tq -- Quit. (Or use Ctl-D or Ctl-C.)\n"
           "\t? -- Print this help message.\n");
}

/*  Finds the appropriate place to *split a Node that is too big into two.
*   如果一个节点太大，就分裂成两个。变成几个，根据长度来决定。
*   如果长度是2的倍速，就变成长度/2
*   如是长度不是2的倍数，就变成（长度/2）+1
*   length=4，则变成2
*   length=5，则变成3
 */
int Cut(int length)
{
    if (length % 2 == 0)
        return length / 2;
    else
        return length / 2 + 1;
}

/* Helper function for printing the  tree out.  See print_tree.
 */
void EnQueue(Node *NewNode)
{
    Node *c;
    if (queue == NULL)
    {
        queue = NewNode;
        queue->next = NULL;
    }
    else
    {
        c = queue;
        while (c->next != NULL)
        {
            c = c->next;
        }
        c->next = NewNode;
        NewNode->next = NULL;
    }
}

/* Helper function for printing the tree out.  See print_tree.
 */
Node *DeQueue(void)
{
    Node *n = queue;
    queue = queue->next;
    n->next = NULL;
    return n;
}

/* Utility function to give the height of the tree, which length in number of edges
 * of the path from the root to any leaf.
 *  从根开始，如果不是leaf，就从最左边的节点指针往下找。
 */
int GetHeight(Node *const root)
{
    int h = 0;
    Node *c = root;
    while (!c->is_leaf)
    {
        c = c->pointers[0];
        h++;
    }
    return h;
}

/* Utility function to give the length in edges of the path from any Node to the root.
 *  返回从节点到root的高度数值。
 */
int PathToRoot(Node *const root, Node *child)
{
    int length = 0;
    Node *c = child;
    while (c != root)
    {
        c = c->parent;
        length++;
    }
    return length;
}



/* Creates a new record to hold the value
 * to which a key refers.
 */
Record * MakeRecord(int value)
{
    Record *new_record = (Record *)malloc(sizeof(Record));
    if (new_record == NULL)
    {
        perror("Record creation.");
        exit(EXIT_FAILURE);
    }
    else
    {
        new_record->value = value;
    }
    return new_record;
}

Node * MakeEmptyNode(void)
{
    Node *NewNode;
    NewNode = malloc(sizeof(Node));
    if (NewNode == NULL)
    {
        perror("Node creation.");
        exit(EXIT_FAILURE);
    }
    NewNode->keys = malloc((order - 1) * sizeof(int)); // （阶数-1） 个key
    if (NewNode->keys == NULL)
    {
        perror("New Node keys array.");
        exit(EXIT_FAILURE);
    }
    NewNode->pointers = malloc(order * sizeof(void *)); // （阶数） 个key
    if (NewNode->pointers == NULL)
    {
        perror("New Node pointers array.");
        exit(EXIT_FAILURE);
    }
    NewNode->is_leaf = false;
    NewNode->num_keys = 0;
    NewNode->parent = NULL;
    NewNode->next = NULL;
    return NewNode;
}

/* Creates a new leaf by creating a Node
 * and then adapting it appropriately.
 */
Node * MakeEmptyLeaf(void)
{
    Node *leaf = MakeEmptyNode();
    leaf->is_leaf = true;
    return leaf;
}

/* Helper function used in insert_into_parent  to find the index of the parent's pointer to 
 * the Node to the left of the key to be inserted.
 * 在insert_into_parent中使用，找到父节点的指针，指向即将插入的key所在的节点，返回该节点的index.
 * 从父节点，从左边开始寻找，如果小于父节点的key数，且父节点的pointer不是left，继续寻找，直到找到是left。
 */
int GetLeftIndex(Node *parent, Node *left)
{
    int left_index = 0;
    while (left_index <= parent->num_keys && parent->pointers[left_index] != left)
        left_index++;
    return left_index;
}

/* 输出BP tree,按照层级，每个节点的数值之间，使用 | 分开
 * 如果设定 verbose_output,输出指针的值，使用16进制
 */
void PrintTree(Node *const root)
{

    Node *n = NULL;
    int i = 0;
    int rank = 0;
    int new_rank = 0;

    if (root == NULL)
    {
        printf("Empty tree.\n");
        return;
    }
    queue = NULL;   //设置队列为空
    EnQueue(root);  // 把root压入队列
    while (queue != NULL)
    {
        n = DeQueue(); //弹出一个Node
        // 如果
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
                printf("%p ", n->pointers[order - 1]);
            else
                printf("%p ", n->pointers[n->num_keys]);
        }
        printf("| ");
    }
    printf("\n");
}


// The MAIN program

int testBPTree()
{
    char *input_file = "../data/testbptree.txt"; // Data file.
    FILE *fp;
    Node *root;
    int input_key, input_key_2;
    char instruction;

    root = NULL;
    verbose_output = false;

    fp = fopen(input_file, "r");
    if (fp == NULL)
    {
        perror("Failure to open input file.");
        exit(EXIT_FAILURE);
    }
    while (!feof(fp))
    {
        if (fscanf(fp, "%d\n", &input_key) == 1)
            root = insert(root, input_key, input_key);
        else
        {
            perror("Input error.");
            exit(EXIT_FAILURE);
        }
    }
    fclose(fp);
    
    usage_1();

    print_tree(root);

    // return EXIT_SUCCESS;

    printf("> ");
    char buffer[BUFFER_SIZE];
    int count = 0;
    bool line_consumed = false;
    while (scanf("%c", &instruction) != EOF)
    {
        line_consumed = false;
        switch (instruction)
        {
        case 'd':
            scanf("%d", &input_key);
            root = delete (root, input_key);
            print_tree(root);
            break;
        case 'i':
            fgets(buffer, BUFFER_SIZE, stdin);
            line_consumed = true;
            count = sscanf(buffer, "%d %d", &input_key, &input_key_2);
            if (count == 1)
                input_key_2 = input_key;
            root = insert(root, input_key, input_key_2);
            print_tree(root);
            break;
        case 'f':
        case 'p':
            scanf("%d", &input_key);
            find_and_print(root, input_key, instruction == 'p');
            break;
        case 'r':
            scanf("%d %d", &input_key, &input_key_2);
            if (input_key > input_key_2)
            {
                int tmp = input_key_2;
                input_key_2 = input_key;
                input_key = tmp;
            }
            find_and_print_range(root, input_key, input_key_2, instruction == 'p');
            break;
        case 'l':
            print_leaves(root);
            break;
        case 'q':
            while (getchar() != (int)'\n')
                ;
            if (root)
            {
                root = destroy_tree(root);
                if (root == NULL)
                    printf("The tree is destoried.\n");
            }

            return EXIT_SUCCESS;
            break;
        case 't':
            print_tree(root);
            break;
        case 'v':
            verbose_output = !verbose_output;
            break;
        case 'x':
            if (root)
                root = destroy_tree(root);
            print_tree(root);
            break;
        default:
            usage_2();
            break;
        }
        if (!line_consumed)
            while (getchar() != (int)'\n')
                ;
        printf("> ");
    }
    printf("\n");

    return EXIT_SUCCESS;
}

