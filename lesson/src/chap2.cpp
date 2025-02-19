#include "../inc/chap2.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// testStatic
int testCount = 1;
int testStatic()
{
    cout << "\nThe call number : " << testCount << endl;

    static int numStatic;
    static char charStatic;
    cout << endl << "1. Static variable numStatic = " << numStatic << endl;
    
    numStatic = 222;
    cout << "2. Static variable numStatic = " << numStatic << endl;

    numStatic = 333;
    cout << "3. Static variable numStatic = " << numStatic << endl;

    testCount ++;
    return 0;
}

// 统计一个c文件中，'{'和'}'的的数量是否一样；
int testBracesMatch()
{
    string fileName("../src/chap2.cpp") ;
    ifstream fileBrace(fileName);
    if (!fileBrace) {
        cout<<  "无法打开文件:" << fileName << endl;
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
    cout << endl <<"Left  Braces Num = " <<leftBracesNum ;
    cout << endl <<"Right Braces Num = " << rightBracesNum << endl;
    if (leftBracesNum == rightBracesNum) {
        cout << endl <<"Left Braces Num = Right Braces Num." << endl;
    }
    else {
        cout << endl <<"Left Braces Num != Right Braces Num." << endl;
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
    cout <<str << "(" << orgNum << ") = " <<num << endl;
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

    // testStatic
    testStatic();

    testStatic();

    testStatic();

    return 0;
}
