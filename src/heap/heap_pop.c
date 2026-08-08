/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_pop.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jose-an2 <jose-an2@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/08 17:49:20 by jose-an2          #+#    #+#             */
/*   Updated: 2026/08/08 17:49:29 by jose-an2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "heap.h"

static void	swap_nodes(t_heap_node *a, t_heap_node *b)
{
	t_heap_node	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

static void	sift_down(t_heap *heap, int i)
{
	int	left;
	int	right;
	int	best;

	while (1)
	{
		left = 2 * i + 1;
		right = 2 * i + 2;
		best = i;
		if (left < heap->size
			&& heap->cmp(&heap->data[left], &heap->data[best]))
			best = left;
		if (right < heap->size
			&& heap->cmp(&heap->data[right], &heap->data[best]))
			best = right;
		if (best == i)
			break ;
		swap_nodes(&heap->data[i], &heap->data[best]);
		i = best;
	}
}

int	heap_pop(t_heap *heap, t_heap_node *out)
{
	if (heap_is_empty(heap))
		return (-1);
	*out = heap->data[0];
	heap->size--;
	heap->data[0] = heap->data[heap->size];
	if (heap->size > 0)
		sift_down(heap, 0);
	return (0);
}
