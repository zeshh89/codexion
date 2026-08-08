/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jose-an2 <jose-an2@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 11:38:37 by jose-an2          #+#    #+#             */
/*   Updated: 2026/07/25 11:38:38 by jose-an2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include "parsing.h"

static void	print_usage(const char *prog)
{
	fprintf(stderr,
		"Usage: %s number_of_coders time_to_burnout time_to_compile "
		"time_to_debug time_to_refactor number_of_compiles_required "
		"dongle_cooldown scheduler\n",
		prog);
}

static void	fill_dests(t_params *params, long *dests[7])
{
	dests[0] = &params->number_of_coders;
	dests[1] = &params->time_to_burnout;
	dests[2] = &params->time_to_compile;
	dests[3] = &params->time_to_debug;
	dests[4] = &params->time_to_refactor;
	dests[5] = &params->number_of_compiles_required;
	dests[6] = &params->dongle_cooldown;
}

static int	parse_numbers(char **argv, long *dests[7])
{
	static const char	*names[7] = {
		"number_of_coders", "time_to_burnout", "time_to_compile",
		"time_to_debug", "time_to_refactor",
		"number_of_compiles_required", "dongle_cooldown"
	};
	int					i;

	i = 0;
	while (i < 7)
	{
		if (parse_strict_long(argv[i + 1], names[i], dests[i]) != 0)
			return (-1);
		i++;
	}
	return (0);
}

int	parse_args(int argc, char **argv, t_params *params)
{
	long	*dests[7];

	if (argc != 9)
	{
		print_usage(argv[0]);
		return (-1);
	}
	fill_dests(params, dests);
	if (parse_numbers(argv, dests) != 0)
		return (-1);
	if (parse_scheduler(argv[8], &params->scheduler) != 0)
		return (-1);
	if (check_semantics(params) != 0)
		return (-1);
	return (0);
}
