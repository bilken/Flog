
#include "flog.h"

// The flog interactive content depends on the configuration elements.
//  This creates a circular dependency where flog.c can't exist in
//  a library. To put it in a library, the configuration part would
//  need to be separated from the library part, but that makes this
//  more complicated than I want.
#include "../flog/flog.c"

#include <stdlib.h>

#include <iostream>

// In a real system, run periodically from a thread and store
//  to a file (or some similar permanent storage)
// And use something more efficient than putc().
static void example_drain(void * arg, const char * data, size_t len)
{
    while (len-- > 0) {
        putc(*data++, stdout);
    }
}

static int parse_args(int argc, char ** argv)
{
    while (argc-- > 1) {
        ++argv;
        if (**argv != '-') {
            char buf[1024];
            flog_interact_s(*argv, buf, sizeof(buf));
            FLOG(INFO, LOG, "%s", buf);
        } else {
            char c = argv[0][1];
            switch (c) {
            case 's':
                std::cout << "Write to sbuf; flush at end" << std::endl;
                flog_pprint_to_sbuf();
                break;
            case 'o':
                std::cout << "Write to stdout" << std::endl;
                flog_pprint_to_stdout();
                break;
            case 'l':
                if (argc < 1) {
                    std::cerr << "No argument for 'l' (log) command" << std::endl;
                    return -1;
                }
                std::cout << "Write to log " << argv[1] << std::endl;
                {
                    FILE * fp = fopen(argv[1], "a");
                    if (fp == NULL) {
                        std::cerr << "Failed to open " << argv[1] << std::endl;
                        return -1;
                    }
                    flog_pprint_to_file(fp);
                }

                argc--;
                argv++;
                break;
            case 'h':
            case '?':
                std::cout <<
                    "Example log output" << std::endl <<
                    "  -o       Log to stdout (default)" << std::endl <<
                    "  -s       Log to a buffer; dump at the end" << std::endl <<
                    "  -l fn    Log to file, fn" << std::endl <<
                    "  -h/-?    Show this help" << std::endl <<
                    "  OPTION   Interact with log options" << std::endl <<
                    "       [Mod][=Sev] - View/set a module's severity" << std::endl <<
                    "       +|-foption  - Adjust formatting" << std::endl;
                return 1;
            default:
                std::cerr << "Unknown option: " << argv[1];
                return -1;
            }
        }
    }
    return 0;
}

int main(int argc, char ** argv)
{
    flog_init();

    int r = parse_args(argc, argv);
    if (r != 0) {
        flog_sbuf_drain(&example_drain, NULL);
        return r;
    }

    FLOG(CRIT, AB, "No arguments are problematic sometimes\n");
    FLOG(INFO, AB, "Normal printf: %u %s\n", 33, "INFO[AB] works!");   // gone with RELEASE=1
    FLOG(INFO, AB, "Normal printf: %s\n", "INFO[AB] works!");   // gone with RELEASE=1
    FLOGS(ERROR, CDEF, "ostream " << std::hex << 333 << std::endl);
    FLOGSTRING(WARN, XYZ, std::string("Print string: ") + std::string("WARN[XYZ] works!\n"));

    //FLOGS(ACK, AB, "Bad severity" << std::endl);
    //FLOGS(ERROR, BADMOD, "Bad module" << std::endl);

    flog_sbuf_drain(&example_drain, NULL);

    return 0;
}

