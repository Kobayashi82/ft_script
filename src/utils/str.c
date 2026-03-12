/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 12:21:40 by vzurera-          #+#    #+#             */
/*   Updated: 2026/03/12 15:11:58 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "utils.h"

#pragma endregion

#pragma region "STRLEN"

	size_t ft_strlen(const char *str) {
		size_t i = 0;

		while (str && str[i]) i++;

		return (i);
	}

#pragma endregion

#pragma region "STRLCPY"

	int ft_strlcpy(char *dst, const char *src, size_t dstsize) {
		size_t i = 0, srclen = 0;

		if (!src) return (0);
		while (src[srclen]) ++srclen;
		if (dstsize > 0) {
			while (src[i] && i < dstsize - 1) {
				dst[i] = src[i];
				++i;
			}
			dst[i] = '\0';
		}

		return (srclen);
	}

#pragma endregion

#pragma region "STRCHR"

	char *ft_strchr(const char *s, int c) {
		if (!s) return (NULL);
		while (*s) {
			if (*s == (char)c) break ;
			s++;
		}
		if (*s == (char)c) return ((char *)s);

		return (NULL);
	}

#pragma endregion

#pragma region "STRCMP"

	int ft_strcmp(const char *s1, const char *s2) {
		if (!s1 || !s2) return (-1);
		while (*s1 && (*s1 == *s2)) {
			s1++;
			s2++;
		}

		return (*(unsigned char *)s1 - *(unsigned char *)s2);
	}

#pragma endregion

#pragma region "MEMSET"

	void *ft_memset(void *b, int c, int len) {
		unsigned char *p = b;

		while (len--)
			*p++ = (unsigned char)c;

		return (b);
	}

#pragma endregion
