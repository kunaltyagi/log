/**
 * @file log.h
 * @author Kunal Tyagi
 * @brief Provides simple logging features, fast, and robust
 * @copyright see LICENSE
 */


#ifndef _LOG_LOG_H_
#define _LOG_LOG_H_

#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <iomanip>

#define __CHECK_FOR_COLOR__ false
#define __COLOR__ true
#define TEXT_FINE_DEBUG 0

enum CsiCode
{
// general attributes
    ATTR_RESET        = 0,
    ATTR_TEXT_BOLD    = 1,
    ATTR_UNDERLINE    = 4,
    ATTR_BLINK        = 5,
    ATTR_REVERSE      = 7,
    ATTR_NO_UNDERLINE = 24,
    ATTR_NO_BLINK     = 25,
    ATTR_NO_REVERSE   = 27,
// foreground color
    FG_BLACK   = 30,
    FG_RED     = 31,
    FG_GREEN   = 32,
    FG_YELLOW  = 33,
    FG_BLUE    = 34,
    FG_MAGENTA = 35,
    FG_CYAN    = 36,
    FG_WHITE   = 37,
    FG_DEFAULT = 39,
// background color
    BG_BLACK   = 40,
    BG_RED     = 41,
    BG_GREEN   = 42,
    BG_YELLOW  = 43,
    BG_BLUE    = 44,
    BG_MAGENTA = 45,
    BG_CYAN    = 46,
    BG_WHITE   = 47,
    BG_DEFAULT = 49,
// decoration
    DEC_FRAMED       = 51,
    DEC_ENCIRCLED    = 52,
    DEC_OVERLINED    = 53,
    DEC_NO_FRAMED    = 54,
    DEC_NO_ENCIRCLED = 54,
    DEC_NO_OVERLINED = 55
};

int colorMap[5] = { FG_GREEN, FG_DEFAULT, FG_YELLOW, FG_RED, FG_RED };
int attributeMap[5] = { ATTR_RESET, ATTR_RESET, ATTR_RESET, ATTR_RESET, ATTR_TEXT_BOLD };

inline bool colorEnabled(bool color = __COLOR__)
{
#if __CHECK_FOR_COLOR__
    char *term = getenv("TERM");
    int erret = 0;
    if (setupterm(NULL, 1, &erret) == ERR)
    {
        return false;
    }
    return has_colors();
#else
    return color;
#endif
}

inline std::string getNormalHeader(std::string fullNamePath, int length = 4)
{
    assert(length > -1);

    std::vector<std::string> path;
    std::stringstream stream;
    stream << fullNamePath;
    std::string str;
    while (getline(stream, str, '/'))
    {
        path.push_back(str);
    }
    str = "";
    std::vector<std::string> prefix, postfix;
    prefix.push_back("<");
    prefix.push_back("(");
    prefix.push_back("[");
    postfix.push_back(">");
    postfix.push_back(")");
    postfix.push_back("]");
    for (int i = length; i != 0; --i)
    {
        if ("src" == path[path.size() - i] || "include" == path[path.size() - i])
        {
            continue;
        }
        if (path[path.size() - i].find("auv") != std::string::npos)
        {
            path[path.size() - i] = path[path.size() - i].substr(4, int(path[path.size() - i].size() - 4));
        }
        str += prefix.back() + path[path.size() - i] + postfix.back();
        prefix.pop_back();
        postfix.pop_back();
        std::transform(str.begin(), str.end(),str.begin(), ::toupper);
    }
    return str;
}

inline std::string getDebugHeader(std::string filepath, std::string function, int line)
{
    return getNormalHeader(filepath) + " {Function: " + function +
           "() Line: " + std::to_string(line) + "}";
}

inline std::string getHeader(std::string filepath, std::string function, int line, int fineDebug = -1)
{
    fineDebug = fineDebug == -1 ? TEXT_FINE_DEBUG : fineDebug;
    return (fineDebug != 0) ? getDebugHeader(filepath, function, line) : getNormalHeader(filepath);
}

char *itoa (int value, char *result, int base = 10)
{
    // check that the base if valid
    if (base < 2 || base > 36) { *result = '\0'; return result; }

    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}

#define __DEBUG__ 0
#define __INFO__  1
#define __WARN__  2
#define __ERROR__ 3
#define __FATAL__ 4

#define __SECURE__(X) do { X; } while(false)

#define MACRO_NAME(X) #X
#define MACRO_VALUE(X) X

#define SECONDS() (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count())
#define NANO_SECONDS() (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count()%1000000000)

#define __CSI_SINGLE_CODE__(X) __SECURE__(printf("\033[%dm", X))
#define __CSI_SINGLE_CODE_STREAM__(X) __SECURE__(std::cout << "\033[" << X << "m")

#define __CSI_CODE__(X, format, ...) __SECURE__(printf("\033[%dm" format "\033[%dm\n", X, ##__VA_ARGS__, ATTR_RESET))
#define __CSI_CODE_STREAM__(X, args) __SECURE__(std::cout << "\033[" << X << "m" << args << "\033[" << ATTR_RESET << "m" << std::endl)

#define LOG(LEVEL, VERBOSE, format, ...) __SECURE__(__CSI_SINGLE_CODE__(attributeMap[__##LEVEL##__]); __CSI_CODE__(colorMap[__##LEVEL##__], "[%5s ] [%11lu:%9lu] %s " format, #LEVEL, SECONDS(), NANO_SECONDS(), getHeader(__BASE_FILE__, __FUNCTION__, __LINE__, VERBOSE).c_str(), ##__VA_ARGS__))
#define LOG_STREAM(LEVEL, VERBOSE, args) __SECURE__(__CSI_SINGLE_CODE_STREAM__(attributeMap[__##LEVEL##__]); __CSI_CODE_STREAM__(colorMap[__##LEVEL##__], "[" << std::setw(5) << #LEVEL << " ] [" << std::setw(11) << SECONDS() << ":" << std::setw(9) << NANO_SECONDS() << "] " << getHeader(__BASE_FILE__, __FUNCTION__, __LINE__, VERBOSE) << " " << args))

#define LOG_DEBUG(format, ...)  __SECURE__(LOG(DEBUG, 0, format, ##__VA_ARGS__))
#define LOG_INFO(format, ...)   __SECURE__(LOG(INFO,  0, format, ##__VA_ARGS__))
#define LOG_WARN(format, ...)   __SECURE__(LOG(WARN,  0, format, ##__VA_ARGS__))
#define LOG_ERROR(format, ...)  __SECURE__(LOG(ERROR, 0, format, ##__VA_ARGS__))
#define LOG_FATAL(format, ...)  __SECURE__(LOG(FATAL, 0, format, ##__VA_ARGS__))

#define LOG_DEBUG_STREAM(args)  __SECURE__(LOG_STREAM(DEBUG, 0, args))
#define LOG_INFO_STREAM(args)   __SECURE__(LOG_STREAM(INFO,  0, args))
#define LOG_WARN_STREAM(args)   __SECURE__(LOG_STREAM(WARN,  0, args))
#define LOG_ERROR_STREAM(args)  __SECURE__(LOG_STREAM(ERROR, 0, args))
#define LOG_FATAL_STREAM(args)  __SECURE__(LOG_STREAM(FATAL, 0, args))

#define LOG_DEBUG_FINE(format, ...)  __SECURE__(LOG(DEBUG, 1, format, ##__VA_ARGS__))
#define LOG_INFO_FINE(format, ...)   __SECURE__(LOG(INFO,  1, format, ##__VA_ARGS__))
#define LOG_WARN_FINE(format, ...)   __SECURE__(LOG(WARN,  1, format, ##__VA_ARGS__))
#define LOG_ERROR_FINE(format, ...)  __SECURE__(LOG(ERROR, 1, format, ##__VA_ARGS__))
#define LOG_FATAL_FINE(format, ...)  __SECURE__(LOG(FATAL, 1, format, ##__VA_ARGS__))

#define LOG_DEBUG_STREAM_FINE(args)  __SECURE__(LOG_STREAM(DEBUG, 1, args))
#define LOG_INFO_STREAM_FINE(args)   __SECURE__(LOG_STREAM(INFO,  1, args))
#define LOG_WARN_STREAM_FINE(args)   __SECURE__(LOG_STREAM(WARN,  1, args))
#define LOG_ERROR_STREAM_FINE(args)  __SECURE__(LOG_STREAM(ERROR, 1, args))
#define LOG_FATAL_STREAM_FINE(args)  __SECURE__(LOG_STREAM(FATAL, 1, args))

//#define LOG_ONCE(LEVEL, format, ...)
//#define LOG_STREAM_ONCE(LEVEL, format, ...)
#define LOG_COND(LEVEL, cond, format, ...) __SECURE__(if(cond) LOG(LEVEL, 0, format, ##__VA_ARGS__))
#define LOG_STREAM_COND(LEVEL, cond, args) __SECURE__(if(cond) LOG_STREAM(LEVEL, 0, args))
#define LOG_COND_FINE(LEVEL, cond, format, ...) __SECURE__(if(cond) LOG(LEVEL, 1, format, ##__VA_ARGS__))
#define LOG_STREAM_COND_FINE(LEVEL, cond, args) __SECURE__(if(cond) LOG_STREAM(LEVEL, 1, args))

#endif  // _LOG_LOG_H_
