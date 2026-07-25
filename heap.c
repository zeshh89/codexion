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

int cmp_fifo(const t_heap_node *a, const t_heap_node *b)
{
    return (a->request_time < b->request_time);
}

int cmp_edf(const t_heap_node *a, const t_heap_node *b)
{
    if (a->deadline != b->deadline)
        return (a->deadline < b->deadline);
    return (a->request_time < b->request_time);
}

int heap_init(t_heap *heap, t_heap_cmp cmp)
{
    heap->data = malloc(sizeof(t_heap_node) * HEAP_INITIAL_CAPACITY);
    if (heap->data == NULL)
        return (-1);
    heap->size = 0;
    heap->capacity = HEAP_INITIAL_CAPACITY;
    heap->cmp = cmp;
    return (0);
}

void heap_destroy(t_heap *heap)
{
    free(heap->data);
    heap->data = NULL;
    heap->size = 0;
    heap->capacity = 0;
}

int heap_is_empty(const t_heap *heap)
{
    return (heap->size == 0);
}

static void swap_nodes(t_heap_node *a, t_heap_node *b)
{
    t_heap_node tmp;

    tmp = *a;
    *a = *b;
    *b = tmp;
}

static void sift_up(t_heap *heap, int i)
{
    int parent;

    while (i > 0)
    {
        parent = (i - 1) / 2;
        if (heap->cmp(&heap->data[i], &heap->data[parent]))
        {
            swap_nodes(&heap->data[i], &heap->data[parent]);
            i = parent;
        }
        else
            break;
    }
}

static void sift_down(t_heap *heap, int i)
{
    int left;
    int right;
    int best;

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
            break;
        swap_nodes(&heap->data[i], &heap->data[best]);
        i = best;
    }
}

static int heap_grow(t_heap *heap)
{
    t_heap_node *new_data;
    int         new_capacity;
    int         i;

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

int heap_push(t_heap *heap, t_heap_node node)
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

int heap_pop(t_heap *heap, t_heap_node *out)
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
