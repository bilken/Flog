#ifndef __FLOG_H__
#define __FLOG_H__

#include <stdio.h>
#include <assert.h>
#include <unistd.h>

/* Define flog elements for example.cpp */

#define FLOG_PRINTF (*flog_pprintf)

#define FLOG_TIME_SIZE 32
#define FLOG_FORMAT_DEC char TSSTR[FLOG_TIME_SIZE];

#define FLOG_FORMAT_LIST(FA, SEVERITY, MODULE) \
    FLOG_FORMAT_LIST_ITEM(TIMESTAMP, CON, "%s", flog_timestamp(TSSTR), "", FA) \
    FLOG_FORMAT_LIST_ITEM(THREADID, ON, "%d ", getpid(), 0, FA) \
    FLOG_FORMAT_LIST_ITEM(SEVMOD, ON, "%12s", #SEVERITY "[" #MODULE "] ", "", FA) \
    FLOG_FORMAT_LIST_ITEM(FILE, CON, "%s", flog_file_name_shorten(__FILE__), "", FA) \
    FLOG_FORMAT_LIST_ITEM(LINE, CON, "%s", LINETOSTRING(__LINE__), "", FA) \
    FLOG_FORMAT_LIST_ITEM(FUNCTION, COFF, "%s", __FUNCTION__, "", FA) \
    FLOG_FORMAT_LIST_ITEM(PAREN, COFF, "%s", "() ", "", FA) \

#ifdef __cplusplus
extern "C" {
#endif

extern const char *flog_timestamp(char *pts);

extern int (*flog_pprintf)(const char *fmt, ...);
extern void flog_pprint_to_stdout();
extern void flog_pprint_to_file(FILE * file);
extern void flog_pprint_to_sbuf();

extern int flog_sbuf_printf(const char *fmt, ...);
typedef void (*flog_sbuf_drain_fp)(void * arg, const char * data, size_t len);
extern void flog_sbuf_drain(flog_sbuf_drain_fp fp, void * arg);

#ifdef __cplusplus
}
#endif

#include "flog_example.h"

#endif

