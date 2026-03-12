/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   options.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 14:09:50 by vzurera-          #+#    #+#             */
/*   Updated: 2026/03/12 14:53:42 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#pragma region "Includes"

	#include <stddef.h>

#pragma endregion

#pragma region "Structures"

typedef struct s_options {
	char	file[4096];
	char	log_in[4096];
	char	log_out[4096];
	char	log_io[4096];
	char	log_timing[4096];
	char	log_format[256];
	char	command[4096];
	char	echo[256];
	size_t	limit_size;
	int		append;
	int		ret;
	int		flush;
	int		force;
	int		quiet;
} t_options;

#pragma endregion

#pragma region "Methods"

	int	options_parse(t_options *options, int argc, const char **argv);

#pragma endregion
