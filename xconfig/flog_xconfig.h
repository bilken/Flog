#ifndef __FLOG_XCONFIG_H__
#define __FLOG_XCONFIG_H__

#define FLOG_SEVERITY_LIST \
    FLOG_SEVERITY_LIST_ITEM(DEBUG) \
    FLOG_SEVERITY_LIST_ITEM(INFO) \
    FLOG_SEVERITY_LIST_ITEM(ERROR) \

#define FLOG_MODULE_LIST \
    FLOG_MODULE_LIST_ITEM( AB, DEBUG, INFO ) \
    FLOG_MODULE_LIST_ITEM( CDEF, DEBUG, INFO ) \
    FLOG_MODULE_LIST_ITEM( GHI, INFO, INFO ) \
    FLOG_MODULE_LIST_ITEM( LOG, INFO, INFO ) \

#define LINE2STR(X) #X
#define LINETOSTRING(X) "[" LINE2STR(X) "] "

#define FLOG_FORMAT_LIST \
    FLOG_FORMAT_LIST_ITEM(SEVMOD, "%s") \
    FLOG_FORMAT_LIST_ITEM(FILE, "%s") \
    FLOG_FORMAT_LIST_ITEM(LINE, "%s") \
    FLOG_FORMAT_LIST_ITEM(FUNCTION, "%s") \
    FLOG_FORMAT_LIST_ITEM(PAREN, "%s") \

#define FLOG_ARGS_LIST(SEVERITY, MODULE) \
    FLOG_ARGS_LIST_ITEM(SEVMOD, #SEVERITY "[" #MODULE "] ", "") \
    FLOG_ARGS_LIST_ITEM(FILE, flog_file_name_shorten(__FILE__), "") \
    FLOG_ARGS_LIST_ITEM(LINE, LINETOSTRING(__LINE__), "") \
    FLOG_ARGS_LIST_ITEM(FUNCTION, __FUNCTION__, "") \
    FLOG_ARGS_LIST_ITEM(PAREN, "() ", "") \

#define FLOGX
#include "../flog/flogx.h"

#endif  /* __FLOG_XCONFIG_H__ */
