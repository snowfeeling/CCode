#ifndef MYTREE_H_
#define MYTREE_H_

/*
*   Mytree.h
*   Created by Wangss on 2022-10-19
*/
#define Version "0.1.0"

#include <stdbool.h>

#define DEFAULT_ORDER   4
#define BUFFER_SIZE 256
// Record Length
#define ID_LENGTH   9
#define NAME_LENGTH 40
#define TIME_LENGTH 20

/* 
 * 这个是key所代表的记录，可能是在DB/文件中的数据
 */
typedef struct data_record
{
    int key;
    char id[ID_LENGTH];
    char name[NAME_LENGTH];
    char create_time[TIME_LENGTH+1];
} Data_Record;

struct tree_node
{
    void **pointers;  // 有DEFAULT_ORDER个Pointer,叶子节点，指向Data_Record; 非叶子节点，指向Tree_Node
    int *keys ;       // 最多有DEFAULT_ORDER-1个Key
    struct tree_node *parent;            //指向父节点
    bool is_leaf;                   // 是否是叶子节点
    int num_keys;                   //最多有DEFAULT_ORDER -1个key
    struct tree_node *next;              // Used for queue.
} ;

typedef struct tree_node Tree_Node;

/* Tree struct
*   Record the tree's character 
*/
struct bp_tree
{
    Tree_Node * root;
    int leaf_num;
    int tree_height;
    int tree_order;
};
typedef struct bp_tree BP_TREE;


int mybptree();


#endif