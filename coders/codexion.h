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
# include <stdbool.h>
# include <sys/time.h>
# include <unistd.h>

# ifndef HEAP_SIZE
#  define HEAP_SIZE 2
# endif

typedef struct s_state	t_state;

typedef struct s_minheap
{
	int					length;
	void				*array[HEAP_SIZE];
	int					(*cmp)(void *, void *);
}						t_minheap;

typedef struct s_args
{
	int					number_of_coders;
	int					time_to_burnout;
	int					time_to_compile;
	int					time_to_debug;
	int					time_to_refactor;
	int					compiles_todo;
	int					dongle_cooldown;
	char				*scheduler;
}						t_args;

typedef struct s_dongle
{
	int					idx;
	long				last_used;
	t_minheap			*heap;
	pthread_mutex_t		condmutex;
	pthread_cond_t		cond;
}						t_dongle;

typedef struct s_coder
{
	int					idx;
	pthread_t			thread;
	t_dongle			*right_dongle;
	t_dongle			*left_dongle;
	long				last_compile;
	int					compiles_done;
	t_state				*state;
	pthread_mutex_t		info_mutex;
}						t_coder;

typedef struct s_state
{
	t_args				args;
	t_coder				*coders;
	t_dongle			*dongles;
	long				start;
	bool				is_over;
	pthread_mutex_t		over_mutex;
	pthread_mutex_t		print_mutex;
	pthread_t			monitor;
}						t_state;

typedef struct s_entry
{
	t_coder				*coder;
	long				queued_at;
	long				deadline;
}						t_entry;

// utils.c
typedef struct s_state	t_state;
t_args					*parse_arguments(int count, char **args);
long					now(void);
void					print(char *str, t_state *state, int coder_idx);

// initialization.c
t_state					init_state(t_args *args);
t_dongle				*init_dongles(t_args *args, t_state *state);
t_coder					*init_coders(t_args *args, t_state *state);
t_minheap				*init_heap(int (*cmp)(void *, void *));

// heap.c
void					*peek(t_minheap *heap);
void					enqueue(t_minheap *heap, void *val);
void					*dequeue(t_minheap *heap);

// coder.c
void					*coder_routine(void *param);

// monitor.c
bool					is_over(t_state *state);
void					*monitor_routine(void *param);

// schedulers.c
int						fifo(void *t1, void *t2);
int						edf(void *t1, void *t2);

// main.c
int						cleanup(t_args *args, t_coder *coders,
							t_dongle *dongles);
int						main(int argc, char **argv);

#endif
