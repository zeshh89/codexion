/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jose-an2 <jose-an2@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 11:37:03 by jose-an2          #+#    #+#             */
/*   Updated: 2026/07/25 11:37:06 by jose-an2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <time.h>
#include "dongle.h"
#include "simulation.h"  /* aca si, para acceder a sim->stop y status_mutex */
#include "time_utils.h"

int	dongle_init(t_dongle *dongle, int id, t_heap_cmp cmp)
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

void	dongle_destroy(t_dongle *dongle)
{
	pthread_cond_destroy(&dongle->cond);
	pthread_mutex_destroy(&dongle->mutex);
	heap_destroy(&dongle->waiters);
}

void	dongle_release(t_dongle *dongle, long release_time, long cooldown_ms)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->in_use = 0;
	dongle->available_since = release_time + cooldown_ms;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}
