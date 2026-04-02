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
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

int	cleanup(t_args *args, t_coder *coders, t_dongle *dongles)
{
	int	i;

	if (coders)
		free(coders);
	if (dongles)
	{
		i = 0;
		while (i < args->number_of_coders)
			free(dongles[i++].heap);
		free(dongles);
	}
	if (args)
		free(args);
	return (0);
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
		pthread_create(&state.coders[i].thread, NULL, coder_routine,
			&state.coders[i]);
	pthread_create(&state.monitor, NULL, monitor_routine, &state);
	i = -1;
	while (++i < args->number_of_coders)
		pthread_join(state.coders[i].thread, NULL);
	pthread_join(state.monitor, NULL);
	cleanup(args, state.coders, state.dongles);
}
