/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: semebrah <semebrah@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 18:07:14 by semebrah          #+#    #+#             */
/*   Updated: 2026/03/22 18:07:31 by semebrah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

long	now(void)
{
	struct timeval	current;

	gettimeofday(&current, NULL);
	return (current.tv_sec * 1000L + current.tv_usec / 1000);
}

void	print(char *str, t_state *state, int coder_idx)
{
	pthread_mutex_lock(&state->print_mutex);
	printf(str, now() - state->start, coder_idx + 1);
	pthread_mutex_unlock(&state->print_mutex);
}

bool	is_over(t_state *state)
{
	bool	is_over;

	pthread_mutex_lock(&state->over_mutex);
	is_over = state->is_over;
	pthread_mutex_unlock(&state->over_mutex);
	return (is_over);
}

void	compile_op(t_coder *coder, t_state *state)
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
	pthread_mutex_lock(&first->lock);
	first->last_used = now();
	print("%ld %d has taken a dongle\n", state, coder->idx);
	pthread_mutex_lock(&second->lock);
	second->last_used = now();
	print("%ld %d has taken a dongle\n", state, coder->idx);
	pthread_mutex_lock(&coder->info_mutex);
	coder->last_compile = now();
	coder->compiles_done += 1;
	pthread_mutex_unlock(&coder->info_mutex);
	print("%ld %d is compiling\n", state, coder->idx);
	usleep(state->args.time_to_compile * 1000L);
	pthread_mutex_unlock(&first->lock);
	pthread_mutex_unlock(&second->lock);
}

void	debug_op(t_coder *coder, t_state *state)
{
	print("%ld %d is debugging\n", state, coder->idx);
	usleep(state->args.time_to_debug * 1000L);
}

void	refactor_op(t_coder *coder, t_state *state)
{
	print("%ld %d is refactoring\n", state, coder->idx);
	usleep(state->args.time_to_refactor * 1000L);
}

void	*run(void *param)
{
	t_coder	*coder;
	t_state	*state;
	int		i;

	coder = param;
	state = coder->state;
	i = 0;
	while (i < state->args.number_of_compiles_required)
	{
		if (is_over(state))
			return (NULL);
		compile_op(coder, state);
		if (is_over(state))
			return (NULL);
		debug_op(coder, state);
		if (is_over(state))
			return (NULL);
		refactor_op(coder, state);
		i++;
	}
	return (NULL);
}

void	*monitor(void *param)
{
	t_state	*state;
	int		i;
	long	last;
	int		compiles_done;

	state = param;
	compiles_done = 0;
	while (compiles_done < state->args.number_of_compiles_required
		&& !is_over(state))
	{
		usleep(1000);
		i = -1;
		while (++i < state->args.number_of_coders)
		{
			pthread_mutex_lock(&state->coders[i].info_mutex);
			last = state->coders[i].last_compile;
			pthread_mutex_unlock(&state->coders[i].info_mutex);
			if (now() - last >= state->args.time_to_burnout)
			{
				pthread_mutex_lock(&state->over_mutex);
				state->is_over = true;
				pthread_mutex_unlock(&state->over_mutex);
				print("%ld %d has burned out\n", state, state->coders[i].idx);
				return (NULL);
			}
		}
		pthread_mutex_lock(&state->coders[i - 1].info_mutex);
		compiles_done = state->coders[i - 1].compiles_done;
		pthread_mutex_unlock(&state->coders[i - 1].info_mutex);
	}
	return (NULL);
}

int	cleanup(t_args *args, t_coder *coders, t_dongle *dongles)
{
	if (args)
		free(args);
	if (coders)
		free(coders);
	if (dongles)
		free(dongles);
	return (0);
}

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
		pthread_mutex_init(&dongles[i].lock, NULL);
		dongles[i].last_used = state->start - args->dongle_cooldown;
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

int	main(int argc, char **argv)
{
	int		i;
	t_args	*args;
	t_state	state;

	args = parse_arguments(argc - 1, &argv[1]);
	if (!args)
		return (cleanup(args, NULL, NULL));
	state = init_state(args);
	state.dongles = init_dongles(args, &state);
	state.coders = init_coders(args, &state);
	if (!state.coders || !state.dongles)
		return (cleanup(args, state.coders, state.dongles));
	i = -1;
	while (++i < args->number_of_coders)
		pthread_create(&state.coders[i].thread, NULL, run, &state.coders[i]);
	pthread_create(&state.monitor, NULL, monitor, &state);
	i = -1;
	while (++i < args->number_of_coders)
		pthread_join(state.coders[i].thread, NULL);
	pthread_join(state.monitor, NULL);
	cleanup(args, state.coders, state.dongles);
}
