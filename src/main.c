/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 14:09:39 by vzurera-          #+#    #+#             */
/*   Updated: 2026/03/14 18:28:05 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "script.h"

	#include <sys/select.h>						// select(), fd_set, FD_ZERO, FD_SET, FD_ISSET
	#include <fcntl.h>							// open()
	#include <errno.h>							// errno

#pragma endregion

#pragma region "Variables"

	t_script script;

#pragma endregion

#pragma region "Set ENV"

	static void set_env(char **env) {
		int i = 0;
		int found_shell = 0;
		int found_term = 0;
		for (; env[i] && i < 253; ++i) {
			if (!found_shell && !ft_strncmp(env[i], "SHELL=", 6)) {
				found_shell = 1;
				if (access(env[i] + 6, X_OK)) {
					script.env[i] = "SHELL=/bin/sh";
					script.shell_path = script.env[i] + 6;
					continue;
				}
				script.shell_path = env[i] + 6;
			}
			if (!found_term  && !ft_strncmp(env[i], "TERM=", 5)) found_term = 1;
			script.env[i] = env[i];
		}
		if (!found_shell) {
			script.env[i] = "SHELL=/bin/sh";
			script.shell_path = script.env[i] + 6;
			i++;
		}
		if (!found_term)  script.env[i++] = "TERM=xterm-256color";
		script.env[i] = NULL;
	}

#pragma endregion

#pragma region "Select"

	static int open_file(int *fd, char *file, int append) {
		if (append)	*fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0644);
		else		*fd = open(file, O_WRONLY | O_CREAT | O_TRUNC,  0644);

		return (*fd == -1);
	}

	static int select_loop() {
		fd_set	read_fds;
		char	buffer[4096];
		ssize_t	readed;
		int		in_fd, out_fd, io_fd, time_fd;
		in_fd = out_fd = io_fd = time_fd = -1;

		int ret = 0;
		if (!ret && script.options.log_in)		ret = open_file(&in_fd,   script.options.in,   script.options.append);
		if (!ret && script.options.log_out)		ret = open_file(&out_fd,  script.options.out,  script.options.append);
		if (!ret && script.options.log_io)		ret = open_file(&io_fd,   script.options.io,   script.options.append);
		if (!ret && script.options.log_time)	ret = open_file(&time_fd, script.options.time, script.options.append);
		if (ret) {
			// failed
			if (in_fd   != -1)	close(in_fd);
			if (out_fd  != -1)	close(out_fd);
			if (io_fd   != -1)	close(io_fd);
			if (time_fd != -1)	close(time_fd);
			return (1);
		}

		// Mensaje de inicio

		while (script.shell_running) {
			FD_ZERO(&read_fds);
			FD_SET(STDIN_FILENO, &read_fds);
			FD_SET(script.master_fd, &read_fds);

			if (select(script.master_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
				if (errno == EINTR) continue;
				break;
			}

			if (FD_ISSET(STDIN_FILENO, &read_fds)) {
				readed = read(STDIN_FILENO, buffer, sizeof(buffer));
				if (readed <= 0) break;
				write(script.master_fd, buffer, readed);
				if (in_fd   != -1)	write(in_fd, buffer,  readed);
				if (io_fd   != -1)	write(io_fd, buffer,  readed);
				if (time_fd != -1) {
					// write(time_fd, buffer, readed);
					// guardar como I
				}
			}

			if (FD_ISSET(script.master_fd, &read_fds)) {
				readed = read(script.master_fd, buffer, sizeof(buffer));
				if (readed <= 0) break;
				write(STDOUT_FILENO, buffer, readed);
				if (out_fd  != -1)	write(out_fd, buffer, readed);
				if (io_fd   != -1)	write(io_fd, buffer,  readed);
				if (time_fd != -1) {
					// write(time_fd, buffer, readed);
					// guardar como O
				}
			}
		}

		// Mensaje de fin

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

#pragma region "Main"

	int main(int argc, char **argv, char **env) {
		int ret = parse_options(argc, argv);
		if (ret) return (ret - 1);

		set_env(env);
		ret = shell_start();
		if (!ret) ret = raw_mode_enable();
		if (!ret) {
			signal_set();
			ret = select_loop();
		}
		raw_mode_disable();

		return (ret);
	}

#pragma endregion
