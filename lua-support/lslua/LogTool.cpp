//////////////////////////////////////////////////////////////////////
/// Desc  LogTool
/// Time  2022/03/09
/// Author youlanhai
/// Home   https://github.com/youlanhai/cpp-bindings
//////////////////////////////////////////////////////////////////////
#include "LogTool.hpp"

#include <cstdio>
#include <cstdarg>

static const char * LogLevelString[] = {
    "VERBOSE",
    "DEBUG",
    "INFO",
    "NOTICE",
    "WARN",
    "ERROR",
    "EXCEPTION",
    "FATAL"
};

const char* ls_getLogLevelString(int level)
{
    return LogLevelString[level];
}

LS_API void ls_logFormat(int moduleLvl, int logLevel, const char * tag, const char *format, ...)
{
    if(logLevel < moduleLvl)
    {
        return;
    }

    printf("[%s]", ls_getLogLevelString(logLevel));

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    printf("\n");
}
