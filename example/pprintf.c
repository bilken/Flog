
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "flog.h"

/* Note: don't change the mode while simultaneously invoking FLOG() */

static enum {
    MODE_STDOUT = 0,
    MODE_FILE,
    MODE_CUSTOM,
    MODE_SBUF,
} mode = MODE_STDOUT;

static FILE * wrap_fp = NULL;
static int flog_fprintf(const char *fmt, ...)
{
    va_list arg;
    int i;

    va_start(arg, fmt);
    i = vfprintf((mode == MODE_FILE) ? wrap_fp : stdout, fmt, arg);

    return i;
}

int (*flog_pprintf)(const char * fmt, ...) = &flog_fprintf;

void flog_pprint_to_stdout(void)
{
    wrap_fp = NULL;
    mode = MODE_STDOUT;
    flog_pprintf = &flog_fprintf;
}

void flog_pprint_to_file(FILE * fp)
{
    wrap_fp = fp;
    mode = MODE_FILE;
    flog_pprintf = &flog_fprintf;
}

void flog_pprint_to_custom(int (*user_fp)(const char *fmt, ...))
{
    flog_pprintf = user_fp;
    mode = MODE_CUSTOM;
}

void flog_pprint_to_sbuf(void)
{
    mode = MODE_SBUF;
    flog_pprintf = &flog_sbuf_printf;
}

