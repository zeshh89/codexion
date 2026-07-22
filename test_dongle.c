#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "dongle.h"
#include "heap.h"
#include "time_utils.h"

typedef struct s_test_ctx
{
    t_dongle        *dongle;
    long            start_time;
    long            hold_ms;
    long            cooldown_ms;
    int             iterations;
    pthread_mutex_t *log_mutex;
}   t_test_ctx;

typedef struct s_thread_arg
{
    t_test_ctx  *ctx;
    int         coder_id;
}   t_thread_arg;

static void log_line(pthread_mutex_t *log_mutex, long start_time,
                        int coder_id, const char *action)
{
    pthread_mutex_lock(log_mutex);
    printf("%ld %d %s\n", get_relative_ms(start_time), coder_id, action);
    pthread_mutex_unlock(log_mutex);
}

static void *coder_routine(void *arg)
{
    t_thread_arg    *targ;
    t_test_ctx      *ctx;
    long            request_time;
    long            release_time;
    int             i;

    targ = (t_thread_arg *)arg;
    ctx = targ->ctx;
    i = 0;
    while (i < ctx->iterations)
    {
        request_time = get_absolute_ms();
        log_line(ctx->log_mutex, ctx->start_time, targ->coder_id,
            "requesting dongle");

        dongle_acquire(ctx->dongle, targ->coder_id, request_time, 0);

        log_line(ctx->log_mutex, ctx->start_time, targ->coder_id,
            "acquired dongle");

        usleep(ctx->hold_ms * 1000);

        release_time = get_absolute_ms();
        dongle_release(ctx->dongle, release_time, ctx->cooldown_ms);

        log_line(ctx->log_mutex, ctx->start_time, targ->coder_id,
            "released dongle");

        i++;
    }
    return (NULL);
}

int main(void)
{
    t_dongle        dongle;
    pthread_mutex_t log_mutex;
    t_test_ctx      ctx;
    t_thread_arg    args[3];
    pthread_t       threads[3];
    int             i;

    dongle_init(&dongle, 1, cmp_fifo);
    pthread_mutex_init(&log_mutex, NULL);

    ctx.dongle = &dongle;
    ctx.start_time = get_absolute_ms();
    ctx.hold_ms = 200;
    ctx.cooldown_ms = 300;
    ctx.iterations = 2;
    ctx.log_mutex = &log_mutex;

    i = 0;
    while (i < 3)
    {
        args[i].ctx = &ctx;
        args[i].coder_id = i + 1;
        pthread_create(&threads[i], NULL, coder_routine, &args[i]);
        i++;
    }

    i = 0;
    while (i < 3)
    {
        pthread_join(threads[i], NULL);
        i++;
    }

    pthread_mutex_destroy(&log_mutex);
    dongle_destroy(&dongle);
    return (0);
}