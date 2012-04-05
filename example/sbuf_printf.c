
#include <stdlib.h>
#include <stdarg.h>

#include "flog.h"

#include <pthread.h>

#define SBUF_SIZE   1024*1024
#define SBUF_RESERVE    1024

static pthread_mutex_t sbuf_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct sbuf_item {
    struct sbuf_item * next;
    char * data;
    size_t offset;
} sbuf_item_t;

static sbuf_item_t * sbuf_head = NULL;
static sbuf_item_t * sbuf_tail = NULL;

static sbuf_item_t * sbuf_create()
{
    sbuf_item_t * p = malloc(sizeof(*p) + SBUF_SIZE);
    p->next = NULL;
    p->data = (char *)p + sizeof(*p);
    p->offset = 0;
    return p;
}

#if 0
#define SBUF_MAX_COUNT 4
static size_t sbuf_count = 0;
#endif

int flog_sbuf_printf(const char *fmt, ...)
{
    va_list arg;
    int i;

    pthread_mutex_lock(&sbuf_mutex);

    if (sbuf_tail == NULL) {
        sbuf_head = sbuf_tail = sbuf_create();
    }

    if (sbuf_tail->offset > (SBUF_SIZE - SBUF_RESERVE)) {
        sbuf_tail->next = sbuf_create();
        sbuf_tail = sbuf_tail->next;
    }

    va_start(arg, fmt);
    i = FLOG_VSNPRINTF(sbuf_tail->data + sbuf_tail->offset, SBUF_RESERVE,
            fmt, arg);
    va_end(arg);

    sbuf_tail->offset += i;

    pthread_mutex_unlock(&sbuf_mutex);

    return i;
}

void flog_sbuf_drain(flog_sbuf_drain_fp fp, void * arg)
{
    sbuf_item_t * head;

    pthread_mutex_lock(&sbuf_mutex);
    head = sbuf_head;
    sbuf_head = sbuf_tail = NULL;
    pthread_mutex_unlock(&sbuf_mutex);

    while (head) {
        sbuf_item_t * last = head;
        fp(arg, head->data, head->offset);
        head = head->next;
        free(last);
    }
}

