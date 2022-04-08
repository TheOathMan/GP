#include "AppCore.h"
#include <cstdio>
#include <stdarg.h>
#include <iostream>

void LogTrace(int line, const char* fileName, const char* msg, ...) {
    va_list args;
    char fullLog[256] = { 0 };
    const char* _fn;
    for (_fn = fileName + strlen(fileName); _fn > fileName && _fn[-1] != '/' && _fn[-1] != '\\'; _fn--) {}
    sprintf_s(fullLog, sizeof(fullLog), "%s (%d) : ", _fn, line);

    char log_buff[256] = { 0 };
    va_start(args, msg);
    vsprintf_s(log_buff, msg, args);
    strcat_s(fullLog, log_buff);
    printf_s(fullLog);
    va_end(args);
    printf_s("\n");
}

std::string LogInfo(int line, const char* fileName){

    char fullLog[256] = { 0 };
    const char* _fn;
    for (_fn = fileName + strlen(fileName); _fn > fileName && _fn[-1] != '/' && _fn[-1] != '\\'; _fn--) {}
    sprintf_s(fullLog, sizeof(fullLog), "%s (%d) : ", _fn, line);
    return std::string(fullLog);
};
int gcd(int a, int b)
{
    if (a == 0)
        return b;
    return gcd(b % a, a);
}

float distance(Vec2 v1, Vec2 v2) { return  (v1 - v2).length(); };
