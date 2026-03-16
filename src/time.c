/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 14:51:34 by vzurera-          #+#    #+#             */
/*   Updated: 2026/03/16 15:23:36 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "script.h"

	#include <sys/time.h>						// gettimeofday(), struct timeval
	#include <unistd.h>							// write()

#pragma endregion

#pragma region "Helpers"

	#pragma region "ULong"

		size_t write_ulong(int fd, unsigned long value) {
			char	buffer[20];
			int		i = 19;

			if (!value) return (write(fd, "0", 1));

			buffer[i] = '\0';
			while (value > 0) {
				buffer[--i] = '0' + (value % 10);
				value /= 10;
			}

			return (write(fd, buffer + i, 19 - i));
		}

	#pragma endregion

	#pragma region "Usec"

		static void write_usec(int fd, size_t *size, long usec) {
			char	pad[6];
			int		i = 5;

			while (i >= 0) {
				pad[i--] = '0' + (usec % 10);
				usec /= 10;
			}
			*size += write(fd, pad, 6);
		}

	#pragma endregion

	#pragma region "Header"

		static void write_time_header(int fd, size_t *size, const char *key, const char *value, time_t *start_time) {
			if (!value && !start_time) return;

			if (value) {
				*size += write(fd, "H 0.000000 ", 11);
				*size += write(fd, key, ft_strlen(key));
				if (value && value[0]) {
					*size += write(fd, " ", 1);
					*size += write(fd, value, ft_strlen(value));
				}
				*size += write(fd, "\n", 1);
			} else {
				int i = 0;
				char stamp[32];
				char *ts = ctime(start_time);
				if (!ts) return ;

				while (ts[i] && ts[i] != '\n' && i < (int)sizeof(stamp) - 1) {
					stamp[i] = ts[i];
					i++;
				}
				stamp[i] = '\0';

				write_time_header(fd, size, key, stamp, NULL);
			}
		}

	#pragma endregion

#pragma endregion

#pragma region "Start Header"

	void write_time_start_headers(int fd, size_t *size) {
		write_time_header(fd, size, "START_TIME",	NULL,					&g_script.start_time);
		write_time_header(fd, size, "TERM",			g_script.term,			NULL);
		write_time_header(fd, size, "TTY",			g_script.tty,			NULL);
		write_time_header(fd, size, "COLUMNS",		g_script.col,			NULL);
		write_time_header(fd, size, "LINES",		g_script.row,			NULL);
		write_time_header(fd, size, "SHELL",		g_script.shell_path,	NULL);
		write_time_header(fd, size, "TIMING_LOG",	g_script.options.time,	NULL);

		if (g_script.options.log_out)		write_time_header(fd, size, "OUTPUT_LOG",	g_script.options.out,		NULL);
		if (g_script.options.log_in)		write_time_header(fd, size, "INPUT_LOG",	g_script.options.in,		NULL);
		if (g_script.options.log_command)	write_time_header(fd, size, "COMMAND",		g_script.options.command,	NULL);
		if (g_script.options.log_time)		write_time_header(fd, size, "FORMAT",		g_script.options.format,	NULL);
	}

#pragma endregion

#pragma region "End Header"

	void write_time_end_headers(int fd, size_t *size) {
		struct timeval	now;

		if (!g_script.log_started) return;
		
		// Get current time
		gettimeofday(&now, NULL);
		// Calculate time delta since last timing entry
		long delta_sec = now.tv_sec - g_script.log_start_tv.tv_sec;
		long delta_usec = now.tv_usec - g_script.log_start_tv.tv_usec;
		// Compensate for negative microseconds
		if (delta_usec < 0) {
			delta_sec--;
			delta_usec += 1000000L;
		}

		*size += write(fd, "H 0.000000 DURATION ", 20);
		*size += write_ulong(fd, (unsigned long)delta_sec);
		*size += write(fd, ".", 1);
		write_usec(fd, size, delta_usec);
		*size += write(fd, "\n", 1);

		char buffer[12];
		itoa_buffered(g_script.exit_code, buffer);
		write_time_header(fd, size, "EXIT_CODE", buffer, NULL);
	}

#pragma endregion

#pragma region "Timing"

	void write_timing(int fd, size_t *size, char *type, ssize_t bytes) {
		struct timeval			now;
		static struct timeval	last_time;
		static int				timing_started;

		// Get current time and initialize on first call
		gettimeofday(&now, NULL);
		if (!timing_started) {
			last_time = now;
			timing_started = 1;
		}

		// Calculate time delta since last timing entry
		long delta_sec  = now.tv_sec  - last_time.tv_sec;
		long delta_usec = now.tv_usec - last_time.tv_usec;
		// Compensate for negative microseconds
		if (delta_usec < 0) {
			delta_sec--;
			delta_usec += 1000000L;
		}
		last_time = now;

		// Write prefix (I/O) only in advanced format
		if (!ft_strcmp(g_script.options.format, "advanced")) *size += write(fd, type, 2);

		// Write seconds
		*size += write_ulong(fd, (unsigned long)delta_sec);
		*size += write(fd, ".", 1);

		// Write microseconds
		write_usec(fd, size, delta_usec);
		*size += write(fd, " ", 1);

		// Write bytes
		*size += write_ulong(fd, (unsigned long)bytes);
		*size += write(fd, "\n", 1);

		if (g_script.options.flush) fsync(fd);
	}

#pragma endregion
