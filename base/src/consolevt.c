/*
Console Virtual Term base functions
Created by Wangss 2023-07-02
*/

#include "../inc/consolevt.h"

int set_console_CodePage(int codepage)
{
#ifdef WIN32
	// 设置UTF8 Code Page
	//setlocale(LC_ALL, ".UTF8");
	SetConsoleCP(codepage);
    SetConsoleOutputCP(codepage);
#endif
    return 0;
}

//设置屏幕为虚拟终端模式
bool enable_VT_Mode()
{
#ifdef WIN32
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
    if (!SetConsoleMode(hOut, dwMode))
    {		
		printf("Error:SetConsoleMode.\n");
        return false;
    }

#endif

    return true;
}

/*======= MAC OS下定义新函数 =========
    _getch()
    _getche()
*/
#if defined(__APPLE__)

int _getch(void)
{
     struct termios tm, tm_old;
     int fd = 0;
     int ch;
     if (tcgetattr(fd, &tm) < 0) {//保存现在的终端设置
          return -1;
     } 
     tm_old = tm;
     cfmakeraw(&tm);//更改终端设置为原始模式，该模式下所有的输入数据以字节为单位被处理
     if (tcsetattr(fd, TCSANOW, &tm) < 0) {//设置上更改之后的设置
          return -1;
     }
     ch = getchar();
     if (tcsetattr(fd, TCSANOW, &tm_old) < 0) {//更改设置为最初的样子
          return -1;
     }
     return ch;
}

int _getche(void)  
{
    struct termios oldt, newt;
    int a;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    a = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return a;
}

int _kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt=oldt;
    newt.c_lflag&=~(ICANON|ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO,F_GETFL,0);
    fcntl(STDIN_FILENO,F_SETFL,oldf|O_NONBLOCK);
    ch=getchar();
    tcsetattr(STDIN_FILENO,TCSANOW,&oldt);
    fcntl(STDIN_FILENO,F_SETFL,oldf);
    if(ch!=EOF){
        ungetc(ch,stdin);
        return 1;
    }
    return 0;
}

#endif