/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   options.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 12:14:27 by vzurera-          #+#    #+#             */
/*   Updated: 2026/03/12 14:54:53 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "options.h"
	#include "utils.h"

#pragma endregion

#pragma region "Help"

	static void help() {
		char *msg =
			"Usage:\n"
			" ft_script [options] [file]\n\n"

			"Make a typescript of a terminal session.\n\n"

			"Options:\n"
			" -I <file>       log stdin to file\n"
			" -O <file>       log stdout to file (default)\n"
			" -B <file>       log stdin and stdout to file\n"
			" -T <file>       log timing information to file\n"
			" -m <name>       force to 'classic' or 'advanced' format\n"
			" -a              append to the log file\n"
			" -c <command>    run command rather than interactive shell\n"
			" -e              return exit code of the child process\n"
			" -f              run flush after each write\n"
			" -F              use output file even when it is a link\n"
			" -E <when>       echo input in session (auto, always or never)\n"
			" -o <size>       terminate if output files exceed size\n"
			" -q              be quiet\n"
			" -h              display this help\n"
			" -V              display version\n\n"

			"Only single flags are supported (-a, not -abc or --append).\n\n"

			"Due to project constraints (no malloc, no getopt),option parsing\n"
			"is intentionally minimal.\n\n"

			"NOTES\n"
			"      ft_script ends when the forked shell exits.\n\n"

			"      Certain interactive commands, such as 'vi', create garbage in the typescript file.\n"
			"      ft_script works best with commands that do not manipulate the screen.\n\n"

			"      It is not recommended to run ft_script in non-interactive shells. The inner shell of ft_script is always\n"
			"      interactive, and this could lead to unexpected results. If you use ft_script in the shell initialization\n"
			"      file, you have to avoid entering an infinite loop.\n\n"

			"      You should also avoid use of ft_script in command pipes, as ft_script can read more input than you would expect.\n\n"

			"BUGS\n"
			"      ft_script places everything in the log file, including linefeeds and backspaces. This is not what the naive user\n"
			"      expects.\n\n"

			"      ft_script is primarily designed for interactive terminal sessions. When stdin is not a terminal (for example: echo\n"
			"      foo | ft_script), then the session can hang, because the interactive shell within the ft_script session misses EOF\n"
			"      and ft_script has no clue when to close the session.\n\n";

		write(STDOUT_FILENO, msg, ft_strlen(msg));
	}

#pragma endregion

#pragma region "Version"

	static void version() {
		char *msg =
			"ft_script 1.0.\n"
			"Copyright (C) 2026 Kobayashi Corp ⓒ.\n"
			"This is free software: you are free to change and redistribute it.\n"
			"There is NO WARRANTY, to the extent permitted by law.\n\n"

			"Written by Kobayashi82 (vzurera-).\n";

		write(STDOUT_FILENO, msg, ft_strlen(msg));
	}

#pragma endregion

#pragma region "Parser"

	static int validate_command(const char *command) {
		(void) command;
		return (0);
	}

	static int validate_file(const char *file) {
		(void) file;
		return (0);
	}

	static int next_arg(char opt, const char **argv, int *i) {
		(*i)++;
		if (!argv[*i]) {
			char tmp[2] = { opt, '\0' };
			write(STDERR_FILENO, "ft_script: option requires an argument -- '", 43);
			write(STDERR_FILENO, tmp, 1);
			write(STDERR_FILENO, "'\n", 2);
			write(STDERR_FILENO, "Try 'ft_script -h' for more information.\n", 41);
			return (1);
		}

		return (0);
	}

	static int process_option(t_options *options, const char **argv, int *i) {
		char opt = argv[*i][1];
		switch (opt) {
			case 'I':
				if (next_arg(opt, argv, i))				return (2);
				ft_strlcpy(options->log_in, argv[*i], sizeof(options->log_in));
				if (validate_file(options->log_in))		return (2);
				return (0);
			case 'O':
				if (next_arg(opt, argv, i))				return (2);
				ft_strlcpy(options->log_out, argv[*i], sizeof(options->log_out));
				if (validate_file(options->log_in))		return (2);
				return (0);
			case 'B':
				if (next_arg(opt, argv, i))				return (2);
				ft_strlcpy(options->log_io, argv[*i], sizeof(options->log_io));
				if (validate_file(options->log_in))		return (2);
				return (0);
			case 'T':
				if (next_arg(opt, argv, i))				return (2);
				ft_strlcpy(options->log_timing, argv[*i], sizeof(options->log_timing));
				if (validate_file(options->log_in))		return (2);
				return (0);
			case 'm':
				if (next_arg(opt, argv, i))				return (2);
				ft_strlcpy(options->log_format, argv[*i], sizeof(options->log_format));
				if (ft_strcmp(options->log_format, "classic") && ft_strcmp(options->log_format, "advanced")) {
					write(STDERR_FILENO, "ft_script: unsupported logging format: '", 40);
					write(STDERR_FILENO, options->log_format, ft_strlen(options->log_format));
					write(STDERR_FILENO, "'\n", 2);
					write(STDERR_FILENO, "Try 'ft_script -h' for more information.\n", 41);
					return (2);
				}
				return (0);
			case 'a':	options->append = 1;			return (0);
			case 'c':
				if (next_arg(opt, argv, i)) 			return (2);
				ft_strlcpy(options->command, argv[*i], sizeof(options->command));
				if (validate_command(options->command))	return (2);
				return (0);
			case 'e':	options->ret = 1;				return (0);
			case 'f':	options->flush = 1;				return (0);
			case 'F':	options->force = 1;				return (0);
			case 'E':
				if (next_arg(opt, argv, i))				return (2);
				ft_strlcpy(options->echo, argv[*i], sizeof(options->echo));
				if (ft_strcmp(options->echo, "auto") && ft_strcmp(options->echo, "always") && ft_strcmp(options->echo, "never")) {
					write(STDERR_FILENO, "ft_script: script: unssuported echo mode: '", 43);
					write(STDERR_FILENO, options->echo, ft_strlen(options->echo));
					write(STDERR_FILENO, "'\n", 2);
					write(STDERR_FILENO, "Try 'ft_script -h' for more information.\n", 41);
					return (2);
				}
				return (0);
			case 'o':
				if (next_arg(opt, argv, i))				return (2);
				// options->size
				// Validate
				return (0);
			case 'q':	options->quiet = 1;				return (0);
			case 'h':	help();							return (1);
			case 'V':	version();						return (1);
		}

		return (0);
	}

	static int is_option(const char *arg) {
		if (!arg[0] || arg[0] != '-') 							return (0);
		if (ft_strchr("IOBTmacefFEoqhV", arg[1]) && !arg[2])	return (1);
		if (arg[1] == '-' && !arg[2])							return (-1);

		write(STDERR_FILENO, "ft_script: invalid option -- '", 30);
		write(STDERR_FILENO, arg + 1, ft_strlen(arg + 1));
		write(STDERR_FILENO, "'\n", 2);
		write(STDERR_FILENO, "Try 'ft_script -h' for more information.\n", 41);
		return (2);
	}

	int options_parse(t_options *options, int argc, const char **argv) {
		int ret = 0, parse = 1;

		for (int i = 1; i < argc; ++i) {
			if (parse && (ret = is_option(argv[i]))) {
				if (ret == -1) { parse = ret = 0; continue; }
				if (ret == 2 || (ret = process_option(options, argv, &i))) return (ret);
			} else {
				ft_strlcpy(options->file, argv[i], sizeof(options->file));
				break;
			}
		}

		return (0);
	}

#pragma endregion
