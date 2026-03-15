/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 11:33:12 by vzurera-          #+#    #+#             */
/*   Updated: 2026/03/15 19:12:18 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "script.h"

	#include <signal.h>
	#include <sys/ioctl.h>						// ioctl()
	#include <sys/wait.h>						// waitpid(), WIFEXITED, WIFSIGNALED, WEXITSTATUS, WTERMSIG
	#include <fcntl.h>							// open()

#pragma endregion

#pragma region "PTY Name"

	static int pty_name(int master_fd, char *name, size_t size) {
		int		pty_fd;
		char	buffer[12];

		if (ioctl(master_fd, TIOCGPTN, &pty_fd) == -1) return (1);
		itoa_buffered(pty_fd, buffer);
		ft_strlcpy(name, "/dev/pts/", size);
		ft_strlcpy(name + 9, buffer, size - 9);

		return (0);
	}

#pragma endregion

#pragma region "Start"

	int shell_start() {
		script.master_fd = open("/dev/ptmx", O_RDWR | O_NOCTTY);
		if (script.master_fd == -1) {
			// failed
			return (1);
		}
		
		int unlock = 0;
		if (ioctl(script.master_fd, TIOCSPTLCK, &unlock) == -1) {
			// failed
			close(script.master_fd);
			return (1);
		}
		
		char name[256];
		if (pty_name(script.master_fd, name, sizeof(name))) {
			// failed
			close(script.master_fd);
			return (1);
		}

		struct winsize ws;
		if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) == -1) {
			ws.ws_row = 24;
			ws.ws_col = 80;
			ws.ws_xpixel = 0;
			ws.ws_ypixel = 0;
		}
		if (ioctl(script.master_fd, TIOCSWINSZ, &ws) == -1) {
			// failed to set PTY window size
			close(script.master_fd);
			return (1);
		}

		pid_t pid = fork();
		if (pid < 0) {
			// fork() failed;
			close(script.master_fd);
			return (1);
		}

		if (pid == 0) {
			if (setsid() == -1) _exit(1);

			script.slave_fd = open(name, O_RDWR);
			if (script.slave_fd == -1) {
				// open slave failed
				_exit(1);
			}

			ioctl(script.slave_fd, TIOCSCTTY, 0);

			dup2(script.slave_fd, STDIN_FILENO);
			dup2(script.slave_fd, STDOUT_FILENO);
			dup2(script.slave_fd, STDERR_FILENO);

			struct termios term;
			if (tcgetattr(script.slave_fd, &term) == 0) {
				if		(!ft_strcmp(script.options.echo, "never"))		term.c_lflag &= ~ECHO;
				else if (!ft_strcmp(script.options.echo, "always"))		term.c_lflag |= ECHO;
				tcsetattr(script.slave_fd, TCSANOW, &term);
			}

			close(script.master_fd);
			if (script.slave_fd > 2) {
				close(script.slave_fd);
			}

			char	*args[4];
			int		argc = 0;
			args[argc++] = script.shell_path;
			if (script.options.log_command) {
				args[argc++] = "-c";
				args[argc++] = script.options.command;
			}
			args[argc] = NULL;

			execve(args[0], args, script.env);
			// execve() failed
			_exit(1);
		}

		script.shell_pid = pid;
		script.shell_running = 1;

		return (0);
	}

#pragma endregion

#pragma region "Close"

	void shell_close() {
		int	i, status;

		kill(script.shell_pid, SIGHUP);								// Ask shell to exit nicely
		i = 0;
		while (i < 10) {
			usleep(10000);											// Wait 10ms per iteration (100ms total)
			if (waitpid(script.shell_pid, &status, WNOHANG) > 0) {
				if (WIFEXITED(status))		script.exit_code = WEXITSTATUS(status);
				if (WIFSIGNALED(status))	script.exit_code = 128 + WTERMSIG(status);
				script.shell_running = 0;
				return;
			}
			i++;
		}

		kill(script.shell_pid, SIGKILL);							// Kill shell
	}

#pragma endregion
