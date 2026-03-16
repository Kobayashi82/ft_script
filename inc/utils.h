/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 12:21:57 by vzurera-          #+#    #+#             */
/*   Updated: 2026/03/16 15:10:40 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#pragma region "Includes"

	#include <unistd.h>

#pragma endregion

#pragma region "Methods"

	size_t	ft_strlen(const char *str);
	size_t	ft_strlcpy(char *dst, const char *src, size_t dstsize);
	char	*ft_strchr(const char *s, int c);
	int		ft_strcmp(const char *s1, const char *s2);
	int		ft_strncmp(const char *s1, const char *s2, size_t n);
	void	*ft_memset(void *b, int c, int len);
	int		ft_isspace(int c);
	int		ft_atos(const char *str, size_t *result);
	void	itoa_buffered(int number, char *buffer);

#pragma endregion
