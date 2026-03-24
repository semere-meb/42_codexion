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

void	compile_op(t_data *data)
{
	struct timeval	now;
	int				diff;

	gettimeofday(&now, NULL);
	diff = (now.tv_sec - data->start.tv_sec) * 1000L + (now.tv_usec
			- data->start.tv_usec) / 1000;
	printf("%d %d is compiling\n", diff, data->coder_id);
	usleep(data->args.time_to_compile * 1000L);
}

void	debug_op(t_data *data)
{
	struct timeval	now;
	int				diff;

	gettimeofday(&now, NULL);
	diff = (now.tv_sec - data->start.tv_sec) * 1000L + (now.tv_usec
			- data->start.tv_usec) / 1000;
	printf("%d %d is debugging\n", diff, data->coder_id);
	usleep(data->args.time_to_debug * 1000L);
}

void	refactor_op(t_data *data)
{
	struct timeval	now;
	int				diff;

	gettimeofday(&now, NULL);
	diff = (now.tv_sec - data->start.tv_sec) * 1000L + (now.tv_usec
			- data->start.tv_usec) / 1000;
	printf("%d %d is refactoring\n", diff, data->coder_id);
	usleep(data->args.time_to_refactor * 1000L);
}

void	*run(void *param)
{
	int		i;
	t_data	*data;

	data = param;
	i = 0;
	while (i < 5)
	{
		compile_op(data);
		debug_op(data);
		refactor_op(data);
		i++;
	}
	return (NULL);
}

int	main(int argc, char **argv)
{
	t_args			*args;
	pthread_t		*coders;
	pthread_mutex_t	*dongles;
	int				i;
	struct timeval	start;
	t_data			*data;

	args = parse_arguments(argc - 1, &argv[1]);
	coders = malloc(sizeof(pthread_t) * args->number_of_coders);
	dongles = malloc(sizeof(pthread_mutex_t) * args->number_of_coders);
	data = malloc(sizeof(t_data) * args->number_of_coders);
	i = 0;
	gettimeofday(&start, NULL);
	while (i < args->number_of_coders)
	{
		data[i].args = *args;
		data[i].coder_id = i + 1;
		data[i].start = start;
		pthread_create(&coders[i], NULL, run, (void *)&data[i]);
		pthread_mutex_init(&dongles[i], NULL);
		i++;
	}
	i = 0;
	while (i < args->number_of_coders)
	{
		pthread_join(coders[i], NULL);
		i++;
	}
}
