/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_checks.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jose-an2 <jose-an2@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/08 18:03:00 by jose-an2          #+#    #+#             */
/*   Updated: 2026/08/08 18:03:09 by jose-an2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <string.h>
#include "parsing.h"

int	parse_scheduler(const char *arg, t_scheduler *out)
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

int	check_semantics(const t_params *p)
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
