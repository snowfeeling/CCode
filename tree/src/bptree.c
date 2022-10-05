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
 *  defined as the maximal number of pointers in any node.
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
void enqueue(Node *new_node);
Node *dequeue(void);

Record *make_record(int value);


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

/* Helper function for printing the  tree out.  See print_tree.
 */
void enqueue(Node *new_node)
{
    Node *c;
    if (queue == NULL)
    {
        queue = new_node;
        queue->next = NULL;
    }
    else
    {
        c = queue;
        while (c->next != NULL)
        {
            c = c->next;
        }
        c->next = new_node;
        new_node->next = NULL;
    }
}

/* Helper function for printing the tree out.  See print_tree.
 */
Node *dequeue(void)
{
    Node *n = queue;
    queue = queue->next;
    n->next = NULL;
    return n;
}

/* Creates a new record to hold the value
 * to which a key refers.
 */
Record *make_record(int value)
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

