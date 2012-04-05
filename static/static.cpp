
/* Define flog elements for static-only project */
#define FLOG_STATIC 1
#include "flog_static.h"

int main(int argc, char ** argv)
{
    FLOG(NOISE, AB, "Won't show up %d\n", argc);
    FLOG(CRIT, AB, "No arguments are problematic sometimes\n");
    FLOG_INFO_AB("Normal printf: %s\n", "INFO[AB] works!");
    FLOGS(INFO, AB, "Normal stream: " << 33 << ", INFO[AB] works!" << std::endl);

    return 0;
}

