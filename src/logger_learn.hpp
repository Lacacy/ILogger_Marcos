
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

    long long timestamp_now();
    double timestamp_now_float();
    time_t last_modify(const string& file);

    // ... 貌似是 C++ 的新特性，写完hpp去搜索确认一下
    string format(const char* fmt, ...);

    // 文件操作
    bool isfile(const string& file);
    bool mkdir(const string& path);
    bool mkdirs(const string& path);
    bool delete_file(const string& path);
    bool rmtree(const string& directory, bool ignore_fail=false);
    bool exists(const string& path);
    
    FILE* fopen_mkdirs(const string& path, const string& mode);
    string directory(const string& path);
    string file_name(const string& path, bool include_suffix);

    

    vector<uint8_t> load_file(const string& file);
    string load_text_file(const string& file);
    size_t file_size(const string& file);


    bool begin_with(const string& str, const string& with);
    bool end_with(const string& str, const string& with);
    vector<string> split_string(const string& str, const string& spstr);
    string replace_string(const string& str, const string& token, int nreplace=-1, int* out_num_replace=nullptr);

    // h[0-1], s[0-1], v[0-1]
    // return, 0-255, 0-255, 0-255
    tuple<uint8_t, uint8_t, uint8_t> hsv2rgb(float h, float s, float v);
    tuple<uint8_t, uint8_t, uint8_t> random_color(int id);

    // abcdefg.pnga     *.png       > false
    // abcdefg.png      *.png       > true
    // abcdefg.png      a?cdefg.png       > true
    bool pattern_match(const char* str, const char* matcher, bool ignore_core=true);
    vector<string> find_files(
        const string& directory,
        const string& filter = "*",
        bool findDirectory = false, 
        bool includeSubDirectory = false
    );
    string align_blank(const string& input, int align_size, char blank=' ');

    // 保存文件
    // 输入: 要保存文件的名字 file, vector 原始数据, 是否根据 file 创建文件夹  
    bool save_file(const string& file, const vector<uint8_t>& data, bool mk_dirs = true);
    // 输入: 要保存文件的名字 file,  原始数据 data , 是否根据 file 创建文件夹  
    bool save_file(const string& file, const string& data, bool mk_dirs = true);
    // 输入: 要保存文件的名字 file,  原始数据 data , 保存数据长度, 是否根据 file 创建文件夹
    bool save_file(const string& file, const void* data, size_t length, bool mk_dirs = true);

    // 循环等待，并捕获例如ctrl+c等终止信号，收到信号后循环跳出并返回信号类型
    // 捕获：SIGINT(2)、SIGQUIT(3)
    int while_loop();

    // 关于 logger 的 API
    const char* level_string(int level);
    void set_logger_save_directory(const string& loggerDirectory);

    // 当日志的级别低于这个设置时， 会打印出来, 否则会 直接跳过
    void set_log_level(int level);
    int get_log_level();
    void __log_func(const char* file, int line, int level, const char* fmt, ...);
    void destory_logger();

/**
转码过程例子：
3*8=4*6
内存1个字节占8位
转前： s 1 3
先转成ascii：对应 115 49 51
2进制： 01110011 00110001 00110011
6个一组（4组） 011100110011000100110011
然后才有后面的 011100 110011 000100 110011
然后计算机一个字节占8位，不够就自动补两个高位0了
所以有了高位补0
科学计算器输入 00011100 00110011 00000100 00110011
得到 28 51 4 51
查对下照表 c z E z
 */
    string base64_decode();
    string base64_encode();

    string get_random_temp_file_name();


}


#endif // ILOGGER_HPP
