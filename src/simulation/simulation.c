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

int	sim_is_stopped(t_sim *sim)
{
	int	stop;

	pthread_mutex_lock(&sim->status_mutex);
	stop = sim->stop;
	pthread_mutex_unlock(&sim->status_mutex);
	return (stop);
}

void	sim_request_stop(t_sim *sim)
{
	pthread_mutex_lock(&sim->status_mutex);
	sim->stop = 1;
	pthread_mutex_unlock(&sim->status_mutex);
}
