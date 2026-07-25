/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jose-an2 <jose-an2@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 11:37:12 by jose-an2          #+#    #+#             */
/*   Updated: 2026/07/25 11:37:15 by jose-an2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DONGLE_H
#define DONGLE_H

#include <pthread.h>
#include "heap.h"

struct s_sim;

typedef struct s_dongle
{
    int             id;
    int             in_use;
    long            available_since;
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    t_heap          waiters;
}   t_dongle;

int     dongle_init(t_dongle *dongle, int id, t_heap_cmp cmp);
void    dongle_destroy(t_dongle *dongle);

int     dongle_acquire(
            t_dongle *dongle,
            struct s_sim *sim,
            int coder_id,
            long request_time,
            long deadline
        );

void    dongle_release(t_dongle *dongle, long release_time, long cooldown_ms);

#endif
