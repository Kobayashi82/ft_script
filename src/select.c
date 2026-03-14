/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   select.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 19:00:27 by vzurera-          #+#    #+#             */
/*   Updated: 2026/03/14 19:29:27 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "script.h"

	#include <sys/select.h>						// select(), fd_set, FD_ZERO, FD_SET, FD_ISSET
	#include <fcntl.h>							// open()
	#include <errno.h>							// errno

#pragma endregion

#pragma region "Select"

	static int open_file(int *fd, char *file, int append, size_t *size) {
		if (append)	*fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0644);
		else		*fd = open(file, O_WRONLY | O_CREAT | O_TRUNC,  0644);

		if (*fd != -1 && script.options.size) {
			(void) size;
			// set *size;
		}

		return (*fd == -1);
	}

	int select_loop() {
		fd_set	read_fds;
		char	buffer[4096];
		ssize_t	readed;
		int		in_fd, out_fd, io_fd, time_fd;
		size_t	in_size, out_size, io_size, time_size;
		in_fd = out_fd = io_fd = time_fd = -1;
		in_size = out_size = io_size = time_size = 0;

		int ret = 0;
		if (!ret && script.options.log_in)		ret = open_file(&in_fd,   script.options.in,   script.options.append, &in_size);
		if (!ret && script.options.log_out)		ret = open_file(&out_fd,  script.options.out,  script.options.append, &out_size);
		if (!ret && script.options.log_io)		ret = open_file(&io_fd,   script.options.io,   script.options.append, &io_size);
		if (!ret && script.options.log_time)	ret = open_file(&time_fd, script.options.time, script.options.append, &time_size);
		if (ret) {
			// failed
			if (in_fd   != -1)	close(in_fd);
			if (out_fd  != -1)	close(out_fd);
			if (io_fd   != -1)	close(io_fd);
			if (time_fd != -1)	close(time_fd);
			return (1);
		}

		if (!script.options.quiet) {
			// Mensaje de inicio
			if (out_fd != -1) {
				write(out_fd, "", 0);
				if (script.options.flush) fsync(out_fd);
				if (script.options.size && out_size >= script.options.size)		ret = 3;
			}
			if (io_fd != -1) {
				write(io_fd, "", 0);
				if (script.options.flush) fsync(io_fd);
				if (script.options.size && io_size >= script.options.size)		ret = 3;
			}
			if (time_fd != -1) {
				// write(time_fd, buffer, readed);
				// guardar como O
				if (script.options.flush) fsync(time_fd);
				if (script.options.size && time_size >= script.options.size)	ret = 3;
			}
		}


		while (!ret && script.shell_running) {
			FD_ZERO(&read_fds);
			FD_SET(STDIN_FILENO, &read_fds);
			FD_SET(script.master_fd, &read_fds);

			if (select(script.master_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
				if (errno == EINTR) continue;
				ret = 2; break;
			}

			// Debe salirse despues de vaciar los FDs

			if (FD_ISSET(STDIN_FILENO, &read_fds)) {
				readed = read(STDIN_FILENO, buffer, sizeof(buffer));
				if (readed <= 0) { ret = 1; break; }
				write(script.master_fd, buffer, readed);
				if (in_fd != -1) {
					write(in_fd, buffer, readed);
					if (script.options.flush) fsync(in_fd);
					if (script.options.size && in_size >= script.options.size)		{ ret = 3; script.shell_running = 0; break; }
				}
				if (io_fd != -1) {
					write(io_fd, buffer, readed);
					if (script.options.flush) fsync(io_fd);
					if (script.options.size && io_size >= script.options.size)		{ ret = 3; script.shell_running = 0; break; }
				}
				if (time_fd != -1) {
					// write(time_fd, buffer, readed);
					// guardar como I
					if (script.options.flush) fsync(time_fd);
					if (script.options.size && time_size >= script.options.size)	{ ret = 3; script.shell_running = 0; break; }
				}
			}

			if (FD_ISSET(script.master_fd, &read_fds)) {
				readed = read(script.master_fd, buffer, sizeof(buffer));
				if (readed <= 0) { ret = 1; break; };
				write(STDOUT_FILENO, buffer, readed);
				if (out_fd != -1) {
					write(out_fd, buffer, readed);
					if (script.options.flush) fsync(out_fd);
					if (script.options.size && out_size >= script.options.size)		{ ret = 3; script.shell_running = 0; break; }
				}
				if (io_fd != -1) {
					write(io_fd, buffer, readed);
					if (script.options.flush) fsync(io_fd);
					if (script.options.size && io_size >= script.options.size)		{ ret = 3; script.shell_running = 0; break; }
				}
				if (time_fd != -1) {
					// write(time_fd, buffer, readed);
					// guardar como O
					if (script.options.flush) fsync(time_fd);
					if (script.options.size && time_size >= script.options.size)	{ ret = 3; script.shell_running = 0; break; }
				}
			}
		}

		if (!script.options.quiet) {
			// Mensaje de fin
			if (out_fd != -1) {
				write(out_fd, "", 0);
				if (script.options.flush) fsync(out_fd);
			}
			if (io_fd != -1) {
				write(io_fd, "", 0);
				if (script.options.flush) fsync(io_fd);
			}
			if (time_fd != -1) {
				// write(time_fd, buffer, readed);
				// guardar como O
				if (script.options.flush) fsync(time_fd);
			}
		}

		if (script.shell_running) {
			// Se ha salido con error
			if (in_fd   != -1)	close(in_fd);
			if (out_fd  != -1)	close(out_fd);
			if (io_fd   != -1)	close(io_fd);
			if (time_fd != -1)	close(time_fd);
			// Matar hijo
			close(script.master_fd);
		}

		// Falta capturar señal de hijo terminado

		return ((script.signal) ? 128 + script.signal : 0);
	}

#pragma endregion
