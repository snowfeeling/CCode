
#include "../inc/consolevt.h"
#include "../inc/sayhello.h"

#define FANGK "■"

int init_main_screen();
int close_main_screen();

int init_oper_screeen();
int oper1();


int SayHello()
{
    int ch;
    init_main_screen();
    printf("Hello World.");
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
    set_console_CodePage();

    SET_CONSOLE_TITLE("你好，新世界");
    return 0;
}
/*=======Close Main Screen ======*/
int close_main_screen()
{


    return 0;
}


int oper1()
{

    int inputkey;
    bool istop = false;
    TURNOFF_CURSOR();
    printf("\nI am listening to the rain.");
    printf("\n" FANGK "\n");

    do
    {
        inputkey = _getch();
        if (inputkey == ADDITIONKEY) inputkey = _getch();
        cursor_jump(10,3);
        printf(CSI "K");  //清除本行光标之后的信息
        printf("(the input key:%d)\n", inputkey);
    
        cursor_jump(10,5);
        printf(CSI "K");  //清除本行光标之后的信息

        switch (inputkey)
        {
        case UPKEY:
            printf("UP");
            /* code */
            break;
        case DOWNKEY:
            printf("DOWN");
            /* code */
            break;
        case LEFTKEY:
            printf("LEFT");
            /* code */
            break;
        case RIGHTKEY:
            printf("RIGHT");
            /* code */
            break;
        case ESCKEY:
            istop = false;
            break;
        case 'q':
            istop = true;
            break;
        //default:
            break;
        }
    } while (!istop );
    
    return 0;
}
int init_oper_screen()
{
    CLEAR_SCREEN();
    GO_SCREEN_HOME();

    return 0;
}


