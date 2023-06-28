/* 俄罗斯方块
Created by Wangss on 2023-06-21.

*/

#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <stdbool.h>
#include <locale.h>
#include "../inc/rb.h"

#define BUFFER_SIZE 100
#define ROW 29 //游戏区行数
#define COL 16 //游戏区列数

#define UPKEY	72 //方向键：上
#define DOWN 	80 //方向键：下
#define LEFT 	75 //方向键：左
#define RIGHT 	77 //方向键：右

#define SPACEKEY 32 //空格键
#define ESCKEY	27 //Esc键

#define ESC "\x1b"
#define CSI "\x1b["
#define FANGK "■"
#define BLANK " "
#define VBOARDER "|"
#define HBOARDER "__"

//#define HBOARDER "_"

#define DATAFNAME "../data/tetris.dat"

typedef struct  Screen
{
	int data [ROW][COL + 10]; //用于标记指定位置是否有方块（1为有，0为无）
	int color[ROW][COL + 10]; //用于记录指定位置的方块颜色编码
} SCREEN;

typedef struct Block
{
	int space[4][4];
} BLOCK;

/*=======宏定义区域=======*/
// 切换到主屏幕缓冲区
#define SWITCH_MAIN_SCREEN()  { printf(CSI "?1049l"); }
// 切换到备屏幕缓冲区
#define SWITCH_ALTERNATE_SCREEN()  { printf(CSI "?1049h"); }
// 跳转到屏幕起点
#define GO_SCREEN_HOME() { printf(CSI "1;1H" ); }
// 清屏 
#define CLEAR_SCREEN() { printf(CSI "2J"); }
// 关闭/打开光标
#define TURNOFF_CURSOR()  { printf(CSI "?25l"); }
#define TURNON_CURSOR()   { printf(CSI "?25h"); }
//光标跳转
#define cursor_jump(x, y) { printf(CSI "%d;%dH", y, x); }
//设置窗口title
#define SET_CONSOLE_TITLE() { printf (ESC "]2;Trteris-2023\x07"); }


/*======函数定义区域======*/
//初始化界面
static void init_game_interface();
//初始化方块信息
static void init_block_info();
//颜色设置
static void set_color(int num);
//画出方块
static void draw_block(int shape, int form, int x, int y);
//在最后一行，显示block投影
static int draw_block_shape(int shape, int form, int x, int y);

//空格覆盖
static void draw_space(int shape, int form, int x, int y);
//合法性判断
static int IsLegal(int shape, int form, int x, int y);
//判断得分与结束
static bool check_lines_status();
//游戏主体逻辑函数
static void StartGame();
//从文件读取最高分
static void ReadGrade();
//更新最高分到文件
static void WriteGrade();
//设置屏幕为虚拟终端模式
static bool enable_VT_Mode();
//初始化屏幕
static int init_game_screen();
// 处理游戏结束
static int handle_game_over();
//状态行显示
static void show_game_status_line(char *str);
//初始化主屏幕
static int init_main_screen();
//显示命令菜单
static void show_command_manual(void);
//主菜单
static int main_manual();

static int change_setting();


/*=====Code Start Line================================================*/

//全局变量
int max_score, grade; 
bool bGameOver = false;
SCREEN scr;
BLOCK block[7][4]; //用于存储7种基本形状方块的各自的4种形态的信息，共28种
const int block_color[8] = {35, 31, 31, 32, 32, 33, 36, 0};
/*
[0]  -35 “T”形方块设置为紫色
[1,2]-31 “L”形和“J”形方块设置为红色
[3,4]-32 “Z”形和“S”形方块设置为绿色
[5]  -33 “O”形方块设置为黄色
[6]  -36 “I”形方块设置为浅蓝色
[7]  - 0 其他默认设置为白色
*/

int mytetris()
{
	init_main_screen();
	main_manual();
	return 0;
}

int testrb()
{
	max_score = 0, grade = 0, bGameOver = false; //初始化变量
	//system("mode con lines=29 cols=60"); //设置cmd窗口的大小

	init_game_screen();
	ReadGrade(); //从文件读取最高分到max变量	
	init_game_interface(); //初始化界面
	init_block_info(); //初始化方块信息
	srand((unsigned int)time(NULL)); //设置随机数生成的起点
	StartGame(); //开始游戏
	handle_game_over();

	return 0;
}

/*=================The functions to show the main manual.=====================
*/
static void show_command_manual1(void)
{
    printf("Enter any of the following commands after the prompt > :\n"
           "\tN -- New Game.\n"
           "\tS -- Setting.\n"
           "\tq -- Quit. (Or use Ctl-C.)\n"
           "\t? -- Print this help message.\n");
}

static void show_command_manual(void)
{

    printf("在提示符 > 后输入命令 :\n"
           "\tN -- 开启游戏\n"
           "\tS -- 设置\n"
           "\t? -- 帮助信息\n"
           "\tq -- 退出 \n");
}

static int init_main_screen()
{
	bool fSuccess = enable_VT_Mode();
    if (!fSuccess)
    {
        return -1;
    }
	//设置title
	SET_CONSOLE_TITLE();

	// 设置UTF8 Code Page
	//setlocale(LC_ALL, ".UTF8");
	SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

	return 0;

}

static int main_manual()
{
    char command;
    bool input_consumed = false;

	show_command_manual();
    printf("> ");

    while (scanf("%c", &command) != EOF)
    {
        input_consumed= false;
        switch (command)
        {
        case 'n':
		case 'N':
			printf("The new game is started.\n");
			testrb();
            break;
        case 's':
			printf("Change the setting.\n");
			Sleep(600);
			change_setting();
            break;
        case 'q':           
			printf("Quit the game.\n");
            return EXIT_SUCCESS;
        case '\n':
            input_consumed = true;
            break;
        default:
            show_command_manual();
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

int test1()
{	
	enable_VT_Mode();

	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
	printf("Main Screen\n");
	printf("开始了。。。");
	Sleep(3000);

	SWITCH_ALTERNATE_SCREEN();
	//CLEAR_SCREEN();
	TURNOFF_CURSOR();
	printf("ALternate screen.\n");
	Sleep(3000);

    // Exit the alternate buffer
	SWITCH_MAIN_SCREEN();
    TURNON_CURSOR();
	printf("Main Screen.\n");
	Sleep(3000);
	return 0;
}

//显示状态行信息
static void show_game_status_line(char * str)
{
	cursor_jump(1, ROW+1);
	printf(CSI "K");  //清除本行光标之后的信息
	printf("%s", str);
};

//设置屏幕为虚拟终端模式
static bool enable_VT_Mode()
{
    // Set output mode to handle virtual terminal sequences
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
    {
		printf("Error:GetConsoleMode.\n");
        return false;
    }

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	//dwMode |= ENABLE_PROCESSED_OUTPUT ;
    if (!SetConsoleMode(hOut, dwMode))
    {		
		printf("Error:SetConsoleMode.\n");
        return false;
    }

    return true;
}

static int init_game_screen()
{

    // 切换备用屏幕
	SWITCH_ALTERNATE_SCREEN();

	//光标到起始点，清屏，关闭光标
	GO_SCREEN_HOME();
	CLEAR_SCREEN();
	TURNOFF_CURSOR();
    
    return 0;
}

/*初始化游戏开始界面
游戏区（列 1~COL-1）
信息区（列 COL+1~ COL+8）
列（0,COL, COL+9）为边界标识
最后一行（行ROW）为边界标识
设置整个区域的数据值；
*/
static void init_game_interface()
{
	int i,j;
	set_color(7); //颜色设置为白色
	for (i = 0; i < ROW; i++) //从0行到ROW行
	{
		for (j = 0; j < COL + 10; j++) //从0列到COL+10列
		{
			if ((j == 0 ) || (j == COL - 1) || (j == COL + 9) ) //0,COL, COL+9为边界标识
			{
				scr.data[i][j] = 1; //标记该位置有方块
				scr.color[i][j] = 0; //颜色为白色
				cursor_jump(2 * j, i);
				printf(VBOARDER);
			}
			else 
				if (i == ROW - 1) //最后一行（行ROW）为边界标识
				{
					scr.data[i][j] = 1; //标记该位置有方块
					scr.color[i][j] = 0;
					cursor_jump(2 * j, i);
					printf(HBOARDER);
				}
				else
					scr.data[i][j] = 0; //标记该位置无方块
		}
	}
	for (i = COL; i < COL + 9; i++) //信息区第8行为边界标识
	{
		scr.data[8][i] = 1; //标记该位置有方块
		scr.color[8][j] = 0; 
		cursor_jump(2 * i, 8);
		printf(HBOARDER);
	}

	cursor_jump(2 * COL, 1);
	printf("下一个方块：");

	cursor_jump(2 * COL + 3, ROW - 19);
	printf("左移：←");

	cursor_jump(2 * COL + 3, ROW - 17);
	printf("右移：→");

	cursor_jump(2 * COL + 3, ROW - 15);
	printf("加速：↓");

	cursor_jump(2 * COL + 3, ROW - 13);
	printf("旋转：↑");

	cursor_jump(2 * COL + 3, ROW - 11);
	printf("暂停: 空格");

	cursor_jump(2 * COL + 3, ROW - 9);
	printf("退出: Esc");

	cursor_jump(2 * COL + 3, ROW - 5);
	printf("最高纪录:%d", max_score);

	cursor_jump(2 * COL + 3, ROW - 3);
	printf("当前分数：%d", grade);
	
}
//初始化方块信息
static void init_block_info()
{
	//“T”形
	for (int i = 0; i <= 2; i++)
		block[0][0].space[1][i] = 1;
	block[0][0].space[2][1] = 1;

	//“L”形
	for (int i = 1; i <= 3; i++)
		block[1][0].space[i][1] = 1;
	block[1][0].space[3][2] = 1;

	//“J”形
	for (int i = 1; i <= 3; i++)
		block[2][0].space[i][2] = 1;
	block[2][0].space[3][1] = 1;

	for (int i = 0; i <= 1; i++)
	{
		//“Z”形
		block[3][0].space[1][i] = 1;
		block[3][0].space[2][i + 1] = 1;
		//“S”形
		block[4][0].space[1][i + 1] = 1;
		block[4][0].space[2][i] = 1;
		//“O”形
		block[5][0].space[1][i + 1] = 1;
		block[5][0].space[2][i + 1] = 1;
	}

	//“I”形
	for (int i = 0; i <= 3; i++)
		block[6][0].space[i][1] = 1;

	int temp[4][4];
	for (int shape = 0; shape < 7; shape++) //7种形状
	{
		for (int form = 0; form < 3; form++) //4种形态（已经有了一种，这里每个还需增加3种）
		{
			//获取第form种形态
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					temp[i][j] = block[shape][form].space[i][j];
				}
			}
			//将第form种形态顺时针旋转，得到第form+1种形态
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					block[shape][form + 1].space[i][j] = temp[3 - j][i];
				}
			}
		}
	}
	show_game_status_line("初始化block信息完成!");
}
//颜色设置
static void set_color(int c)
{
    printf(CSI "%dm", (c>=0 && c <= 7) ? (block_color[c]) : (block_color[7]));
}

//从(x,y)的位置画方块
static void draw_block(int shape, int form, int x, int y)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (block[shape][form].space[i][j] == 1) //如果该位置有方块
			{
				cursor_jump(2 * (x + j), y + i); //光标跳转到指定位置
				printf(FANGK); //输出方块
			}
		}
	}
}
//空格覆盖
static void draw_space(int shape, int form, int x, int y)
{
	int i,j;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			if (block[shape][form].space[i][j] == 1) //如果该位置有方块
			{
				cursor_jump(2 * (x + j), y + i); //光标跳转到指定位置
				printf(BLANK BLANK); //打印空格覆盖（两个空格）
			}
		}
	}
}

/*判断在当前位置，是否可以放置block;
	输入参数：block的形状 和 要放置的位置(x,y)
	返回值：0-表示不可以放置；1-表示可以放置
*/
static int IsLegal(int shape, int form, int x, int y)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			//如果方块落下的位置本来就已经有方块了，则不可以放置block
			if ((block[shape][form].space[i][j] == 1) && (scr.data[y + i][x + j] == 1))
				return 0; //0 表示不可以放置
		}
	}
	return 1; //1表示可以放置
}

/* 判断游戏是否结束
判断条件：在顶层（scr的第1层）是否有方块存在。
*/
static bool is_game_over()
{
	for (int j = 1; j < COL - 1; j++)
	{
		if (scr.data[1][j] == 1) //顶层有方块存在（以第1行为顶层，不是第0行）
		{
			return true;
		}
	}
	return false;
}

//更新分数显示
static int show_score(int grade)
{
	char str[100];
	set_color(7); //颜色设置为白色
	cursor_jump(2 * COL + 3, ROW - 3); //光标跳转到显示当前分数的位置
	printf("当前分数：%d", grade); //更新当前分数
	sprintf(str, "当前分数：%d", grade);
	show_game_status_line(str);
	return 0;
}


/*一个block下落期间的状态判断
* 判断得分、是否结束
* 先判断一轮：从最底部开始，判断每一行是否满行；
	如果该行满行，就消除，并下移该满行之上的行数；然后从底部开始，继续判断；
	如果该行不是满行，继续判断下一行；直至找到空白行；
*/
static bool check_lines_status()
{
	int i, j, m, n, sum, nsum=0;
	bool check_completed,  istop ;
	bool game_status = false;

	do
	{
		check_completed = true; //判断是否检查结束；
		istop = false; //判断这一循环是否结束；
		//从ROW-2行，倒数第二行开始；最后一行（ROW行），是边界行
		for (i = ROW - 2; i > 4  && !istop; i--)
		{
			istop = false;
			sum = 0; //记录第i行的方块个数
			for (j = 1; j < COL - 1; j++)
			{
				sum += scr.data[i][j]; //统计第i行的方块个数
			}
			if (sum == 0) //该行没有方块，无需再判断其上的层次（无需再继续判断是否得分）
			{
				check_completed = true;
				istop = true;
				break; //跳出循环
			}
			else
			{
				if (sum == COL - 2) //该行全是方块，计算得分；然后把上面所有的行都挪下一行。但是不判断所挪动的行是否有满行
				{
					nsum ++; //一个下落block期间，消除行数的计算。
					grade += 10 * nsum; //加分规则：第一加10分；后面的行数*10；所以有连续行的话，消除两行=10+10*2=30；消除三行=10+10*2+10*3=60；消除四行=10+10*2+10*3+10*4=100
					show_score(grade);  //显示得分
					if (nsum >2 )
					{
						char str[BUFFER_SIZE];
						sprintf(str, "Bingo! 获得高分%d!", 10*nsum);
						show_game_status_line(str);
					}
					/*//清除得分行的方块信息；其实可以不用清除，因为后面会把上一行移动下来覆盖之；
					for (j = 1; j < COL - 1; j++) 
					{
						scr.data[i][j] = 0; //该位置得分后被清除，标记为无方块
						cursor_jump(2 * j, i); //光标跳转
						printf(BLANK BLANK); //打印两个空格。
					}*/
					//把上面的行都向下挪一格；但是不判断是否满行；
					for (m = i; m >1; m--)
					{
						sum = 0; //记录上一行的方块个数
						for (n = 1; n < COL - 1; n++)
						{
							sum += scr.data[m - 1][n]; //统计上一行的方块个数
							scr.data[m][n] = scr.data[m - 1][n]; //将上一行方块的标识移到下一行
							scr.color[m][n] = scr.color[m - 1][n]; //将上一行方块的颜色编号移到下一行
							cursor_jump(2 * n, m); //光标跳转到该位置
							if (scr.data[m][n] == 1) //上一行移下来的是方块，打印方块
							{
								set_color(scr.color[m][n]); //颜色设置为方块的颜色
								printf(FANGK); //打印方块
							}
							else //否则，上一行移下来的是空格，打印
							{
								printf(BLANK BLANK); //打印空格（两个空格）
							}
			
						}
						if (sum == 0) //上一行全是空格，说明上面的行都是空，无需再将上层的方块向下移动（移动结束）；但是移动下来的可能还有满行，需要从底部再开始判断；
						{
							check_completed = false;
							istop = true;
							break;
						}	
					}
				}
				else //如本行不是满行方块，不能消除，就继续检查上一行。
				{
					istop =false;
					//check_completed = true;
				}
			}	
		}

		//判断游戏是否结束
		game_status = is_game_over();
		if (game_status)
		{
			check_completed = true; // 如果判断游戏结束，则检查标志就为true；
		}	
	} while (!check_completed);

	return game_status;

}

//处理游戏结束
static int handle_game_over()
{
	char str[BUFFER_SIZE];

	if (grade>max_score)
	{
		sprintf(str, "恭喜!你以%d分打破最高记录。", grade);
		WriteGrade();
	}
	else 
		if (grade == max_score)
		{
			sprintf(str, "恭喜你的得分%d与最高记录持平！", grade);
		}
		else
		{
			sprintf(str, "本次得分%d当前与最高记录相差%d。请继续加油！", grade, max_score - grade);
		}
	printf(CSI "1m" CSI "5m" CSI "91m"); //1m增强、5m闪亮、91m亮红色
	show_game_status_line(str);
	Sleep(1800);
	show_game_status_line("GAME OVER!");
	Sleep(1800);

	//恢复主屏幕
	SWITCH_MAIN_SCREEN();
	//打开光标
	TURNON_CURSOR();

	return 0;
}

/* 显示方块的投影
	显示在最后一行（边界行）上，
*/
static int draw_block_shape(int shape, int form, int x, int y)
{
	int row, col, sum[4]={0,0,0,0};
	int ix, ilen;
	for ( col = 0; col < 4; col++) 
	{
		for ( row = 0; row < 4; row++)
		{
			//投影计算：统计每列的方块数
			sum[col] += block[shape][form].space[row][col];			
		}
	}
	ix = 0,  ilen = 0;
	for (col = 0; col < 4; col++)
	{
		if (sum[col] >0) //如果该列有方块
		{
			if(ix==0) //如果该列有方块，就设为 有方块的首列。位置需要+x
				ix = col + x;
			ilen ++ ; //累加计算投影的长度
		} 
	}
	
	for (col = 1; col < COL; col++)
	{
		cursor_jump(2*col, ROW-1);  //跳到最后一行，是边界行
		if (col>=ix && col<ix+ilen)
			printf(CSI "91m"); //如果在投影范围内，显示（91亮红色，或者可以 92亮绿色）
		else
		{
			printf(CSI "0;97m"); //不在投影范围内，显示亮白色
		}
		printf(HBOARDER );  //显示边界"_" 
	}
	return 0;
}
//游戏主体逻辑函数
static void StartGame()
{
	bGameOver = false; // 游戏结束标志

	int shape = rand() % 7, form = rand() % 4; //随机获取方块的形状和形态
	while (!bGameOver)
	{
		int t = 0;
		int nextShape = rand() % 7, nextForm = rand() % 4; //随机获取下一个方块的形状和形态
		int x = COL / 2 - 2, y = 0; //方块初始下落位置的横纵坐标
		set_color(nextShape); //颜色设置为下一个方块的颜色
		draw_block(nextShape, nextForm, COL + 3, 3); //将下一个方块显示在右上角
		while (!bGameOver)
		{
			set_color(shape); //颜色设置为当前正在下落的方块
			draw_block(shape, form, x, y); //将该block显示在初始下落位置
			draw_block_shape(shape, form, x, y); //显示block投影位置
			if (t == 0)
			{
				t = 15000; //这里t越小，方块下落越快（可以根据此设置游戏难度）
			}
			while (--t)
			{
				if (kbhit() != 0) //若键盘被敲击，则退出循环
					break;
			}
			if (t == 0) //键盘未被敲击
			{
				if (IsLegal(shape, form, x, y + 1) == 0) //方块再下落就不合法了（已经到达底部）
				{
					//scr:记录界面的每个位置是否有方块，若有方块还需记录该位置方块的颜色。
					for (int i = 0; i < 4; i++)
					{
						for (int j = 0; j < 4; j++)
						{
							if (block[shape][form].space[i][j] == 1)
							{
								scr.data[y + i][x + j] = 1; //将该位置标记为有方块
								scr.color[y + i][x + j] = shape; //记录该方块的颜色数值
							}
							//其他部位可能已经有了方块，不可能置0
						}
					}
					//这是本程序最重要的判断；//判断此次方块下落是否得分以及游戏是否结束
					bGameOver = check_lines_status();
					break; //跳出当前死循环，准备进行下一个方块的下落
				}
				else //未到底部
				{
					draw_space(shape, form, x, y); //用空格覆盖当前方块所在位置
					y++; //纵坐标+1（下一次显示方块时就相当于下落了一格了）
				}
			}
			else //键盘被敲击
			{
				char ch = getch(); //读取键盘值
				switch (ch)
				{
				case DOWN: //方向键：下
					if (IsLegal(shape, form, x, y + 1) == 1) //判断方块向下移动一位后是否合法
					{
						draw_space(shape, form, x, y); //用空格覆盖当前方块所在位置
						y++; //纵坐标自增（下一次显示方块时就相当于下落了一格了）
					}
					break;
				case LEFT: //方向键：左
					if (IsLegal(shape, form, x - 1, y) == 1) //判断方块向左移动一位后是否合法
					{
						draw_space(shape, form, x, y); //用空格覆盖当前方块所在位置
						x--; //横坐标自减（下一次显示方块时就相当于左移了一格了）
					}
					break;
				case RIGHT: //方向键：右
					if (IsLegal(shape, form, x + 1, y) == 1) //判断方块向右移动一位后是否合法
					{
						draw_space(shape, form, x, y); //用空格覆盖当前方块所在位置
						x++; //横坐标自增（下一次显示方块时就相当于右移了一格了）
					}
					break;
				case UPKEY: //方向键：上,在原地旋转
					if (IsLegal(shape, (form + 1) % 4, x, y ) == 1) //判断方块旋转后是否合法，原地旋转
					{
						draw_space(shape, form, x, y); //用空格覆盖当前方块所在位置
						form = (form + 1) % 4; //方块的形态自增（下一次显示方块时就相当于旋转了）
					}
					break;
				case ESCKEY: //Esc键
					bGameOver = true;
					break;
				case SPACEKEY: //空格键,暂停
					getch();
					//system("pause>nul"); //暂停（按任意键继续）
					break;
				}
			}
		}
		shape = nextShape, form = nextForm; //获取下一个方块的信息
		draw_space(nextShape, nextForm, COL + 3, 3); //将右上角的方块信息用空格覆盖
	}
}
//从文件读取最高分
static void ReadGrade()
{
	FILE* pf = fopen(DATAFNAME, "r"); //以只读方式打开文件
	if (pf == NULL) //打开文件失败
	{
		pf = fopen(DATAFNAME, "w"); //以只写方式打开文件（文件不存在可以自动创建该文件）
		fwrite(&grade, sizeof(int), 1, pf); //将max写入文件（此时max为0），即将最高历史得分初始化为0
	}
	fseek(pf, 0, SEEK_SET); //使文件指针pf指向文件开头
	fread(&max_score, sizeof(int), 1, pf); //读取文件中的最高历史得分到max当中
	fclose(pf); //关闭文件
	pf = NULL; //文件指针及时置空
}
//更新最高分到文件
static void WriteGrade()
{
	FILE* pf = fopen(DATAFNAME, "w"); //以只写方式打开文件
	if (pf == NULL) //打开文件失败
	{
		printf("保存最高得分记录失败\n");
		exit(0);
	}
	fwrite(&grade, sizeof(int), 1, pf); //将本局游戏得分写入文件当中（更新最高历史得分）
	fclose(pf); //关闭文件
	pf = NULL; //文件指针及时置空
}

static int change_setting()
{
	SWITCH_ALTERNATE_SCREEN();
	
	show_game_status_line("Change the game setting.");
	Sleep(3000);

	SWITCH_MAIN_SCREEN();
	return 0;
}

