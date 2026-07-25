/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jose-an2 <jose-an2@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 11:38:20 by jose-an2          #+#    #+#             */
/*   Updated: 2026/07/25 11:38:21 by jose-an2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "monitor.h"
#include "time_utils.h"
#include <unistd.h>

#define MONITOR_POLL_US 500

static void wake_all_dongles(t_sim *sim)
{
    int i;

    i = 0;
    while (i < sim->params.number_of_coders)
    {
        pthread_mutex_lock(&sim->dongles[i].mutex);
        pthread_cond_broadcast(&sim->dongles[i].cond);
        pthread_mutex_unlock(&sim->dongles[i].mutex);
        i++;
    }
}


static int check_burnout(t_coder *coder, long now)
{
    long    deadline;
    int     burned;

    burned = 0;
    pthread_mutex_lock(&coder->sim->status_mutex);
    if (coder->state != STATE_COMPILING
        && coder->state != STATE_BURNED_OUT
        && coder->state != STATE_FINISHED)
    {
        deadline = coder->last_compile_start
            + coder->sim->params.time_to_burnout;
        if (now >= deadline)
        {
            coder->state = STATE_BURNED_OUT;
            burned = 1;
        }
    }
    pthread_mutex_unlock(&coder->sim->status_mutex);
    return (burned);
}

static int any_burned_out(t_sim *sim, long now)
{
    int i;

    i = 0;
    while (i < sim->params.number_of_coders)
    {
        if (check_burnout(&sim->coders[i], now))
        {
            log_event(sim, sim->coders[i].id, "burned out");
            return (1);
        }
        i++;
    }
    return (0);
}

static int should_stop(t_sim *sim)
{
    int stop;

    pthread_mutex_lock(&sim->status_mutex);
    stop = sim->stop;
    pthread_mutex_unlock(&sim->status_mutex);
    return (stop);
}

void *monitor_routine(void *arg)
{
    t_sim   *sim;
    long    now;

    sim = (t_sim *)arg;
    while (!should_stop(sim))
    {
        now = get_absolute_ms();
        if (any_burned_out(sim, now))
        {
            pthread_mutex_lock(&sim->status_mutex);
            sim->stop = 1;
            pthread_mutex_unlock(&sim->status_mutex);
            wake_all_dongles(sim);
            break;
        }
        usleep(MONITOR_POLL_US);
    }
    return (NULL);
}
