/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_status.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jose-an2 <jose-an2@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 11:31:45 by jose-an2          #+#    #+#             */
/*   Updated: 2026/07/25 11:31:48 by jose-an2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coder.h"

long	coder_get_deadline(t_coder *coder)
{
	long	last_start;

	pthread_mutex_lock(&coder->sim->status_mutex);
	last_start = coder->last_compile_start;
	pthread_mutex_unlock(&coder->sim->status_mutex);
	return (last_start + coder->sim->params.time_to_burnout);
}

void	coder_mark_compile_start(t_coder *coder, long now)
{
	pthread_mutex_lock(&coder->sim->status_mutex);
	coder->last_compile_start = now;
	coder->state = STATE_COMPILING;
	pthread_mutex_unlock(&coder->sim->status_mutex);
}

void	coder_set_state(t_coder *coder, t_coder_state state)
{
	pthread_mutex_lock(&coder->sim->status_mutex);
	coder->state = state;
	pthread_mutex_unlock(&coder->sim->status_mutex);
}

int	coder_compiles_left(t_coder *coder)
{
	int	done;

	pthread_mutex_lock(&coder->sim->status_mutex);
	done = coder->compiles_done;
	pthread_mutex_unlock(&coder->sim->status_mutex);
	return (done < coder->sim->params.number_of_compiles_required);
}
