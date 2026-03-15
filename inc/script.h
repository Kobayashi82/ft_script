/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   script.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 14:09:50 by vzurera-          #+#    #+#             */
/*   Updated: 2026/03/15 18:47:04 by vzurera-         ###   ########.fr       */
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
		char	in[4096];										// 
		int		log_in;											// 
		char	out[4096];										// 
		int		log_out;										// 
		char	io[4096];										// 
		int		log_io;											// 
		char	time[4096];										// 
		int		log_time;										// 
		char	command[4096];									// 
		int		log_command;									// 
		char	echo[256];										// 
		char	format[256];									// 
		size_t	size;											// 
		int		append;											// 
		int		retur;											// 
		int		flush;											// 
		int		quiet;											// 
	} t_options;

	typedef struct s_script {
		t_options		options;								// 
		time_t			start_time;								// 
		int				signal;									// 
		int				exit_code;								// 
		char			*env[256];								// Environment variables
		char			*shell_path;							// Path for the shell
		int				shell_pid;								// PID of the shell process
		int				shell_running;							// Flag indicating if shell is running
		int				master_fd;								// Master FD of the shell terminal
		int				slave_fd;								// Slave FD of the shell terminal
		int				raw_enabled;							// 
		struct termios	original_termios;						// 
		int				in_fd, out_fd, io_fd, time_fd;			// 
		size_t			in_size, out_size, io_size, time_size;	// 
	} t_script;

#pragma endregion

#pragma region "Variables"

	extern t_script script;										// 

#pragma endregion

#pragma region "Methods"

	// Options
	int			parse_options(int argc, char **argv);
	// Shell
	int			shell_start();
	void		shell_close();
	// Signal
	const char	*signal_name(int sig);
	void		signal_set();
	// Select
	int			select_loop();
	// Terminal
	int			raw_mode_enable();
	void		raw_mode_disable();
	// Files
	int			open_files();
	void		close_files();
	// Log
	int			log_start();
	void		log_end(int ret);
	int			log_files(const char *buffer, ssize_t bytes, int output);

#pragma endregion
