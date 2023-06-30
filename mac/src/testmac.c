
#include "../inc/testmac.h"

int testmac()
{
    SWITCH_ALTERNATE_SCREEN();
    GO_SCREEN_HOME();
    CLEAR_SCREEN();

    printf("Test the terminal mode" );
    //Sleep(1200);
    
    //SWITCH_MAIN_SCREEN();

    return 0;
}