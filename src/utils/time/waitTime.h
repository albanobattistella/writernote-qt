#ifndef WAITTIME_H
#define WAITTIME_H

#include "utils/common_def.h"
#include "utils/common_script.h"

#if defined(WIN32) || defined(WIN64)
#include <windows.h> // for Sleep
#endif

force_inline void waitTime(int ms)
{
#if defined(WIN32) || defined(WIN64)
    Sleep(uint(ms));
#else
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
    nanosleep(&ts, NULL);
#endif
}

#endif // WAITTIME_H
