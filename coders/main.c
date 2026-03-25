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

void	compile_op(t_coder *coder, t_state *state)
{
	// acquire right dongle
	pthread_mutex_lock(&coder->left_dongle->lock);
	coder->left_dongle->last_used = now();
	pthread_mutex_lock(&state->print_mutex);
	printf("%ld %d has taken a dongle\n", now() - state->start,
		coder->coder_id);
	pthread_mutex_unlock(&state->print_mutex);
	// acquire right dongle
	pthread_mutex_lock(&coder->right_dongle->lock);
	coder->right_dongle->last_used = now();
	pthread_mutex_lock(&state->print_mutex);
	printf("%ld %d has taken a dongle\n", now() - state->start,
		coder->coder_id);
	pthread_mutex_unlock(&state->print_mutex);
	coder->last_compile = now();
	pthread_mutex_lock(&state->print_mutex);
	printf("%ld %d is compiling\n", now() - state->start, coder->coder_id);
	pthread_mutex_unlock(&state->print_mutex);
	usleep(state->args.time_to_compile * 1000L);
	pthread_mutex_unlock(&coder->left_dongle->lock);
	pthread_mutex_unlock(&coder->right_dongle->lock);
	coder->compiles_done += 1;
}

void	debug_op(t_coder *coder, t_state *state)
{
	pthread_mutex_lock(&state->print_mutex);
	printf("%ld %d is debugging\n", now() - state->start, coder->coder_id);
	pthread_mutex_unlock(&state->print_mutex);
	usleep(state->args.time_to_debug * 1000L);
}

void	refactor_op(t_coder *coder, t_state *state)
{
	pthread_mutex_lock(&state->print_mutex);
	printf("%ld %d is refactoring\n", now() - state->start, coder->coder_id);
	pthread_mutex_unlock(&state->print_mutex);
	usleep(state->args.time_to_refactor * 1000L);
}

void	*run(void *param)
{
	t_coder	*coder;
	t_state	*state;
	int		i;

	coder = param;
	state = (t_state *)(coder->state);
	i = 0;
	while (i < state->args.number_of_compiles_required)
	{
		compile_op(coder, state);
		debug_op(coder, state);
		refactor_op(coder, state);
		i++;
	}
	return (NULL);
}

void	*monitor(void *param)
{
	(void)param;
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

int	main(int argc, char **argv)
{
	int			i;
	t_args		*args;
	t_coder		*coders;
	t_dongle	*dongles;
	t_state		state;

	args = parse_arguments(argc - 1, &argv[1]);
	if (!args)
		return (cleanup(args, NULL, NULL));
	coders = malloc(sizeof(t_coder) * args->number_of_coders);
	dongles = malloc(sizeof(t_dongle) * args->number_of_coders);
	if (!coders || !dongles)
		return (cleanup(args, coders, dongles));
	state.start = now();
	state.args = *args;
	state.is_over = 0;
	state.coders = coders;
	state.dongles = dongles;
	pthread_mutex_init(&state.print_mutex, NULL);
	pthread_mutex_init(&state.over_mutex, NULL);
	i = 0;
	while (i < args->number_of_coders)
	{
		pthread_mutex_init(&dongles[i].lock, NULL);
		dongles[i].last_used = state.start - args->dongle_cooldown;
		i++;
	}
	i = 0;
	while (i < args->number_of_coders)
	{
		coders[i].coder_id = i + 1;
		coders[i].compiles_done = 0;
		coders[i].last_compile = state.start;
		coders[i].left_dongle = &dongles[i];
		coders[i].right_dongle = &dongles[(i + 1) % args->number_of_coders];
		coders[i].state = &state;
		pthread_create(&coders[i].thread, NULL, run, &coders[i]);
		i++;
	}
	pthread_create(&state.monitor, NULL, monitor, &state);
	i = 0;
	while (i < args->number_of_coders)
	{
		pthread_join(coders[i].thread, NULL);
		i++;
	}
	pthread_join(state.monitor, NULL);
	cleanup(args, coders, dongles);
}
