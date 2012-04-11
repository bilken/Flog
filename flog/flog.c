
#ifndef FLOG_MODULE_LIST
#error "FLOG_MODULE_LIST not defined! FLOG header may not be included."
#endif

#ifndef FLOG_VSNPRINTF
#include <stdio.h>
#define FLOG_VSNPRINTF vsnprintf
#endif

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifndef ARRAY_COUNT
#define ARRAY_COUNT(a) ( sizeof(a) / sizeof((a)[0]) )
#endif

int flog_flags;

typedef unsigned char severity_list_t[FLOG_MOD_NUM_ELEMENTS];

static severity_list_t scurrent;

static const severity_list_t smaximums =
{
#define FLOG_MODULE_LIST_ITEM(m, ms, ds) FLOG_SVR_##ms,
    FLOG_MODULE_LIST
#undef FLOG_MODULE_LIST_ITEM
};

static const severity_list_t sdefaults =
{
#define FLOG_MODULE_LIST_ITEM(m, ms, ds) FLOG_SVR_##ds,
    FLOG_MODULE_LIST
#undef FLOG_MODULE_LIST_ITEM
};

void flog_init(void)
{
    memcpy(scurrent, sdefaults, sizeof(scurrent));
    flog_flags = FLOG_FLAGS_DEFAULT;
}

#define MODULE_IS_VALID(M) ((M >= 0) && (M < FLOG_MOD_NUM_ELEMENTS))
#define SEVERITY_IS_VALID(S) ((S >= 0) && (S < FLOG_SVR_NUM_ELEMENTS))

int flog_ms_is_set(flog_sev_mod_t ms)
{
    flog_severity_t s;
    flog_module_t m;

    FLOG_DECODE(ms, s, m);
    return (MODULE_IS_VALID(m) & SEVERITY_IS_VALID(s) && (s >= scurrent[m])) ?
            1 :
            0;
}

int flog_set(flog_severity_t s, flog_module_t m)
{
    if (SEVERITY_IS_VALID(s)) {
        if (m == FLOG_MOD_ALL) {
            for (m = 0; m < FLOG_MOD_NUM_ELEMENTS; m++) {
                scurrent[m] = s;
            }
            return 0;
        }
        if (MODULE_IS_VALID(m)) {
            scurrent[m] = s;
            return 0;
        }
    }
    return -1;
}

static flog_severity_t _severity_get(flog_module_t m)
{
    return (MODULE_IS_VALID(m)) ? scurrent[m] : FLOG_SVR_INVALID;
}

static flog_severity_t _severity_get_max(flog_module_t m)
{
    return (MODULE_IS_VALID(m)) ? smaximums[m] : FLOG_SVR_INVALID;
}

/* The list of module strings */
static const char mstr_list[][16] =
{
#define FLOG_MODULE_LIST_ITEM(m, ms, ds) #m,
    FLOG_MODULE_LIST
#undef FLOG_MODULE_LIST_ITEM
    "ALL",
};

/* The list of severity strings */
static const char sstr_list[][8] =
{
#define FLOG_SEVERITY_LIST_ITEM(s) #s,
    FLOG_SEVERITY_LIST
#undef FLOG_SEVERITY_LIST_ITEM
    "NONE",
};


static const char *_severity_to_string(flog_severity_t s)
{
    return (SEVERITY_IS_VALID(s)) ? sstr_list[s] : "UNKNOWN";
}

static const char *_module_to_string(flog_module_t m)
{
    return (MODULE_IS_VALID(m)) ? mstr_list[m] : "UNKNOWN";
}

static int _str_nccmp( const  char *s1, const  char *s2 )
{
    int r = 0;

    do {
        r = (*s1++ - *s2++) & ~0x20;
    } while (*s1 && (r == 0));

    if (*s2) {
        return 1;
    }
    return r;
}

static size_t find_string_in_list(const void *plist,
        size_t rowSize, size_t num,
        const char *str)
{
    size_t i;

    const char * l = (const char *)plist;
    for (i = 0; i < num; i++) {
        if (_str_nccmp(str, l) == 0) {
            break;
        }
        l += rowSize;
    }
    return i;
}

static flog_severity_t _severity_from_string(const char *str)
{
    flog_severity_t s;

    s = (flog_severity_t)find_string_in_list(sstr_list,
            sizeof(sstr_list[0]), ARRAY_COUNT(sstr_list),
            str);
    if (!SEVERITY_IS_VALID(s)) {
        return FLOG_SVR_INVALID;
    }
    return s;
}

static flog_module_t _module_from_string(const char *str)
{
    flog_module_t m;

    m = (flog_module_t)find_string_in_list(mstr_list,
            sizeof(mstr_list[0]), ARRAY_COUNT(mstr_list),
            str);
    if (!MODULE_IS_VALID(m) && m != FLOG_MOD_ALL) {
        return FLOG_MOD_INVALID;
    }
    return m;
}

int flog_psnprintf( psn_t * psn, const char *format, ... )
{
    va_list arg;
    int i;

    va_start(arg, format);
    i = FLOG_VSNPRINTF(psn->buf, psn->size, format, arg);
    va_end(arg);

    if (i > (int)psn->size) {
        /* Note, vsnprintf overruns in glibc (and crash). :-s */
        i = psn->size;
    }

    psn->buf += i;
    psn->size -= i;

    return i;
}

static size_t _print_module(flog_module_t m, psn_t *p)
{
    flog_severity_t s;

    s = _severity_get(m);
    return flog_psnprintf(p, " %s[%s/%s]",
            _module_to_string(m),
            _severity_to_string(s),
            _severity_to_string(_severity_get_max(m)));
}

static void _print_modules(psn_t *p)
{
    flog_module_t m;
    size_t len = 0;

    flog_psnprintf(p, "Log modules:\n");
    for (m = 0; m < FLOG_MOD_NUM_ELEMENTS; m++) {
        len += _print_module(m, p);
        len += flog_psnprintf(p, ",");
        if (len > 60) {
            flog_psnprintf(p, "\n");
            len = 0;
        }
    }
    if (len > 0) {
        flog_psnprintf(p, "\n");
    }
}

static void _print_severities(psn_t *p)
{
    flog_severity_t s;

    flog_psnprintf(p, "Log severities:\n");
    for (s = 0; s < FLOG_SVR_NUM_ELEMENTS; s++) {
        flog_psnprintf(p, " %s(%d),", _severity_to_string(s), s);
    }
    flog_psnprintf(p, "\n");
}

static int _is_alpha(char c)
{
    c &= ~0x20;
    return ((c >= 'A' && c <= 'Z') || (c == '_'));
}

static int _is_numeric(char c)
{
    if (c >= '0' && c <= '9') {
        return 1;
    }
    c &= ~0x20;
    return (c == 'x');
}

static int _is_alNum(char c)
{
    return _is_alpha(c) || _is_numeric(c);
}

static void _parse_string(char *cp, size_t len, const char **parg)
{
    const char *arg = *parg;

    while (*arg && !_is_alNum(*arg)) {
        arg++;
    }

    while ((len-- > 1) && *arg && _is_alNum(*arg)) {
        *cp++ = *arg++;
    }
    *cp = 0;

    *parg = arg;
}

static const struct
{
    const char * name;
    int bit;
}
flog_cfg[] =
{
#undef FLOG_FORMAT_LIST_ITEM
#define FLOG_FORMAT_LIST_ITEM(name, fmt) {#name, FLOG_FLAG_##name},
    FLOG_FORMAT_LIST
#undef FLOG_FORMAT_LIST_ITEM
#define FLOG_FORMAT_LIST_ITEM(name, fmt) fmt
};

static void _print_configs(psn_t * p)
{
    size_t i;

    flog_psnprintf(p, "Configurations: '+|- CFG'\n");
    for (i = 0; i < ARRAY_COUNT(flog_cfg); i++) {
        if (flog_cfg[i].bit & FLOG_FLAGS_NONCONFIG) {
            continue;
        }
        flog_psnprintf(p, " %c%s,",
                (flog_flags & flog_cfg[i].bit) ? '+' : '-',
                flog_cfg[i].name);
    }
    flog_psnprintf(p, "\n");
}

static int _log_config(char *cp, size_t len, const char *arg)
{
    int enable;
    size_t i;

    while (*arg && ((*arg == ' ') || (*arg == '\t'))) {
        arg++;
    }

    enable = (*arg == '+') ? 1 : (*arg == '-') ? 0 : -1;
    if (enable == -1) {
        return 0;
    }

    _parse_string(cp, len, &arg);
    for (i = 0; i < ARRAY_COUNT(flog_cfg); i++) {
        if (flog_cfg[i].bit & FLOG_FLAGS_NONCONFIG) {
            continue;
        }
        if (_str_nccmp(cp, flog_cfg[i].name) == 0) {
            if (enable) {
                flog_flags |= flog_cfg[i].bit;
            } else {
                flog_flags &= ~flog_cfg[i].bit;
            }
            return 1;
        }
    }

    return -1;
}

int flog_interact_s(const char *arg, char *outStr, size_t outStrLen)
{
    flog_severity_t s;
    flog_module_t m;
    int r;
    char str[32];

    psn_t p = {outStr, outStrLen};

    if (arg == NULL) {
        flog_psnprintf(&p, "No arg!\n");
        return -1;
    }

    r = _log_config(str, sizeof(str), arg);
    if (r == 1) {
        flog_psnprintf(&p, "Set configuration '%s'\n", str);
        return 0;
    } else if (r == -1) {
        flog_psnprintf(&p, "Unknown configuration '%s'\n", str);
        _print_configs(&p);
        return -1;
    }

    _parse_string(str, sizeof(str), &arg);
    m = _module_from_string(str);
    if (!MODULE_IS_VALID(m) && m != FLOG_MOD_ALL) {
        if (_str_nccmp(str, "init") == 0) {
            flog_psnprintf(&p, "Resetting all modules\n");
            flog_init();
            return 0;
        }
        flog_psnprintf(&p, "Unknown request '%s'\n", str);
        _print_modules(&p);
        _print_configs(&p);
        return -1;
    }

    _parse_string(str, sizeof(str), &arg);
    s = _severity_from_string(str);
    if (SEVERITY_IS_VALID(s)) {
        if (flog_set(s, m) == 0) {
            flog_psnprintf(&p, "Successfully set severity\n");
        }
    } else if (str[0]) {
        flog_psnprintf(&p, "Unknown severity '%s' %d\n", str, s);
        _print_severities(&p);
        return -1;
    } else {
        flog_psnprintf(&p, "Current setting:\n");
    }

    if (m == FLOG_MOD_ALL) {
        flog_psnprintf(&p, "  ALL = %s(%d)\n", _severity_to_string(s), s);
    } else {
        _print_module(m, &p);
        flog_psnprintf(&p, "\n");
    }

    return 0;
}

/* Return the address after the final '/' or '\\' */
const char *flog_file_name_shorten(const char *fn)
{
    const char *cp;

    for (cp = fn; *cp; cp++) {
        if (*cp == '/' || *cp == '\\') {
            fn = cp + 1;
        }
    }

    return fn;
}

