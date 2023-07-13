
#include "../inc/sayhello.h"
#include "../../base/inc/consolevt.h"

#define FANGK "■"

int init_main_screen();
int close_main_screen();

int init_oper_screeen();
int oper1();
bool mykbhit();



int SayHello()
{
    int ch;
    init_main_screen();
    printf("Hello World.\n");
    TURNOFF_CURSOR();
    do
    {
        ch = _getch();
        printf("(the input key :%d)\n", ch);

    } while (ch != ESCKEY);

    SWITCH_ALTERNATE_SCREEN();
    oper1();
    SWITCH_MAIN_SCREEN();
    TURNON_CURSOR();

    return 0;
}

int init_main_screen()
{
    enable_VT_Mode();
    set_console_CodePage(CODEPAGE_UTF8);

    SET_CONSOLE_TITLE("你好，新世界");
    return 0;
}
/*=======Close Main Screen ======*/
int close_main_screen()
{

    SWITCH_MAIN_SCREEN();
    return 0;
}

int oper1()
{

    int inputkey;
    bool istop = false;
    GO_SCREEN_HOME();
    TURNOFF_CURSOR();
    // CLEAR_SCREEN();
    printf("I am listening to the rain.");

    fflush(stdin);
    #if defined(__APPLE__)
    changemode(1);
    #endif

    do
    {
        int i = 0;
        
        while (!_kbhit())
        {
            cursor_jump(10, 16);
            printf(CSI "K"); // 清除本行光标之后的信息
            printf("Waitting for input:%6d", ++i);
        }
    
        printf(" AAAAAA [%d]", i);

        inputkey = _getch();
        
        if (inputkey == ADDITIONKEY || inputkey ==0) 
        {
            cursor_jump(10, 3);
            printf(CSI "K"); // 清除本行光标之后的信息
            printf("Function key:%3d - %3c", inputkey, inputkey);
            SLEEP(1000);
            inputkey = _getch();
            if (inputkey == 91)
            {
                cursor_jump(10, 3);
                printf(CSI "K"); // 清除本行光标之后的信息
                printf("Function key:%3d - %3c", inputkey, inputkey);
                SLEEP(1600);
                inputkey = _getch();
            }
        }
        cursor_jump(10, 3);
        printf(CSI "K"); // 清除本行光标之后的信息
        printf("Input key    :%3d - %3c", inputkey, inputkey);
        //SLEEP(600);

        cursor_jump(10, 5);
        printf(CSI "K"); // 清除本行光标之后的信息

        switch (inputkey)
        {
        case UPKEY:
            printf("UP");
            break;
        case DOWNKEY:
            printf("DOWN");
            break;
        case LEFTKEY:
            printf("LEFT");
            break;
        case RIGHTKEY:
            printf("RIGHT");
            break;
        case ESCKEY:
        case 'q':
            istop = true;
            break;
        default:
            break;
        }
    } while (!istop);
    cursor_jump(10, 15);
    printf("END.\n");

    return 0;
}
int init_oper_screen()
{
    CLEAR_SCREEN();
    GO_SCREEN_HOME();

    return 0;
}

