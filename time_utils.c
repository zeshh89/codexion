/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jose-an2 <jose-an2@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 11:39:23 by jose-an2          #+#    #+#             */
/*   Updated: 2026/07/25 11:39:26 by jose-an2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "time_utils.h"
#include <stddef.h>

long get_absolute_ms(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000L + tv.tv_usec / 1000L);
}

long get_relative_ms(long start_time)
{
    return (get_absolute_ms() - start_time);
}

void ms_to_timespec(long timestamp_ms, struct timespec *ts)
{
    ts->tv_sec = timestamp_ms / 1000;
    ts->tv_nsec = (timestamp_ms % 1000) * 1000000L;
}
