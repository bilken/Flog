#ifndef __FLOGX_H__
#define __FLOGX_H__


#if 0
#define FLOG_SEVERITY_LIST \
    FLOG_SEVERITY_LIST_ITEM(DEBUG) \
    FLOG_SEVERITY_LIST_ITEM(INFO) \
    FLOG_SEVERITY_LIST_ITEM(ERROR) \

#define FLOG_MODULE_LIST \
    FLOG_MODULE_LIST_ITEM( AB, DEBUG, INFO ) \
    FLOG_MODULE_LIST_ITEM( CDEF, DEBUG, INFO ) \

#define FLOG_FORMAT_LIST \
    FLOG_FORMAT_LIST_ITEM(SEVMOD, "%s") \
    FLOG_FORMAT_LIST_ITEM(FILE, "%s") \

#define FLOG_ARGS_LIST(SEVERITY, MODULE) \
    FLOG_ARGS_LIST_ITEM(SEVMOD, #SEVERITY "[" #MODULE "] ", "") \
    FLOG_ARGS_LIST_ITEM(FILE, flog_file_name_shorten(__FILE__), "") \

#else
#if !defined(FLOG_SEVERITY_LIST) || !defined(FLOG_MODULE_LIST) || !defined(FLOG_FORMAT_LIST)
#error "Missing FLOG_SEVERITIES/FLOG_MODULES/FLOG_FORMAT defintion"
#endif
#endif

enum {
    FLOG_SVR_INVALID = -1,
#define FLOG_SEVERITY_LIST_ITEM(SVR) FLOG_SVR_##SVR,
    FLOG_SEVERITY_LIST
#undef FLOG_SEVERITY_LIST_ITEM
    FLOG_SVR_NONE,
    FLOG_SVR_NUM_ELEMENTS,

#define FLOG_MODULE_LIST_ITEM(MOD, MAX, DEF) FLOG_MOD_MAX_##MOD = FLOG_SVR_##MAX,
    FLOG_MODULE_LIST
#undef FLOG_MODULE_LIST_ITEM
};

enum {
    FLOG_MOD_INVALID = -1,
#define FLOG_MODULE_LIST_ITEM(MOD, MAX, DEF) FLOG_MOD_##MOD,
    FLOG_MODULE_LIST
#undef FLOG_MODULE_LIST_ITEM
    FLOG_MOD_NUM_ELEMENTS = 4,
    FLOG_MOD_ALL = FLOG_MOD_NUM_ELEMENTS,
};

enum {
    FLOG_FLAGBIT_ZERO = 0,
#define FLOG_FORMAT_LIST_ITEM(NAME, FMT) FLOG_FLAGBIT_##NAME,
    FLOG_FORMAT_LIST
#undef FLOG_FORMAT_LIST_ITEM

    FLOG_FLAG_ZERO = 0,
#define FLOG_FORMAT_LIST_ITEM(NAME, FMT) FLOG_FLAG_##NAME = 1 << FLOG_FLAGBIT_##NAME,
    FLOG_FORMAT_LIST
#undef FLOG_FORMAT_LIST_ITEM
};

#define FLOG_FORMAT_LIST_ITEM(NAME, FMT) FMT

#define FLOG_ARGS_LIST_ITEM(NAME, ARG, DISARG) , FLOG_FLAG(NAME) ? ARG : DISARG

#define FLOG_FLAG(FF) (flog_flags & FLOG_FLAG_##FF)
#define FLOG_FLAGS_DEFAULT (0x7f)

#ifdef __cplusplus
#include <sstream>
#endif

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

void flog_init(void);

typedef int flog_severity_t;
typedef int flog_module_t;
typedef int flog_sev_mod_t;

extern int flog_interact_s(const char *arg, char *outStr, size_t outStrLen);
extern int flog_set(flog_severity_t s, flog_module_t m);

typedef struct
{
    char * buf;
    size_t size;
} psn_t;
extern int flog_psnprintf( psn_t * psn, const char *format, ... );

extern int flog_ms_is_set(flog_sev_mod_t ms);
extern int flog_flags;
extern const char *flog_file_name_shorten(const char *fn);

#ifdef __cplusplus
}
#endif


#ifndef FLOG_PRINTF
#include <stdio.h>
#define FLOG_PRINTF printf
#endif

#ifndef FLOG_VSNPRINTF
#include <stdio.h>
#define FLOG_VSNPRINTF vsnprintf
#endif

#ifndef FLOG_FORMAT_DEC
#define FLOG_FORMAT_DEC
#endif

#define FLOG_VA_NONE 0
#define FLOG_VA_GCC  1
#define FLOG_VA_C99  2

#if defined(__GNUC__) || defined(__clang__)
#  define FLOG_VA_TYPE FLOG_VA_GCC
#elif 0 /* C-99 compiler */
#  define FLOG_VA_TYPE FLOG_VA_C99
#else   /* No variadic macros */
#error "not supported"
#  define FLOG_VA_TYPE FLOG_VA_NONE
#endif

#ifndef FLOG_STATIC
#define FLOG_ENCODE_SEV_BITS 4
#define FLOG_ENCODE(S, M) \
    ( (flog_sev_mod_t) ( ( (M) << FLOG_ENCODE_SEV_BITS ) | (S) ) )
#define FLOG_DECODE(MS, S, M) \
    do { \
        (S) = (MS) & ((1 << FLOG_ENCODE_SEV_BITS) - 1); \
        (M) = (MS) >> FLOG_ENCODE_SEV_BITS; \
    } while (0)
#ifdef FLOGX
#define IF_FLOG_SET(S, M) if ((FLOG_SVR_##S >= FLOG_MOD_MAX_##M) && flog_ms_is_set(FLOG_ENCODE(FLOG_SVR_##S, FLOG_MOD_##M)))
#else
#define IF_FLOG_SET(S, M) if (flog_ms_is_set(FLOG_ENCODE(FLOG_SVR_##S, FLOG_MOD_##M)))
#endif
#else
#define IF_FLOG_SET(S, M)
#endif

#define FLOGSTRING(SEVERITY, MODULE, STR) FLOG(SEVERITY, MODULE, "%s", (STR).c_str())

#ifdef FLOGX
#define FLOGS(SEVERITY, MODULE, SSTR) _FLOGS_CHKDO(SEVERITY, MODULE, SSTR)
#else
#define FLOGS(SEVERITY, MODULE, SSTR) FLOGS_##SEVERITY##_##MODULE(SSTR)
#endif

#if FLOG_VA_TYPE == FLOG_VA_GCC
#define FLOG(SEVERITY, MODULE, FMT, ARGS...) _FLOG_CHKDO(SEVERITY, MODULE, FMT , ##ARGS)
#define _FLOG_DO(SEVERITY, MODULE, FMT, ARGS...) \
    FLOG_FORMAT_DEC FLOG_PRINTF(FLOG_FORMAT_LIST FMT FLOG_ARGS_LIST(SEVERITY, MODULE) , ##ARGS)
#define _FLOG_CHKDO(SEVERITY, MODULE, FMT, ARGS...) \
    do { IF_FLOG_SET(SEVERITY, MODULE) { _FLOG_DO( SEVERITY, MODULE, FMT , ##ARGS ); } } while (0)
#define _FLOGS_CHKDO(SEVERITY, MODULE, SSTR) \
    do { IF_FLOG_SET(SEVERITY, MODULE) { \
        std::ostringstream _mSSTR; _mSSTR << SSTR; \
        _FLOG_DO(SEVERITY, MODULE, "%s", _mSSTR.str().c_str()); \
    } } while (0)
#endif /* FLOG_VA_TYPE == GCC */

#if FLOG_VA_TYPE == FLOG_VA_C99
/* Note, the extra "" arg lets fmt-only FLOG() calls work */
#define FLOG(SEVERITY, MODULE, ...) _FLOG_CHKDO(SEVERITY, MODULE, __VA_ARGS__, "")
#define _FLOG_DO(SEVERITY, MODULE, FMT, ...) \
    FLOG_FORMAT_DEC; FLOG_PRINTF(FLOG_FORMAT_LIST FMT "%s" FLOG_ARGS_LIST(SEVERITY, MODULE), __VA_ARGS__)
#define _FLOG_CHKDO(SEVERITY, MODULE, FMT, ...) \
    do { IF_FLOG_SET(SEVERITY, MODULE) { _FLOG_DO( SEVERITY, MODULE, FMT, __VA_ARGS__); } } while (0)
#define _FLOGS_CHKDO(SEVERITY, MODULE, SSTR) \
    do { IF_FLOG_SET(SEVERITY, MODULE) { \
        std::ostringstream _mSSTR; _mSSTR << SSTR; \
        _FLOG_DO(SEVERITY, MODULE, "%s", _mSSTR.str().c_str(), ""); \
    } } while (0)
#endif /* FLOG_VA_TYPE == C99 */

#endif  /* __FLOGX_H__ */

