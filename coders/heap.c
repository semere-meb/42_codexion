/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: semebrah <semebrah@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 18:07:14 by semebrah          #+#    #+#             */
/*   Updated: 2026/03/22 18:07:31 by semebrah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	sift_up(t_minheap *heap, int i, int (*cmp)(void *, void *))
{
	int		parent;
	void	*temp;

	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (cmp(heap->array[parent], heap->array[i]) > 0)
		{
			temp = heap->array[parent];
			heap->array[parent] = heap->array[i];
			heap->array[i] = temp;
			i = parent;
		}
		else
			break ;
	}
}

static void	sift_down(t_minheap *heap, int i, int (*cmp)(void *, void *))
{
	int		smallest;
	int		left;
	int		right;
	void	*temp;

	while (true)
	{
		smallest = i;
		left = 2 * i + 1;
		right = 2 * i + 2;
		if (left < heap->length && cmp(heap->array[left],
				heap->array[smallest]) < 0)
			smallest = left;
		if (right < heap->length && cmp(heap->array[right],
				heap->array[smallest]) < 0)
			smallest = right;
		if (smallest == i)
			break ;
		temp = heap->array[smallest];
		heap->array[smallest] = heap->array[i];
		heap->array[i] = temp;
		i = smallest;
	}
}

void	*peek(t_minheap *heap)
{
	if (!heap || !heap->length)
		return (NULL);
	return (heap->array[0]);
}

void	enqueue(t_minheap *heap, void *val)
{
	if (heap->length >= HEAP_SIZE)
		return ;
	heap->array[heap->length] = val;
	heap->length++;
	sift_up(heap, heap->length - 1, heap->cmp);
}

void	*dequeue(t_minheap *heap)
{
	void	*temp;

	if (!heap || !heap->length)
		return (NULL);
	temp = heap->array[0];
	heap->array[0] = heap->array[heap->length - 1];
	heap->length--;
	sift_down(heap, 0, heap->cmp);
	return (temp);
}
