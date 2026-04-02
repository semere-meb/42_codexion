/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: semebrah <semebrah@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 18:07:14 by semebrah          #+#    #+#             */
/*   Updated: 2026/03/22 18:07:31 by semebrah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	fifo(void *t1, void *t2)
{
	t_entry	*entry1;
	t_entry	*entry2;

	entry1 = t1;
	entry2 = t2;
	return (entry1->queued_at - entry2->queued_at);
}

int	edf(void *t1, void *t2)
{
	return (((t_entry *)t1)->deadline - ((t_entry *)t2)->deadline);
}
