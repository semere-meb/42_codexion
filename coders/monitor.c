/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: semebrah <semebrah@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 18:07:14 by semebrah          #+#    #+#             */
/*   Updated: 2026/03/22 18:07:31 by semebrah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	stop_prog(t_state *state, int i)
{
	pthread_mutex_lock(&state->over_mutex);
	state->is_over = true;
	pthread_mutex_unlock(&state->over_mutex);
	print("%ld %d has burned out\n", state, state->coders[i].idx);
}

bool	is_over(t_state *state)
{
	bool	is_over;

	pthread_mutex_lock(&state->over_mutex);
	is_over = state->is_over;
	pthread_mutex_unlock(&state->over_mutex);
	return (is_over);
}

void	*monitor_routine(void *param)
{
	t_state	*state;
	int		i;
	long	last;
	int		done_coders;

	state = param;
	done_coders = 0;
	while (done_coders < state->args.number_of_coders && !is_over(state))
	{
		done_coders = 0;
		i = -1;
		while (++i < state->args.number_of_coders)
		{
			pthread_mutex_lock(&state->coders[i].info_mutex);
			last = state->coders[i].last_compile;
			if (state->coders[i].compiles_done == state->args.compiles_todo)
				done_coders++;
			pthread_mutex_unlock(&state->coders[i].info_mutex);
			if (now() - last >= state->args.time_to_burnout)
				return (stop_prog(state, i), NULL);
		}
		usleep(1000);
	}
	return (NULL);
}
