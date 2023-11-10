#ifdef _WIN32

#include "stdio.h"
#include "stdarg.h"
#include "stdlib.h"

void errx(int ret_code, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");
    exit(ret_code);
}

void err(int ret_code, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");
    exit(ret_code);
}

#endif
