#ifndef __FLOG_H__
#define __FLOG_H__

#include <stdio.h>
#include <assert.h>
#include <unistd.h>

/* Define flog elements for example.cpp */

#define FLOG_PRINTF (*flog_pprintf)

#define FLOG_TIME_SIZE 32
#define FLOG_TIME(ARG) flog_timestamp(ARG)
#define FLOG_TIME_DEC char TSSTR[FLOG_TIME_SIZE];
#define FLOG_TIME_ARG TSSTR

#define FLOG_THREAD()   getpid()

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

