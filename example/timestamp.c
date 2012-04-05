
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include "flog.h"

const char *flog_timestamp( char *pts )
{
    time_t t;
    struct tm tm;
    struct timeval tv;
    size_t size = FLOG_TIME_SIZE;

    time(&t);
    localtime_r(&t, &tm);
    gettimeofday(&tv, NULL);

    psn_t p = {pts, size};
    flog_psnprintf(&p, "%04u-%02u-%02u %02u:%02u:%02u.%03u ", 
            tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday,
            tm.tm_hour, tm.tm_min, tm.tm_sec,
            tv.tv_usec / 1000);

    return pts;
}

