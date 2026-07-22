#ifndef PARSING_H
#define PARSING_H

typedef enum e_scheduler
{
    SCHED_FIFO,
    SCHED_EDF
}   t_scheduler;

typedef struct s_params
{
    long        number_of_coders;
    long        time_to_burnout;
    long        time_to_compile;
    long        time_to_debug;
    long        time_to_refactor;
    long        number_of_compiles_required;
    long        dongle_cooldown;
    t_scheduler scheduler;
}   t_params;

int parse_args(int argc, char **argv, t_params *params);

#endif