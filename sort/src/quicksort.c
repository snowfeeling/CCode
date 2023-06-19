
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <conio.h> /*键盘输入获取*/

#include "../inc/mylog.h"
#include "../inc/quicksort.h"

#define SQFNAME "../log/quicksort.log"

FILE *flog;
HANDLE hOutput, hOutBuffer;
COORD coord;
BOOL first_show = true;

int scr_width=50, scr_height=30;
BOOL switch_over;
char scr_data[30][50];

void init_double_buffer(void);
static void refresh_screen();
static int put_original_data_to_buffer(Squeue *p);
//static int put_sorting_data_to_buffer(Squeue *p, int low, int high, int j, int tmp);
static int put_sorting_data_to_buffer(Squeue *p, int low, int high, int cur, int tmp, int ii, int jj);

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
    put_original_data_to_buffer(&p);
    refresh_screen();

    put_sorting_data_to_buffer(&p, 0, p.qnum-1, -1, -1, 0, p.qnum-1);
    refresh_screen();

    QuickSort(&p, 0, p.qnum - 1);

    put_sorting_data_to_buffer(&p, 0, p.qnum-1, -1, -1, 0, p.qnum-1);
    refresh_screen();

    do
    {
        Sleep(1200);
        /* code */
    } while (!_kbhit());

    printf("The sorted data is :\n");
    ShowData(&p);

    close_log_file(flog);
    
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

/* write original data to buffer*/
static int put_original_data_to_buffer(Squeue *p)
{
    char str[200];
    for (int i = 0; i < p->qnum; i++)
    {
        sprintf(scr_data[i], "[%3d]%5d", i, p->value[i]);
    }
    return 0;
} 

/*====Partition the queue ======
*/
int partition_queue(Squeue *p, int low, int high)
{
    int tmp, i, j;
    char str[30];
    i = low;
    j = high;
    
    tmp = p->value[i]; // 取第一个值，为比较值
    put_sorting_data_to_buffer(p, low, high, i, tmp, i, j);
    refresh_screen();
    sprintf(str, "[Begin:P=%6d %3d -%3d]\n", tmp, i, j);
    LogMeg(str);
    LogQueueData(p);
    
    do
    {
        while (p->value[j] > tmp && i < j)
        {
            put_sorting_data_to_buffer(p,low, high, j, tmp, i, j);
            refresh_screen();
            j--;  // 如果从右的数字大于比较值，就向左走，直到等于或者小于比较值。
        }
        if (i < j)
        {
            put_sorting_data_to_buffer(p,low, high, j, tmp, i, j);
            refresh_screen();
            put_sorting_data_to_buffer(p, low, high, i, tmp, i, j);
            refresh_screen();
            p->value[i] = p->value[j]; // 把右边的较小值，放到左边
            sprintf(str, "[R->L :P=%6d %3d -%3d]\n", tmp, j, i);
            LogMeg(str);
            LogQueueData(p);
            put_sorting_data_to_buffer(p, low, high, i, tmp, i, j);
            refresh_screen();
            i++;  //往右走一个
        }
        while (p->value[i] < tmp && i < j) 
        {
            put_sorting_data_to_buffer(p,low, high, i, tmp, i, j);
            refresh_screen();
            i++;   // 如果从左的数字小于比较值，就向右走，直到等于或者大于比较值。
        }
        if (i < j)
        {
            put_sorting_data_to_buffer(p,low, high, i, tmp, i, j);
            refresh_screen();
            put_sorting_data_to_buffer(p, low, high, j, tmp, i, j);
            refresh_screen();
            p->value[j] = p->value[i]; //把左边较大的值，放到右边
            // 记录
            sprintf(str, "[L->R :P=%6d %3d -%3d]\n", tmp, i, j);
            LogMeg(str);
            LogQueueData(p);
            put_sorting_data_to_buffer(p, low, high, j, tmp, i, j);
            refresh_screen();
            j--; // 往左走一步
        }
    } while (i != j);

    put_sorting_data_to_buffer(p, low, high, j, tmp, i, j);
    refresh_screen();

    p->value[i] = tmp; // 把比较值，放到中间的的位置。
    sprintf(str, "[TMP  :P=%6d %3d -%3d]\n", tmp, i, j);
    LogMeg(str);
    LogQueueData(p);
    put_sorting_data_to_buffer(p, low, high, i, tmp, i, j);
    refresh_screen();
    
    return i;
} 
/*====Quick Sort=========
*/
void QuickSort(Squeue *p, int low, int high)
{
    if (low >= high)
        return;
    int pivot_loc;
    pivot_loc = partition_queue(p, low, high);

    QuickSort(p, low, pivot_loc - 1);  // 比较左边的
    QuickSort(p, pivot_loc + 1, high); // 比较右边的
}


/* write sorting data to buffer*/
static int put_sorting_data_to_buffer(Squeue *p, int low, int high, int cur, int tmp, int ii, int jj)
{
    char str1[20], str2[20], str3[20], str4[20], str5[20];
    
    int k;
    for ( k = 0; k < p->qnum; k++)
    {
        memset(str1, 0, 20);
        memset(str2, 0, 20);
        memset(str3, 0, 20);
        memset(str4, 0, 20);
        memset(str5, 0, 20);
        if (k!=cur || cur < 0)
        {
            sprintf(str1, "%9s", " ");
        }
        else
        {
            sprintf(str1, "[%5d]->", tmp);
        }
        
        sprintf(str2, "%5d", p->value[k]);

        if ( k == ii)
            sprintf(str3, "%4s","[i]");
        else
            sprintf(str3, "%4s"," ");
        
        if ( k == jj)
            sprintf(str4, "%4s","[j]");
        else
            sprintf(str4, "%4s","   ");

        if (k == low)
            sprintf(str5, "%3s", "(L)");
        else
            if(k == high)
                sprintf(str5, "%3s","(H)");
            else
                sprintf(str5, "%3s", "  ");

        sprintf(scr_data[k], "%9s%5s%4s%4s%3s",str1, str2, str3, str4, str5);

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
            WriteConsoleOutputCharacterA(hOutBuffer,    scr_data[i], scr_width, coord, &scr_bytes);
            WriteConsoleOutputCharacterA(hOutput,       scr_data[i], scr_width, coord, &scr_bytes);
        }
        else
        {
            coord.X = 13;
            WriteConsoleOutputCharacterA(hOut,          scr_data[i], scr_width, coord, &scr_bytes);
        }
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
