/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jose-an2 <jose-an2@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 11:38:41 by jose-an2          #+#    #+#             */
/*   Updated: 2026/07/25 11:38:47 by jose-an2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#ifndef PARSING_H
# define PARSING_H

typedef enum e_scheduler
{
	SCHEDULER_FIFO,
	SCHEDULER_EDF
}	t_scheduler;

typedef struct s_params
{
	long		number_of_coders;
	long		time_to_burnout;
	long		time_to_compile;
	long		time_to_debug;
	long		time_to_refactor;
	long		number_of_compiles_required;
	long		dongle_cooldown;
	t_scheduler	scheduler;
}	t_params;

int	parse_args(int argc, char **argv, t_params *params);
int	parse_strict_long(const char *arg, const char *name, long *out);
int	parse_scheduler(const char *arg, t_scheduler *out);
int	check_semantics(const t_params *p);

#endif
