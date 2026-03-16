/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 12:21:40 by vzurera-          #+#    #+#             */
/*   Updated: 2026/03/16 15:10:19 by vzurera-         ###   ########.fr       */
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

	size_t ft_strlcpy(char *dst, const char *src, size_t dstsize) {
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

	int ft_strncmp(const char *s1, const char *s2, size_t n) {
		unsigned char	*str1;
		unsigned char	*str2;
		size_t				i;

		if (!s1 || !s2) return (-1);
		str1 = (unsigned char *)s1;
		str2 = (unsigned char *)s2;
		i = 0;
		while (*str1 && (*str1 == *str2) && i < n && n > 0) {
			++str1;
			++str2;
			++i;
		}
		if ((!*str1 && !*str2) || i == n) return (0);
		return (*str1 - *str2);
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

#pragma region "ISSPACE"

	int ft_isspace(int c) { return ((c >= 9 && c <= 13) || c == 32); }

#pragma endregion

#pragma region "ATOS"

	int ft_atos(const char *str, size_t *result) {
		size_t	value = 0;
		int		i = 0;

		if (!str) return (1);

		while (ft_isspace(str[i])) i++;
		if (str[i] == '+') i++;
		if (!str[i]) return (1);

		while (str[i]) {
			if (str[i] < '0' || str[i] > '9')					return (1);
			if (value > (((size_t)-1) - (str[i] - '0')) / 10)	return (2);

			value = value * 10 + (str[i] - '0');
			i++;
		}

		*result = value;
		return (0);
	}

#pragma endregion

#pragma region "ITOA BUFFERED"

	void itoa_buffered(int number, char *buffer) {
		char	digits[12];
		int		i = 0;

		if (number == 0) {
			buffer[0] = '0';
			buffer[1] = '\0';
			return;
		}

		while (number > 0) {
			digits[i++] = '0' + (number % 10);
			number /= 10;
		}

		int tmp = 0;
		while (tmp < i) {
			buffer[tmp] = digits[i - tmp - 1];
			tmp++;
		}
		buffer[tmp] = '\0';
	}

#pragma endregion
