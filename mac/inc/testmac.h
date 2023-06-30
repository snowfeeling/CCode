#include <stdio.h>
#include <stdbool.h>


#define ESC "\x1b"
#define CSI "\x1b["
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


int testmac();
