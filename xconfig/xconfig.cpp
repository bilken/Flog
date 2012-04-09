
#include "flog_xconfig.h"
#include "../flog/flog.c"

#include <stdlib.h>
#include <stdio.h>

static int parse_args(int argc, char ** argv)
{
    while (argc-- > 1) {
        ++argv;
        if (**argv != '-') {
            char buf[1024];
            flog_interact_s(*argv, buf, sizeof(buf));
            FLOG(INFO, LOG, "%s", buf);
        } else {
            printf("%s",
                "Xconfig log output\n"
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

    FLOG(ERROR, AB, "No arguments are problematic sometimes\n");
    FLOG(INFO, AB, "Normal printf: %u %s\n", 33, "INFO[AB] works!");
    FLOGS(INFO, CDEF, "Normal ostream: " << 44 << ", okay!" << std::endl);

    return 0;
}

