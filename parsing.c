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
#include <string.h>
#include "parsing.h"

#define MAX_LONG 2147483647L

static int	is_all_digits(const char *s)
{
	int	i;

	if (s == NULL || s[0] == '\0')
		return (0);
	i = 0;
	while (s[i])
	{
		if (s[i] < '0' || s[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

static int	parse_strict_long(const char *arg, const char *name, long *out)
{
	long	result;
	int		i;

	if (arg[0] == '-')
	{
		fprintf(stderr,
			"Error: %s must not be negative (got \"%s\")\n", name, arg);
		return (-1);
	}
	if (!is_all_digits(arg))
	{
		fprintf(stderr,
			"Error: %s must be a valid non-negative integer (got \"%s\")\n",
			name, arg);
		return (-1);
	}
	result = 0;
	i = 0;
	while (arg[i])
	{
		if (result > (MAX_LONG - (arg[i] - '0')) / 10)
		{
			fprintf(stderr,
				"Error: %s is out of range (got \"%s\")\n", name, arg);
			return (-1);
		}
		result = result * 10 + (arg[i] - '0');
		i++;
	}
	*out = result;
	return (0);
}

static int parse_scheduler(const char *arg, t_scheduler *out)
{
	if (strcmp(arg, "fifo") == 0)
	{
		*out = SCHEDULER_FIFO;
		return (0);
	}
	if (strcmp(arg, "edf") == 0)
	{
		*out = SCHEDULER_EDF;
		return (0);
	}
	fprintf(stderr,
		"Error: scheduler must be exactly \"fifo\" or \"edf\" (got \"%s\")\n",
		arg);
	return (-1);
}

static int	check_semantics(const t_params *p)
{
	if (p->number_of_coders < 1)
	{
		fprintf(stderr,
			"Error: number_of_coders must be at least 1\n");
		return (-1);
	}
	if (p->time_to_burnout < 1)
	{
		fprintf(stderr,
			"Error: time_to_burnout must be at least 1\n");
		return (-1);
	}
	return (0);
}

int	parse_args(int argc, char **argv, t_params *params)
{
	static const char	*names[7] = {
		"number_of_coders", "time_to_burnout", "time_to_compile",
		"time_to_debug", "time_to_refactor",
		"number_of_compiles_required", "dongle_cooldown"
	};
	long				*dests[7];
	int					i;

	if (argc != 9)
	{
		fprintf(stderr,
			"Usage: %s number_of_coders time_to_burnout time_to_compile "
			"time_to_debug time_to_refactor number_of_compiles_required "
			"dongle_cooldown scheduler\n",
			argv[0]);
		return (-1);
	}
	dests[0] = &params->number_of_coders;
	dests[1] = &params->time_to_burnout;
	dests[2] = &params->time_to_compile;
	dests[3] = &params->time_to_debug;
	dests[4] = &params->time_to_refactor;
	dests[5] = &params->number_of_compiles_required;
	dests[6] = &params->dongle_cooldown;
	i = 0;
	while (i < 7)
	{
		if (parse_strict_long(argv[i + 1], names[i], dests[i]) != 0)
			return (-1);
		i++;
	}
	if (parse_scheduler(argv[8], &params->scheduler) != 0)
		return (-1);
	if (check_semantics(params) != 0)
		return (-1);
	return (0);
}
