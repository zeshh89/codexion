#include <stdio.h>
#include "parsing.h"

static void print_params(const t_params *p)
{
    printf("number_of_coders: %ld\n", p->number_of_coders);
    printf("time_to_burnout: %ld\n", p->time_to_burnout);
    printf("time_to_compile: %ld\n", p->time_to_compile);
    printf("time_to_debug: %ld\n", p->time_to_debug);
    printf("time_to_refactor: %ld\n", p->time_to_refactor);
    printf("number_of_compiles_required: %ld\n",
        p->number_of_compiles_required);
    printf("dongle_cooldown: %ld\n", p->dongle_cooldown);
    printf("scheduler: %s\n",
        p->scheduler == SCHED_FIFO ? "fifo" : "edf");
}

int main(int argc, char **argv)
{
    t_params    params;

    if (parse_args(argc, argv, &params) != 0)
        return (1);
    print_params(&params);
    return (0);
}