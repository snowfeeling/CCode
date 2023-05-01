/*
 *  src/bptree.c
 *
 *  Copyright (C) 2023,  Wangss(Wang Shu Song)
 *  
 * Created by Wangss on 2023-04-29 10:06
 * To study the BPlus Tree.
 * 
 */
#include "../inc/bptree.h"

/*=============================================================================
* Global variable
*
*/
int tree_degree = DEFALUT_TREE_DEGREE;

static BPLUS_TREE bptree;
static BPT_NODE *queue = NULL;
static BPT_DATA_RECORD dr;

/*
* Global Functions.
*/
static void usage();
static size_t get_current_time( char * time_info);

/*=============================================================================
* Fundation functions
* 
*/
static void bpt_usage()
{
    printf("\n**********************************************");
    printf("\n* Welcoming to BPlus Tree Demo.\n");
    printf("*\tPlease see the README for your usage.\n");
    printf("**********************************************\n");
    printf("\n");
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


static void enqueue(BPT_NODE *new_node)
{
    BPT_NODE *c;
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

/* Get one node from queue.
*  
 */
static BPT_NODE *dequeue(void)
{
    BPT_NODE *n = queue;
    queue = queue->print_point;
    n->print_point = NULL;
    return n;
}

static void show_msg(char *msg)
{
    get_current_time(dr.create_time);
    printf("\n[%s]%s\n", dr.create_time, msg);
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

/*
* print the data of one BPT record. 
*/
static void print_one_bpt_data_record(BPT_DATA_RECORD *drp)
{
    printf("[Key:%4d] [ID:%8s] [Name:%20s] [Create_Time:%20s]\n", drp->key, drp->id, drp->name, drp->create_time);

}
/* Function: To list the detail information of all leaves. 
*  Input    : The root of tree.
*  Output   : <None>
*  Return   : <void>
*/

static void list_bptree_leaves(BPT_NODE * const root)
{
    if (root == NULL)
    {
        printf("The tree is empty.\n");
        return;
    }
    int i;
    BPT_NODE * c = root;
    while (! c->is_leaf)
        c= c->pointers[0];
    while (true)
    {
        for (i = 0; i< c->keys_num; i++)
        {
            BPT_DATA_RECORD * drp = (BPT_DATA_RECORD*) (c->leaf[i]);
            print_one_bpt_data_record(drp);
        }
        if (c->next != NULL)
        {
            c = c->next;
        }
        else
            break;
    }
    printf("\n");
}

/* 
* Print the all keys of leaves.
*/
static int print_key_of_bptree_leaves(BPLUS_TREE bptree)
{
    BPT_NODE *r = bptree.root;
    BPT_NODE *tmp;
    if (r != NULL)
    {
        tmp = r;
        printf("The Leaves in the tree:\n");
        while (!tmp->is_leaf)
        {
            tmp = tmp->pointers[0];
        }
        while (tmp != NULL)
        {
            printf("[");
            for (int i = 0; i < tmp->keys_num; i++)
            {
                printf(" %d", tmp->keys[i]);
            }    
            printf("] ");
            tmp = tmp->next;
        }
        printf("\n");
    }
    return EXIT_SUCCESS;
}

/*
* Get the hight from chilf to root;
*/
static int hight_to_root(BPT_NODE *const root, BPT_NODE *child)
{
    int length = 0;
    BPT_NODE *c = child;
    while (c != root)
    {
        c = c->parent;
        length++;
    }
    return length;
}


/* 
* Print the tree from the root to leaves.
*/
static void print_bptree(BPT_NODE *const root)
{
    if (root == NULL)
    {
        printf("The tree is empty.\n");
        return;
    }

    BPT_NODE *n = NULL;
    int i = 0;
    int rank = 0;
    int new_rank = 0;

    queue = NULL;
    enqueue(root);
    while (queue != NULL)
    {
        n = dequeue();
        if (n->parent != NULL && n == n->parent->pointers[0])
        {
            new_rank = hight_to_root(root, n);
            if (new_rank != rank)
            {
                rank = new_rank;
                printf("\n");
            }
        }
        printf("[");
        for (i = 0; i < n->keys_num; i++)
        {
            printf("%d ", n->keys[i]);
        }
        if (!n->is_leaf)
            for (i = 0; i <= n->keys_num; i++)
                enqueue(n->pointers[i]);

        printf("] ");
    }
    printf("\n");
}

/* ======= Destory Tree functions. =========
*/
/* Destory the BPlus noedes of BPlus tree.
*  Input    : root
*  Return   : <void>
*/
static void destroy_bptree_nodes(BPT_NODE *root)
{
    int i;
    if (root->is_leaf)
        for (i = 0; i < root->keys_num; i++)
        {
            free(root->leaf[i]);
        }
    else
        for (i = 0; i < root->keys_num + 1; i++)
            destroy_bptree_nodes(root->pointers[i]);
    free(root);
}

static BPT_NODE *destroy_bptree(BPLUS_TREE *bptree)
{
    if (bptree->root)
        destroy_bptree_nodes(bptree->root);
    return NULL;
}

/* Exit the program after listing and destorying the BP Tree;
*/
static void ERROR_EXIT(char* str)
{
    if (bptree.root)
    {
        list_bptree_leaves(bptree.root);
        destroy_bptree(&bptree);
    }

    perror(str);
    exit(EXIT_FAILURE);
}

/* 功能：根据传入的记录，创建一个数据块。
* 输入： 数据指针
* 输出： 返回生成记录的指针。
 */
static BPT_DATA_RECORD *create_data_record(BPT_DATA_RECORD *drp)
{
    BPT_DATA_RECORD *new_record = (BPT_DATA_RECORD *)malloc(sizeof(BPT_DATA_RECORD));
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

static BPT_NODE *create_empty_node()
{
    BPT_NODE *new_node;
    new_node = malloc(sizeof(BPT_NODE));
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
static BPT_NODE *create_empty_leaf()
{
    BPT_NODE *leaf = create_empty_node();
    leaf->is_leaf = true;
    return leaf;
}


/* 如果是第一次插入，就建立一个新树root  */
static BPT_NODE *create_new_tree(BPT_DATA_RECORD *drp)
{
    BPT_NODE *root = create_empty_leaf();
    BPT_DATA_RECORD *lrp = create_data_record(drp);
    root->keys[0] = drp->key;
    root->pointers[0] = NULL;
    root->leaf[0] = lrp;
    root->parent = NULL;
    root->keys_num++;
    root->next = NULL;

    return root;
}

/* 拆分一个节点
*/
BPT_NODE *split_bptree(BPT_NODE *np)
{
    if (np->keys_num <= bptree.max_key_num)
    {
        printf("The node is not need to repair.");
        return np;
    }
    
    BPT_NODE *rightNode = create_empty_leaf();
	
	int risingKey = np->keys[bptree.split_index];
    int i, parentIndex;
	if (np->parent != NULL)
	{
		BPT_NODE *currentParent = np->parent;
        //找到在父节点中，本节点的位置 parentIndex；
		for (parentIndex = 0; parentIndex < currentParent->keys_num + 1 && currentParent->pointers[parentIndex] != np; parentIndex++);
		if (parentIndex == currentParent->keys_num + 1)
		{   //如果没有找到，就是肯定时出错了
			ERROR_EXIT("Couldn't find which child we were!");
		}
		for (i = currentParent->keys_num; i > parentIndex; i--)
		{   //把父节点中比上升key大的key的位置往后挪动；在父节点中，leaf是没有意义的
			currentParent->pointers[i+1] = currentParent->pointers[i];    
			currentParent->keys[i] = currentParent->keys[i-1];
            currentParent->leaf[i] = currentParent->leaf[i-1];
		}

		currentParent->keys_num++;  //父节点的key增加了
		currentParent->keys[parentIndex] = risingKey;  //把上升的key写入到所应在的位置中
		currentParent->pointers[parentIndex+1] = rightNode;  // 把右节点的位置写入到上升key的右边指针
		rightNode->parent = currentParent;      //把右节点的父节点设置成当前的父节点；
	}
	
    //处理右边的拆分数据
	int rightSplit;
	
	if (np->is_leaf)
	{   //如果是叶子节点，拆分数量点是树的拆分点
		rightSplit = bptree.split_index; 
		rightNode->next = np->next; //右节点的next，继承当前节点的next
		np->next = rightNode;   //当前节点的next, 指定右节点
	}
	else
	{   //如果不是叶子节点，拆分数量是树的拆分数量+1
		rightSplit = bptree.split_index + 1;
	}	
	rightNode->keys_num = np->keys_num - rightSplit;    //计算右节点的key数量 = 当前节点的-分拆数量
	for (int i = rightSplit; i < np->keys_num + 1; i++)
	{
        BPT_NODE * tmp = (BPT_NODE*) np->pointers[i];
		rightNode->pointers[i - rightSplit] = tmp;
		rightNode->keys[i - rightSplit] = np->keys[i];
        rightNode->leaf[i - rightSplit] = np->leaf[i];

		if (tmp != NULL)
		{
			rightNode->is_leaf = false;
			tmp->parent = rightNode;
            tmp = NULL;
		}
	}
    // To handle the LEFT node.
	BPT_NODE *leftNode = np;
	leftNode->keys_num = bptree.split_index;

	if (np->parent != NULL)
	{
		return np->parent;
	}
	else //			if (tree.parent == null)
	{
		BPT_NODE *treeRoot = create_empty_node();
		treeRoot->keys[0] = risingKey;
		treeRoot->pointers[0] = leftNode;
		treeRoot->pointers[1] = rightNode;
		leftNode->parent = treeRoot;
		rightNode->parent = treeRoot;
        treeRoot->keys_num = 1;
		treeRoot->is_leaf = false;
		return treeRoot;
	}
	
}


/* To check the node, if the node is overfow, need to repair it.
*/
BPT_NODE *insert_repair(BPT_NODE *np)
{
	if (np->keys_num <= bptree.max_key_num) 
	{
		return np;
	}
	else if (np->parent == NULL)
	{
		bptree.root = split_bptree(np);
		return (BPT_NODE *) (bptree.root);
	}
	else
	{
		BPT_NODE *newNode  = split_bptree(np);
		return insert_repair(newNode);
	}			    
}


/* insert one record to one node.
*/
static BPT_NODE *insert_into_bptree_node(BPT_NODE *np, BPT_DATA_RECORD *drp)
{
    BPT_NODE * c = np;
    if(c->is_leaf)
    {   // If the node is leaf, insert it into the node.
        c->keys_num ++;
        int insert_index = c->keys_num  - 1;
        while (insert_index > 0 )
        {   // 从最右边开始，挪动比新来的记录更大的节点。
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
        BPT_DATA_RECORD *new_leaf = create_data_record(drp);
        c->pointers[insert_index] = NULL;
        c->leaf[insert_index] = new_leaf;
        // Valid the node;
        return insert_repair(c);
    }
    else
    {   // if the node is not leaf.
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

/* Insert one record to tree(from root);
*/
static BPT_NODE *insert_record_to_tree(BPLUS_TREE *tree, BPT_DATA_RECORD *drp)
{
    if (tree->root == NULL)
    {   // If the tree is empty，make a new tree.
        tree->root = create_new_tree(drp);
        return tree->root;
    }
    else
    {   // Insert the record to the tree.
        return insert_into_bptree_node(tree->root, drp);
    }
}

/* Make the BPlus Tree from the designated Data file, and print the leaves.
* Input: <void>
* Return: the root pointer (BPT_Node*). 
*/
static BPT_NODE *make_tree_from_file()
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
        BPT_DATA_RECORD dr;
        int i;
        int count =0;

        while (!feof(fp))
        {
            i = fscanf(fp, "%d %s %s\n", &dr.key, dr.id, dr.name);
            if (i == 3)
            {
                char str1[BUFFER_SIZE];
                get_current_time(dr.create_time);
                sprintf(str1, "[Key:%4d] [ID:%8s] [Name:%20s] [Create_Time: %20s]\n", dr.key, dr.id, dr.name, dr.create_time);
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
        printf("\n%d records are created.\n", count);
        //print_bptree_leaves(bptree);

        return bptree.root;
    }
}

/*=======================Functions of Finding key in tree==============================*/
/* 功能：找到>=Key的叶子节点
*  输入：root, key所在的记录
*  返回：叶子节点的指针。
 * 从root开始，找key，直到到叶子。
 * 非叶子节点上，Key的所处的叶子，是在key的右部指针上的。所以，在非叶子节点中，找到比key大的指针。
 */
static BPT_NODE *find_leaf_node_in_bptree(BPT_NODE *const root, BPT_DATA_RECORD * drp)
{
    if (root == NULL || drp == NULL)
    {   //处理异常情况
        return NULL;
    }
    else
    {
        int i = 0;
        BPT_NODE *c = root;
        while (!c->is_leaf)
        {
            for (i = 0; i < c->keys_num; i++)
            {   //如果要查找的key <= 节点上的key，就找到了所在的节点指针；否则就往下找。直到找到本节点的最后一个
                if (drp->key <= c->keys[i])
                    break;
            }
            c = (BPT_NODE *)c->pointers[i]; //走到下一层去。直到叶子节点去。
        }
        return c;
    }
}

/* 功能：在Tree里面，找是否有有这个Key
*  输入参数： root 和 drp 传入的数据指针。Leaf_out: 找到的可以所在叶子节点的位置指针
*  返回参数：在叶子节点上的数据指针
*/
static BPT_DATA_RECORD * find_leaf_data_in_bptree(BPT_NODE *root, BPT_DATA_RECORD *drp, BPT_NODE ** leaf_out, int *fIndex)
{
    if (root == NULL || drp == NULL)
    {
        if (leaf_out != NULL)
            *leaf_out = NULL;
        return NULL;
    }

    int i = 0;
    BPT_NODE *leaf = NULL;

    /* 如果root不为空，先找到叶子节点(包含想要查询的key的范围) 
     */
    leaf = find_leaf_node_in_bptree(root, drp);
    for (i = 0; i < leaf->keys_num; i++)
        if (leaf->keys[i] ==  drp->key )
            break;
    if (leaf_out != NULL)
    {
        *leaf_out = leaf;
        *fIndex = i;
    }
    if (i == leaf->keys_num)
        return NULL;  // 如找到最后了，说明叶子节点中没有这个值。返回NULL。
    else
        return (BPT_DATA_RECORD *)leaf->leaf[i];
}

/* 功能：从tree中找到一个key所在的记录，并打印出来
*  输入：root，要查找的值，显示标识
*  返回：无
 */
static void find_and_print_record(BPT_NODE *const root, BPT_DATA_RECORD *drp)
{
    BPT_NODE *fNode = NULL;
    int fIndex = 0;
    BPT_DATA_RECORD *r;
    r = find_leaf_data_in_bptree(root, drp, &fNode, &fIndex);

    if (r == NULL)
    {    
        printf("Not found the key %d.\n", drp->key);
    }
    else
    {
        int i, iStop =0;
        BPT_DATA_RECORD * nr;
        BPT_NODE * c = fNode;
        while (!iStop)
        {
            for (i = fIndex; i< c->keys_num; i++)
            {
                nr = (BPT_DATA_RECORD*) (c->leaf[i]);
                if (nr->key== drp->key)
                {  
                    print_one_bpt_data_record(nr);
                }
                else
                {
                    iStop = 1;
                    break;
                }
            }
            fIndex  = 0;
            if (c->next != NULL)
            {
                c = c->next;
            }
            else
                iStop = 1;
        }
        printf("\n");

        /*printf("Record at %p -- key:[%d], ID:[%s] NAME:[%s] Create Time:[%s].\n",
               nr, nr->key, nr->id, nr->name, nr->create_time);*/
    
    }
}


/*=================The functions to show the main manual.=====================
*/
static void show_bpt_manual(void)
{
    printf("Enter any of the following commands after the prompt > :\n"
           "\ti <key> <ID> <Name> -- Insert the value <Key> (an integer), ID (String) , Name (String).\n"
           "\tf <k>  -- Find the value under key <k>.\n"
           "\tp <k> -- Print the path from the root to key k and its associated value.\n"
           "\tr <k1> <k2> -- Print the keys and values found in the range [<k1>, <k2>\n"
           "\td <k>  -- Delete key <k> and its associated value.\n"
           "\tx -- Destroy the whole tree.  \n"
           "\tt -- Show the tree information and Print the B+ tree.\n"
           "\tl -- Print the keys of the leaves (bottom row of the tree).\n"
           "\tR -- Destory and recreate the tree.\n"
           "\tq -- Quit. (Or use Ctl-D or Ctl-C.)\n"
           "\t? -- Print this help message.\n");
}

/* ===================================================================================
*  功能：主操作界面
*  返回值：0
*/
static int bpt_manual()
{

    char command;
    bool input_consumed = false;
    char buffer[BUFFER_SIZE];
    BPT_DATA_RECORD dr;
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
                show_msg("Delete one key.");
                //deleteElement(input_key);
                //print_bptree(bptree.root);;
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
                //show_msg(" Insert one key.");
                get_current_time(dr.create_time);
                insert_record_to_tree(&bptree, &dr);
                print_bptree(bptree.root);;
            }
            else
            {                
                printf("Input error. Please press ? to get help.\n");
            }
            break;
        case 'f':
            count = scanf("%d", &dr.key);
            if (count == 1)
            {
                //show_msg("Find one key.");
                get_current_time(dr.create_time);
                find_and_print_record(bptree.root, &dr);
            }
            else
            {
                printf("Input error. Please press ? to get help.\n");
            }
            break;
        case 'l':
            show_msg("\nList the bplus tree.");
            list_bptree_leaves(bptree.root);
            print_bptree(bptree.root);
            break;
         case 'x':
            //show_msg("Destory the bplus tree.");
            bptree.root = destroy_bptree(&bptree);            
            printf("The tree is destoryed.\n");
            break;
        case 't':
            if (bptree.root)
            {
                show_msg("Show the tree.");
                //get_tree_info(root);
                //show_tree_info();
                //print_tree(root);
            }
            else
                printf("The tree is empty.\n");
            break;
        case 'R':
            //show_msg("ReCreate the bplus tree.");
            bptree.root = destroy_bptree(&bptree);            
            make_tree_from_file();
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
                destroy_bptree(&bptree);
                printf("The tree is not empty and is cleaned. Quit safely.\n");
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
                //get_and_print_range(bptree.root, input_key, input_key_2);
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
* BPT Testing Program.
*/
int test_bplus_tree()
{
    bpt_usage();
    init_tree(&bptree);
    make_tree_from_file();
    bpt_manual();
    return EXIT_SUCCESS;
}








