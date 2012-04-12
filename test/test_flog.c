
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

static char test_text[1024];
static char *ttp;
static size_t ttsize;
static void init_tt(void)
{
    memset(test_text, 0, sizeof(test_text));
    ttp = test_text;
    ttsize = sizeof(test_text) - 1;
}
static const char *get_tt(void)
{
    return test_text;
}
static int test_printf(const char *fmt, ...)
{
    va_list args;
    int i;

    va_start( args, fmt );
    i = vsnprintf(ttp, ttsize, fmt, args);
    va_end( args );

    ttp += i;
    ttsize -= i;

    return i;
}


/*
    Define test modules/severities:
        MINI, NONE
        MAXI, WEAK
        MIN_MAX, NONE, WEAK
        MAX_MIN, WEAK, NONE
        NORM, MID
        SPLIT, MID, ROBUST
*/
#define FLOG_SEVERITY_LIST \
    FLOG_SEVERITY_LIST_ITEM(WEAK) \
    FLOG_SEVERITY_LIST_ITEM(MID) \
    FLOG_SEVERITY_LIST_ITEM(ROBUST) \
    FLOG_SEVERITY_LIST_ITEM(STRONG) \

#define FLOG_MODULE_LIST \
    FLOG_MODULE_LIST_ITEM( MAXI, WEAK, WEAK ) \
    FLOG_MODULE_LIST_ITEM( MAX_MIN, WEAK, NONE ) \
    FLOG_MODULE_LIST_ITEM( MINI, NONE, NONE ) \
    FLOG_MODULE_LIST_ITEM( MIN_MAX, NONE, WEAK ) \
    FLOG_MODULE_LIST_ITEM( NORM, MID, MID ) \
    FLOG_MODULE_LIST_ITEM( SPLIT, MID, ROBUST ) \

#define FLOG_FORMAT_LIST \
    FLOG_FORMAT_LIST_ITEM(SEVMOD, "%s") \
    FLOG_FORMAT_LIST_ITEM(FILE, "%s") \
    FLOG_FORMAT_LIST_ITEM(LINE, "%s") \
    FLOG_FORMAT_LIST_ITEM(FUNCTION, "%s ") \

#define FLOG_ARGS_LIST(SEVERITY, MODULE) \
    FLOG_ARGS_LIST_ITEM(SEVMOD, CON, #SEVERITY "[" #MODULE "] ", "") \
    FLOG_ARGS_LIST_ITEM(FILE, CON, flog_file_name_shorten(__FILE__), "") \
    FLOG_ARGS_LIST_ITEM(LINE, CON, LINETOSTRING(__LINE__), "") \
    FLOG_ARGS_LIST_ITEM(FUNCTION, CON, __FUNCTION__, "") \

/* Use the test printf to capture log output */
#define FLOG_PRINTF test_printf

#define FLOG_ASSERT(COND, FMT, ARGS...) \
    do { \
        if (!(COND)) { \
            fprintf(stderr, "!'%s')\n\t" FMT , #COND , ##ARGS); \
            assert(COND); \
        } \
    } while (0)

#define FLOGX
#include "flogx.h"
#include "flog.c"


/* Test compile time settings */
static void test_ctime(void)
{
    const char *cp;

    flog_init();

    /* Enable max run-time flog settings */
    flog_set(FLOG_SVR_WEAK, FLOG_MOD_ALL);

    /* These should be compiled in */
    init_tt();
    FLOG(WEAK, MAXI, "weak max\n");
    cp = get_tt();
    FLOG_ASSERT(strlen(cp) >= 9, "missing flog output: %s\n", cp);

    init_tt();
    FLOG(MID, SPLIT, "mid split\n");
    cp = get_tt();
    FLOG_ASSERT(strlen(cp) >= 10, "missing flog output: %s\n", cp);

    /* All of these should be compiled out */
    init_tt();
    FLOG(STRONG, MINI, "strong min\n");
    FLOG(STRONG, MIN_MAX, "strong min_max\n");
    FLOG(WEAK, NORM, "weak norm\n");
    FLOG(WEAK, SPLIT, "weak split\n");
    cp = get_tt();
    FLOG_ASSERT(strlen(cp) == 0, "unexpected flog output: %s\n", cp);
}

/* Test format configuration */
static void test_smff(void)
{
    const char *cp;

    flog_init();

    init_tt();
    FLOG(STRONG, NORM, "testing\n");
    cp = get_tt();
    FLOG_ASSERT(strstr(cp, "STRONG"), "missing severity in flog: %s\n", cp);
    FLOG_ASSERT(strstr(cp, "NORM"), "missing module in flog: %s\n", cp);
    FLOG_ASSERT(strstr(cp, "test_flog.c"), "missing filename in flog: %s\n", cp);
    FLOG_ASSERT(strstr(cp, "test_smff"), "missing function in flog: %s\n", cp);
}

typedef struct
{
    const char *istr;
    int iresult;
    flog_module_t m;
    flog_severity_t s;
    int presult;
} test_interact_t;

static char ibuf[1024];

/* Test run-time configuration of flog settings */
static void _test_interact(const test_interact_t * tp)
{
    int r;

    flog_init();

    init_tt();
    r = flog_interact_s(tp->istr, ibuf, sizeof(ibuf));
    FLOG_ASSERT(r == tp->iresult,
            "Bad result %d != %d, %s\n %s",
            r, tp->iresult, ibuf, get_tt());

    r = flog_ms_is_set(FLOG_ENCODE(tp->s, tp->m));
    FLOG_ASSERT(r == tp->presult,
            "Incorrect severity setting %d != %d\nInteract %s\n",
            r, tp->presult, ibuf);
}

static void test_interact(void)
{
    static const test_interact_t list[] = {
        {"", -1, FLOG_MOD_SPLIT, FLOG_SVR_ROBUST, 1},
        {"", -1, FLOG_MOD_SPLIT, FLOG_SVR_MID, 0},
        {"BAD", -1, FLOG_MOD_SPLIT, FLOG_SVR_ROBUST, 1},
        {"BAD", -1, FLOG_MOD_SPLIT, FLOG_SVR_MID, 0},
        {"SPLITBAD", -1, FLOG_MOD_SPLIT, FLOG_SVR_ROBUST, 1},
        {"SPLIT", 0, FLOG_MOD_SPLIT, FLOG_SVR_ROBUST, 1},
        {"SPLIT", 0, FLOG_MOD_SPLIT, FLOG_SVR_MID, 0},
        {"SPLIT MID", 0, FLOG_MOD_SPLIT, FLOG_SVR_MID, 1},
        {"SPLIT MID", 0, FLOG_MOD_SPLIT, FLOG_SVR_WEAK, 0},
        {"SPLIT WEAK", 0, FLOG_MOD_SPLIT, FLOG_SVR_MID, 1},
        {"  sPlIT   = miD   ", 0, FLOG_MOD_SPLIT, FLOG_SVR_ROBUST, 1},
        {"  sPlIT   = miD   ", 0, FLOG_MOD_SPLIT, FLOG_SVR_MID, 1},
        {"  sPlIT   = miD   ", 0, FLOG_MOD_SPLIT, FLOG_SVR_WEAK, 0},
        {"ALL NONE", 0, FLOG_MOD_SPLIT, FLOG_SVR_MID, 0},
    };

    size_t i;
    for (i = 0; i < sizeof(list)/sizeof(list[0]); i++) {
        _test_interact(list + i);
    }
}

typedef enum
{
    CFGB_SM = 1,
    CFGB_F = 2,
    CFGB_L = 4,
    CFGB_FU = 8,
} config_bit_t;

static void _test_config(long cfg)
{
    config_bit_t bits = (config_bit_t)cfg;

    const char *cp;
    const char *sfn;
    int line;
    size_t i;

    flog_init();

    flog_interact_s((bits & CFGB_SM) ? "+sevmod" : "-sevmod", ibuf, sizeof(ibuf));
    flog_interact_s((bits & CFGB_F) ? "+file" : "-file", ibuf, sizeof(ibuf));
    flog_interact_s((bits & CFGB_L) ? "+line" : "-line", ibuf, sizeof(ibuf));
    flog_interact_s((bits & CFGB_FU) ? "+function" : "-function", ibuf, sizeof(ibuf));

    init_tt();
    line = __LINE__; FLOG(MID, MAXI, "plain text\n");
    cp = get_tt();

    if (bits & CFGB_SM) {
        FLOG_ASSERT(strstr(cp, "MAXI"), "Missing module %s\n", cp);
        FLOG_ASSERT(strstr(cp, "MID"), "Missing severity %s\n", cp);
    } else {
        FLOG_ASSERT(strstr(cp, "MAXI") == NULL, "Unexpected module %s\n", cp);
        FLOG_ASSERT(strstr(cp, "MID") == NULL, "Unexpected severity %s\n", cp);
    }

    sfn = flog_file_name_shorten(__FILE__);
    if (bits & CFGB_F) {
        FLOG_ASSERT(strstr(cp, sfn), "Missing file name %s: %s\n", sfn, cp);
    } else {
        FLOG_ASSERT(strstr(cp, sfn) == NULL,
                "Unexpected file name %s: %s\n", sfn, cp);
    }

    if (bits & CFGB_L) {
        char ls[8];
        i = sizeof(ls);
        snprintf(ls, i, "%d", line);
        FLOG_ASSERT(strstr(cp, ls),
                "Missing line %d (%s): %s\n", line, ls, cp);
    }

    if (bits & CFGB_FU) {
        FLOG_ASSERT(strstr(cp, __FUNCTION__),
                "Missing function %s: %s\n", __FUNCTION__, cp);
    } else {
        FLOG_ASSERT(strstr(cp, __FUNCTION__) == NULL,
                "Unexpected function %s: %s\n", __FUNCTION__, cp);
    }
}

static void test_config(void)
{
    long i;
    for (i = 0; i < 8; i++) {
        _test_config(i);
    }
}


int main(int argc, char **argv)
{
    static const struct {
        const char * name;
        void (*fp)();
    } tests[] = {
#define DEF_TEST(n) {#n, n},
        DEF_TEST(test_ctime)
        DEF_TEST(test_smff)
        DEF_TEST(test_interact)
        DEF_TEST(test_config)
    };
    size_t i;

    flog_init();

    for (i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        printf("Run test %s\n", tests[i].name);
        tests[i].fp();
        printf("Success!\n");
    }

    return 0;
}

