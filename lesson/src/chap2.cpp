#include "../inc/chap2.h"

// increment函数实现参数加1
int Calculator::increment(int num) {
    return num + 1;
}

// negate函数实现取参数负值
int Calculator::negate(int num) {
    return -num;
}

int showNumChanged(char *str, int orgNum, int num) {
    printf("%s(%d) = %d\n", str, orgNum, num);
    return 0;
}

int testCalc()
{
    Calculator calc;
    int num;
    num = 10;
    showNumChanged("Increment", num, calc.increment(num));
    num = 0;
    showNumChanged("Negate", num, calc.negate(num));
    num = 100;
    showNumChanged("Increment", num, calc.increment(num));
    num = -10;
    showNumChanged("Negate", num, calc.negate(num));
    
    return 0;
}

// 验证程序
int testChap2()
{
    int xy/*Blald Bald */;
    printf("\"Blunder\?\?!\??\"\n");

    printf("\100 \n");

    //test 2.7 - No 6:
    int Case, If, While, Stop, stop;

    //test 2.8 using class;
    testCalc();

    return 0;
}
