#include <stdlib.h>
#include <time.h>
#include "dongle.h"
#include "time_utils.h"

int dongle_init(t_dongle *dongle, int id, t_heap_cmp cmp)
{
    dongle->id = id;
    dongle->in_use = 0;
    dongle->available_since = 0;
    if (heap_init(&dongle->waiters, cmp) != 0)
        return (-1);
    if (pthread_mutex_init(&dongle->mutex, NULL) != 0)
    {
        heap_destroy(&dongle->waiters);
        return (-1);
    }
    if (pthread_cond_init(&dongle->cond, NULL) != 0)
    {
        pthread_mutex_destroy(&dongle->mutex);
        heap_destroy(&dongle->waiters);
        return (-1);
    }
    return (0);
}

void dongle_destroy(t_dongle *dongle)
{
    pthread_cond_destroy(&dongle->cond);
    pthread_mutex_destroy(&dongle->mutex);
    heap_destroy(&dongle->waiters);
}

static int is_my_turn(t_dongle *dongle, int coder_id)
{
    if (heap_is_empty(&dongle->waiters))
        return (0);
    return (dongle->waiters.data[0].coder_id == coder_id);
}

int dongle_acquire(
    t_dongle *dongle,
    int coder_id,
    long request_time,
    long deadline
)
{
    t_heap_node     node;
    t_heap_node     popped;
    struct timespec ts;

    node.coder_id = coder_id;
    node.request_time = request_time;
    node.deadline = deadline;

    pthread_mutex_lock(&dongle->mutex);
    if (heap_push(&dongle->waiters, node) != 0)
    {
        pthread_mutex_unlock(&dongle->mutex);
        return (-1);
    }
    while (dongle->in_use || !is_my_turn(dongle, coder_id))
        pthread_cond_wait(&dongle->cond, &dongle->mutex);

    while (get_absolute_ms() < dongle->available_since)
    {
        ms_to_timespec(dongle->available_since, &ts);
        pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &ts);
        while (dongle->in_use || !is_my_turn(dongle, coder_id))
            pthread_cond_wait(&dongle->cond, &dongle->mutex);
    }

    heap_pop(&dongle->waiters, &popped);
    dongle->in_use = 1;
    pthread_mutex_unlock(&dongle->mutex);
    return (0);
}

void dongle_release(t_dongle *dongle, long release_time, long cooldown_ms)
{
    pthread_mutex_lock(&dongle->mutex);
    dongle->in_use = 0;
    dongle->available_since = release_time + cooldown_ms;
    pthread_cond_broadcast(&dongle->cond);
    pthread_mutex_unlock(&dongle->mutex);
}