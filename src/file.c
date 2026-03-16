/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 17:55:58 by vzurera-          #+#    #+#             */
/*   Updated: 2026/03/16 11:48:28 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "script.h"

	#include <sys/stat.h>						// fstat(), struct stat
	#include <fcntl.h>							// open()

#pragma endregion

#pragma region "Open File"

	static int open_file(int *fd, char *file, int append, size_t *size) {
		struct stat	st;

		if (append)	*fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0644);
		else		*fd = open(file, O_WRONLY | O_CREAT | O_TRUNC,  0644);

		if (*fd != -1 && g_script.options.size) {
			if (append && fstat(*fd, &st) == 0)	*size = (size_t)st.st_size;
			else								*size = 0;
		}

		if (*fd == -1) {
			write(STDERR_FILENO, "ft_script: cannot open '", 24);
			write(STDERR_FILENO, file, ft_strlen(file));
			write(STDERR_FILENO, "'\n", 2);
			return (1);
		}

		return (0);
	}

#pragma endregion

#pragma region "Open Files"

	int open_files() {
		int  ret = 0;

		g_script.in_fd   = g_script.out_fd   = g_script.time_fd   = -1;
		g_script.in_size = g_script.out_size = g_script.time_size = 0;

		if (!ret && g_script.options.log_in)	ret = open_file(&g_script.in_fd,   g_script.options.in,   g_script.options.append, &g_script.in_size);
		if (!ret && g_script.options.log_out)	ret = open_file(&g_script.out_fd,  g_script.options.out,  g_script.options.append, &g_script.out_size);
		if (!ret && g_script.options.log_time)	ret = open_file(&g_script.time_fd, g_script.options.time, g_script.options.append, &g_script.time_size);
		if (ret) {
			if (g_script.in_fd		!= -1) close(g_script.in_fd);
			if (g_script.out_fd		!= -1) close(g_script.out_fd);
			if (g_script.time_fd	!= -1) close(g_script.time_fd);
			if (g_script.master_fd	!= -1) close(g_script.master_fd);
			return (1);
		}	

		return (0);
	}

#pragma endregion

#pragma region "Close Files"

	void close_files() {
		if (g_script.in_fd		!= -1) close(g_script.in_fd);
		if (g_script.out_fd		!= -1) close(g_script.out_fd);
		if (g_script.time_fd	!= -1) close(g_script.time_fd);
		if (g_script.master_fd	!= -1) close(g_script.master_fd);
	}

#pragma endregion
