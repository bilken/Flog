#ifndef __FLOG_H__
#define __FLOG_H__

#include <stdio.h>
#include <assert.h>
#include <unistd.h>

/* Define flog elements for example.cpp */

#define FLOG_PRINTF (*flog_pprintf)

#define FLOG_TIME_SIZE 32
#define FLOG_FORMAT_DEC char TSSTR[FLOG_TIME_SIZE];

#define FLOG_FORMAT_LIST \
    FLOG_FORMAT_LIST_ITEM(TIMESTAMP, "%s") \
    FLOG_FORMAT_LIST_ITEM(THREADID, "%d ") \
    FLOG_FORMAT_LIST_ITEM(SEVMOD, "%s") \
    FLOG_FORMAT_LIST_ITEM(FILE, "%s") \
    FLOG_FORMAT_LIST_ITEM(LINE, "%s") \
    FLOG_FORMAT_LIST_ITEM(FUNCTION, "%s") \
    FLOG_FORMAT_LIST_ITEM(PAREN, "%s") \

#define FLOG_ARGS_LIST(SEVERITY, MODULE) \
    FLOG_ARGS_LIST_ITEM(TIMESTAMP, CON, flog_timestamp(TSSTR), "") \
    FLOG_ARGS_LIST_ITEM(THREADID, ON, getpid(), 0) \
    FLOG_ARGS_LIST_ITEM(SEVMOD, ON, #SEVERITY "[" #MODULE "] ", "") \
    FLOG_ARGS_LIST_ITEM(FILE, CON, flog_file_name_shorten(__FILE__), "") \
    FLOG_ARGS_LIST_ITEM(LINE, CON, LINETOSTRING(__LINE__), "") \
    FLOG_ARGS_LIST_ITEM(FUNCTION, COFF, __FUNCTION__, "") \
    FLOG_ARGS_LIST_ITEM(PAREN, COFF, "() ", "") \

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

