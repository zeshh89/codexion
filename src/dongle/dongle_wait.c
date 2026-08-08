/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_wait.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jose-an2 <jose-an2@student.42barcelona.co  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/08 11:52:02 by jose-an2          #+#    #+#             */
/*   Updated: 2026/08/08 11:52:09 by jose-an2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <time.h>
#include "dongle.h"
#include "simulation.h"
#include "time_utils.h"

int	dongle_is_my_turn(t_dongle *dongle, int coder_id)
{
	if (heap_is_empty(&dongle->waiters))
		return (0);
	return (dongle->waiters.data[0].coder_id == coder_id);
}

void	dongle_remove_waiter(t_dongle *dongle, int coder_id)
{
	t_heap		tmp;
	t_heap_node	node;

	heap_init(&tmp, dongle->waiters.cmp);
	while (!heap_is_empty(&dongle->waiters))
	{
		heap_pop(&dongle->waiters, &node);
		if (node.coder_id != coder_id)
			heap_push(&tmp, node);
	}
	heap_destroy(&dongle->waiters);
	dongle->waiters = tmp;
}

static void	wait_for_turn(t_dongle *dongle, struct s_sim *sim, int coder_id)
{
	while (!sim_is_stopped(sim)
		&& (dongle->in_use || !dongle_is_my_turn(dongle, coder_id)))
		pthread_cond_wait(&dongle->cond, &dongle->mutex);
}

static void	wait_for_cooldown(t_dongle *dongle, struct s_sim *sim, int id)
{
	struct timespec	ts;

	while (!sim_is_stopped(sim)
		&& get_absolute_ms() < dongle->available_since)
	{
		ms_to_timespec(dongle->available_since, &ts);
		pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &ts);
		wait_for_turn(dongle, sim, id);
	}
}

int	dongle_acquire(t_dongle *dongle, struct s_sim *sim, t_heap_node node)
{
	pthread_mutex_lock(&dongle->mutex);
	if (heap_push(&dongle->waiters, node) != 0)
	{
		pthread_mutex_unlock(&dongle->mutex);
		return (-1);
	}
	wait_for_turn(dongle, sim, node.coder_id);
	wait_for_cooldown(dongle, sim, node.coder_id);
	if (sim_is_stopped(sim))
	{
		dongle_remove_waiter(dongle, node.coder_id);
		pthread_mutex_unlock(&dongle->mutex);
		return (-1);
	}
	heap_pop(&dongle->waiters, &node);
	dongle->in_use = 1;
	pthread_mutex_unlock(&dongle->mutex);
	return (0);
}
