#include <unistd.h>
#include "coder.h"
#include "time_utils.h"

static int should_stop(t_sim *sim)
{
    int stop;

    pthread_mutex_lock(&sim->status_mutex);
    stop = sim->stop;
    pthread_mutex_unlock(&sim->status_mutex);
    return (stop);
}

static long get_deadline(t_coder *coder)
{
    long    last_start;

    pthread_mutex_lock(&coder->sim->status_mutex);
    last_start = coder->last_compile_start;
    pthread_mutex_unlock(&coder->sim->status_mutex);
    return (last_start + coder->sim->params.time_to_burnout);
}

static int acquire_both_dongles(t_coder *coder, long request_time)
{
    long    deadline;

    deadline = get_deadline(coder);
    if (dongle_acquire(coder->dongle_a, coder->sim, coder->id,
            request_time, deadline) != 0)
        return (-1);
    log_event(coder->sim, coder->id, "has taken a dongle");

    if (coder->dongle_a != coder->dongle_b)
    {
        if (dongle_acquire(coder->dongle_b, coder->sim, coder->id,
                request_time, deadline) != 0)
        {
            dongle_release(coder->dongle_a, get_absolute_ms(),
                coder->sim->params.dongle_cooldown);
            return (-1);
        }
        log_event(coder->sim, coder->id, "has taken a dongle");
    }
    return (0);
}

static void release_both_dongles(t_coder *coder, long release_time)
{
    long cooldown;

    cooldown = coder->sim->params.dongle_cooldown;
    dongle_release(coder->dongle_a, release_time, cooldown);
    if (coder->dongle_a != coder->dongle_b)
        dongle_release(coder->dongle_b, release_time, cooldown);
}

static void mark_compile_start(t_coder *coder, long now)
{
    pthread_mutex_lock(&coder->sim->status_mutex);
    coder->last_compile_start = now;
    coder->state = STATE_COMPILING;
    pthread_mutex_unlock(&coder->sim->status_mutex);
}

static void set_state(t_coder *coder, t_coder_state state)
{
    pthread_mutex_lock(&coder->sim->status_mutex);
    coder->state = state;
    pthread_mutex_unlock(&coder->sim->status_mutex);
}

static int compiles_left(t_coder *coder)
{
    int done;

    pthread_mutex_lock(&coder->sim->status_mutex);
    done = coder->compiles_done;
    pthread_mutex_unlock(&coder->sim->status_mutex);
    return (done < coder->sim->params.number_of_compiles_required);
}

void *coder_routine(void *arg)
{
    t_coder *coder;
    long    request_time;
    long    now;

    coder = (t_coder *)arg;
    while (!should_stop(coder->sim) && compiles_left(coder))
    {
        request_time = get_absolute_ms();
        if (acquire_both_dongles(coder, request_time) != 0)
            break;

        now = get_absolute_ms();
        mark_compile_start(coder, now);
        log_event(coder->sim, coder->id, "is compiling");
        usleep(coder->sim->params.time_to_compile * 1000);

        release_both_dongles(coder, get_absolute_ms());

        pthread_mutex_lock(&coder->sim->status_mutex);
        coder->compiles_done++;
        pthread_mutex_unlock(&coder->sim->status_mutex);

        if (should_stop(coder->sim))
            break;

        set_state(coder, STATE_DEBUGGING);
        log_event(coder->sim, coder->id, "is debugging");
        usleep(coder->sim->params.time_to_debug * 1000);

        if (should_stop(coder->sim))
            break;

        set_state(coder, STATE_REFACTORING);
        log_event(coder->sim, coder->id, "is refactoring");
        usleep(coder->sim->params.time_to_refactor * 1000);
    }
    if (!should_stop(coder->sim))
        set_state(coder, STATE_FINISHED);
    return (NULL);
}