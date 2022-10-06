#ifndef BPTREE_H_
#define BPTREE_H_

#define Version "0.1.0"

#include <stdbool.h>

// Default order is 4.B+树的阶数
#define DEFAULT_ORDER 6

// 最小阶数必须是3，最大可以自己确定
#define MIN_ORDER 3
#define MAX_ORDER 20

// Constant for optional command-line input with "i" command.
#define BUFFER_SIZE 256

// TYPES.

/* 
 * 这个是key所代表的记录，可能是在DB/文件中的数据
 */
typedef struct record
{
    int value;
} Record;

/* Type representing a node in the B+ tree.
 * B+树的节点定义
 * This type is general enough to serve for both
 * the leaf and the internal node.
 * 可以代表叶子和内部节点
 * 核心是Key指针组和相关的指针。key和pointer的区别在于叶子和内部节点。
 * 在叶子上，key的index等于pointer的index，最大是阶数-1个 Key-Pointer对。
 * 最后的pointer指向右边或NULL（最右边的叶子中）
 * 在内部节点中，第一个Pointer指向小于是最小key的节点组。Index从0开始，pointrt[i+1]指向大于等于 key[i]的节点组
 * num_keys 是本节点中的key数量。
 * 在内部节点中，有效Pointer数目=num_keys+1， 存放的是指针。
 * 在叶子节点中，有效Pointer数目=num_keys，存放的是data。
 * 最后的叶子Pointer指向下一个叶子。
 */
struct _node
{
    void **pointers;  // 有DEFAULT_ORDER个Pointer，
    int *keys ;       // 最多有DEFAULT_ORDER-1个Key
    struct _node *parent;            //指向父节点
    bool is_leaf;                   // 是否是叶子节点
    int num_keys;                   //最多有DEFAULT_ORDER -1个key
    struct _node *next;              // Used for queue.
} ;

typedef struct _node node;

/* Tree struct
*   Record the tree's character 
*   
*/

struct my_tree
{
    node * root;
    int leaf_num;
    int tree_height;
    int tree_order;
};


typedef struct my_tree TREE;

/*=======================================
*   Tree functions
*/
int get_tree_height();
int get_tree_leaf_num();

int testBPTree();

#endif