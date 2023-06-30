// SPDX-License-Identifier: GPL-2.0-or-later.
/*
*  BPlusTree.h
*   Created by Wangss on 2022-10-30
*/

#ifndef BPLUSTREE_H_
#define BPLUSTREE_H_

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
#define DEFALUT_TREE_DEGREE 4

/* Leaf - Data Record Struct
*/
struct data_record
{
    int key;
    char id[BUFFER_SIZE];
    char name[NAME_LENGTH];
    char create_time[TIME_LENGTH];
};
typedef struct data_record DATA_RECORD;

/* Internal Node Struct of BPPlus tree.
*/ 
struct bpt_node
{
    int keys[DEFALUT_TREE_DEGREE];
    struct bpt_node *pointers[DEFALUT_TREE_DEGREE + 1]; 
    DATA_RECORD *leaf[DEFALUT_TREE_DEGREE];
    bool is_leaf;
    int keys_num;
    struct bpt_node *parent;
    struct bpt_node *next;
    struct bpt_node *print_point;
};
typedef struct bpt_node BPT_Node;


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
    BPT_Node *root;
};
typedef struct bplus_tree BPLUS_TREE;

int test_bplus_tree(); 

#endif
