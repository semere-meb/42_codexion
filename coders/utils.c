/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: semebrah <semebrah@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 18:07:24 by semebrah          #+#    #+#             */
/*   Updated: 2026/03/22 18:07:31 by semebrah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

int	isnumeric(char *str)
{
	size_t	i;

	i = -1;
	while (++i < strlen(str))
		if (str[i] < '0' || str[i] > '9')
			return (0);
	return (1);
}

t_data	*parse_arguments(int count, char **args)
{
	int		args_int[8];
	t_data	*data;
	int		i;

	if (count != 8)
		return (NULL);
	data = malloc(sizeof(t_data));
	if (!data)
		return (NULL);
	i = -1;
	while (++i < 8)
	{
		if (!isnumeric(args[i]))
			return (NULL);
		args_int[i] = atoi(args[i]);
	}
	data->number_of_coders = args_int[0];
	data->time_to_burnout = args_int[1];
	data->time_to_compile = args_int[2];
	data->time_to_debug = args_int[3];
	data->time_to_refactor = args_int[4];
	data->number_of_compiles_required = args_int[5];
	data->dongle_cooldown = args_int[6];
	data->scheduler = args_int[7];
	return (data);
}
