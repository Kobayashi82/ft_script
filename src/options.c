/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   options.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 12:14:27 by vzurera-          #+#    #+#             */
/*   Updated: 2026/03/16 12:57:10 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "script.h"

#pragma endregion

#pragma region "Help"

	static void help() {
		const char *msg =
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
		const char *msg =
			"ft_script 1.0.\n"
			"Copyright (C) 2026 Kobayashi Corp ⓒ.\n"
			"This is free software: you are free to change and redistribute it.\n"
			"There is NO WARRANTY, to the extent permitted by law.\n\n"

			"Written by Kobayashi82 (vzurera-).\n";

		write(STDOUT_FILENO, msg, ft_strlen(msg));
	}

#pragma endregion

#pragma region "Environment"

	static void set_env(char **env) {
		int i = 0;
		int found_shell = 0;
		int found_term = 0;
		for (; env[i] && i < 253; ++i) {
			if (!found_shell && !ft_strncmp(env[i], "SHELL=", 6)) {
				found_shell = 1;
				if (access(env[i] + 6, X_OK)) {
					g_script.env[i] = "SHELL=/bin/sh";
					g_script.shell_path = g_script.env[i] + 6;
					continue;
				}
				g_script.shell_path = env[i] + 6;
			}
			if (!found_term && !ft_strncmp(env[i], "TERM=", 5)) {
				found_term = 1;
				g_script.term = env[i] + 5;
			}
			g_script.env[i] = env[i];
		}
		if (!found_shell) {
			g_script.env[i] = "SHELL=/bin/sh";
			g_script.shell_path = g_script.env[i] + 6;
			i++;
		}
		if (!found_term) {
			g_script.env[i] = "TERM=xterm-256color";
			g_script.term = g_script.env[i] + 5;
			i++;
		}
		g_script.env[i] = NULL;
	}

#pragma endregion

#pragma region "Options"

	#pragma region "Set"

		#pragma region "Next Arg"

			static int next_arg(char opt, char **argv, int *i) {
				(*i)++;
				if (!argv[*i]) {
					char tmp[2] = { opt, '\0' };
					write(STDERR_FILENO, "ft_script: option requires an argument -- '", 43);
					write(STDERR_FILENO, tmp, 1);
					write(STDERR_FILENO, "'\nTry 'ft_script -h' for more information.\n", 43);
					return (1);
				}

				return (0);
			}

		#pragma endregion

		#pragma region "Set Echo"

			static int set_echo(char opt, char **argv, int *i, char *dst, size_t size) {
				if (next_arg(opt, argv, i)) return (2);
				ft_strlcpy(dst, argv[*i], size);

				if (ft_strcmp(dst, "auto") && ft_strcmp(dst, "always") && ft_strcmp(dst, "never")) {
					write(STDERR_FILENO, "ft_script: script: unssuported echo mode: '", 43);
					write(STDERR_FILENO, dst, ft_strlen(dst));
					write(STDERR_FILENO, "'\nTry 'ft_script -h' for more information.\n", 43);
					return (2);
				}

				return (0);
			}

		#pragma endregion

		#pragma region "Set Format"

			static int set_format(char opt, char **argv, int *i, char *dst, size_t size) {
				if (next_arg(opt, argv, i)) return (2);
				ft_strlcpy(dst, argv[*i], size);

				if (ft_strcmp(dst, "classic") && ft_strcmp(dst, "advanced")) {
					write(STDERR_FILENO, "ft_script: unsupported logging format: '", 40);
					write(STDERR_FILENO, dst, ft_strlen(dst));
					write(STDERR_FILENO, "'\nTry 'ft_script -h' for more information.\n", 43);
					return (2);
				}

				return (0);
			}

		#pragma endregion

		#pragma region "Set Size"

			static int set_size(char opt, char **argv, int *i, size_t *dst) {
				if (next_arg(opt, argv, i)) return (2);

				int ret = ft_atos(argv[*i], dst);
				if (ret) {
					write(STDERR_FILENO, "ft_script: failed to parse output limit size: '", 47);
					write(STDERR_FILENO, argv[*i], ft_strlen(argv[*i]));
					if (ret == 2)	write(STDERR_FILENO, "': Numerical result out of range\n", 33);
					else			write(STDERR_FILENO, "': Invalid argument\n", 20);
					write(STDERR_FILENO, "Try 'ft_script -h' for more information.\n", 41);
					return (2);
				}

				return (0);
			}

		#pragma endregion

		#pragma region "Set Value"

			static int set_value(char opt, char **argv, int *i, char *dst, int *log_dst, size_t size) {
				*log_dst = 1;
				if (next_arg(opt, argv, i)) return (2);
				ft_strlcpy(dst, argv[*i], size);

				return (0);
			}

		#pragma endregion

	#pragma endregion

	#pragma region "Process"

		static int process_option(t_options *options, char **argv, int *i) {
			char opt = argv[*i][1];
			switch (opt) {
				case 'I':	return (set_value(opt, argv, i, options->in, &options->log_in, sizeof(options->in)));
				case 'O':	return (set_value(opt, argv, i, options->out, &options->log_out, sizeof(options->out)));
				case 'B': {
							int ret = set_value(opt, argv, i, options->out, &options->log_out, sizeof(options->out));
							if (!ret) {
								ft_strlcpy(options->in, options->out, sizeof(options->in));
								options->log_in = 1;
							}
							return (ret);
				}
				case 'T':	return (set_value(opt, argv, i, options->time, &options->log_time, sizeof(options->time)));
				case 'c':	return (set_value(opt, argv, i, options->command, &options->log_command, sizeof(options->command)));
				case 'E':	return (set_echo(opt, argv, i, options->echo, sizeof(options->echo)));
				case 'm':	return (set_format(opt, argv, i, options->format, sizeof(options->format)));
				case 'o':	return (set_size(opt, argv, i, &options->size));
				case 'a':	options->append = 1;	return (0);
				case 'e':	options->retur = 1;		return (0);
				case 'f':	options->flush = 1;		return (0);
				case 'q':	options->quiet = 1;		return (0);
				case 'h':	help();					return (1);
				case 'V':	version();				return (1);
			}

			return (0);
		}

	#pragma endregion

	#pragma region "Is Option"

		static int is_option(const char *arg) {
			if (!arg[0] || arg[0] != '-') 							return (0);
			if (ft_strchr("IOBTmacefEoqhV", arg[1]) && !arg[2])	return (1);
			if (arg[1] == '-' && !arg[2])							return (-1);

			write(STDERR_FILENO, "ft_script: invalid option -- '", 30);
			write(STDERR_FILENO, arg + 1, ft_strlen(arg + 1));
			write(STDERR_FILENO, "'\nTry 'ft_script -h' for more information.\n", 43);

			return (2);
		}

	#pragma endregion

	#pragma region "Parse"

		int parse_options(int argc, char **argv, char **env) {
			t_options *options = &g_script.options;
			int	ret = 0;
			int	parse = 1;

			ft_strlcpy(options->echo,   "auto",     sizeof(options->echo));
			ft_strlcpy(options->format, "advanced", sizeof(options->format));

			for (int i = 1; i < argc; ++i) {
				if (parse && (ret = is_option(argv[i]))) {
					if (ret == -1) { parse = ret = 0; continue; }
					if (ret == 2 || (ret = process_option(options, argv, &i))) return (ret);
				} else {
					if (options->log_in || options->log_out || i + 1 < argc) {
						write(STDERR_FILENO, "ft_script: unexpected number of arguments\n", 42);
						write(STDERR_FILENO, "Try 'ft_script -h' for more information.\n", 41);
						return (2);
					}
					ft_strlcpy(options->out, argv[i], sizeof(options->out));
					options->log_out = 1;
					break;
				}
			}

			if (!options->log_in && !options->log_out) {
				ft_strlcpy(options->out, "typescript", sizeof(options->out));
				options->log_out = 1;
			}

			set_env(env);

			return (0);
		}

	#pragma endregion

#pragma endregion
