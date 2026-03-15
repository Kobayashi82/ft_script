/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 14:09:39 by vzurera-          #+#    #+#             */
/*   Updated: 2026/03/15 22:00:09 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "script.h"

	#include <sys/select.h>						// select(), fd_set, FD_ZERO, FD_SET, FD_ISSET
	#include <errno.h>							// errno

#pragma endregion

#pragma region "Variables"

	t_script g_script;

#pragma endregion

#pragma region "Select"

	static int select_loop() {
		fd_set			read_fds;
		char			buffer[4096];
		ssize_t			readed = 0;
		int				ret = 0;
		int				max_fd = (STDIN_FILENO > g_script.master_fd) ? STDIN_FILENO : g_script.master_fd;

		if (open_files()) return (1);
		log_start();

		while (g_script.shell_running) {
			FD_ZERO(&read_fds);
			FD_SET(STDIN_FILENO, &read_fds);
			FD_SET(g_script.master_fd, &read_fds);

			if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
				if (errno == EINTR) continue;
				ret = 2; break;
			}

			// User input: forward to PTY and log to files
			if (FD_ISSET(STDIN_FILENO, &read_fds)) {
				readed = read(STDIN_FILENO, buffer, sizeof(buffer));
				if (readed <= 0) { g_script.shell_running = 0; break; }
				write(g_script.master_fd, buffer, readed);
				if ((ret = log_files(buffer, readed, 0))) break;
			}

			// Shell output: forward to terminal and log to files
			if (FD_ISSET(g_script.master_fd, &read_fds)) {
				readed = read(g_script.master_fd, buffer, sizeof(buffer));
				if (readed <= 0) { ret = 1; break; }
				write(STDOUT_FILENO, buffer, readed);
				if ((ret = log_files(buffer, readed, 1))) break;
			}
		}

		// Kill shell if it is still running
		if (g_script.shell_running) shell_close();

		// Drain any remaining PTY output after shell exits
		struct timeval timeout;
		while (1) {
			timeout.tv_sec = 0;
			timeout.tv_usec = 0;
			FD_ZERO(&read_fds);
			FD_SET(g_script.master_fd, &read_fds);
			if (select(max_fd + 1, &read_fds, NULL, NULL, &timeout) <= 0)	break;
			if (!FD_ISSET(g_script.master_fd, &read_fds))						break;
			readed = read(g_script.master_fd, buffer, sizeof(buffer));
			if (readed <= 0)												break;
			write(STDOUT_FILENO, buffer, readed);
			if ((ret = log_files(buffer, readed, 1)))						break;
		}

		// log_end(ret);
		close_files();

		return (0);
	}

#pragma endregion

#pragma region "Main"

	int main(int argc, char **argv, char **env) {
		int ret = 0;

		if ((ret = parse_options(argc, argv, env)))	return (ret -1);
		if (shell_start())							return (1);
		if (signal_set())							return (1);
		if (!raw_mode_enable())						ret = select_loop();
		if (raw_mode_disable())						ret = 1;
		if (g_script.options.retur)					return (g_script.exit_code);

		return (ret);
	}

#pragma endregion
