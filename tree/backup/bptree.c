
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int size = 5; // number of pointers of a node

#include "../inc/bptree.h"

void print_tree(Node *root);
Record *find(Node *root, char *key);
Node *find_leaf(Node *root, char *key);

// Insertion
Record *new_record(int value);
Node *make_new_Node();
Node *make_new_leaf();
Node *make_new_tree(char *key, Record *rec);
Node *make_new_root(Node *left, Node *right, char *key);
Node *insert(Node *root, char *key, int value);
Node *insert_into_parent(Node *root, Node *left, Node *right, char *key);
void insert_into_Node(Node *nd, Node *right, int index, char *key);
Node *insert_into_Node_after_splitting(Node *root, Node *nd, Node *right, int index, char *key);
Node *insert_into_leaf_after_splitting(Node *root, Node *leaf, int index, char *key, Record *rec);
void insert_into_leaf(Node *leaf, int index, char *key, Record *rec);

// Deletion
void destroy_Node(Node *nd);
void remove_entry(Node *nd, int index);
Node *delete (Node *root, char *key);
Node *delete_entry(Node *root, Node *nd, int index);
Node *adjust_root(Node *root);
int get_Node_index(Node *nd);
Node *coalesce_Nodes(Node *root, Node *nd, Node *neighbor, int nd_index);
void distribute_Nodes(Node *nd, Node *neighbor, int nd_index);

// for test
void test_find(Node *root)
{
    char *key;
    Record *r;
    key = malloc(MAX_KEY_LEN);
    while (1)
    {
        scanf("%s", key);
        if (strcmp(key, "exit") == 0)
            break;
        r = find(root, key);
        if (r == NULL)
        {
            printf("Not found!!\n");
            continue;
        }
        printf("Record of %s: %d\n", key, r->value);
    }
}

Node *test_delete(Node *root)
{
    char *key;
    key = malloc(MAX_KEY_LEN);
    while (1)
    {
        scanf("%s", key);
        if (strcmp(key, "exit") == 0)
            break;
        root = delete (root, key);
        print_tree(root);
    }
    return root;
}
// end of test

void print_tree(Node *root)
{
    Node *p, *p_down;
    int i;
    if (root == NULL)
    {
        printf("Empty tree!\n");
        return;
    }
    p = root;
    p_down = root;
    while (!p->is_leaf)
    {
        for (i = 0; i < p->num_keys; i++)
            printf("%s ", p->keys[i]);

        // printf("%d ", p->keys[i][0]);  // for test
        printf("| ");
        p = p->next;
        if (!p)
        {
            p_down = p_down->pointers[0]; // next level
            p = p_down;
            printf("\n");
        }
    }

    while (p)
    {
        for (i = 0; i < p->num_keys; i++)
            //printf("%s ", p->keys[i]);
            printf("%s(%d) ", p->keys[i], ((Record *)p->pointers[i])->value);

        printf(" | ");
        p = p->pointers[size - 1]; //为什么是这个？
    }
    printf("\n");
}

Record *find(Node *root, char *key)
{
    Node *leaf;
    int i;
    leaf = find_leaf(root, key);
    if (leaf == NULL)
        return NULL;
    for (i = 0; i < leaf->num_keys && strcmp(leaf->keys[i], key) != 0; i++)
        ;
    if (i == leaf->num_keys)
        return NULL;
    return (Record *)leaf->pointers[i];
}

Node *find_leaf(Node *root, char *key)
{
    Node *nd;
    int i;
    if (root == NULL)
        return root;
    nd = root;
    while (!nd->is_leaf)
    {
        for (i = 0; i < nd->num_keys && strcmp(nd->keys[i], key) <= 0; i++)
            ;
        nd = (Node *)nd->pointers[i];
    }
    return nd;
}

Record *new_record(int value)
{
    Record *rec;
    rec = (Record *)malloc(sizeof(Record));
    rec->value = value;
    return rec;
}

Node *make_new_Node()
{
    Node *nd;
    nd = (Node *)malloc(sizeof(Node));
    nd->pointers = malloc(size * sizeof(void *));
    nd->keys = malloc((size - 1) * sizeof(char *));
    nd->parent = NULL;
    nd->num_keys = 0;
    nd->is_leaf = false;
    nd->next = NULL;
    return nd;
}

Node *make_new_leaf()
{
    Node *leaf;
    leaf = make_new_Node();
    leaf->is_leaf = true;
    return leaf;
}

Node *make_new_tree(char *key, Record *rec)
{
    Node *root;
    root = make_new_leaf();
    root->pointers[0] = rec;
    root->keys[0] = malloc(MAX_KEY_LEN);
    strcpy(root->keys[0], key);
    root->pointers[size - 1] = NULL;
    root->num_keys++;
    return root;
}

Node *make_new_root(Node *left, Node *right, char *key)
{
    Node *root;
    root = make_new_Node();
    root->pointers[0] = left;
    root->pointers[1] = right;
    root->keys[0] = malloc(MAX_KEY_LEN);
    strcpy(root->keys[0], key);
    root->num_keys++;
    left->parent = root;
    right->parent = root;
    return root;
}

Node *insert(Node *root, char *key, int value)
{
    Record *rec;
    Node *leaf;
    int index, cond;
    leaf = find_leaf(root, key);
    if (!leaf)
    { // cannot find the leaf, the tree is empty
        rec = new_record(value);
        return make_new_tree(key, rec);
    }
    // for (index = 0; index < leaf->num_keys && (cond = strcmp(leaf->keys[index], key)) <= 0; index++) // 找到第一个大于的项,会加入重复项；
    for (index = 0; index < leaf->num_keys && (cond = strcmp(leaf->keys[index], key)) < 0; index++) // 如果只是 <0,则会去掉重复的值
        ;

    if (cond == 0) // ignore duplicates
    {
        printf("[Duplicated key: %s]\n", key);
        return root;
    }
    rec = new_record(value);
    if (leaf->num_keys < size - 1)
    {
        insert_into_leaf(leaf, index, key, rec);
        return root; // the root remains unchanged
    }
    return insert_into_leaf_after_splitting(root, leaf, index, key, rec);
}

Node *insert_into_parent(Node *root, Node *left, Node *right, char *key)
{
    Node *parent;
    int index, i;
    parent = left->parent;

    if (parent == NULL)
    {
        return make_new_root(left, right, key);
    }

    for (index = 0; index < parent->num_keys && parent->pointers[index] != left; index++)
        ;
    ;
    if (parent->num_keys < size - 1)
    {
        insert_into_Node(parent, right, index, key);
        return root; // the root remains unchanged
    }
    return insert_into_Node_after_splitting(root, parent, right, index, key);
}

void insert_into_Node(Node *nd, Node *right, int index, char *key)
{
    int i;
    for (i = nd->num_keys; i > index; i--)
    {
        nd->keys[i] = nd->keys[i - 1];
        nd->pointers[i + 1] = nd->pointers[i];
    }
    nd->keys[index] = malloc(MAX_KEY_LEN);
    strcpy(nd->keys[index], key);
    nd->pointers[index + 1] = right;
    nd->num_keys++;
}

Node *insert_into_Node_after_splitting(Node *root, Node *nd, Node *right, int index, char *key)
{
    int i, split;
    Node **temp_ps, *new_nd, *child;
    char **temp_ks, *new_key;
    temp_ps = malloc((size + 1) * sizeof(Node *));
    temp_ks = malloc(size * sizeof(char *));

    for (i = 0; i < size + 1; i++)
    {
        if (i == index + 1)
            temp_ps[i] = right;
        else if (i < index + 1)
            temp_ps[i] = nd->pointers[i];
        else
            temp_ps[i] = nd->pointers[i - 1];
    }
    for (i = 0; i < size; i++)
    {
        if (i == index)
        {
            temp_ks[i] = malloc(MAX_KEY_LEN);
            strcpy(temp_ks[i], key);
        }
        else if (i < index)
            temp_ks[i] = nd->keys[i];
        else
            temp_ks[i] = nd->keys[i - 1];
    }

    split = size % 2 ? size / 2 + 1 : size / 2; // split is #pointers
    nd->num_keys = split - 1;
    for (i = 0; i < split - 1; i++)
    {
        nd->pointers[i] = temp_ps[i];
        nd->keys[i] = temp_ks[i];
    }
    nd->pointers[i] = temp_ps[i]; // i == split - 1
    new_key = temp_ks[split - 1];

    new_nd = make_new_Node();
    new_nd->num_keys = size - split;
    for (++i; i < size; i++)
    {
        new_nd->pointers[i - split] = temp_ps[i];
        new_nd->keys[i - split] = temp_ks[i];
    }
    new_nd->pointers[i - split] = temp_ps[i];
    new_nd->parent = nd->parent;
    for (i = 0; i <= new_nd->num_keys; i++)
    { //  #pointers == num_keys + 1
        child = (Node *)(new_nd->pointers[i]);
        child->parent = new_nd;
    }
    new_nd->next = nd->next;
    nd->next = new_nd;

    free(temp_ps);
    free(temp_ks);
    return insert_into_parent(root, nd, new_nd, new_key);
}

void insert_into_leaf(Node *leaf, int index, char *key, Record *rec)
{
    int i;
    for (i = leaf->num_keys; i > index; i--)
    {
        leaf->keys[i] = leaf->keys[i - 1];
        leaf->pointers[i] = leaf->pointers[i - 1];
    }
    leaf->keys[index] = malloc(MAX_KEY_LEN);
    strcpy(leaf->keys[index], key);
    leaf->pointers[index] = rec;
    leaf->num_keys++;
}

Node *insert_into_leaf_after_splitting(Node *root, Node *leaf, int index, char *key, Record *rec)
{
    Node *new_leaf;
    Record **temp_ps;
    char **temp_ks, *new_key;
    int i, split;

    temp_ps = malloc(size * sizeof(Record *));
    temp_ks = malloc(size * sizeof(char *));
    for (i = 0; i < size; i++)
    {
        if (i == index)
        {
            temp_ps[i] = rec;
            temp_ks[i] = malloc(MAX_KEY_LEN);
            strcpy(temp_ks[i], key);
        }
        else if (i < index)
        {
            temp_ps[i] = leaf->pointers[i];
            temp_ks[i] = leaf->keys[i];
        }
        else
        {
            temp_ps[i] = leaf->pointers[i - 1];
            temp_ks[i] = leaf->keys[i - 1];
        }
    }

    split = size / 2;
    leaf->num_keys = split;
    for (i = 0; i < split; i++)
    {
        leaf->pointers[i] = temp_ps[i];
        leaf->keys[i] = temp_ks[i];
    }

    new_leaf = make_new_leaf();
    new_leaf->num_keys = size - split;
    for (; i < size; i++)
    {
        new_leaf->pointers[i - split] = temp_ps[i];
        new_leaf->keys[i - split] = temp_ks[i];
    }

    new_leaf->parent = leaf->parent;
    new_leaf->pointers[size - 1] = leaf->pointers[size - 1];
    leaf->pointers[size - 1] = new_leaf;
    free(temp_ps);
    free(temp_ks);
    new_key = new_leaf->keys[0];
    return insert_into_parent(root, leaf, new_leaf, new_key);
}

Node *delete (Node *root, char *key)
{
    Node *leaf;
    Record *rec;
    int i;
    leaf = find_leaf(root, key);
    if (leaf == NULL)
        return root;
    for (i = 0; i < leaf->num_keys && strcmp(leaf->keys[i], key) != 0; i++)
        ;
    if (i == leaf->num_keys) // no such key
        return root;
    rec = (Record *)leaf->pointers[i];
    root = delete_entry(root, leaf, i);
    return root;
}

Node *delete_entry(Node *root, Node *nd, int index)
{
    int min_keys, cap, nd_index;
    Node *neighbor;

    remove_entry(nd, index);
    if (nd == root)
        return adjust_root(nd);
    min_keys = nd->is_leaf ? size / 2 : (size - 1) / 2;
    if (nd->num_keys >= min_keys)
    {
        return root;
    }

    nd_index = get_Node_index(nd);
    if (nd_index == 0)
        neighbor = nd->parent->pointers[1]; // right neighbor
    else
        neighbor = nd->parent->pointers[nd_index - 1]; // left neighbor

    cap = nd->is_leaf ? size - 1 : size - 2;
    if (neighbor->num_keys + nd->num_keys <= cap)
        return coalesce_Nodes(root, nd, neighbor, nd_index);

    distribute_Nodes(nd, neighbor, nd_index);
    return root;
}

void distribute_Nodes(Node *nd, Node *neighbor, int nd_index)
{
    int i;
    Node *tmp;
    if (nd_index != 0)
    {
        if (!nd->is_leaf)
            nd->pointers[nd->num_keys + 1] = nd->pointers[nd->num_keys];
        for (i = nd->num_keys; i > 0; i--)
        { // shift to right by 1
            nd->keys[i] = nd->keys[i - 1];
            nd->pointers[i] = nd->pointers[i - 1];
        }
        if (!nd->is_leaf)
        {
            nd->keys[0] = nd->parent->keys[nd_index - 1];

            nd->pointers[0] = neighbor->pointers[neighbor->num_keys];
            tmp = (Node *)nd->pointers[0];
            tmp->parent = nd;
            neighbor->pointers[neighbor->num_keys] = NULL;

            nd->parent->keys[nd_index - 1] = neighbor->keys[neighbor->num_keys - 1];
            neighbor->keys[neighbor->num_keys - 1] = NULL;
        }
        else
        {
            nd->keys[0] = neighbor->keys[neighbor->num_keys - 1];
            neighbor->keys[neighbor->num_keys - 1] = NULL;

            nd->pointers[0] = neighbor->pointers[neighbor->num_keys - 1];
            neighbor->pointers[neighbor->num_keys - 1] = NULL;

            // nd->parent->keys[nd_index - 1] = nd->keys[0];  //  share the same key with child !!
            strcpy(nd->parent->keys[nd_index - 1], nd->keys[0]);
        }
    }
    else
    {
        if (!nd->is_leaf)
        {
            nd->keys[nd->num_keys] = nd->parent->keys[0]; // link to father's key
            nd->pointers[nd->num_keys + 1] = neighbor->pointers[0];
            tmp = (Node *)nd->pointers[nd->num_keys + 1];
            tmp->parent = nd;
            nd->parent->keys[0] = neighbor->keys[0]; //
        }
        else
        {
            nd->keys[nd->num_keys] = neighbor->keys[0];
            nd->pointers[nd->num_keys] = neighbor->pointers[0];
            // nd->parent->keys[0] = neighbor->keys[1];  // share the same key with chid !!
            strcpy(nd->parent->keys[0], neighbor->keys[1]);
        }
        for (i = 0; i < neighbor->num_keys - 1; i++)
        {
            neighbor->keys[i] = neighbor->keys[i + 1];
            neighbor->pointers[i] = neighbor->pointers[i + 1];
        }
        neighbor->keys[i] = NULL;
        if (!nd->is_leaf)
            neighbor->pointers[i] = neighbor->pointers[i + 1];
        else
            neighbor->pointers[i] = NULL;
    }

    neighbor->num_keys--;
    nd->num_keys++;
}

Node *coalesce_Nodes(Node *root, Node *nd, Node *neighbor, int nd_index)
{
    int i, j, start, end;
    char *k_prime;
    Node *tmp, *parent;

    if (nd_index == 0)
    { // make sure neighbor is on the left
        tmp = nd;
        nd = neighbor;
        neighbor = tmp;
        nd_index = 1;
    }
    parent = nd->parent;

    start = neighbor->num_keys;
    if (nd->is_leaf)
    {
        for (i = start, j = 0; j < nd->num_keys; i++, j++)
        {
            neighbor->keys[i] = nd->keys[j];
            neighbor->pointers[i] = nd->pointers[j];
            nd->keys[j] = NULL;
            nd->pointers[j] = NULL;
        }
        neighbor->num_keys += nd->num_keys;
        neighbor->pointers[size - 1] = nd->pointers[size - 1];
    }
    else
    {
        neighbor->keys[start] = malloc(MAX_KEY_LEN);
        strcpy(neighbor->keys[start], parent->keys[nd_index - 1]);
        // neighbor->keys[start] = parent->keys[nd_index - 1];
        for (i = start + 1, j = 0; j < nd->num_keys; i++, j++)
        {
            neighbor->keys[i] = nd->keys[j];
            neighbor->pointers[i] = nd->pointers[j];
        }
        neighbor->pointers[i] = nd->pointers[j];
        neighbor->num_keys += nd->num_keys + 1;
        neighbor->next = nd->next;

        for (i = 0; i <= neighbor->num_keys; i++)
        {
            tmp = (Node *)neighbor->pointers[i];
            tmp->parent = neighbor;
        }
    }
    destroy_Node(nd);
    return delete_entry(root, parent, nd_index);
}

int get_Node_index(Node *nd)
{
    Node *parent;
    int i;
    parent = nd->parent;
    for (i = 0; i < parent->num_keys && parent->pointers[i] != nd; i++)
        ;
    return i;
}

void destroy_Node(Node *nd)
{
    free(nd->keys);
    free(nd->pointers);
    free(nd);
}

Node *adjust_root(Node *root)
{
    Node *new_root;
    if (root->num_keys > 0) // at least two childs
        return root;
    if (!root->is_leaf)
    { // root has only one child
        new_root = root->pointers[0];
        new_root->parent = NULL;
    }
    else
        new_root = NULL;
    destroy_Node(root);
    return new_root;
}

void remove_entry(Node *nd, int index)
{
    int i, index_k;

    if (nd->is_leaf)
    {
        free(nd->keys[index]);
        free(nd->pointers[index]); // destroy the Record
        for (i = index; i < nd->num_keys - 1; i++)
        {
            nd->keys[i] = nd->keys[i + 1];
            nd->pointers[i] = nd->pointers[i + 1];
        }
        nd->keys[i] = NULL;
        nd->pointers[i] = NULL;
    }
    else
    {
        index_k = index - 1; // index_p == index
        free(nd->keys[index_k]);
        for (i = index_k; i < nd->num_keys - 1; i++)
        {
            nd->keys[i] = nd->keys[i + 1];
            nd->pointers[i + 1] = nd->pointers[i + 2];
        }
        nd->keys[i] = NULL;
        nd->pointers[i + 1] = NULL;
    }
    nd->num_keys--;
}

int testBPTree()
{
    Node *root = NULL;
    Record *r;
    char *terms[] = {"a", "b", "c", "d", "e", "f", "g",
                     "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r",
                     "s", "t", "u", "v", "w", "x", "y", "z"};
    int i;
    size = 6;
    for (i = 0; i < 26; i++)
    {
        root = insert(root, terms[i], i + 1000);
        printf("[Key=%s Value=%d processed.]------------------------------\n", terms[i], i + 1000);
        print_tree(root);
    }
    print_tree(root);
    return 0;
}
