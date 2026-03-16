/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 11:33:12 by vzurera-          #+#    #+#             */
/*   Updated: 2026/03/16 11:57:24 by vzurera-         ###   ########.fr       */
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

	static int pty_name(char *name, size_t size) {
		int		pty_fd;
		char	buffer[12];

		if (ioctl(g_script.master_fd, TIOCGPTN, &pty_fd) == -1) return (1);
		itoa_buffered(pty_fd, buffer);
		ft_strlcpy(name, "/dev/pts/", size);
		ft_strlcpy(name + 9, buffer, size - 9);

		return (0);
	}

#pragma endregion

#pragma region "Start"

	int shell_start() {
		// Open PTY master
		g_script.master_fd = open("/dev/ptmx", O_RDWR | O_NOCTTY);
		if (g_script.master_fd == -1) {
			write(STDERR_FILENO, "ft_script: failed to open PTY master\n", 37);
			return (1);
		}

		// Unlock PTY slave so it can be opened
		int unlock = 0;
		if (ioctl(g_script.master_fd, TIOCSPTLCK, &unlock) == -1) {
			write(STDERR_FILENO, "ft_script: failed to unlock PTY\n", 32);
			close(g_script.master_fd);
			return (1);
		}

		// Get PTY slave device name
		char name[256];
		if (pty_name(name, sizeof(name))) {
			write(STDERR_FILENO, "ft_script: failed to get PTY slave name\n", 40);
			close(g_script.master_fd);
			return (1);
		}
		ft_strlcpy(g_script.tty, name, sizeof(g_script.tty));

		// Get current terminal size
		struct winsize ws;
		if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) == -1) {
			ws.ws_row = 24;
			ws.ws_col = 80;
			ws.ws_xpixel = 0;
			ws.ws_ypixel = 0;
		}
		itoa_buffered(ws.ws_col, g_script.col);
		itoa_buffered(ws.ws_row, g_script.row);

		//  Apply terminal size to the PTY
		if (ioctl(g_script.master_fd, TIOCSWINSZ, &ws) == -1) {
			write(STDERR_FILENO, "ft_script: failed to set PTY window size\n", 41);
			close(g_script.master_fd);
			return (1);
		}

		// Create error pipe to detect child failures before execve
		int err_pipe[2];
		if (pipe(err_pipe) == -1) {
			write(STDERR_FILENO, "ft_script: failed to create error pipe\n", 39);
			close(g_script.master_fd);
			return (1);
		}

		// Fork child process
		pid_t pid = fork();
		if (pid < 0) {
			write(STDERR_FILENO, "ft_script: failed to fork process\n", 34);
			close(g_script.master_fd);
			close(err_pipe[0]);
			close(err_pipe[1]);
			return (1);
		}

		if (pid == 0) {
			// Reset signal handlers
			signal_reset();

			// Close unneeded fds and mark error pipe as close-on-exec
			close(g_script.master_fd);
			close(err_pipe[0]);
			fcntl(err_pipe[1], F_SETFD, FD_CLOEXEC);

			// Create a new session so the PTY slave becomes the controlling terminal
			if (setsid() == -1) {
				write(err_pipe[1], "setsid failed\n", 14);
				_exit(1);
			}

			// Open PTY slave
			g_script.slave_fd = open(name, O_RDWR);
			if (g_script.slave_fd == -1) {
				write(err_pipe[1], "open slave failed\n", 18);
				_exit(1);
			}

			// Set PTY slave as controlling terminal
			ioctl(g_script.slave_fd, TIOCSCTTY, 0);

			// Redirect stdin, stdout and stderr to PTY slave
			int ret = 0;
			if (!ret && dup2(g_script.slave_fd, STDIN_FILENO)  == -1) ret = 1;
			if (!ret && dup2(g_script.slave_fd, STDOUT_FILENO) == -1) ret = 1;
			if (!ret && dup2(g_script.slave_fd, STDERR_FILENO) == -1) ret = 1;
			if (ret) {
				close(g_script.slave_fd);
				write(err_pipe[1], "dup2 failed\n", 12);
				_exit(1);
			}

			// Apply echo settings
			struct termios term;
			if (tcgetattr(g_script.slave_fd, &term) == 0) {
				if		(!ft_strcmp(g_script.options.echo, "never"))  term.c_lflag &= ~ECHO;
				else if (!ft_strcmp(g_script.options.echo, "always")) term.c_lflag |= ECHO;
				tcsetattr(g_script.slave_fd, TCSANOW, &term);
			}
			close(g_script.slave_fd);

			// Create arguments
			char	*args[4];
			int		argc = 0;
			args[argc++] = g_script.shell_path;
			if (g_script.options.log_command) {
				args[argc++] = "-c";
				args[argc++] = g_script.options.command;
			}
			args[argc] = NULL;

			// Execute the shell
			execve(args[0], args, g_script.env);

			// In case of execve fail
			write(err_pipe[1], "execve failed\n", 14);
			_exit(1);
		}

		// Wait until child closes pipe (execve success) or writes an error
		close(err_pipe[1]);
		char buffer[64];
		ssize_t readed = read(err_pipe[0], buffer, sizeof(buffer) - 1);
		close(err_pipe[0]);
		if (readed > 0) {
			buffer[readed] = '\0';
			write(STDERR_FILENO, "ft_script: ", 11);
			write(STDERR_FILENO, buffer, readed);
			waitpid(pid, NULL, 0);
			close(g_script.master_fd);
			return (1);
		}

		// Store child PID and mark shell as running
		g_script.shell_pid = pid;
		g_script.shell_running = 1;

		return (0);
	}

#pragma endregion

#pragma region "Close"

	void shell_close() {
		struct timespec	ts;
		int				status;

		ts.tv_sec  = 0;
		ts.tv_nsec = 10000000;	// 10ms

		// Ask shell to exit nicely
		kill(g_script.shell_pid, SIGHUP);								
		// Wait 10ms per iteration (100ms total)
		for (int i = 0; i < 10; ++i) {
			nanosleep(&ts, NULL);
			if (waitpid(g_script.shell_pid, &status, WNOHANG) > 0) {
				if (WIFEXITED(status)) g_script.exit_code = WEXITSTATUS(status);
				if (WIFSIGNALED(status)) {
					g_script.signal = WTERMSIG(status);
					g_script.exit_code = 128 + g_script.signal;
				}
				g_script.shell_running = 0;
				return;
			}
		}

		// Force kill unresponsive shell
		kill(g_script.shell_pid, SIGKILL);						
	}

#pragma endregion
