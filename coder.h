/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jose-an2 <jose-an2@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 11:30:46 by jose-an2          #+#    #+#             */
/*   Updated: 2026/07/25 11:30:55 by jose-an2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODER_H
# define CODER_H

# include "simulation.h"

int		coder_should_stop(t_sim *sim);
long	coder_get_deadline(t_coder *coder);
void	coder_mark_compile_start(t_coder *coder, long now);
void	coder_set_state(t_coder *coder, t_coder_state state);
int		coder_compiles_left(t_coder *coder);

int		coder_acquire_both_dongles(t_coder *coder, long request_time);
void	coder_release_both_dongles(t_coder *coder, long release_time);

void	*coder_routine(void *arg);

#endif
