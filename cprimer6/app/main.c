
#include "../include/hello.h"
#include "../include/mylog.h"

int main()
{
    initLogFile();

    hello();
    
    closeLogFile();

    return 0;
}