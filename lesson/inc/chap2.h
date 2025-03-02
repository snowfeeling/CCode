#ifndef LESSON_CHAP2_H
#define LESSON_CHAP2_H
#include <stdio.h>


int testBracesMatch();


class testCalculator {
    public:
        // increment函数实现参数加1
        int increment(int num) ;
        // negate函数实现取参数负值
        int negate(int num) ;
    };

int testChap2();
void printArray(int *arr, int num);


#endif