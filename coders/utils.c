/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: semebrah <semebrah@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 18:07:14 by semebrah          #+#    #+#             */
/*   Updated: 2026/03/22 18:07:31 by semebrah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int	isnumeric(char *str)
{
	size_t	i;

	i = -1;
	while (++i < strlen(str))
		if (str[i] < '0' || str[i] > '9')
			return (0);
	return (1);
}

t_args	*parse_arguments(int count, char **args)
{
	int		args_int[7];
	t_args	*data;
	int		i;

	if (count != 8 || !(!strcmp(args[7], "fifo") || (!strcmp(args[7], "edf"))))
		return (NULL);
	data = malloc(sizeof(t_args));
	if (!data)
		return (NULL);
	i = -1;
	while (++i < 7)
	{
		if (!isnumeric(args[i]))
			return (free(data), NULL);
		args_int[i] = atoi(args[i]);
	}
	data->scheduler = args[7];
	data->number_of_coders = args_int[0];
	data->time_to_burnout = args_int[1];
	data->time_to_compile = args_int[2];
	data->time_to_debug = args_int[3];
	data->time_to_refactor = args_int[4];
	data->compiles_todo = args_int[5];
	data->dongle_cooldown = args_int[6];
	return (data);
}

long	now(void)
{
	struct timeval	current;

	gettimeofday(&current, NULL);
	return (current.tv_sec * 1000L + current.tv_usec / 1000);
}

void	print(char *str, t_state *state, int coder_idx)
{
	bool	is_over;

	pthread_mutex_lock(&state->over_mutex);
	is_over = state->is_over;
	pthread_mutex_unlock(&state->over_mutex);
	if (!is_over)
	{
		pthread_mutex_lock(&state->print_mutex);
		printf(str, now() - state->start, coder_idx);
		pthread_mutex_unlock(&state->print_mutex);
	}
}
