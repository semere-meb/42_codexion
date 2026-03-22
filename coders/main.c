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
#include <stdio.h>
#include <stdlib.h>

int	main(int argc, char **argv)
{
	t_data	*data;

	data = parse_arguments(argc - 1, &argv[1]);
	if (!data)
		return (0);
	printf("d = [%d]\n", data->number_of_coders);
	printf("d = [%d]\n", data->time_to_burnout);
	printf("d = [%d]\n", data->time_to_compile);
	printf("d = [%d]\n", data->time_to_debug);
	printf("d = [%d]\n", data->time_to_refactor);
	printf("d = [%d]\n", data->number_of_compiles_required);
	printf("d = [%d]\n", data->dongle_cooldown);
	printf("d = [%d]\n", data->scheduler);
	if (data)
		free(data);
	data = NULL;
}
