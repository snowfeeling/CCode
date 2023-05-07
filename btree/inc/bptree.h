/*
*  BTree.h
*   Created by Wangss on 2023-04-29
*/

#ifndef BTREE_H_
#define BTREE_H_

#define Version "0.1.0"

 #include <stdio.h>
 #include <string.h>
 #include <stdbool.h>
 #include <time.h>
 #include <stdlib.h>

/*
const definition.
*/
#define BUFFER_SIZE 256
#define ID_LENGTH 9
#define NAME_LENGTH 40
#define TIME_LENGTH 20
#define DEFALUT_TREE_DEGREE 7

/* Leaf - Data Record Struct
*/
struct bpt_data_record
{
    int key;
    char id[ID_LENGTH];
    char name[NAME_LENGTH];
    char create_time[TIME_LENGTH];
};
typedef struct bpt_data_record BPT_DATA_RECORD;

/* Internal Node Struct of BPPlus tree.
*/ 
struct bpt_node
{
    int keys[DEFALUT_TREE_DEGREE];
    struct bpt_node *pointers[DEFALUT_TREE_DEGREE + 1]; 
    BPT_DATA_RECORD *leaf[DEFALUT_TREE_DEGREE];
    bool is_leaf;
    int keys_num;
    struct bpt_node *parent;
    struct bpt_node *next;
    struct bpt_node *print_point;
};
typedef struct bpt_node BPT_NODE;


/* BPlus Tree Stuct
*/
struct bplus_tree
{
    int max_degreee;
    int min_key_num;
    int max_key_num;
    int split_index;
    int next_index;
    int leaf_num;
    int tree_height;
    BPT_NODE *root;
};
typedef struct bplus_tree BPLUS_TREE;

int test_bplus_tree(); 

#endif
