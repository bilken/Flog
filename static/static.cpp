
/* Define flog elements for static-only project */
#define FLOG_STATIC 1

#define FLOG_FORMAT_LIST(FA, SEVERITY, MODULE) \
    FLOG_FORMAT_LIST_ITEM(SEVMOD, ON, "%s", #SEVERITY "[" #MODULE "] ", "", FA) \
    FLOG_FORMAT_LIST_ITEM(FILE, ON, "%s", __FILE__, "", FA) \
    FLOG_FORMAT_LIST_ITEM(LINE, ON, "%s", LINETOSTRING(__LINE__), "", FA) \

#include "flog_static.h"

int main(int argc, char ** argv)
{
    FLOG(NOISE, AB, "Won't show up %d\n", argc);
    FLOG(CRIT, AB, "No arguments are problematic sometimes\n");
    FLOG_INFO_AB("Normal printf: %s\n", "INFO[AB] works!");
    FLOGS(INFO, AB, "Normal stream: " << 33 << ", INFO[AB] works!" << std::endl);

    return 0;
}

