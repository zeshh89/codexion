/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jose-an2 <jose-an2@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 11:38:55 by jose-an2          #+#    #+#             */
/*   Updated: 2026/07/25 11:38:58 by jose-an2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "simulation.h"
#include "time_utils.h"

static t_heap_cmp	pick_cmp(t_scheduler scheduler)
{
	if (scheduler == SCHEDULER_FIFO)
		return (cmp_fifo);
	return (cmp_edf);
}

int	sim_init(t_sim *sim, t_params *params)
{
	int	i;
	int	n;

	n = params->number_of_coders;
	sim->params = *params;
	sim->start_time = get_absolute_ms();
	sim->stop = 0;
	sim->dongles = malloc(sizeof(t_dongle) * n);
	sim->coders = malloc(sizeof(t_coder) * n);
	if (sim->dongles == NULL || sim->coders == NULL)
		return (-1);
	if (pthread_mutex_init(&sim->log_mutex, NULL) != 0)
		return (-1);
	if (pthread_mutex_init(&sim->status_mutex, NULL) != 0)
		return (-1);
	i = 0;
	while (i < n)
	{
		if (dongle_init(&sim->dongles[i], i, pick_cmp(params->scheduler)) != 0)
			return (-1);
		i++;
	}
	i = 0;
	while (i < n)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].index = i;
		sim->coders[i].state = STATE_WAITING;
		sim->coders[i].compiles_done = 0;
		sim->coders[i].last_compile_start = sim->start_time;
		sim->coders[i].sim = sim;
		t_dongle *right = &sim->dongles[i];
		t_dongle *left = &sim->dongles[(i - 1 + n) % n];
		if (i % 2 == 0)
		{
			sim->coders[i].dongle_a = right;
			sim->coders[i].dongle_b = left;
		}
		else
		{
			sim->coders[i].dongle_a = left;
			sim->coders[i].dongle_b = right;
		}
		i++;
	}
	return (0);
}

void	sim_destroy(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->params.number_of_coders)
	{
		dongle_destroy(&sim->dongles[i]);
		i++;
	}
	free(sim->dongles);
	free(sim->coders);
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_mutex_destroy(&sim->status_mutex);
}
