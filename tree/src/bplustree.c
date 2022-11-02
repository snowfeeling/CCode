
// SPDX-License-Identifier: GPL-2.0-or-later.
/*
 *  src/bplustree.c
 *
 *  Copyright (C) 2022,  Wangss(Wang Shu Song)
 *  
 * Created by Wangss on 2022-10-30 17:08
 * To study the BPlus Tree.
 * 
 */
#include "../inc/bplustree.h"

static void usage();
static void ERROR_EXIT(char* str);
static size_t get_current_time( char * time_info);

/*=============================================================================
* BP Tree functions.
*
*/
static DATA_RECORD *create_data_record(DATA_RECORD *drp);
static BPT_Node *create_empty_node();
static BPT_Node *create_empty_leaf();
static BPT_Node *create_new_tree(DATA_RECORD *drp);
static BPT_Node *insert_record_to_tree(BPLUS_TREE *tree, DATA_RECORD *drp);
static BPT_Node *insert_into_bptree_node(BPT_Node *np, DATA_RECORD *drp);
static BPT_Node *make_tree_from_file();


/*=============================================================================
* Global variable
*
*/
int tree_degree = DEFALUT_TREE_DEGREE;
static BPLUS_TREE bptree;
static BPT_Node *queue = NULL;

/*=============================================================================
* 有关Tree 处理程序定义
*/
static void enqueue(BPT_Node *new_node);
static BPT_Node *dequeue(void);
static int path_to_root(BPT_Node *const root, BPT_Node *child);


int init_tree(BPLUS_TREE *tree);
BPT_Node *insert_repair(BPT_Node *np);
BPT_Node *insert_into_bptree(BPT_Node *np, DATA_RECORD *drp);
BPT_Node *split_bptree(BPT_Node *np);

static int print_bptree_leaves(BPLUS_TREE bptree);
static void print_bptree(BPT_Node *const root);


/*=============================================================================
* Fundation functions
* 
*/
static void usage()
{
    printf("\nWelcoming to BPlus Tree Demo.\n");
    printf("\tPlease see the README for your usage.\n");
    printf("\tThe default tree degreee is 4. You can change it when the tree is empty.\n\n\n");
}

static void ERROR_EXIT(char* str)
{
    /*if (root)
    {
        list_leaves(root);
        destroy_tree(root);
    }*/

    perror(str);
    exit(EXIT_FAILURE);
}

static size_t get_current_time( char * time_info)
{
    time_t current_time;
    struct tm *ptime;

    time(&current_time);
    ptime = localtime(&current_time);
    size_t result_num = strftime (time_info, 20,"%Y-%m-%d %H:%M:%S",ptime);
    return result_num;
}


/*=============================================================================
* 有关Tree 处理程序
*/
/* Tree初始化
*/
int init_tree(BPLUS_TREE *tree)
{
    tree->max_degreee = tree_degree;
    tree->min_key_num = (tree_degree + 1 ) / 2 -1;
    tree->max_key_num = tree_degree - 1;
    tree->split_index = tree_degree /2 ;
    tree->next_index = 3;
    tree->leaf_num = 0;
    tree->tree_height = 0;
    tree->root =  NULL;

}

static int path_to_root(BPT_Node *const root, BPT_Node *child)
{
    int length = 0;
    BPT_Node *c = child;
    while (c != root)
    {
        c = c->parent;
        length++;
    }
    return length;
}


static void enqueue(BPT_Node *new_node)
{
    BPT_Node *c;
    if (queue == NULL)
    {
        queue = new_node;
        queue->print_point = NULL;
    }
    else
    {
        c = queue;
        while (c->print_point != NULL)
            c = c->print_point;
        c->print_point = new_node;
        new_node->print_point = NULL;
    }
}

/* Get one mode from queue.
*  
 */
static BPT_Node *dequeue(void)
{
    BPT_Node *n = queue;
    queue = queue->print_point;
    n->print_point = NULL;
    return n;
}

/* Show the tree leaves.
*/
static int print_bptree_leaves(BPLUS_TREE bptree)
{
    BPT_Node *r = bptree.root;
    BPT_Node *tmp;
    if (r != NULL)
    {
        tmp = r;
        while (!tmp->is_leaf)
        {
            tmp = tmp->pointers[0];
        }
        while (tmp != NULL)
        {
            for (int i = 0; i < tmp->keys_num; i++)
            {
                printf(" %d", tmp->keys[i]);
            }    
            printf(" |");
            tmp = tmp->next;
        }
        printf("\n");
    }
    return EXIT_SUCCESS;
}

/* Show the tree from the root to leaves
*/
static void print_bptree(BPT_Node *const root)
{

    BPT_Node *n = NULL;
    int i = 0;
    int rank = 0;
    int new_rank = 0;

    if (root == NULL)
    {
        printf("The tree is empty.\n");
        return;
    }
    queue = NULL;
    enqueue(root);
    while (queue != NULL)
    {
        n = dequeue();
        if (n->parent != NULL && n == n->parent->pointers[0])
        {
            new_rank = path_to_root(root, n);
            if (new_rank != rank)
            {
                rank = new_rank;
                printf("\n");
            }
        }
        for (i = 0; i < n->keys_num; i++)
        {
            printf("%d ", n->keys[i]);
        }
        if (!n->is_leaf)
            for (i = 0; i <= n->keys_num; i++)
                enqueue(n->pointers[i]);

        printf("|");
    }
    printf("\n");
}


/* 功能：根据传入的记录，创建一个数据块。
* 输入： 数据指针
* 输出： 返回生成记录的指针。
 */
static DATA_RECORD *create_data_record(DATA_RECORD *drp)
{
    DATA_RECORD *new_record = (DATA_RECORD *)malloc(sizeof(DATA_RECORD));
    if (new_record == NULL)
    {
        ERROR_EXIT("Record creation.");
    }
    else
    {
        new_record->key = drp->key;
        strcpy(new_record->id, drp->id);
        strcpy (new_record->name , drp->name);
        strcpy(new_record->create_time, drp->create_time);
    }
    return new_record;
}

static BPT_Node *create_empty_node()
{
    BPT_Node *new_node;
    new_node = malloc(sizeof(BPT_Node));
    if (new_node == NULL)
        ERROR_EXIT("Node creation.");
    
    /*new_node->keys = malloc((tree_degree - 1) * sizeof(int)); // （阶数-1） 个key
    if (new_node->keys == NULL)
    {
        ERROR_EXIT("New node keys array.");
    }
    new_node->pointers = malloc((tree_degree) * sizeof(void *)); // （阶数） 个key
    if (new_node->pointers == NULL)
    {
        ERROR_EXIT("New node pointers array.");
    }
    */
    new_node->is_leaf = false;
    new_node->keys_num = 0;
    new_node->parent = NULL;
    new_node->next = NULL;
    int i;
    for ( i = 0; i < tree_degree; i++)
    {
        new_node->pointers[i] = NULL;
        new_node->leaf[i] = NULL;
    }
    new_node->pointers[i] = NULL;

    return new_node;
}

/* 创建一个叶子节点。先创立一个Node，然后设置成叶子节点的标识 */
static BPT_Node *create_empty_leaf()
{
    BPT_Node *leaf = create_empty_node();
    leaf->is_leaf = true;
    return leaf;
}

/* 如果是第一次插入，就建立一个新树root  */
static BPT_Node *create_new_tree(DATA_RECORD *drp)
{
    BPT_Node *root = create_empty_leaf();
    DATA_RECORD *lrp = create_data_record(drp);
    root->keys[0] = drp->key;
    root->pointers[0] = NULL;
    root->leaf[0] = lrp;
    root->parent = NULL;
    root->keys_num++;
    root->next = NULL;

    return root;
}

/* Insert one record to tree(from root);
*/
static BPT_Node *insert_record_to_tree(BPLUS_TREE *tree, DATA_RECORD *drp)
{
    if (tree->root == NULL)
    {
        // make a new tree.
        tree->root = create_new_tree(drp);
        return tree->root;
    }
    else
    {
        return insert_into_bptree_node(tree->root, drp);
    }
}
/* The node is overfow, need to be repaired.
*/
BPT_Node *insert_repair(BPT_Node *np)
{
	if (np->keys_num <= bptree.max_key_num) 
	{
		return np;
	}
	else if (np->parent == NULL)
	{
		bptree.root = split_bptree(np);
		return (BPT_Node *) (bptree.root);
	}
	else
	{
		BPT_Node *newNode  = split_bptree(np);
		return insert_repair(newNode);
	}			    

}

/* 拆分一个节点
*/
BPT_Node *split_bptree(BPT_Node *np)
{
    if (np->keys_num <= bptree.max_key_num)
    {
        printf("The node is not need to repair.");
        return np;
    }
    
    BPT_Node *rightNode = create_empty_leaf();
	
	int risingNode = np->keys[bptree.split_index];
    int i, parentIndex;
	if (np->parent != NULL)
	{
		BPT_Node *currentParent = np->parent;
		for (parentIndex = 0; parentIndex < currentParent->keys_num + 1 && currentParent->pointers[parentIndex] != np; parentIndex++);
		if (parentIndex == currentParent->keys_num + 1)
		{
			ERROR_EXIT("Couldn't find which child we were!");
		}
		for (i = currentParent->keys_num; i > parentIndex; i--)
		{
			currentParent->pointers[i+1] = currentParent->pointers[i];

			currentParent->keys[i] = currentParent->keys[i-1];
		}
		currentParent->keys_num++;
		currentParent->keys[parentIndex] = risingNode;
		currentParent->pointers[parentIndex+1] = rightNode;
		rightNode->parent = currentParent;
	}
	
	int rightSplit;
	
	if (np->is_leaf)
	{
		rightSplit = bptree.split_index;
		rightNode->next = np->next;
		np->next = rightNode;
	}
	else
	{
		rightSplit = bptree.split_index + 1;
	}
	
	rightNode->keys_num = np->keys_num - rightSplit;
	if (np->is_leaf)
	{				
	}
//	for (int i = rightSplit; i < np->keys_num + 1; i++)
	for (int i = rightSplit; i < np->keys_num + 1; i++)
	{
        BPT_Node * tmp = (BPT_Node*) np->pointers[i];
		rightNode->pointers[i - rightSplit] = tmp;

		if (tmp != NULL)
		{
			rightNode->is_leaf = false;
			tmp->parent = rightNode;
            tmp = NULL;
		}
	}
    for (int i = rightSplit; i < np->keys_num; i++)
	{
		rightNode->keys[i - rightSplit] = np->keys[i];
        rightNode->leaf[i - rightSplit] = np->leaf[i];
	}
	BPT_Node *leftNode = np;
	leftNode->keys_num = bptree.split_index;
	// TO MAKE UNDO WORK -- CAN REMOVE LATER VV

	if (np->parent != NULL)
	{
		return np->parent;
	}
	else //			if (tree.parent == null)
	{
		BPT_Node *treeRoot = create_empty_node();
		treeRoot->keys[0] = risingNode;
		treeRoot->pointers[0] = leftNode;
		treeRoot->pointers[1] = rightNode;
		leftNode->parent = treeRoot;
		rightNode->parent = treeRoot;
        treeRoot->keys_num = 1;
		treeRoot->is_leaf = false;
		return treeRoot;
	}
	
}

/* insert one record to one node.
*/
static BPT_Node *insert_into_bptree_node(BPT_Node *np, DATA_RECORD *drp)
{
    BPT_Node * c = np;
    if(c->is_leaf)
    {
        c->keys_num ++;
        int insert_index = c->keys_num  - 1;
        while (insert_index > 0 )
        {
            if (c->keys[insert_index - 1] > drp->key)
            {
                c->keys[insert_index] = c->keys[insert_index  - 1];
                c->pointers[insert_index] = c->pointers[insert_index  - 1];
                c->leaf[insert_index] = c->leaf[insert_index - 1];
                insert_index --;
            }
            else
            {
                break;
            }
        }
        c->keys[insert_index] = drp->key;
        DATA_RECORD *new_leaf = create_data_record(drp);
        c->pointers[insert_index] = NULL;
        c->leaf[insert_index] = new_leaf;
        return insert_repair(c);
    }
    else
    {
        int find_index = 0;
        while ( find_index < c->keys_num )
        {
            if ( c->keys[find_index] < drp->key)
            {
                find_index ++;
            }
            else
                break;
        }
        return insert_into_bptree_node(c->pointers[find_index], drp);
    }
}


static BPT_Node *make_tree_from_file()
{
    FILE *fp;
    char * input_file_name = "../data/mytree.txt";

    fp = fopen(input_file_name, "r");
    if ( fp== NULL)
    {
        ERROR_EXIT("Failure to open input file.");
    }
    else
    {
        DATA_RECORD dr;
        int i;
        int count =0;

        while (!feof(fp))
        {
            i = fscanf(fp, "%d %s %s\n", &dr.key, dr.id, dr.name);
            if (i == 3)
            {
                char str1[BUFFER_SIZE];
                get_current_time(dr.create_time);
                sprintf(str1, "[Key:%4d] [No:%8s] [Name:%30s] [Create_Time: %20s]\n", dr.key, dr.id, dr.name, dr.create_time);
                printf(str1);
                insert_record_to_tree(&bptree, &dr);
                print_bptree(bptree.root);
                printf("\n");
                count ++;
            }
            else
            {   //输入值有问题。废弃掉。
                fclose(fp);
                ERROR_EXIT("Input data error.");
            }
        }
        fclose(fp);
        printf("\n%4d records are created.\n", count);
        print_bptree_leaves(bptree);

        return bptree.root;
    }
}

/*=============================================================================
* 有关操作菜单的处理程序
*/
/* 显示菜单和操作界面
*/
void show_bpt_manual(void)
{
    printf("Enter any of the following commands after the prompt > :\n"
           "\ti <key> <ID> <Name> -- Insert the value <Key> (an integer), ID (String) , Name (String).\n"
           "\tf <k>  -- Find the value under key <k>.\n"
           "\tp <k> -- Print the path from the root to key k and its associated value.\n"
           "\tr <k1> <k2> -- Print the keys and values found in the range [<k1>, <k2>\n"
           "\td <k>  -- Delete key <k> and its associated value.\n"
           "\tx -- Destroy the whole tree.  Start again with an empty tree of the same order.\n"
           "\tt -- Show the tree information and Print the B+ tree.\n"
           "\tl -- Print the keys of the leaves (bottom row of the tree).\n"
           "\tv -- Toggle output of pointer addresses (\"verbose\") in tree and leaves.\n"
           "\tq -- Quit. (Or use Ctl-D or Ctl-C.)\n"
           "\t? -- Print this help message.\n");
}

/* 功能：主操作界面
*  返回值：0
*/
static int bpt_manual()
{

    char command;
    bool input_consumed = false;
    char buffer[BUFFER_SIZE];
    DATA_RECORD dr;
    int count = 0;
    int input_key, input_key_2;

    printf("> ");
    while (scanf("%c", &command) != EOF)
    {
        input_consumed= false;
        switch (command)
        {
        case 'd':
            count = scanf("%d", &input_key);
            if (count ==1)
            {
                //root = delete_key (root, input_key);
                //print_tree(root);
            }
            else
            {
                printf("Input error. Please press ? to get help.\n");
            }
            break;
        case 'i':
            fgets(buffer, BUFFER_SIZE, stdin);
            input_consumed = true;
            count = sscanf(buffer, "%d %s %s", &dr.key, dr.id, dr.name);;
            if (count ==3)
            {
                get_current_time(dr.create_time);
                insert_record_to_tree(&bptree, &dr);
                //print_tree(root);;
            }
            else
            {                
                printf("Input error. Please press ? to get help.\n");
            }
            break;
        case 'f':
        case 'p':
            count = scanf("%d", &dr.key);
            if (count == 1)
            {
                get_current_time(dr.create_time);
                //find_and_print_record(root, &dr, command == 'p');
            }
            else
            {
                printf("Input error. Please press ? to get help.\n");
            }
            break;
        case 'l':
            print_bptree(bptree.root);
            //print_leaves(root);
            //list_leaves(root);
            break;
         case 'x':
            //root = destroy_tree(root);            
            printf("The tree is destoryed.\n");
            break;
        case 't':
            if (bptree.root)
            {
                //get_tree_info(root);
                //show_tree_info();
                //print_tree(root);
            }
            else
                printf("The tree is empty.\n");
            break;
        case 'R':
            //root = destroy_tree(root);
            //root = get_data_from_file();    //reload the data from file.
            //print_tree(root);
            break;
        case 'o':
            if (bptree.root)
            {
                printf("The tree is active. Please press \'x\' to release or \'R\' to reload the tree.\n");
             }
            else
            {
                int input_order;
                count = scanf("%d", &input_order);
                if (count == 1)
                    bptree.max_degreee = input_order;
                else
                    printf("Input error. Please press ? to get help.\n");
            }
            break;
        case 'q':           
            if (bptree.root)
            {
                //root = destroy_tree(root);
                printf("The tree is not empty and is cleanned. Quit safely.\n");
            }
            else
            {
                printf("The tree is empty. Quit safely.\n");
            }
            return EXIT_SUCCESS;
        case 'r':
            if (scanf("%d %d", &input_key, &input_key_2) == 2)
            {
                if (input_key > input_key_2)
                {
                    int tmp = input_key_2;
                    input_key_2 = input_key;
                    input_key = tmp;
                }
                //get_and_print_range(root, input_key, input_key_2, command == 'p');
            }
            else
            {
                printf("Input error. Please press ? to get help.\n");
            }
            break;
        case '\n':
            input_consumed = true;
            break;
        default:
            show_bpt_manual();
            //input_consumed = true;
        }
        if (!input_consumed)
        {
            while ( getchar() != (int)'\n')
                ;
        }
        printf("> ");
    }
    printf("\n");

    return EXIT_SUCCESS;
}

/*=============================================================================
* Main Program.
*/
int test_bplus_tree()
{
    usage();
    init_tree(&bptree);
    make_tree_from_file();
    bpt_manual();
    return EXIT_SUCCESS;
}

