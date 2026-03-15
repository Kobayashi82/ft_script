/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 17:30:37 by vzurera-          #+#    #+#             */
/*   Updated: 2026/03/15 19:01:18 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "script.h"

	#include <time.h>							// time(), localtime(), struct tm, time_t
	#include <unistd.h>							// write()

#pragma endregion

#pragma region "Utils"

	static size_t write_ulong(int fd, unsigned long n) {
		char	buffer[20];
		int		i = 19;

		if (n == 0) { write(fd, "0", 1); return (1); }
		buffer[i] = '\0';
		while (n > 0) { buffer[--i] = '0' + (n % 10); n /= 10; }
		write(fd, buffer + i, 19 - i);
		return (19 - i);
	}

	static void write_timing(int fd, size_t *size, char type, ssize_t bytes) {
		static struct timespec	last_time;
		static int				timing_started;
		struct timespec			now;
		long					delta_sec;
		long					delta_nsec;
		char					pad[9];
		long					n;
		int						i;

		clock_gettime(CLOCK_MONOTONIC, &now);

		if (!timing_started) {
			last_time = now;
			timing_started = 1;
		}

		delta_sec  = now.tv_sec  - last_time.tv_sec;
		delta_nsec = now.tv_nsec - last_time.tv_nsec;
		if (delta_nsec < 0) {
			delta_sec--;
			delta_nsec += 1000000000L;
		}
		last_time = now;

		if (!ft_strcmp(script.options.format, "advanced")) {
			write(fd, &type, 1);
			write(fd, " ", 1);
		}

		write_ulong(fd, (unsigned long)delta_sec);
		write(fd, ".", 1);
		n = delta_nsec;
		i = 8;
		while (i >= 0) { pad[i--] = '0' + (n % 10); n /= 10; }
		write(fd, pad, 9);
		write(fd, " ", 1);
		write_ulong(fd, (unsigned long)bytes);
		write(fd, "\n", 1);

		if (size) *size += bytes;
		if (script.options.flush) fsync(fd);
	}

#pragma endregion

#pragma region "Start"

	#pragma region "Log Files"

		// static size_t log_start_file(int fd) {
		// 	size_t	total;
		// 	char	*term;
		// 	char	*tty;
		// 	char	*cols;
		// 	char	*lines;
		// 	char	*ts;

		// 	total = 0;
		// 	total += write(fd, "Script started on ", 18);
		// 	ts = ctime(&script.start_time);
		// 	if (ts) total += write(fd, ts, 24);
		// 	term  = getenv("TERM");
		// 	tty   = ttyname(STDIN_FILENO);
		// 	cols  = getenv("COLUMNS");
		// 	lines = getenv("LINES");
		// 	total += write(fd, " [TERM=\"", 8);
		// 	if (term)  total += write(fd, term,  ft_strlen(term));
		// 	total += write(fd, "\" TTY=\"", 7);
		// 	if (tty)   total += write(fd, tty,   ft_strlen(tty));
		// 	total += write(fd, "\" COLUMNS=\"", 11);
		// 	if (cols)  total += write(fd, cols,  ft_strlen(cols));
		// 	total += write(fd, "\" LINES=\"", 9);
		// 	if (lines) total += write(fd, lines, ft_strlen(lines));
		// 	total += write(fd, "\"]\n", 3);
		// 	return (total);
		// }

	#pragma endregion

	#pragma region "Log Terminal"

		// int log_start() {
		// 	size_t	total = 0;
		// 	int		ret = 0;

		// 	script.start_time = time(NULL);

		// 	if (!script.options.quiet)
		// 		start_message_term();
		// 	if (out_fd != -1)
		// 	{
		// 		log_tot= start_message_file(out_fd);
		// 		*out_size += total;
		// 		if (script.options.flush) fsync(out_fd);
		// 		if (script.options.size && *out_size >= script.options.size) ret = 3;
		// 	}
		// 	if (io_fd != -1)
		// 	{
		// 		log_tot= start_message_file(io_fd);
		// 		*io_size += total;
		// 		if (script.options.flush) fsync(io_fd);
		// 		if (script.options.size && *io_size >= script.options.size) ret = 3;
		// 	}
		// 	if (time_fd != -1 && total > 0)
		// 		write_timing(time_fd, time_size, 'O', total);

		// 	return (ret);
		// }

	#pragma endregion

#pragma endregion

#pragma region "End"

	#pragma region "Log Files"

		// static size_t end_message_file(int fd, int ret) {
		// 	size_t		total;
		// 	const char	*name;
		// 	time_t		end_time;
		// 	char		*ts;

		// 	total    = 0;
		// 	end_time = time(NULL);
		// 	ts       = ctime(&end_time);
		// 	if (ret == 3)
		// 	{
		// 		total += write(fd, "Script stopped on ", 18);
		// 		if (ts) total += write(fd, ts, 24);
		// 		total += write(fd, " [COMMAND_EXIT_CODE=\"", 21);
		// 		total += write_ulong(fd, (unsigned long)script.exit_code);
		// 		total += write(fd, "\" SIZE_LIMIT_REACHED=\"1\"]\n", 26);
		// 	}
		// 	else if (script.signal)
		// 	{
		// 		total += write(fd, "Script stopped on ", 18);
		// 		if (ts) total += write(fd, ts, 24);
		// 		total += write(fd, " [COMMAND_EXIT_CODE=\"", 21);
		// 		total += write_ulong(fd, (unsigned long)script.exit_code);
		// 		total += write(fd, "\" SIGNAL=\"", 10);
		// 		total += write_ulong(fd, (unsigned long)script.signal);
		// 		total += write(fd, "\" SIGNAL_NAME=\"", 15);
		// 		name = signal_name(script.signal);
		// 		total += write(fd, name, ft_strlen(name));
		// 		total += write(fd, "\"]\n", 3);
		// 	}
		// 	else
		// 	{
		// 		total += write(fd, "Script done on ", 15);
		// 		if (ts) total += write(fd, ts, 24);
		// 		total += write(fd, " [COMMAND_EXIT_CODE=\"", 21);
		// 		total += write_ulong(fd, (unsigned long)script.exit_code);
		// 		total += write(fd, "\"]\n", 3);
		// 	}

		// 	return (total);
		// }

	#pragma endregion

	#pragma region "Log Terminal"

		// void log_end(int ret) {
		// 	const char *name;

		// 	end_message_file(fd, ret);

		// 	if (ret == 3) {
		// 		write(STDOUT_FILENO, "Script terminated, max output files size ", 41);
		// 		write_ulong(STDOUT_FILENO, (unsigned long)script.options.size);
		// 		write(STDOUT_FILENO, " exceeded.\n", 11);
		// 	}
		// 	else if (script.signal)
		// 	{
		// 		write(STDOUT_FILENO, "Script terminated: signal ", 26);
		// 		write_ulong(STDOUT_FILENO, (unsigned long)script.signal);
		// 		write(STDOUT_FILENO, " (", 2);
		// 		name = signal_name(script.signal);
		// 		write(STDOUT_FILENO, name, ft_strlen(name));
		// 		write(STDOUT_FILENO, ").\n", 3);
		// 	}

		// 	write(STDOUT_FILENO, "Script done.\n", 13);
		// }

	#pragma endregion

#pragma endregion

#pragma region "Files"

	int log_files(const char *buffer, ssize_t bytes, int output) {
		int ret = 0;

		int fd = (output) ? script.out_fd : script.in_fd;
		size_t *size = (output) ? &script.out_size : &script.in_size;

		// Input (-I) or Output (-O)
		if (fd != -1) {
			write(fd, buffer, bytes);
			*size += bytes;
			if (script.options.flush) fsync(fd);
			if (script.options.size && *size >= script.options.size) {
				script.shell_running = 0;
				ret = 3;
			}
		}

		// Input and Output (-B)
		if (script.io_fd != -1) {
			write(script.io_fd, buffer, bytes);
			script.io_size += bytes;
			if (script.options.flush) fsync(script.io_fd);
			if (script.options.size && script.io_size >= script.options.size) {
				script.shell_running = 0;
				ret = 3;
			}
		}

		// Timing (-T)
		if (script.time_fd != -1) {
			write_timing(script.time_fd, &script.time_size, (output) ? 'O' : 'I', bytes);
		}

		return (ret);
	}

#pragma endregion

// En terminal (no -q):
// Script started, output log file is 'prueba', input log file is 'prueba'.
// Script done.

// En archivo:
// Script started, output log file is 'typescript'.
// Script started, output log file is 'prueba', input log file is 'prueba', timing file is 'popo'.
// Script started on 2026-03-15 17:16:57+01:00 [TERM="xterm-256color" TTY="/dev/pts/0" COLUMNS="99" LINES="22"]
// Script done on 2026-03-15 17:17:02+01:00 [COMMAND_EXIT_CODE="0"]
