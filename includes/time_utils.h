/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time_utils.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jose-an2 <jose-an2@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 11:39:30 by jose-an2          #+#    #+#             */
/*   Updated: 2026/07/25 11:39:37 by jose-an2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#ifndef TIME_UTILS_H
# define TIME_UTILS_H

# include <sys/time.h>

long	get_absolute_ms(void);
long	get_relative_ms(long start_time);
void	ms_to_timespec(long timestamp_ms, struct timespec *ts);

#endif
