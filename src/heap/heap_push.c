/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_push.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jose-an2 <jose-an2@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/08 17:48:31 by jose-an2          #+#    #+#             */
/*   Updated: 2026/08/08 17:48:58 by jose-an2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "heap.h"

static void	swap_nodes(t_heap_node *a, t_heap_node *b)
{
	t_heap_node	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

static void	sift_up(t_heap *heap, int i)
{
	int	parent;

	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (heap->cmp(&heap->data[i], &heap->data[parent]))
		{
			swap_nodes(&heap->data[i], &heap->data[parent]);
			i = parent;
		}
		else
			break ;
	}
}

static int	heap_grow(t_heap *heap)
{
	t_heap_node	*new_data;
	int			new_capacity;
	int			i;

	new_capacity = heap->capacity * 2;
	new_data = malloc(sizeof(t_heap_node) * new_capacity);
	if (new_data == NULL)
		return (-1);
	i = 0;
	while (i < heap->size)
	{
		new_data[i] = heap->data[i];
		i++;
	}
	free(heap->data);
	heap->data = new_data;
	heap->capacity = new_capacity;
	return (0);
}

int	heap_push(t_heap *heap, t_heap_node node)
{
	if (heap->size == heap->capacity)
	{
		if (heap_grow(heap) != 0)
			return (-1);
	}
	heap->data[heap->size] = node;
	sift_up(heap, heap->size);
	heap->size++;
	return (0);
}
