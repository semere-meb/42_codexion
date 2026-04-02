/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initialization.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: semebrah <semebrah@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 18:07:14 by semebrah          #+#    #+#             */
/*   Updated: 2026/03/22 18:07:31 by semebrah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdlib.h>
#include <string.h>

t_state	init_state(t_args *args)
{
	t_state	state;

	state.start = now();
	state.args = *args;
	state.is_over = false;
	pthread_mutex_init(&state.print_mutex, NULL);
	pthread_mutex_init(&state.over_mutex, NULL);
	return (state);
}

t_dongle	*init_dongles(t_args *args, t_state *state)
{
	int			i;
	t_dongle	*dongles;

	dongles = malloc(sizeof(t_dongle) * args->number_of_coders);
	if (!dongles)
		return (NULL);
	i = -1;
	while (++i < args->number_of_coders)
	{
		dongles[i].idx = i;
		pthread_mutex_init(&dongles[i].condmutex, NULL);
		pthread_cond_init(&dongles[i].cond, NULL);
		dongles[i].last_used = state->start - args->dongle_cooldown;
		if (!strcmp(state->args.scheduler, "fifo"))
			dongles[i].heap = init_heap(fifo);
		else
			dongles[i].heap = init_heap(edf);
	}
	return (dongles);
}

t_coder	*init_coders(t_args *args, t_state *state)
{
	t_coder	*coders;
	int		i;

	coders = malloc(sizeof(t_coder) * args->number_of_coders);
	if (!coders)
		return (NULL);
	i = -1;
	while (++i < args->number_of_coders)
	{
		coders[i].idx = i;
		coders[i].compiles_done = 0;
		coders[i].last_compile = state->start;
		coders[i].left_dongle = &state->dongles[i];
		coders[i].right_dongle = &state->dongles[(i + 1)
			% args->number_of_coders];
		coders[i].state = state;
		pthread_mutex_init(&coders[i].info_mutex, NULL);
	}
	return (coders);
}

t_minheap	*init_heap(int (*cmp)(void *, void *))
{
	t_minheap	*heap;

	heap = malloc(sizeof(t_minheap));
	if (!heap)
		return (NULL);
	heap->length = 0;
	heap->cmp = cmp;
	return (heap);
}
