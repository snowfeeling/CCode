/* pe17-7b.c -- copy of tree.c -- tree support functions */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../inc/tree.h"

/* local data type */
typedef struct pair
{
    Node *parent;
    Node *child;
} Pair;

/* protototypes for local functions */
static Node *MakeNode(const Item *pi);
static bool ToLeft(const Item *i1, const Item *i2);
static bool ToRight(const Item *i1, const Item *i2);
static void AddNode(Node *new_node, Node *root);
static void InOrder(const Node *root, void (*pfun)(Item item));
static Pair SeekItem(const Item *pi, const Tree *ptree);
static void DeleteNode(Node **ptr);
static void DeleteAllNodes(Node *ptr);

/* function definitions */
void InitializeTree(Tree *ptree)
{
    ptree->root = NULL;
    ptree->size = 0;
}
bool TreeIsEmpty(const Tree *ptree)
{
    if (ptree->root == NULL)
        return true;
    else
        return false;
}

bool TreeIsFull(const Tree *ptree)
{
    if (ptree->size == MAXITEMS)
        return true;
    else
        return false;
}
int TreeItemCount(const Tree *ptree)
{
    return ptree->size;
}

bool AddItem(const Item *pi, Tree *ptree)
{
    Node *new;
    Pair seek;
    if (TreeIsFull(ptree))
    {
        fprintf(stderr, "Tree is full\n");
        return false; /* early return */
    }
    if ((seek = SeekItem(pi, ptree)).child != NULL)
    {
        seek.child->item.count++;
        return true; /* early return */
    }
    new = MakeNode(pi); /* new points to new node */
    if (new == NULL)
    {
        fprintf(stderr, "Couldn't create node\n");
        return false; /* early return */
    }
    /* succeeded in creating a new node */
    ptree->size++;
    if (ptree->root == NULL)       /* case 1: tree is empty */
        ptree->root = new;         /* new node is tree root */
    else                           /* case 2: not empty */
        AddNode(new, ptree->root); /* add new node to tree */
    return true;
}
bool InTree(const Item *pi, const Tree *ptree)
{
    return (SeekItem(pi, ptree).child == NULL) ? false : true;
}
const Item *WhereInTree(const Item *pi, const Tree *ptree)
{
    Node *pn;
    pn = SeekItem(pi, ptree).child;
    if (pn != NULL)
        return &(pn->item);
    else
        return NULL;
}

bool DeleteItem(const Item *pi, Tree *ptree)
{
    Pair look;
    look = SeekItem(pi, ptree);
    if (look.child == NULL)
        return false;
    if (look.child->item.count > 0)
        look.child->item.count--;
    else
    {
        /* delete root item */
        if (look.parent == NULL)
            DeleteNode(&ptree->root);
        else if (look.parent->left == look.child)
            DeleteNode(&look.parent->left);
        else
            DeleteNode(&look.parent->right);
        ptree->size--;
    }
    return true;
}
void Traverse(const Tree *ptree, void (*pfun)(Item item))
{
    if (ptree != NULL)
        InOrder(ptree->root, pfun);
}

void DeleteAll(Tree *ptree)
{
    if (ptree != NULL)
        DeleteAllNodes(ptree->root);
    ptree->root = NULL;
    ptree->size = 0;
}
/* local functions */
static void InOrder(const Node *root, void (*pfun)(Item item))
{
    if (root != NULL)
    {
        InOrder(root->left, pfun);
        (*pfun)(root->item);
        InOrder(root->right, pfun);
    }
}

void SetTreeLevel(const Tree *ptree, int k, char ch)
{
    if (ptree != NULL)
        InOrder1(ptree->root, k, ch);
}
static void InOrder1(Node *root, int k, char ch)
{
    if (root != NULL)
    {
        InOrder1(root->left, k + 1, 'L');
        root->item.level = k;
        root->item.LR = ch;
        InOrder1(root->right, k + 1, 'R');
    }
}

static void DeleteAllNodes(Node *root)
{
    Node *pright;
    if (root != NULL)
    {
        pright = root->right;
        DeleteAllNodes(root->left);
        printf("%s [*%d] [%c:%d]\n", root->item.word, root->item.count, root->item.LR, root->item.level);
        free(root);
        DeleteAllNodes(pright);
    }
}
static void AddNode(Node *new_node, Node *root)
{
    if (ToLeft(&new_node->item, &root->item))
    {
        if (root->left == NULL)    /* empty subtree */
            root->left = new_node; /* so add node here */
        else
            AddNode(new_node, root->left); /* else process subtree*/
    }
    else if (ToRight(&new_node->item, &root->item))
    {
        if (root->right == NULL)
            root->right = new_node;
        else
            AddNode(new_node, root->right);
    }
    else
    { /* should be no duplicates */
        fprintf(stderr, "location error in AddNode()\n");
        exit(1);
    }
}
static bool ToLeft(const Item *i1, const Item *i2)
{
    if (strcmp(i1->word, i2->word) < 0)
        return true;
    else
        return false;
}
static bool ToRight(const Item *i1, const Item *i2)
{
    if (strcmp(i1->word, i2->word) > 0)
        return true;
    else
        return false;
}
static Node *MakeNode(const Item *pi)
{
    Node *new_node;
    new_node = (Node *)malloc(sizeof(Node));
    if (new_node != NULL)
    {
        new_node->item = *pi;
        new_node->item.count = 1;
        new_node->left = NULL;
        new_node->right = NULL;
    }
    return new_node;
}
static Pair SeekItem(const Item *pi, const Tree *ptree)
{
    Pair look;
    look.parent = NULL;
    look.child = ptree->root;
    if (look.child == NULL)
        return look;
    while (look.child != NULL)
    {
        if (ToLeft(pi, &(look.child->item)))
        {
            look.parent = look.child;
            look.child = look.child->left;
        }
        /* early return   */
        else if (ToRight(pi, &(look.child->item)))
        {
            look.parent = look.child;
            look.child = look.child->right;
        }
        else       /* must be same if not to left or right */
            break; /* look.child is address of node with item */
    }
    return look; /* successful return */
}
static void DeleteNode(Node **ptr)
/* ptr is address of parent member pointing to target node */
{
    Node *temp;
    printf("%s", (*ptr)->item.word);

    if ((*ptr)->left == NULL)
    {
        temp = *ptr;
        *ptr = (*ptr)->right;
        free(temp);
    }
    else if ((*ptr)->right == NULL)
    {
        temp = *ptr;
        *ptr = (*ptr)->left;
        free(temp);
    }
    else /* deleted node has two children */
    {    /* find where to reattach right subtree */
        for (temp = (*ptr)->left; temp->right != NULL;
             temp = temp->right)
            continue;
        temp->right = (*ptr)->right;
        temp = *ptr;
        *ptr = (*ptr)->left;
        free(temp);
    }
}
