/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jose-an2 <jose-an2@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 11:38:08 by jose-an2          #+#    #+#             */
/*   Updated: 2026/07/25 11:38:11 by jose-an2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include "parsing.h"
#include "simulation.h"
#include "coder.h"
#include "monitor.h"

static int launch_threads(t_sim *sim, pthread_t *monitor_thread)
{
    int i;
    int n;

    n = sim->params.number_of_coders;
    if (pthread_create(monitor_thread, NULL, monitor_routine, sim) != 0)
        return (-1);
    i = 0;
    while (i < n)
    {
        if (pthread_create(&sim->coders[i].thread, NULL,
                coder_routine, &sim->coders[i]) != 0)
            return (-1);
        i++;
    }
    return (0);
}

static void join_all(t_sim *sim, pthread_t monitor_thread)
{
    int i;
    int n;

    n = sim->params.number_of_coders;
    i = 0;
    while (i < n)
    {
        pthread_join(sim->coders[i].thread, NULL);
        i++;
    }
    pthread_mutex_lock(&sim->status_mutex);
    sim->stop = 1;
    pthread_mutex_unlock(&sim->status_mutex);
    pthread_join(monitor_thread, NULL);
}

int main(int argc, char **argv)
{
    t_params    params;
    t_sim       sim;
    pthread_t   monitor_thread;

    if (parse_args(argc, argv, &params) != 0)
        return (1);

    if (sim_init(&sim, &params) != 0)
    {
        fprintf(stderr, "Error: failed to initialize simulation\n");
        sim_destroy(&sim);
        return (1);
    }

    if (launch_threads(&sim, &monitor_thread) != 0)
    {
        fprintf(stderr, "Error: failed to create threads\n");
        pthread_mutex_lock(&sim.status_mutex);
        sim.stop = 1;
        pthread_mutex_unlock(&sim.status_mutex);
        sim_destroy(&sim);
        return (1);
    }

    join_all(&sim, monitor_thread);
    sim_destroy(&sim);
    return (0);
}
