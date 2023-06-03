
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <conio.h> /*键盘输入获取*/

#include "../inc/mylog.h"
#include "../inc/quicksort.h"

#define SQFNAME "../log/quicklog.txt"

FILE *flog;
HANDLE hOutput, hOutBuffer;
COORD coord;
BOOL first_show = true;

int scr_width=50, scr_height=30;
BOOL switch_over;
char scr_data[30][50];

void init_double_buffer(void);
static void refresh_screen();
static int put_data_to_buffer(Squeue *p);

bool InitializeSource(Squeue *p);
int ShowData(const Squeue *p);
int LogMeg(char *str);
int LogQueueData(Squeue *p);

int testQuickSort()
{
    Squeue p;
    if (!init_log_file(&flog, SQFNAME))
        return EXIT_FAILURE;

    if (!InitializeSource(&p))
        return EXIT_FAILURE;

    printf("The original data is :\n");
    ShowData(&p);

    init_double_buffer();
    put_data_to_buffer(&p);
    refresh_screen();
    Sleep(2400);
        
    QuickSort(&p, 0, p.qnum - 1);

    printf("The sorted data is :\n");
    ShowData(&p);

    close_log_file(flog);
    do
    {
        Sleep(1200);
        /* code */
    } while (!_kbhit());

    
    return 0;
}

bool InitializeSource(Squeue *p)
{
    FILE *fp;
    char *fname = "../data/testqs.txt";
    int tmp, i = 0;
    if (!(fp = fopen(fname, "r")))
    {
        printf("Can't open file %s. Bye.\n", fname);
        return false;
    }
    while (i < PMAX /* condition */)
    {
        int k = fscanf(fp, "%d", &tmp);
        if (k == 1)
        {
            p->value[i] = tmp;
            i++;
        }
        else if (k == EOF)
            break;
        else if (k != 1) // 如果读的有错（非数字），则 K==0
        {
            printf("Error data found.\n");
            fclose(fp);
            return false;
        }
    }
    fclose(fp);
    p->qnum = i; // 计数器

    return true;
}
int ShowData(const Squeue *p)
{
    for (int i = 0; i < p->qnum; i++)
    {
        printf("[%2d]%6d\n", i, p->value[i]);
    }

    return 0;
}

/*
*  Quick Sort
*/
void QuickSort(Squeue *p, int low, int high)
{
    if (low >= high)
        return;
    int tmp, i, j;
    char str[30];
    i = low;
    j = high;
    tmp = p->value[i]; // 取第一个值，为比较值
    sprintf(str, "[Begin:P=%6d %3d -%3d]\n", tmp, i, j);
    LogMeg(str);
    LogQueueData(p);
    do
    {
        while (p->value[j] > tmp && i < j) 
            j--;  // 如果从右的数字大于比较值，就向左走，直到等于或者小于比较值。
        if (i < j)
        {
            p->value[i] = p->value[j]; // 把右边的较小值，放到左边
            sprintf(str, "[R->L :P=%6d %3d -%3d]\n", tmp, j, i);
            LogMeg(str);
            LogQueueData(p);
            i++;  //往右走一个
            put_data_to_buffer(p);
            refresh_screen();

        }
        while (p->value[i] < tmp && i < j) 
            i++;   // 如果从左的数字小于比较值，就向右走，直到等于或者大于比较值。
        if (i < j)
        {
            p->value[j] = p->value[i]; //把左边较大的值，放到右边
            // 记录
            sprintf(str, "[L->R :P=%6d %3d -%3d]\n", tmp, i, j);
            LogMeg(str);
            LogQueueData(p);
            j--; // 往左走一步
            put_data_to_buffer(p);
            refresh_screen();
        }
    } while (i != j);
    p->value[i] = tmp; // 把比较值，放到中间的的位置。
    sprintf(str, "[TMP  :P=%6d %3d -%3d]\n", tmp, i, j);
    LogMeg(str);
    LogQueueData(p);

    QuickSort(p, low, i - 1);  // 比较左边的
    QuickSort(p, i + 1, high); // 比较右边的
}

int LogQueueData(Squeue *p)
{
    int j = 0;
    char str[1000];
    memset(str, 0, sizeof(str));
    for (int i = 0; i < p->qnum; i++)
    {
        j += sprintf(str + j, "%6d", p->value[i]);
    }
    sprintf(str + j, "\n");
    LogMeg(str);
    return 0;
}
int LogMeg(char *str)
{
    send_to_log(flog, str);
    return 0;
}

/* write buffer*/
static int put_data_to_buffer(Squeue *p)
{
    char str[200];
    for (int i = 0; i < p->qnum; i++)
    {
        sprintf(scr_data[i], "%6d", p->value[i]);
    }
    return 0;
} 

/*============================================*/
static void refresh_screen()
{
    /*双buff切换显示*/
	int i;
    DWORD scr_bytes;
    HANDLE hOut;
	if(switch_over == true)
	{
        hOut = hOutBuffer;
    }
	else
	{
        hOut = hOutput;
    }
    for (i = 0; i < scr_height; i++)
    {
        coord.Y = i;
        if (first_show)
        {
            coord.X = 0;
        }
        else
            coord.X = 12;
        WriteConsoleOutputCharacterA(hOut, scr_data[i], scr_width, coord, &scr_bytes);
    }
    first_show = false;
    //设置新的缓冲区为活动显示缓冲
    SetConsoleActiveScreenBuffer(hOut);	
	switch_over = !switch_over;
    Sleep(1200);
}


/*双缓冲技术解决闪屏问题*/
void init_double_buffer(void)
{
   	/*创建新的控制台缓冲区*/
    hOutBuffer = CreateConsoleScreenBuffer(
        GENERIC_WRITE,/*定义进程可以往缓冲区写数据*/
        FILE_SHARE_WRITE,/*定义缓冲区可共享写权限*/
        NULL,
        CONSOLE_TEXTMODE_BUFFER,
        NULL
    );
    hOutput = CreateConsoleScreenBuffer(
        GENERIC_WRITE,/*定义进程可以往缓冲区写数据*/
        FILE_SHARE_WRITE,/*定义缓冲区可共享写权限*/
        NULL,
        CONSOLE_TEXTMODE_BUFFER,
        NULL
    );
    
    /*隐藏两个缓冲区的光标*/
    CONSOLE_CURSOR_INFO cci;
    cci.bVisible = 0;
    cci.dwSize = 1;
    SetConsoleCursorInfo(hOutput, &cci);
    SetConsoleCursorInfo(hOutBuffer, &cci);	


} 
