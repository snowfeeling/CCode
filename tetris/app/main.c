#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#ifdef WIN32
#include <conio.h> /*键盘输入获取*/
#endif

#include "../inc/mylog.h"
#include "../inc/rb.h"

int main ()
{

    mytetris();

    return 0;
}