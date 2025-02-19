#include "../inc/chap2.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int testBracesMatch()
{
    char fileName[] ="../src/chap2.cpp" ;
    ifstream fileBrace(fileName);
    if (!fileBrace) {
        printf ( "无法打开文件:%s \n",  fileName);
        return false;
    }
    int leftBracesNum=0, rightBracesNum = 0;

    char ch;
    while (fileBrace.get(ch)) {
        if (ch == '{') {
            leftBracesNum ++;
        } else if (ch == '}') {
            rightBracesNum ++;
        }
    }
    
    fileBrace.close();
    printf("\nLeft  Braces Num = %d \nRight Brace Num=%d\n", leftBracesNum, rightBracesNum);
    if (leftBracesNum == rightBracesNum) {
        printf("\nLeft Braces Num = Right Braces Num.\n");
    }
    else {
        printf("\nLeft Braces Num != Right Braces Num.\n");
    }
    
    return (leftBracesNum == rightBracesNum);
}

// increment函数实现参数加1
int testCalculator::increment(int num) {
    return num + 1;
}

// negate函数实现取参数负值
int testCalculator::negate(int num) {
    return -num;
}

int showNumChanged(char *str, int orgNum, int num) {
    printf("%s(%d) = %d\n", str, orgNum, num);
    return 0;
}

int testCalc()
{
    testCalculator calc;
    int num;
    num = 10;
    showNumChanged((char *)"Increment", num, calc.increment(num));
    num = 0;
    showNumChanged((char *)"Negate", num, calc.negate(num));
    num = 100;
    showNumChanged((char *)"Increment", num, calc.increment(num));
    num = -10;
    showNumChanged((char *)"Negate", num, calc.negate(num));
    
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

    //test 2.8-1 using class;
    testCalc();
    
    // Test 2.8-2;
    testBracesMatch();

    return 0;
}
