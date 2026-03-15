/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   select.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 19:00:27 by vzurera-          #+#    #+#             */
/*   Updated: 2026/03/15 19:01:15 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "script.h"

	#include <sys/select.h>						// select(), fd_set, FD_ZERO, FD_SET, FD_ISSET
	#include <errno.h>							// errno

#pragma endregion

#pragma region "Select"

	int select_loop() {
		fd_set			read_fds;
		char			buffer[4096];
		ssize_t			readed = 0;
		int				ret = 0;

		if (open_files()) return (1);
		// log_start();

		int max_fd = (STDIN_FILENO > script.master_fd) ? STDIN_FILENO : script.master_fd;
		while (!ret && script.shell_running) {
			FD_ZERO(&read_fds);
			FD_SET(STDIN_FILENO, &read_fds);
			FD_SET(script.master_fd, &read_fds);

			if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
				if (errno == EINTR) continue;
				ret = 2; break;
			}

			if (FD_ISSET(STDIN_FILENO, &read_fds)) {
				readed = read(STDIN_FILENO, buffer, sizeof(buffer));
				if (readed <= 0) { script.shell_running = 0; break; }
				write(script.master_fd, buffer, readed);
				log_files(buffer, readed, 0);
			}

			if (FD_ISSET(script.master_fd, &read_fds)) {
				readed = read(script.master_fd, buffer, sizeof(buffer));
				if (readed <= 0) { ret = 1; break; }
				write(STDOUT_FILENO, buffer, readed);
				log_files(buffer, readed, 1);
			}
		}

		// Close shell
		if (script.shell_running) {
			shell_close();
			script.shell_running = 0;
		}

		// Drain master_fd
		struct timeval timeout;
		while (1) {
			timeout.tv_sec = 0;
			timeout.tv_usec = 0;
			FD_ZERO(&read_fds);
			FD_SET(script.master_fd, &read_fds);
			if (select(script.master_fd + 1, &read_fds, NULL, NULL, &timeout) <= 0) break;
			if (!FD_ISSET(script.master_fd, &read_fds)) break;
			readed = read(script.master_fd, buffer, sizeof(buffer));
			if (readed <= 0) break;
			write(STDOUT_FILENO, buffer, readed);
			log_files(buffer, readed, 1);
		}

		// log_end(ret);
		close_files();
		close(script.master_fd);

		return (script.exit_code);
	}

#pragma endregion
