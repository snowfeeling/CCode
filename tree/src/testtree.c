#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../inc/tree.h"
#include "../inc/testtree.h"
// Define const variable
#define SLEN 81

// Define the local functions type
void printitem(Item item);
char menu(void);
void showwords(const Tree *pt);
void findword(const Tree *pt);
char *s_gets(char *st, int n);
void setlevel(Tree *pt);

// Define the functions
int testTree1()
{

    Tree wordcount;
    FILE *fp;
    char filename[SLEN];
    char word[SLEN];
    Item entry;
    char choice;
    //printf("Enter name of file to be processed: \n");
    //s_gets(filename, SLEN);
    strcpy(filename, "../data/testtree.txt");
    if ((fp = fopen(filename, "r")) == 0)
    {
        printf("Can't open file %s. Bye.\n", filename);
        exit(EXIT_FAILURE);
    }
    InitializeTree(&wordcount);
    while (fscanf(fp, "%s", word) == 1 && !TreeIsFull(&wordcount))
    {
        strcpy(entry.word, word);
        AddItem(&entry, &wordcount);
    }

    setlevel(&wordcount);

    while ((choice = menu()) != 'q')
    {
        switch (choice)
        {
        case 's':
            showwords(&wordcount);
            break;
        case 'f':
            findword(&wordcount);
            break;
        default:
            puts("Switching error");
        }
    }
    fclose(fp);
    puts("Done");
    return 0;
}

char *s_gets(char *st, int n)
{
    char *ret_val;
    char *find;
    ret_val = fgets(st, n, stdin);

    if (ret_val)
    {

        find = strchr(st, '\n'); //
        if (find)
            *find = '\0';
        else
            while (getchar() != '\n')
                continue;
    }
    return ret_val;
}

char menu(void)
{
    int ch;

    puts("Word counting program");
    puts("Enter the letter corresponding to your choice:");
    puts("s) show word list \nf) find a word");
    puts("q) quit");
    while ((ch = getchar()) != EOF)
    {
        while (getchar() != '\n') /* discard rest of line */
            continue;
        ch = tolower(ch);
        if (strchr("sfq", ch) == NULL)
            puts("Please enter an s, f, or q:");
        else
            break;
    }
    if (ch == EOF)
        ch = 'q';
    return ch;
}

void showwords(const Tree *pt)
{
    if (TreeIsEmpty(pt))
        puts("No entries!");
    else
        Traverse(pt, printitem);
}

void setlevel(Tree *pt)
{
    if (TreeIsEmpty(pt))
        puts("No entries!");
    else
        SetTreeLevel(pt, 1, 'C');
}
void findword(const Tree *pt)
{
    char word[SLEN];
    Item entry;
    const Item *pi;
    if (TreeIsEmpty(pt))
    {
        puts("No entries!");
        return; /* quit function if tree is empty */
    }
    printf("Enter the word to find: ");
    scanf("%s", word);
    while (getchar() != '\n')
        continue;
    strcpy(entry.word, word);
    pi = WhereInTree(&entry, pt);
    if (pi == NULL)
        printf("%s is not in the list.\n", word);
    else
        printf("%s appears %d times.\n", word, pi->count);
}

void printitem(Item item)
{
    printf("%3d[%c:%2d] \"%s\"\n", item.count,  item.LR, item.level, item.word);
}

int testTree()
{

    Tree wordcount;
    FILE *fp;
    char filename[SLEN];
    char word[SLEN];
    Item entry;
    char choice;

    strcpy(filename, "../bin/testtree.txt");
    if ((fp = fopen(filename, "r")) == 0)
    {
        printf("Can't open file %s. Bye.\n", filename);
        exit(EXIT_FAILURE);
    }
    InitializeTree(&wordcount);
    while (fscanf(fp, "%s", word) == 1 && !TreeIsFull(&wordcount))
    {
        strcpy(entry.word, word);
        AddItem(&entry, &wordcount);
    }
    fclose(fp);

    setlevel(&wordcount);
    printf("%d words are read from the file to Bi-Tree.\n", wordcount.size);

    showwords(&wordcount);

    DeleteAll(&wordcount);

    puts("Done");
    return 0;
}
