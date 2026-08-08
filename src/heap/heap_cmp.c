/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_cmp.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jose-an2 <jose-an2@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/08 17:46:19 by jose-an2          #+#    #+#             */
/*   Updated: 2026/08/08 17:46:31 by jose-an2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "heap.h"

int	cmp_fifo(const t_heap_node *a, const t_heap_node *b)
{
	return (a->request_time < b->request_time);
}

int	cmp_edf(const t_heap_node *a, const t_heap_node *b)
{
	if (a->deadline != b->deadline)
		return (a->deadline < b->deadline);
	return (a->request_time < b->request_time);
}
