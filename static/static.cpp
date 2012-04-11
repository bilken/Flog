
/* Define flog elements for static-only project */
#define FLOG_STATIC 1

#define FLOG_FORMAT_LIST \
    FLOG_FORMAT_LIST_ITEM(SEVMOD, "%s") \
    FLOG_FORMAT_LIST_ITEM(FILE, "%s") \
    FLOG_FORMAT_LIST_ITEM(LINE, "%s") \

#define FLOG_ARGS_LIST(SEVERITY, MODULE) \
    FLOG_ARGS_LIST_ITEM(SEVMOD, ON, #SEVERITY "[" #MODULE "] ", "") \
    FLOG_ARGS_LIST_ITEM(FILE, ON, __FILE__, "") \
    FLOG_ARGS_LIST_ITEM(LINE, ON, LINETOSTRING(__LINE__), "") \

#include "flog_static.h"

int main(int argc, char ** argv)
{
    FLOG(NOISE, AB, "Won't show up %d\n", argc);
    FLOG(CRIT, AB, "No arguments are problematic sometimes\n");
    FLOG_INFO_AB("Normal printf: %s\n", "INFO[AB] works!");
    FLOGS(INFO, AB, "Normal stream: " << 33 << ", INFO[AB] works!" << std::endl);

    return 0;
}

