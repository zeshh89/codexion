/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation_coders.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jose-an2 <jose-an2@student.42barcelona.co  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/08 11:59:57 by jose-an2          #+#    #+#             */
/*   Updated: 2026/08/08 12:00:25 by jose-an2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "simulation.h"
#include "time_utils.h"

static void	assign_dongles(t_coder *coder, t_dongle *first, t_dongle *second)
{
	coder->dongle_a = first;
	coder->dongle_b = second;
}

void	init_coders(t_sim *sim, int n)
{
	int			i;
	t_dongle	*right;
	t_dongle	*left;

	i = 0;
	while (i < n)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].index = i;
		sim->coders[i].state = STATE_WAITING;
		sim->coders[i].compiles_done = 0;
		sim->coders[i].last_compile_start = sim->start_time;
		sim->coders[i].sim = sim;
		right = &sim->dongles[i];
		left = &sim->dongles[(i - 1 + n) % n];
		if (i % 2 == 0)
			assign_dongles(&sim->coders[i], right, left);
		else
			assign_dongles(&sim->coders[i], left, right);
		i++;
	}
}

int	sim_init(t_sim *sim, t_params *params)
{
	int	n;

	n = params->number_of_coders;
	sim->params = *params;
	sim->start_time = get_absolute_ms();
	sim->stop = 0;
	if (alloc_arrays(sim, n) != 0)
		return (-1);
	if (init_dongles(sim, n) != 0)
		return (-1);
	init_coders(sim, n);
	return (0);
}
