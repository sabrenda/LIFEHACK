/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchariss <tchariss@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/09 11:31:31 by tchariss          #+#    #+#             */
/*   Updated: 2021/04/11 18:19:58 by tchariss         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_atoi(const char *str)
{
	size_t			i;
	unsigned long	res;
	int				b;

	i = 0;
	res = 0;
	b = 1;
	while (str[i] == ' ' || str[i] == '\t' || str[i] == '\n'
		|| str[i] == '\r' || str[i] == '\f' || str[i] == '\v')
		i++;
	if (str[i] == '-' || str[i] == '+')
		if (str[i++] == '-')
			b = -1;
	while (str[i] >= 48 && str[i] <= 57)
		res = res * 10 + (str[i++] - '0');
	if (res > 9223372036854775807)
	{
		if (b == 1)
			return (-1);
		return (0);
	}
	return (b * res);
}
