/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jose-an2 <jose-an2@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 11:31:10 by jose-an2          #+#    #+#             */
/*   Updated: 2026/07/25 11:31:18 by jose-an2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include "coder.h"
#include "time_utils.h"

static void	do_compile(t_coder *coder)
{
	long	now;

	now = get_absolute_ms();
	coder_mark_compile_start(coder, now);
	log_event(coder->sim, coder->id, "is compiling");
	usleep(coder->sim->params.time_to_compile * 1000);
	coder_release_both_dongles(coder, get_absolute_ms());
	pthread_mutex_lock(&coder->sim->status_mutex);
	coder->compiles_done++;
	pthread_mutex_unlock(&coder->sim->status_mutex);
}

static void	do_debug_and_refactor(t_coder *coder)
{
	coder_set_state(coder, STATE_DEBUGGING);
	log_event(coder->sim, coder->id, "is debugging");
	usleep(coder->sim->params.time_to_debug * 1000);
	if (sim_is_stopped(coder->sim))
		return ;
	coder_set_state(coder, STATE_REFACTORING);
	log_event(coder->sim, coder->id, "is refactoring");
	usleep(coder->sim->params.time_to_refactor * 1000);
}

static int	run_one_cycle(t_coder *coder)
{
	long	request_time;

	request_time = get_absolute_ms();
	if (coder_acquire_both_dongles(coder, request_time) != 0)
		return (-1);
	do_compile(coder);
	if (sim_is_stopped(coder->sim))
		return (-1);
	do_debug_and_refactor(coder);
	return (0);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	while (!sim_is_stopped(coder->sim) && coder_compiles_left(coder))
	{
		if (run_one_cycle(coder) != 0)
			break ;
	}
	if (!sim_is_stopped(coder->sim))
		coder_set_state(coder, STATE_FINISHED);
	return (NULL);
}
