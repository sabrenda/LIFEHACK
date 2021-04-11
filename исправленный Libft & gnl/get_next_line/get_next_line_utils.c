/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sabrenda <sabrenda@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/30 13:31:45 by tchariss          #+#    #+#             */
/*   Updated: 2021/04/11 20:13:00 by sabrenda         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

int	norm_three_g(char **next_line, char **static_memory, char **line)
{
	*next_line = ft_strchr(*static_memory, '\n');
	if (*next_line)
	{
		**next_line = '\0';
		*line = ft_strdup(*static_memory);
		if (!(*line))
			return (-1);
		ft_strcopy(*static_memory, ++(*next_line));
		return (1);
	}
	*line = ft_strdup(*static_memory);
	if (!(*line))
		return (-1);
	return (0);
}

char	*ft_strdup(const char *s)
{
	char	*new_s;
	int		a;
	int		b;

	a = 0;
	b = 0;
	while (s[a])
		a++;
	new_s = malloc(sizeof(char) * (a + 1));
	if (new_s == NULL)
		return (NULL);
	a = 0;
	while (s[a] != '\0')
	{
		new_s[b] = s[a];
		a++;
		b++;
	}
	new_s[b] = '\0';
	return (new_s);
}

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*new_str;
	int		a;
	int		b;

	a = 0;
	b = 0;
	while (s1[a])
		a++;
	while (s2[b])
		b++;
	if (s1 == NULL || s2 == NULL)
		return (NULL);
	new_str = malloc(sizeof(char) * (a + b) + 1);
	if (new_str == NULL)
		return (NULL);
	a = -1;
	b = 0;
	while (s1[++a] != '\0')
		new_str[a] = s1[a];
	while (s2[b] != '\0')
		new_str[a++] = s2[b++];
	new_str[a] = '\0';
	return (new_str);
}

char	*ft_strchr(const char *s, int c)
{
	while (*s != '\0')
	{
		if (*s == c && c != '\0')
			return ((char *)s);
		s++;
	}
	return (0);
}

char	*ft_strcopy(char *s1, const char *s2)
{
	while (*s2 != '\0')
	{
		*s1++ = *s2++;
	}
	*s1 = '\0';
	return (s1);
}
