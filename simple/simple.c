
#define FLOG_FORMAT_LIST(FA, SEVERITY, MODULE) \
    FLOG_FORMAT_LIST_ITEM(SEVMOD, CON, "%s", #SEVERITY "[" #MODULE "] ", "", FA) \
    FLOG_FORMAT_LIST_ITEM(FILE, COFF, "%s", flog_file_name_shorten(__FILE__), "", FA) \
    FLOG_FORMAT_LIST_ITEM(LINE, COFF, "%s", LINETOSTRING(__LINE__), "", FA) \
    FLOG_FORMAT_LIST_ITEM(FUNCTION, COFF, "%s", __FUNCTION__, "", FA) \
    FLOG_FORMAT_LIST_ITEM(PAREN, COFF, "%s", "() ", "", FA) \

#include "flog_simple.h"
#include "flog.c"

#include <stdlib.h>
#include <stdio.h>

static int parse_args(int argc, char ** argv)
{
    while (argc-- > 1) {
        char buf[1024];
        int r;

        ++argv;
        r = flog_interact_s(*argv, buf, sizeof(buf));
        FLOG(INFO, LOG, "%s", buf);
        if (r) {
            printf("%s",
                "Simple log output\n"
                "  OPTION   Interact with log options\n"
                "       [Mod][=Sev] - View/set a module's severity\n"
                "       +|-foption  - Adjust formatting\n");
            return 1;
        }
    }
    return 0;
}

int main(int argc, char ** argv)
{
    flog_init();

    int r = parse_args(argc, argv);
    if (r != 0) {
        return r;
    }

    FLOG(CRIT, AB, "No arguments are problematic sometimes\n");
    FLOG(INFO, AB, "Normal printf: %u %s\n", 33, "INFO[AB] works!");
    FLOG(INFO, AB, "Normal printf: %s\n", "INFO[AB] works!");

    return 0;
}

