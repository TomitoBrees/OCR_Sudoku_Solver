#pragma once

#ifdef _WIN32

void err(int eval, const char *fmt, ...);

void errx(int eval, const char *fmt, ...);

#else

#include "err.h"

#endif

