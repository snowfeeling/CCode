#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <Windows.h>
#include <time.h>
#include <conio.h> /*键盘输入获取*/
#include "../inc/mylog.h"

bool gameOver;
bool stop = false;
bool hit = false;
/*游戏的边框大小*/
#define width 50
#define height 20

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

/*开始菜单*/
void menu()
{
    int a;
    printf("+--------------------------------------------------------------------+\n");
    printf("|                              Snake Game                            |\n");
    printf("|                              1) New Game                           |\n");
    printf("|                              2) Setting                            |\n");
    printf("|                              3) Quit                               |\n");
    printf("+--------------------------------------------------------------------+\n");
    printf("---->Please select: ");
    scanf("%d", &a);
}
/*初始化状态*/
void setup()
{
    gameOver = false;
    /*根据当前时间设置“随机数种子”*/
    srand(time(NULL));
    Dir = STOP;
    
    /*贪吃蛇头的位置,固定在中间*/
    x= width/2;
    y= height/2;
    /*食物的位置，位置是随机的*/
    fruitX = rand()%width+1;
    fruitY = rand()%height+1;
    score = 0;
}

bool switchover = false;
#define BORDER_SHOWS (1)

/*===========================================*/
static void set_status_bar()
{
    if (!gameOver)
    {
        char str1[width+2];
        sprintf(str1, "[Cord:%d,%d] [$(%d : %d)] [Score: %d]", x,y, fruitX, fruitY, score);
        strcpy(data[height+2], str1);
    }
    else
    {
        char str1[width+2];
        sprintf(str1, "[Cord:%d,%d] [$(%d : %d)] [Score: %d] Game Over!", x,y, fruitX, fruitY, score);
        strcpy(data[height+2], str1);
    }
}

/*============================================*/
static void switch_buffer()
{
	int i;
    /*双buff切换显示*/
	if(switchover == true)
	{
	    for (i = 0; i < height+3; i++)
	    {
	        coord.Y = i;
	        WriteConsoleOutputCharacterA(hOutBuf, data[i], width+2, coord, &bytes);
	    }
	    /*设置新的缓冲区为活动显示缓冲*/
	    SetConsoleActiveScreenBuffer(hOutBuf);	 
	}
	else
	{
		for (i = 0; i < height+3; i++)
	    {
	        coord.Y = i;
	        WriteConsoleOutputCharacterA(hOutput, data[i], width+2, coord, &bytes);
	    }
	    //设置新的缓冲区为活动显示缓冲
	    SetConsoleActiveScreenBuffer(hOutput);	
	}
	switchover = !switchover;
}


/*绘制界面*/
void draw()
{
	if(stop == true)
	{
		return;
	}

    //send_to_log(flog_handle, "Starting draw in draw()\n");
	int i;
	int j;

#if BORDER_SHOWS    
	/*第一行*/
    data[0][0] ='+';
    for(i= 1 ;i<width+1;i++)
	{ 
        data[0][i] = '=';
    }
    data[0][width+1] = '+';
#endif    
    /*画中间的画面*/ 
    int p;
    char a = 'a';
    for(p= 1 ;p<height+1;p++)/*高度*/
	{
        int q;
        for(q= 0 ;q<width+2;q++)/*宽度*/
		{
			/*第一行最后已给字符*/
            if(q==0 || q==width+1)
			{ 
				if (p<10) 
                    a = '0'+p;
                else
                    a = '0'+p-10;
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
                    /*贪吃蛇的长度 默认长度是 3*/
                    for( k = 0; k < ntail; k++)
                    {
                        if(tailX[k]==q && tailY[k]==p)
                        {
                            a = '*';
                            print = true;
                            break;
                        }
                    }
                    /*如果这个位置打印了 * 就不要打印空格了*/
                    if(!print)
                    {
                        a = '_';
                    }
	            }
        	}
            data[p][q] = a;
        }
    }
#if BORDER_SHOWS
	/*最后一行*/
    data[height+1][0]='+';
    for(j= 1 ;j<width+1;j++)
	{
        data[height+1][j] = '=';
    }
    data[height+1][width+1]='+';
    /*状态行*/
    set_status_bar();
#endif

    switch_buffer();
	Sleep(300);
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
                hit= true;
                strcpy(in_keys, "LEFT\n");
                break;
            case 72:/*上键*/
                Dir = UP;
                hit= true;
                strcpy(in_keys, "UP\n");
                break;
            case 77:/*右键*/
                Dir = RIGHT;
                hit= true;
                strcpy(in_keys, "RIGHT\n");
                break;
            case 80:/*向下键盘键 */
                Dir = DOWN;
                hit= true;                
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
	else 
        if(!hit && stop == false)/*如果刚开始没有改变方向的话*/
        {
            x++;
        }
}

/*判断贪吃蛇的长度*/ 
void logic()
{
	if(stop == true)
	{
		return;
	}
    //send_to_log(flog_handle, "Starting the logics in logic()\n");
	
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
    
    /*根据方向来改变x y 的值*/
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

    if(x<1 || x> width+1 || y<1 || y > height+1)
	{
        send_to_log(flog_handle, "Come to the boarder in logic()\n");

        gameOver = true;
        set_status_bar();
        draw();
        switch_buffer();
        Sleep(4200);
    }

        if(x==fruitX && y==fruitY)
        {
            /*吃了一个食物，蛇的长度增加1*/
            ntail++;
            score+=10;
            send_to_log(flog_handle, "Bingo!\n");
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
    //menu();
    setup();
    logic();
    draw();

    /*循环画贪吃蛇的界面*/
	while(!gameOver)
	{
        input();
        draw();
        logic(); 
    }

    close_snake_program();

    return 0;
}