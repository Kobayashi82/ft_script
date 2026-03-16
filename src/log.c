/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 17:30:37 by vzurera-          #+#    #+#             */
/*   Updated: 2026/03/16 15:44:26 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "script.h"

	#include <sys/time.h>						// gettimeofday(), struct timeval
	#include <unistd.h>							// write()

#pragma endregion

#pragma region "Start"

	#pragma region "Output"

		static size_t log_start_output(int fd) {
			size_t total = write(fd, "Script started on ", 18);
			char *ts = ctime(&g_script.start_time);
			if (ts) total += write(fd, ts, 24);
			total += write(fd, " [", 2);
			if (g_script.options.log_command) {
				total += write(fd, "COMMAND=\"", 9);
				total += write(fd, g_script.options.command, ft_strlen(g_script.options.command));
				total += write(fd, "\" ", 2);
			}
			total += write(fd, "TERM=\"", 6);
			total += write(fd, g_script.term,  ft_strlen(g_script.term));
			total += write(fd, "\" TTY=\"", 7);
			total += write(fd, g_script.tty,   ft_strlen(g_script.tty));
			total += write(fd, "\" COLUMNS=\"", 11);
			total += write(fd, g_script.col,  ft_strlen(g_script.col));
			total += write(fd, "\" LINES=\"", 9);
			total += write(fd, g_script.row, ft_strlen(g_script.row));
			total += write(fd, "\"]\n", 3);

			return (total);
		}

	#pragma endregion

	#pragma region "Terminal"

		int log_start() {
			size_t	total = 0;
			int		ret = 0;

			g_script.start_time = time(NULL);
			gettimeofday(&g_script.log_start_tv, NULL);
			g_script.log_started = 1;

			if (!g_script.options.quiet) {
				write(STDOUT_FILENO, "Script started", 14);
				if (g_script.out_fd != -1) {
					write(STDOUT_FILENO, ", output log file is '", 22);
					write(STDOUT_FILENO, g_script.options.out, ft_strlen(g_script.options.out));
					write(STDOUT_FILENO, "'", 1);
				}
				if (g_script.in_fd != -1) {
					write(STDOUT_FILENO, ", input log file is '", 21);
					write(STDOUT_FILENO, g_script.options.in, ft_strlen(g_script.options.in));
					write(STDOUT_FILENO, "'", 1);
				}
				if (g_script.time_fd != -1) {
					write(STDOUT_FILENO, ", timing log file is '", 22);
					write(STDOUT_FILENO, g_script.options.time, ft_strlen(g_script.options.time));
					write(STDOUT_FILENO, "'", 1);
				}
				write(STDOUT_FILENO, ".\n", 2);
			}

			if (g_script.out_fd != -1) {
				total = log_start_output(g_script.out_fd);
				g_script.out_size += total;
				if (g_script.options.flush) fsync(g_script.out_fd);
				if (g_script.options.size && g_script.out_size >= g_script.options.size) ret = 3;
			}

			if (g_script.time_fd != -1 && !ft_strcmp(g_script.options.format, "advanced")) {
				write_time_start_headers(g_script.time_fd, &g_script.time_size);
				if (g_script.options.flush) fsync(g_script.time_fd);
			}

			return (ret);
		}

	#pragma endregion

#pragma endregion

#pragma region "End"

	#pragma region "Output"

		static size_t log_end_output(int fd, int ret) {
			size_t total = 0;
			time_t end_time = time(NULL);
			char *ts = ctime(&end_time);

			if (ret == 3) {
				total += write(fd, "\nScript stopped on ", 19);
				if (ts) total += write(fd, ts, 24);
				total += write(fd, " [<max output size exceeded>]\n", 30);
			}
			else if (g_script.shell_signal)
			{
				total += write(fd, "\nScript stopped on ", 19);
				if (ts) total += write(fd, ts, 24);
				total += write(fd, " [COMMAND_EXIT_CODE=\"", 21);
				total += write_ulong(fd, (unsigned long)g_script.exit_code);
				total += write(fd, "\" SIGNAL=\"", 10);
				total += write_ulong(fd, (unsigned long)g_script.shell_signal);
				total += write(fd, "\" SIGNAL_NAME=\"", 15);
				const char *name = signal_name(g_script.shell_signal);
				total += write(fd, name, ft_strlen(name));
				total += write(fd, "\"]\n", 3);
			}
			else
			{
				total += write(fd, "\nScript done on ", 16);
				if (ts) total += write(fd, ts, 24);
				total += write(fd, " [COMMAND_EXIT_CODE=\"", 21);
				total += write_ulong(fd, (unsigned long)g_script.exit_code);
				total += write(fd, "\"]\n", 3);
			}

			return (total);
		}

	#pragma endregion

	#pragma region "Terminal"

		void log_end(int ret) {
			if (!g_script.options.quiet) {
				if (ret == 3) {
					write(STDOUT_FILENO, "Script terminated, max output files size ", 41);
					write_ulong(STDOUT_FILENO, (unsigned long)g_script.options.size);
					write(STDOUT_FILENO, " exceeded.\n", 11);
				}

				write(STDOUT_FILENO, "Script done.\n", 13);
			}
	
			size_t total = 0;
			if (g_script.out_fd != -1) {
				total = log_end_output(g_script.out_fd, ret);
				g_script.out_size += total;
				if (g_script.options.flush) fsync(g_script.out_fd);
				if (g_script.options.size && g_script.out_size >= g_script.options.size) ret = 3;
			}

			if (g_script.time_fd != -1 && !ft_strcmp(g_script.options.format, "advanced")) {
				write_time_end_headers(g_script.time_fd, &g_script.time_size);
				if (g_script.options.flush)
					fsync(g_script.time_fd);
			}
		}

	#pragma endregion

#pragma endregion

#pragma region "Files"

	int log_output(const char *buffer, ssize_t bytes, int output) {
		int		ret = 0;

		// Input (-I) / Output (-O)
		int fd = (output) ? g_script.out_fd : g_script.in_fd;
		size_t *size = (output) ? &g_script.out_size : &g_script.in_size;
		if (fd != -1) {
			write(fd, buffer, bytes);
			*size += (size_t)bytes;
			if (g_script.in_fd != -1 && g_script.out_fd != -1 && g_script.in_fd == g_script.out_fd) {
				g_script.in_size = g_script.out_size = *size;
			}
			if (g_script.options.flush) fsync(fd);
			if (g_script.options.size && *size >= g_script.options.size) {
				g_script.shell_running = 0;
				ret = 3;
			}
		}

		// Timing (-T)
		if (g_script.time_fd != -1) {
			write_timing(g_script.time_fd, &g_script.time_size, (output) ? "O " : "I ", bytes);
			if (g_script.options.size && g_script.time_size >= g_script.options.size) {
				g_script.shell_running = 0;
				ret = 3;
			}
		}

		return (ret);
	}

#pragma endregion
