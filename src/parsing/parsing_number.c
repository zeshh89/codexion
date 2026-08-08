/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_number.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jose-an2 <jose-an2@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/08 18:02:30 by jose-an2          #+#    #+#             */
/*   Updated: 2026/08/08 18:02:46 by jose-an2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
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

static int	validate_format(const char *arg, const char *name)
{
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
	return (0);
}

static int	accumulate_digits(const char *arg, const char *name, long *out)
{
	long	result;
	int		i;

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

int	parse_strict_long(const char *arg, const char *name, long *out)
{
	if (validate_format(arg, name) != 0)
		return (-1);
	if (accumulate_digits(arg, name, out) != 0)
		return (-1);
	return (0);
}
