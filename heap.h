#ifndef HEAP_H
#define HEAP_H

# define HEAP_INITIAL_CAPACITY 16

typedef struct s_heap_node
{
    int     coder_id;
    long    request_time;
    long    deadline;
}   t_heap_node;

typedef int (*t_heap_cmp)(const t_heap_node *a, const t_heap_node *b);

typedef struct s_heap
{
    t_heap_node *data;
    int         size;
    int         capacity;
    t_heap_cmp  cmp;
}   t_heap;

int     heap_init(t_heap *heap, t_heap_cmp cmp);
void    heap_destroy(t_heap *heap);
int     heap_push(t_heap *heap, t_heap_node node);
int     heap_pop(t_heap *heap, t_heap_node *out);
int     heap_is_empty(const t_heap *heap);

int     cmp_fifo(const t_heap_node *a, const t_heap_node *b);
int     cmp_edf(const t_heap_node *a, const t_heap_node *b);

#endif