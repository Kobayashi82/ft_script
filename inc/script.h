/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   script.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 14:09:50 by vzurera-          #+#    #+#             */
/*   Updated: 2026/03/14 18:16:02 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#pragma region "Includes"

	#include "utils.h"

	#include <termios.h>						// tcgetattr(), tcsetattr(), termios, winsize

#pragma endregion

#pragma region "Structures"

	typedef struct s_options {
		char	in[4096];
		int		log_in;
		char	out[4096];
		int		log_out;
		char	io[4096];
		int		log_io;
		char	time[4096];
		int		log_time;
		char	command[4096];
		int		log_command;
		char	echo[256];
		char	format[256];
		size_t	size;
		int		append;
		int		retur;
		int		flush;
		int		quiet;
	} t_options;

	typedef struct s_script {
		t_options		options;
		int				signal;					// 
		char			*env[256];				// Environment variables
		char			*shell_path;			// Path for the shell
		int				shell_pid;				// PID of the shell process
		int				shell_running;			// Flag indicating if shell is running
		int				master_fd;				// Master FD of the shell terminal
		int				slave_fd;				// Slave FD of the shell terminal
		int				raw_enabled;			// 
		struct termios	original_termios;		// 
	} t_script;

#pragma endregion

#pragma region "Variables"

	extern t_script script;

#pragma endregion

#pragma region "Methods"

	int		parse_options(int argc, char **argv);

	int		shell_start();
	void	signal_set();

	int		raw_mode_enable();
	void	raw_mode_disable();

#pragma endregion
