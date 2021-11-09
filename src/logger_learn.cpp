
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
#   include <sys/stat.h>
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
    
string date_now()
{
    char time_string[20];
    __GetTimeBlock;

    sprintf(time_string, "%04d-%02d-%02d", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);
    return time_string;
}

}
