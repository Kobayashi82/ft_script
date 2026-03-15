/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 17:55:58 by vzurera-          #+#    #+#             */
/*   Updated: 2026/03/15 19:00:45 by vzurera-         ###   ########.fr       */
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

		if (*fd != -1 && script.options.size) {
			if (append && fstat(*fd, &st) == 0)	*size = (size_t)st.st_size;
			else								*size = 0;
		}

		return (*fd == -1);
	}

#pragma endregion

#pragma region "Open Files"

	int open_files() {
		int  ret = 0;

		script.in_fd   = script.out_fd   = script.io_fd   = script.time_fd   = -1;
		script.in_size = script.out_size = script.io_size = script.time_size = 0;

		if (!ret && script.options.log_in)		ret = open_file(&script.in_fd,   script.options.in,   script.options.append, &script.in_size);
		if (!ret && script.options.log_out)		ret = open_file(&script.out_fd,  script.options.out,  script.options.append, &script.out_size);
		if (!ret && script.options.log_io)		ret = open_file(&script.io_fd,   script.options.io,   script.options.append, &script.io_size);
		if (!ret && script.options.log_time)	ret = open_file(&script.time_fd, script.options.time, script.options.append, &script.time_size);
		if (ret) {
			if (script.in_fd   != -1)	close(script.in_fd);
			if (script.out_fd  != -1)	close(script.out_fd);
			if (script.io_fd   != -1)	close(script.io_fd);
			if (script.time_fd != -1)	close(script.time_fd);
			return (1);
		}	

		return (0);
	}

#pragma endregion

#pragma region "Close Files"

	void close_files() {
		if (script.in_fd   != -1)	close(script.in_fd);
		if (script.out_fd  != -1)	close(script.out_fd);
		if (script.io_fd   != -1)	close(script.io_fd);
		if (script.time_fd != -1)	close(script.time_fd);
	}

#pragma endregion
