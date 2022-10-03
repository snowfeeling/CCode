#ifndef BPTREE_H_
#define BPTREE_H_

#include <stdbool.h>

#define MAX_KEY_LEN 15

// each key is a string pointer
// num_keys if the number of keys, number of pointers = num_keys + 1
struct node
{
    void **pointers;        // 叶子结点，放的是Value，中间节点是下面的指针；
    char **keys;            //放的是键值
    struct node *parent;    //父节点的指针
    int num_keys;           //本节点的key数量
    bool is_leaf;           //是否叶子节点
    struct node *next;      //临近节点的指针
};

typedef struct node Node;

struct record
{
    int value;
};

typedef struct record Record;

int testBPTree();

#endif