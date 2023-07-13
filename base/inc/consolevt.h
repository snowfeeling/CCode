#ifndef _CONSOLE_VT_H_
#define _CONSOLE_VT_H_

#include <stdio.h>
#include <stdbool.h>
#include <locale.h>
#include <unistd.h>
#include <fcntl.h>

#ifdef WIN32
    #include <windows.h>
    #include <conio.h>
#endif

#if defined(__APPLE__)
    #include <TargetConditionals.h>
    #include <termios.h>
#endif

/*======= CodePage      ======*/
#define CODEPAGE_UTF8 65001

/*======= 虚拟终端定义  =======*/
#define ESC "\x1b"
#define CSI "\x1b["
#define OSC "\x1b]"


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
#define SET_CONSOLE_TITLE(str) { printf (ESC "]2;%s\x07", str); }
//从当前位置清除到行尾巴
#define CLEAN_LINE_TO_END { printf(CSI "K"); }

/*====== 虚拟终端函数定义 ======*/
//设置屏幕为虚拟终端模式
bool enable_VT_Mode();
int set_console_CodePage(int codepage);

#if defined(__APPLE__)
int _getch(void);
int _getche(void);
int _kbhit(void);
void changemode(int dir);

#endif


/*=======Keys Ddefine =========
*/
#if defined(WIN32)
    #define UPKEY	    72 //方向键：上
    #define DOWNKEY 	80 //方向键：下
    #define LEFTKEY 	75 //方向键：左
    #define RIGHTKEY 	77 //方向键：右
    #define ADDITIONKEY  224 //功能健的前8位
#endif

#if defined(__APPLE__)
    #define UPKEY	    65 //方向键：上A
    #define DOWNKEY 	66 //方向键：下B
    #define LEFTKEY 	68 //方向键：左C
    #define RIGHTKEY 	67 //方向键：右D
    #define ADDITIONKEY  27 //功能健的前8位
#endif

#define SPACEKEY    32 //空格键
#define ESCKEY	    27 //Esc键


#if defined(_WIN32)
    //code for Windows (32-bit and 64-bit, this part is common)
    #define CHECKKEY _kbhit()
    #define NBGETCHAR _getch()
    #define SLEEP(t) Sleep(t)

#elif defined(__APPLE__)
    //code for mac
    #define CHECKKEY _kbhit()
    #define NBGETCHAR _getch()
    #define SLEEP(t)    sleep(t/600)

#elif defined(__linux__)
    //code for linux
    #define CHECKKEY  _kbhit()
    #define NBGETCHAR _getch()
    #define SLEEP(t)    sleep(t/600)

#else
#   error "Unknown compiler"
#endif

#endif