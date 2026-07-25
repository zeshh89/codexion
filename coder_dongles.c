/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_dongles.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jose-an2 <jose-an2@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 11:31:29 by jose-an2          #+#    #+#             */
/*   Updated: 2026/07/25 11:31:32 by jose-an2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coder.h"
#include "time_utils.h"

int	coder_acquire_both_dongles(t_coder *coder, long request_time)
{
	long	deadline;

	deadline = coder_get_deadline(coder);
	if (dongle_acquire(coder->dongle_a, coder->sim, coder->id,
			request_time, deadline) != 0)
		return (-1);
	log_event(coder->sim, coder->id, "has taken a dongle");
	if (coder->dongle_a != coder->dongle_b)
	{
		if (dongle_acquire(coder->dongle_b, coder->sim, coder->id,
				request_time, deadline) != 0)
		{
			dongle_release(coder->dongle_a, get_absolute_ms(),
				coder->sim->params.dongle_cooldown);
			return (-1);
		}
		log_event(coder->sim, coder->id, "has taken a dongle");
	}
	return (0);
}

void	coder_release_both_dongles(t_coder *coder, long release_time)
{
	long	cooldown;

	cooldown = coder->sim->params.dongle_cooldown;
	dongle_release(coder->dongle_a, release_time, cooldown);
	if (coder->dongle_a != coder->dongle_b)
		dongle_release(coder->dongle_b, release_time, cooldown);
}
