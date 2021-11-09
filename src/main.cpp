#include <iostream>
#include "logger_learn.hpp"

// 宏定义学习 变参学习
#if 0

#define MERGE(...)    test(__VA_ARGS__, #__VA_ARGS__)

void test(int a, int b, float c, const char* d){
    printf("a = %d, b= %d, c= %f, d = %s\n", a, b, c, d);
}

int test_ARGS(){
    // 打印为：a = 555, b= 2, c = 555, 2
    MERGE(555, 2, 1.19);
    return 0;
}
#endif

int main(){

    using namespace std;
    // 获取当前系统日期
    string strCurrentTime = iLogger::date_now();
    cout << "woaini" << endl;
    cout << strCurrentTime << endl;

    
    return 0;
}