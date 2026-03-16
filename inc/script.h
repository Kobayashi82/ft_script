/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   script.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 14:09:50 by vzurera-          #+#    #+#             */
/*   Updated: 2026/03/16 12:31:25 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#pragma region "Includes"

	#include "utils.h"

	#include <time.h>											// clock_gettime(), struct timespec
	#include <termios.h>										// tcgetattr(), tcsetattr(), termios, winsize

#pragma endregion

#pragma region "Structures"

	typedef struct s_options {
		char	in[4096];										// Input log file path
		int		log_in;											// Enable input logging
		char	out[4096];										// Output log file path
		int		log_out;										// Enable output logging
		char	time[4096];										// Timing log file path
		int		log_time;										// Enable timing logging
		char	command[4096];									// Command to execute instead of interactive shell
		int		log_command;									// Enable command execution
		char	echo[256];										// Echo mode: "auto", "always" or "never"
		char	format[256];									// Timing format: "classic" or "advanced"
		size_t	size;											// Max log file size in bytes (0 = unlimited)
		int		append;											// Append to log files instead of overwriting
		int		retur;											// Return exit code of the child shell
		int		flush;											// Flush log files after every write
		int		quiet;											// Suppress start and end messages
	} t_options;

	typedef struct s_script {
		t_options		options;								// Parsed options
		time_t			start_time;								// Timestamp when the session started
		int				signal;									// Signal received in parent
		int				exit_code;								// Exit code of the shell process
		char			*env[256], tty[256], col[9], row[9];	// Environment variables passed to the shell
		char			*term, *shell_path;						// Path to the shell executable
		int				shell_kill;								// Flag indicating to kill the shell
		int				shell_pid;								// PID of the child shell processs
		int				shell_running;							// Flag indicating if the shell is still running
		int				shell_signal;							// Signal that terminated the shell
		int				master_fd;								// Master side of the PTY
		int				slave_fd;								// Slave side of the PTY
		int				raw_enabled;							// Flag indicating if terminal is in raw mode
		struct termios	original_termios;						// Saved terminal settings to restore on exit
		int				in_fd, out_fd, time_fd;					// File descriptors for log files
		size_t			in_size, out_size, time_size;			// Current sizes of log files in bytes
	} t_script;

#pragma endregion

#pragma region "Variables"

	extern t_script g_script;

#pragma endregion

#pragma region "Methods"

	// Options
	int			parse_options(int argc, char **argv, char **env);
	// Shell
	int			shell_start();
	void		shell_close();
	// Signal
	const char	*signal_name(int sig);
	int			signal_set();
	void		signal_reset();
	// Terminal
	int			raw_mode_enable();
	int			raw_mode_disable();
	// File
	int			open_files();
	void		close_files();
	// Log
	int			log_start();
	void		log_end(int ret);
	int			log_files(const char *buffer, ssize_t bytes, int output);

#pragma endregion
