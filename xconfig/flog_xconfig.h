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

/* Note that format options cannot be defined */

#define FLOGX
#include "../flog/flogx.h"

#endif  /* __FLOG_XCONFIG_H__ */

