/* Trtris.c
    Created by Wangss on 2023-06-19.

*/
// System headers
#include <windows.h>

// Standard library C-style
#include <wchar.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "../inc/tetris.h"

#define ESC "\x1b"
#define CSI "\x1b["

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
        return false;
    }

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode))
    {
        return false;
    }
    return true;
}

static void show_row_num(COORD Size)
{
    

    for( int i = 0; i<Size.Y; i++)
    {
        printf(CSI "%d;1H" "%2d", i+1, i+1);
    }
}

static void print_V_border()
{
    printf(ESC "(0"); // Enter Line drawing mode
    printf(CSI "104;93m"); // bright yellow on bright blue
    printf("x"); // in line drawing mode, \x78 -> \u2502 "Vertical Bar"
    printf(CSI "0m"); // restore color
    printf(ESC "(B"); // exit line drawing mode
}

static void print_H_border(COORD const Size, bool fIsTop)
{
    printf(ESC "(0"); // Enter Line drawing mode
    printf(CSI "104;93m"); // Make the border bright yellow on bright blue
    printf(fIsTop ? "l" : "m"); // print left corner 

    for (int i = 1; i < Size.X - 1; i++)
        printf("q"); // in line drawing mode, \x71 -> \u2500 "HORIZONTAL SCAN LINE-5"

    printf(fIsTop ? "k" : "j"); // print right corner
    printf(CSI "0m");
    printf(ESC "(B"); // exit line drawing mode
}


static int init_screen()
{
    bool fSuccess = enable_VT_Mode();
    if (!fSuccess)
    {
        printf("Unable to enter VT processing mode. Quitting.\n");
        return -1;
    }
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
    {
        printf("Couldn't get the console handle. Quitting.\n");
        return -1;
    }

    CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo;
    GetConsoleScreenBufferInfo(hOut, &ScreenBufferInfo);
    COORD Size;
    Size.X = ScreenBufferInfo.srWindow.Right - ScreenBufferInfo.srWindow.Left + 1;
    Size.Y = ScreenBufferInfo.srWindow.Bottom - ScreenBufferInfo.srWindow.Top + 1;

    // Enter the alternate buffer
    printf(CSI "?1049h");

    // Clear screen
    printf(CSI "1;1H");
    printf(CSI "2J"); // 清除屏幕；

    printf(CSI "?25l"); // 关闭光标
    
//    show_row_num(Size);

    return 0;
}

static int close_screen()
{
    // Exit the alternate buffer
    
    //printf(CSI "?1049l");
    printf(CSI "?25h"); //打开光标

}

static int show_V_block(COORD c, int H_size)
{
    if (H_size < 1 || H_size >4)
    {
        return -1;
    }
    
    printf(CSI "%d;%dH", c.Y, c.X);
    printf(CSI "K"); // clear the line

    printf(ESC "(0"); // Enter Line drawing mode
    //printf(CSI "104;93m"); // bright yellow on bright blue
    printf("lk");    

    for (int i = 1; i < H_size ; i ++)
    {
        printf(CSI "%d;%dH", c.Y+i, c.X);
        printf("tu"); 
    }

    printf(CSI "%d;%dH", c.Y+H_size, c.X);
    printf("mj");

    printf(CSI "0m"); // restore color
    printf(ESC "(B"); // exit line drawing mode

    Sleep(1200);

}

static int show_H_block(COORD c, int H_size)
{
    if (H_size < 1 || H_size >4)
    {
        return -1;
    }
    printf(CSI "%d;%dH", c.Y, c.X);
    printf(CSI "K"); // clear the line

    printf(ESC "(0"); // Enter Line drawing mode
    //printf(CSI "104;93m"); // bright yellow on bright blue
    printf("l");    
    printf(CSI "%d;%dH", c.Y+1, c.X);
    printf("m");    

    for (int i = 1; i < H_size ; i ++)
    {
        printf(CSI "%d;%dH", c.Y, c.X+i);
        printf("w"); 
        printf(CSI "%d;%dH", c.Y+1, c.X+i);
        printf("v"); 
    }

    printf(CSI "%d;%dH", c.Y, c.X+H_size);
    printf("k");
    printf(CSI "%d;%dH", c.Y+1, c.X+H_size);
    printf("j");

    printf(CSI "0m"); // restore color
    printf(ESC "(B"); // exit line drawing mode

    Sleep(1200);    

}


int tetris()
{
    init_screen();
    COORD c;
    c.X = 0;
    c.Y = 1;

    show_V_block( c, 4 );

    for (int j=0; j<4; j++)
    {
        printf(CSI "%d;%dH" CSI "K", c.Y, c.X);
        c.Y ++;
        show_V_block( c, 4 );
    }
   
    printf(ESC "(0"); // Enter Line drawing mode

    printf("■");
    printf(CSI "0m");
    printf(ESC "(B"); // exit line drawing mode

    char wch =  _getwch();

    close_screen();
    return 0;

}