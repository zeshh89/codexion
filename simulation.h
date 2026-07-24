#ifndef SIMULATION_H
#define SIMULATION_H

#include <pthread.h>
#include "parsing.h"
#include "dongle.h"

typedef enum e_coder_state
{
    STATE_WAITING,
    STATE_COMPILING,
    STATE_DEBUGGING,
    STATE_REFACTORING,
    STATE_BURNED_OUT,
    STATE_FINISHED
}   t_coder_state;

typedef struct s_coder
{
    int             id;
    int             index;
    t_dongle        *dongle_a;
    t_dongle        *dongle_b;
    t_coder_state   state;
    int             compiles_done;
    long            last_compile_start;
    pthread_t       thread;
    struct s_sim    *sim;
}   t_coder;

typedef struct s_sim
{
    t_params        params;
    long            start_time;
    t_dongle        *dongles;
    t_coder         *coders;
    pthread_mutex_t log_mutex;
    pthread_mutex_t status_mutex;
    int             stop;
}   t_sim;

void    log_event(t_sim *sim, int coder_id, const char *action);
int     sim_init(t_sim *sim, t_params *params);
void    sim_destroy(t_sim *sim);

#endif