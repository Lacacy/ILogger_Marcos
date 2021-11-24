
#if defined(_WIN32)
#   define U_OS_WINDOWS
#else 
#   define U_OS_LINUX
#endif

#include "logger_learn.hpp"
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <string>
#include <mutex>
#include <memory>
#include <vector>
#include <thread>
#include <atomic>
#include <fstream>
#include <stack>
#include <signal.h>
#include <functional>
#include <sys/syscall.h>

#if defined(U_OS_WINDOWS)
#	define HAS_UUID
#	include <Windows.h>
#   include <wingdi.h>
#	include <Shlwapi.h>
#	pragma comment(lib, "shlwapi.lib")
#   pragma comment(lib, "ole32.lib")
#   pragma comment(lib, "gdi32.lib")
#	undef min
#	undef max
#endif

#if defined(U_OS_LINUX)
#   include <dirent.h>
#   include <sys/types.h>
#   include <sys/stat.h>    // stat 蕴含了丰富的文件属性
#   include <unistd.h>
#   include <stdarg.h>
#   define strtok_s strtok_r
#endif


#if defined(U_OS_LINUX)
#define __GetTimeBlock  \
    time_t timep;       \
    time(&timep);       \
    tm& t = *(tm*)localtime(&timep);
#endif

#if defined(U_OS_WINDOWS)
#define __GetTimeBlock  \
    tm t;               \
    _getsystime(&t);
#endif

namespace iLogger{
    using namespace std;

    const char* level_string(int level){
        switch (level){
        case ILOGGER_VERBOSE: return "verbo";
        case ILOGGER_INFO: return "info";
        case ILOGGER_WARNING: return "warn";
        case ILOGGER_ERROR: return "error";
        case ILOGGER_FATAL: return "fatal";
        default: return "unknow";
        }
    }

// 将每一个像素点的值从 hsv 转为 rgb
    std::tuple<uint8_t, uint8_t, uint8_t> hsv2rgb(float h, float s, float v){
        const int h_i = static_cast<int>(h * 6);
        const float f = h * 6 - h_i;
        const float p = v * (1 - s);
        const float q = v * (1 - f*s);
        const float t = v * (1 - (1 - f) * s);
        float r, g, b;
        switch (h_i) {
        case 0:r = v; g = t; b = p;break;
        case 1:r = q; g = v; b = p;break;
        case 2:r = p; g = v; b = t;break;
        case 3:r = p; g = q; b = v;break;
        case 4:r = t; g = p; b = v;break;
        case 5:r = v; g = p; b = q;break;
        default:r = 1; g = 1; b = 1;break;}
        return make_tuple(static_cast<uint8_t>(r * 255), static_cast<uint8_t>(g * 255), static_cast<uint8_t>(b * 255));
    }

// 随机生成 颜色
    std::tuple<uint8_t, uint8_t, uint8_t> random_color(int id){
        float h_plane = ((((unsigned int)id << 2) ^ 0x937151) % 100) / 100.0f;;
        float s_plane = ((((unsigned int)id << 3) ^ 0x315793) % 100) / 100.0f;
        return hsv2rgb(h_plane, s_plane, 1);
    }
    
// 根据宏定义以及time.h 中的对象计算当前日期
// - time_t timep;  // 创建time_t为 timep;   
// - time(&timep);  // 传入数据类型为 &timep; 形参为指针变量, 实参为地址 
// - tm& t = *(tm*)localtime(&timep);  // 返回一个结构体对象; 存储当前时间信息

// - 返回一个C风格给字符串 string对象;
// - 获取当前的日期 年月日
    string date_now()
    {
        char time_string[20];
        __GetTimeBlock;

        sprintf(time_string, "%04d-%02d-%02d", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);
        return time_string;
    }

// - 获取当前的时间 年月日时分秒
    string time_now(){
        char time_string[20];
        __GetTimeBlock;

        sprintf(time_string, "%04d-%02d-%02d %02d:%02d:%02d", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
        return time_string;
    }

// - 返回文件长度
    size_t file_size(const string& file){
#if defined(U_OS_LINUX)
        struct stat st;
        stat(file.c_str(), &st);
        return st.st_size;
#elif defined(U_OS_WINDOWS)
        WIN32_FIND_DATAA find_data;
        HANDLE hFind = FindFirstFileA(file.c_str(), &find_data);
        if (hFind == INVALID_HANDLE_VALUE)
            return 0;

        FindClose(hFind);
        return (uint64_t)find_data.nFileSizeLow | ((uint64_t)find_data.nFileSizeHigh << 32);
#endif
        }

// -  获得最近一次改动的文件的日期
    time_t last_modify(const string& file){

#if defined(U_OS_LINUX)
        struct stat st;
        stat(file.c_str(), &st);
        return st.st_mtim.tv_sec;
#elif defined(U_OS_WINDOWS)
        INFOW("Last Modify has not support on windows os.");
        return 0;
#endif
    }

// - 程序睡眠
    void sleep(int ms){
        this_thread::sleep_for(std::chrono::milliseconds(ms));
    }

// 通过 名字 得到月份的索引
    int get_month_by_name(char* month){
        if(strcmp(month,"Jan") == 0)return 0;
        if(strcmp(month,"Feb") == 0)return 1;
        if(strcmp(month,"Mar") == 0)return 2;
        if(strcmp(month,"Apr") == 0)return 3;
        if(strcmp(month,"May") == 0)return 4;
        if(strcmp(month,"Jun") == 0)return 5;
        if(strcmp(month,"Jul") == 0)return 6;
        if(strcmp(month,"Aug") == 0)return 7;
        if(strcmp(month,"Sep") == 0)return 8;
        if(strcmp(month,"Oct") == 0)return 9;
        if(strcmp(month,"Nov") == 0)return 10;
        if(strcmp(month,"Dec") == 0)return 11;
        return -1;
    }

// 通过名字得到是 周几 的索引
    int get_week_day_by_name(char* wday){
        if(strcmp(wday,"Sun") == 0)return 0;
        if(strcmp(wday,"Mon") == 0)return 1;
        if(strcmp(wday,"Tue") == 0)return 2;
        if(strcmp(wday,"Wed") == 0)return 3;
        if(strcmp(wday,"Thu") == 0)return 4;
        if(strcmp(wday,"Fri") == 0)return 5;
        if(strcmp(wday,"Sat") == 0)return 6;
        return -1;
    }

// 未知
    time_t gmtime2ctime(const string& gmt){

        char week[4] = {0};
        char month[4] = {0};
        tm date;
        sscanf(gmt.c_str(),"%3s, %2d %3s %4d %2d:%2d:%2d GMT",week,&date.tm_mday,month,&date.tm_year,&date.tm_hour,&date.tm_min,&date.tm_sec);
        date.tm_mon = get_month_by_name(month);
        date.tm_wday = get_week_day_by_name(week);
        date.tm_year = date.tm_year - 1900;
        return mktime(&date);
    }

// 未知
    string gmtime(time_t t){
        t += 28800;
        tm* gmt = ::gmtime(&t);

        // http://en.cppreference.com/w/c/chrono/strftime
        // e.g.: Sat, 22 Aug 2015 11:48:50 GMT
        //       5+   3+4+   5+   9+       3   = 29
        const char* fmt = "%a, %d %b %Y %H:%M:%S GMT";
        char tstr[30];
        strftime(tstr, sizeof(tstr), fmt, gmt);
        return tstr;
    }

// 未知
    string gmtime_now() {
        return gmtime(time(nullptr));
    }

// 创建文件夹
    bool mkdir(const string& path){
#ifdef U_OS_WINDOWS
        return CreateDirectoryA(path.c_str(), nullptr);
#else
// 疑问: ::的作用
        return ::mkdir(path.c_str(), 0755) == 0;
#endif
    }

// 根据路径递归创建文件夹
    bool mkdirs(const string& path){

        if (path.empty()) return false;
        if (exists(path)) return true;

        string _path = path;
        char* dir_ptr = (char*)_path.c_str();
        char* iter_ptr = dir_ptr;
        
        // 路径名字是否为 空， 来判断是否继续创建子文件夹
        bool keep_going = *iter_ptr not_eq 0;
        while (keep_going){

            if (*iter_ptr == 0)
                keep_going = false;

#ifdef U_OS_WINDOWS
            if (*iter_ptr == '/' or *iter_ptr == '\\' or *iter_ptr == 0){
#else
            if ((*iter_ptr == '/' and iter_ptr not_eq dir_ptr) or *iter_ptr == 0){
#endif
                char old = *iter_ptr;
                *iter_ptr = 0;
                if (!exists(dir_ptr)){
                    if (!mkdir(dir_ptr)){
                        if(!exists(dir_ptr)){
                            INFOE("mkdirs %s return false.", dir_ptr);
                            return false;
                        }
                    }
                }
                *iter_ptr = old;
            }
            iter_ptr++;
        }
        return true;
    }

// - 是否是一个文件
    bool isfile(const string& file){

#if defined(U_OS_LINUX)
        struct stat st;
        stat(file.c_str(), &st);
        return S_ISREG(st.st_mode);
#elif defined(U_OS_WINDOWS)
        INFOW("is_file has not support on windows os");
        return 0;
#endif
    }


// 未知
    FILE* fopen_mkdirs(const string& path, const string& mode){

        FILE* f = fopen(path.c_str(), mode.c_str());
        if (f) return f;

        int p = path.rfind('/');

#if defined(U_OS_WINDOWS)
        int e = path.rfind('\\');
        p = std::max(p, e);
#endif
        if (p == -1)
            return nullptr;
        
        string directory = path.substr(0, p);
        if (!mkdirs(directory))
            return nullptr;

        return fopen(path.c_str(), mode.c_str());
    }

// 路径所对应的文件是否存在
    bool exists(const string& path){

#ifdef U_OS_WINDOWS
        return ::PathFileExistsA(path.c_str());
#elif defined(U_OS_LINUX)
        return access(path.c_str(), R_OK) == 0;
#endif
    }

//  返回指定格式的字符串
    string format(const char* fmt, ...) {//todo 
        va_list vl;
        va_start(vl, fmt);
        char buffer[2048];
        vsnprintf(buffer, sizeof(buffer), fmt, vl);
        return buffer;
    }

// 未知
    string file_name(const string& path, bool include_suffix){

        if (path.empty()) return "";

        int p = path.rfind('/');

#ifdef U_OS_WINDOWS
        int e = path.rfind('\\');
        p = max(p, e);
#endif
        p += 1;

        //include suffix
        if (include_suffix)
            return path.substr(p);

        int u = path.rfind('.');
        if (u == -1)
            return path.substr(p);

        if (u <= p) u = path.size();
        return path.substr(p, u - p);
    }

// 未知
    string directory(const string& path){
        if (path.empty())
            return ".";

        int p = path.rfind('/');

#ifdef U_OS_WINDOWS
        int e = path.rfind('\\');
        p = max(p, e);
#endif
        if(p == -1)
            return ".";

        return path.substr(0, p + 1);
    }

// 字符串以什么为 起始; 成功返回 0
    bool begin_with(const string& str, const string& with){

        if (str.length() < with.length())
            return false;
        return strncmp(str.c_str(), with.c_str(), with.length()) == 0;
    }

// 字符串以什么为 结束; 成功返回 0
    bool end_with(const string& str, const string& with){

        if (str.length() < with.length())
            return false;

        return strncmp(str.c_str() + str.length() - with.length(), with.c_str(), with.length()) == 0;
    }

// 获得现在的时间
    long long timestamp_now() {
        return chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
    }

// 获得现在的时间 : float
    double timestamp_now_float() {
        return chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count() / 1000.0;
    }



























} // namespace iLogger
