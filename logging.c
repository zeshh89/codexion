#include <stdio.h>
#include "simulation.h"
#include "time_utils.h"

void log_event(t_sim *sim, int coder_id, const char *action)
{
    pthread_mutex_lock(&sim->log_mutex);
    printf("%ld %d %s\n", get_relative_ms(sim->start_time), coder_id, action);
    pthread_mutex_unlock(&sim->log_mutex);
}