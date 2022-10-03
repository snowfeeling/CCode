#ifndef BTREE_H_
#define BTREE_H_


#define DEGREE		3
typedef int KEY_VALUE;
 
typedef struct _btree_node {
	KEY_VALUE *keys;
	struct _btree_node **childrens;
	int num;
	int leaf;
} btree_node;
 
typedef struct _btree {
	btree_node *root;
	int t;
} btree;
 
int testBTree();


#endif
