
#ifndef ILOGGER_HPP
#define ILOGGER_HPP

#include <string>
#include <vector>
#include <tuple>
#include <time.h>    // C 风格的代码

#define ILOGGER_VERBOSE        4
#define ILOGGER_INFO           3
#define ILOGGER_WARNING        2
#define ILOGGER_ERROR          1
#define ILOGGER_FATAL          0
#define INFOV(...)       iLogger::__log_func(__FILE__, __LINE__, ILOGGER_VERBOSE, __VA_ARGS__)
#define INFO(...)        iLogger::__log_func(__FILE__, __LINE__, ILOGGER_INFO, __VA_ARGS__)
#define INFOW(...)       iLogger::__log_func(__FILE__, __LINE__, ILOGGER_WARNING, __VA_ARGS__)
#define INFOE(...)       iLogger::__log_func(__FILE__, __LINE__, ILOGGER_ERROR, __VA_ARGS__)
#define INFOF(...)       iLogger::__log_func(__FILE__, __LINE__, ILOGGER_FATAL, __VA_ARGS__)
 
namespace iLogger{
    using namespace std;

    // 时间操作
    string date_now();
    string time_now();
    string gmtime_now();
    string gmtime(time_t t);
    time_t gmtime2ctime(const string& gmt);
    void sleep(int ms);

}


#endif // ILOGGER_HPP
