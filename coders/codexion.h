/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: semebrah <semebrah@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 18:07:06 by semebrah          #+#    #+#             */
/*   Updated: 2026/03/22 18:07:31 by semebrah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H
# include <pthread.h>
# include <sys/time.h>

typedef struct s_args
{
	int				number_of_coders;
	int				time_to_burnout;
	int				time_to_compile;
	int				time_to_debug;
	int				time_to_refactor;
	int				number_of_compiles_required;
	int				dongle_cooldown;
	char			*scheduler;
}					t_args;

typedef struct s_dongle
{
	pthread_mutex_t	lock;
	long			last_used;
}					t_dongle;

typedef struct s_coder
{
	int				coder_id;
	pthread_t		thread;
	t_dongle		*right_dongle;
	t_dongle		*left_dongle;
	long			last_compile;
	int				compiles_done;
	void			*state;
}					t_coder;

typedef struct s_state
{
	t_args			args;
	t_coder			*coders;
	t_dongle		*dongles;
	long			start;
	int				is_over;
	pthread_mutex_t	over_mutex;
	pthread_mutex_t	print_mutex;
	pthread_t		monitor;
}					t_state;

t_args				*parse_arguments(int count, char **args);

#endif
