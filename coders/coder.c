/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: semebrah <semebrah@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 18:07:14 by semebrah          #+#    #+#             */
/*   Updated: 2026/03/22 18:07:31 by semebrah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdlib.h>

static void	acquire_dongle(t_coder *coder, t_dongle *dongle)
{
	t_entry	*entry;
	long	remaining;

	entry = malloc(sizeof(t_entry));
	entry->coder = coder;
	entry->queued_at = now();
	pthread_mutex_lock(&coder->info_mutex);
	entry->deadline = coder->last_compile + coder->state->args.time_to_burnout;
	pthread_mutex_unlock(&coder->info_mutex);
	pthread_mutex_lock(&dongle->condmutex);
	enqueue(dongle->heap, entry);
	while (((t_entry *)peek(dongle->heap))->coder != coder)
		pthread_cond_wait(&dongle->cond, &dongle->condmutex);
	remaining = (dongle->last_used + coder->state->args.dongle_cooldown)
		- now();
	pthread_mutex_unlock(&dongle->condmutex);
	if (remaining > 0)
		usleep(remaining * 1000);
	print("%ld %d has taken a dongle\n", coder->state, coder->idx);
}

static void	release_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->condmutex);
	if (((t_entry *)peek(dongle->heap))->coder == coder)
	{
		dongle->last_used = now();
		free(dequeue(dongle->heap));
		pthread_cond_signal(&dongle->cond);
	}
	pthread_mutex_unlock(&dongle->condmutex);
}

static void	compile_op(t_coder *coder, t_state *state)
{
	t_dongle	*first;
	t_dongle	*second;

	if (coder->right_dongle->idx < coder->left_dongle->idx)
	{
		first = coder->left_dongle;
		second = coder->right_dongle;
	}
	else
	{
		first = coder->right_dongle;
		second = coder->left_dongle;
	}
	acquire_dongle(coder, first);
	acquire_dongle(coder, second);
	pthread_mutex_lock(&coder->info_mutex);
	coder->last_compile = now();
	coder->compiles_done += 1;
	pthread_mutex_unlock(&coder->info_mutex);
	print("%ld %d is compiling\n", state, coder->idx);
	usleep(state->args.time_to_compile * 1000L);
	release_dongle(coder, first);
	release_dongle(coder, second);
}

void	*coder_routine(void *param)
{
	t_coder	*coder;
	t_state	*state;
	int		i;

	coder = param;
	state = coder->state;
	i = 0;
	while (i < state->args.compiles_todo)
	{
		if (is_over(state))
			return (NULL);
		compile_op(coder, state);
		if (is_over(state))
			return (NULL);
		print("%ld %d is debugging\n", state, coder->idx);
		usleep(state->args.time_to_debug * 1000L);
		if (is_over(state))
			return (NULL);
		print("%ld %d is refactoring\n", state, coder->idx);
		usleep(state->args.time_to_refactor * 1000L);
		i++;
	}
	return (NULL);
}
