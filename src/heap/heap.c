/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jose-an2 <jose-an2@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 11:37:23 by jose-an2          #+#    #+#             */
/*   Updated: 2026/07/25 11:37:25 by jose-an2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "heap.h"

int	heap_init(t_heap *heap, t_heap_cmp cmp)
{
	heap->data = malloc(sizeof(t_heap_node) * HEAP_INITIAL_CAPACITY);
	if (heap->data == NULL)
		return (-1);
	heap->size = 0;
	heap->capacity = HEAP_INITIAL_CAPACITY;
	heap->cmp = cmp;
	return (0);
}

void	heap_destroy(t_heap *heap)
{
	free(heap->data);
	heap->data = NULL;
	heap->size = 0;
	heap->capacity = 0;
}

int	heap_is_empty(const t_heap *heap)
{
	return (heap->size == 0);
}
