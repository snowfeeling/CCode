#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <Windows.h>
#include <time.h>
#include <conio.h> /*键盘输入获取*/
#include "../inc/mylog.h"

bool gameOver;
bool stop = false;
/*游戏的边框大小*/
#define width   60
#define height  30

/*蛇的坐标，食物的坐标还有分数*/
int x,y,fruitX,fruitY,score;
/*蛇每个点的坐标*/
int tailX[200],tailY[200];
/*蛇的默认长度*/
int ntail=3;
/*控制台屏幕缓冲区句柄*/
HANDLE hOutput, hOutBuf;
COORD coord = { 0,0 };
/*双缓冲处理显示*/
DWORD bytes = 0;
char data[height+3][width+2];

FILE *flog_handle;

typedef enum  
{
	STOP = 0,
	LEFT,
	RIGHT,
	UP,
	DOWN
}Direction;
Direction Dir;
bool switchover = false;

/*====Init the  program====*/
static int init_snake_program()
{
    init_log_file(&flog_handle, "../log/snake.log");
    send_to_log(flog_handle, "============================\n");
    send_to_log(flog_handle, "The SNAKE game started.\n");
}

static int close_snake_program()
{
    send_to_log(flog_handle, "The SNAKE game stopped.\n");
    send_to_log(flog_handle, "============================\n");
    close_log_file(flog_handle);
}


/*初始化状态*/
static void init_snake_setting()
{
    gameOver = false;
    /*根据当前时间设置“随机数种子”*/
    srand(time(NULL));
    /*初始方向向右*/
    Dir = RIGHT;
    
    /*贪吃蛇头的位置,固定在中间*/
    x= width/2;
    y= height/2;
    /*食物的位置，位置是随机的*/
    fruitX = rand()%width+1;
    fruitY = rand()%height+1;
    score = 0;
}

/*===========================================*/
static void set_status_bar()
{
    if (!gameOver)
    {
        char str1[width+2];
        sprintf(str1, "[Pos:%2d,%2d][$%2d:%2d][Score:%3d][Length:%2d]", x,y, fruitX, fruitY, score, ntail);
        strcpy(data[height+2], str1);
    }
    else
    {
        char str1[width+2];
        sprintf(str1, "[Pos:%2d,%2d][$%2d:%2d][Score:%3d][Length:%2d] Game Over!", x,y, fruitX, fruitY, score, ntail);
        strcpy(data[height+2], str1);
    }
}

/*============================================*/
static void switch_buffer()
{
	int i;
    HANDLE hOut;
    /*双buff切换显示*/
	if(switchover == true)
	{
        hOut = hOutBuf;
	}
	else
	{
        hOut = hOutput;
    }
    for (i = 0; i < height+3; i++)
    {
        coord.Y = i;
        WriteConsoleOutputCharacterA(hOut, data[i], width+2, coord, &bytes);
    }
    //设置新的缓冲区为活动显示缓冲
    SetConsoleActiveScreenBuffer(hOut);	

	switchover = !switchover;
}

/*绘制界面*/
static void draw()
{
	if(stop == true)
	{
		return;
	}
	int i;
	int j;

	/*设置第一行*/
    data[0][0] = '+';
    for(i= 1 ;i<width+1;i++)
	{ 
        data[0][i] = '=';
    }
    data[0][width+1] = '+';

    /*设置中间的画面数据*/ 
    int p;
    char a = 'a';
    for(p= 1 ;p<height+1;p++)/*高度*/
	{
        int q;
        for(q= 0 ; q<width+2; q++)/*宽度*/
		{
			/*第一列和最后一列显示墙符*/
            if(q==0 || q==width+1)
			{ 
                    a = '|';
            }
            else
			{
				if(p == fruitY && q == fruitX)/*食物的随机坐标*/
				{
	                a = '$';
	            }
				else
				{
                    int k=0;
                    bool print = false;
                    /*显示贪吃蛇的形状 */
                    if(tailX[0]==q && tailY[0]==p)
                        {
                            switch (Dir)
                            {
                            case UP:
                                a = 'A';
                                break;
                            case LEFT:
                                a = '<';
                                break;
                            case RIGHT:
                                a = '>';
                                break;
                            case DOWN:
                                a = 'V';
                                break;    
                            }
                            print = true;
                        }         
                    for( k = 1; k < ntail; k++)
                    {
                        if(tailX[k]==q && tailY[k]==p)
                        {
                            a = '*';
                            print = true;
                            break;
                        }
                    }
                   
                     /*如果这个位置没有打印 "*", 就打印空字符*/
                    if(!print)
                    {
                        a = '-';
                    }
	            }
        	}
           data[p][q] = a;
        }
    }

	/*设置最后一行数据*/
    data[height+1][0]= '+';
    for(j= 1 ;j<width+1;j++)
	{
        data[height+1][j] = '=';
    }
    data[height+1][width+1]= '+';

    /*设置状态行信息*/
    set_status_bar();

    /*刷新屏幕*/
    switch_buffer();
    if (gameOver)
        Sleep(4200);
    else 
	    Sleep(200);
}

/*按键输入控制*/
void input()
{
    //send_to_log(flog_handle, "Starting checking key_input in input()\n");

	/*如果有按键按下*/
    if(_kbhit())
	{
        char in_keys[10];
        char ch;
        do
        {
            ch =  _getch();
        } while (_kbhit());

		/*获取键盘的输入字符*/
        switch(ch)
		{
            case 75:/*左键*/
                Dir = LEFT;
                strcpy(in_keys, "LEFT\n");
                break;
            case 72:/*上键*/
                Dir = UP;
                strcpy(in_keys, "UP\n");
                break;
            case 77:/*右键*/
                Dir = RIGHT;
                strcpy(in_keys, "RIGHT\n");
                break;
            case 80:/*向下键盘键 */
                Dir = DOWN;
                strcpy(in_keys, "DOWN\n");
                break;
            case 27:/*ESC*/
                gameOver = true;
                strcpy(in_keys, "ESC\n");
                break;
            case 32:/*空格 暂停键*/
				stop = !stop;
                strcpy(in_keys, "SPACE\n");
				break; 
            default:
                sprintf(in_keys, "OTHERS:%c\n", ch);

        }
        send_to_log(flog_handle, in_keys);
    }
}

/*判断贪吃蛇的长度*/ 
void logic()
{
	if(stop == true)
	{
		return;
	}
	
	/*把上一个位置记下*/
    int lastX = tailX[0];
    int lastY = tailY[0];
    int last2X, last2Y;
    /*重新获取当前的位置*/
	tailX[0]=x;
    tailY[0]=y;
    int i=0;
    /*遍历整条蛇的长度 把 0 的位置空出来，其余蛇的位置往后面的空间移动*/
    for(i=1; i<ntail;i++)
	{
        last2X = tailX[i];
        last2Y = tailY[i];
        tailX[i]=lastX; 
        tailY[i]=lastY;
        lastX = last2X; 
        lastY = last2Y;
    }
    
    /*根据方向来改变(x,y)坐标值*/
    switch(Dir)
	{
        case UP:
            y--;
            break;
        case DOWN:
            y++;
            break;
        case LEFT:
            x--;
            break;
        case RIGHT:
            x++;
            break;
    }
    /*如果到了边界值，就设置gameover*/
    if(x<1 || x> width || y<1 || y > height)
	{
        send_to_log(flog_handle, "Come to the border.\n");
        gameOver = true;
    }

    /*如果是食物点，就设置分数、蛇的长度、下一个食物的位置*/
    if(x==fruitX && y==fruitY)
    {
        /*吃了一个食物，蛇的长度增加1*/
        ntail ++;
        score += 10;
        send_to_log(flog_handle, "<Bingo!>\n");
        /*更新下一个食物的位置*/
        fruitX = rand()%width+1;
        fruitY = rand()%height+1;
    }
}

/*双缓冲技术解决闪屏问题*/
void double_buff_init(void)
{
   	/*创建新的控制台缓冲区*/
    hOutBuf = CreateConsoleScreenBuffer(
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
    SetConsoleCursorInfo(hOutBuf, &cci);	
} 

int snake()
{
    init_snake_program();
	double_buff_init();
    init_snake_setting();
    //logic();
    //draw();

    /*循环画贪吃蛇的界面*/
	while(!gameOver)
	{
        input();
        logic(); 
        draw();
    }

    close_snake_program();

    return 0;
}