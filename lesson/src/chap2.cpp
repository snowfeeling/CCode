#include "../inc/chap2.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <iomanip>

using namespace std;

// get Sqrt(n)
int getSqrt(int num)
{
    double guess = 1;
    double lastGuess;
    double tolerance = 1e-10;
    int i =0;

    do
    {
        lastGuess = guess;
        guess = (guess+ num/guess)/2;
        i++;
    } while (abs(guess-lastGuess) > tolerance);
    
    //if (abs(guess-lastGuess) <= tolerance) 
    cout << "Computing times: " << i << endl;
        
    cout << fixed << setprecision(10);    
    cout << guess << endl; 
    printf("%.20f \n||\n%.20f\n", lastGuess, guess);
    return 0;
}

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
    printf("\"{」Blunder\?\?!\??\"\n");

    printf("\100 \n");

    //test 2.7 - No 6:
    int Case, If, While, Stop, stop;

    //test 2.8-1 using class;
    testCalc();
    
    // Test 2.8-2;
    testBracesMatch();

    // test Static variable
    //testStatic();

    // Test expression
    getSqrt(99);

    int a =4, b;
    b = a ++;
    int c = a + (b <= 4);
    cout  << "a="<<a << "  b="<< b << " c="<< c << endl;

    int d = a * b++;
    cout<< "d="<<d << "  b=" << b<< endl;
    return 0;
}
