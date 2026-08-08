/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation_init.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jose-an2 <jose-an2@student.42barcelona.co  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/08 11:59:23 by jose-an2          #+#    #+#             */
/*   Updated: 2026/08/08 11:59:38 by jose-an2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "simulation.h"
#include "time_utils.h"

t_heap_cmp	pick_cmp(t_scheduler scheduler)
{
	if (scheduler == SCHEDULER_FIFO)
		return (cmp_fifo);
	return (cmp_edf);
}

int	alloc_arrays(t_sim *sim, int n)
{
	sim->dongles = malloc(sizeof(t_dongle) * n);
	sim->coders = malloc(sizeof(t_coder) * n);
	if (sim->dongles == NULL || sim->coders == NULL)
		return (-1);
	if (pthread_mutex_init(&sim->log_mutex, NULL) != 0)
		return (-1);
	if (pthread_mutex_init(&sim->status_mutex, NULL) != 0)
		return (-1);
	return (0);
}

int	init_dongles(t_sim *sim, int n)
{
	int	i;

	i = 0;
	while (i < n)
	{
		if (dongle_init(&sim->dongles[i], i, pick_cmp(sim->params.scheduler))
			!= 0)
			return (-1);
		i++;
	}
	return (0);
}
