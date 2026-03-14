/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 11:33:12 by vzurera-          #+#    #+#             */
/*   Updated: 2026/03/14 12:24:30 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "script.h"

	#include <signal.h>

	#include <sys/wait.h>														// close(), fork(), setsid(), setgid(), setuid(), dup2(), chdir(), execvp()
	#include <sys/ioctl.h>														// ioctl()
	#include <fcntl.h>															// open()
	#include <unistd.h>															// access()

#pragma endregion

	static void	int_to_str(int n, char *buf)
	{
		int		i;
		int		tmp;
		char	digits[12];

		i = 0;
		if (n == 0) { buf[0] = '0'; buf[1] = '\0'; return ; }
		while (n > 0) { digits[i++] = '0' + (n % 10); n /= 10; }
		tmp = 0;
		while (tmp < i) { buf[tmp] = digits[i - tmp - 1]; tmp++; }
		buf[tmp] = '\0';
	}

	static int	get_pty_name(int master_fd, char *pty_name, size_t size)
	{
		int		ptyno;
		char	num[12];

		if (ioctl(master_fd, TIOCGPTN, &ptyno) == -1)
			return (1);
		int_to_str(ptyno, num);
		ft_strlcpy(pty_name, "/dev/pts/", size);
		ft_strlcpy(pty_name + 9, num, size - 9);
		return (0);
	}

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
	
	char pty_name[256];
	if (get_pty_name(script.master_fd, pty_name, sizeof(pty_name))) {
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
		// Epoll::close(); // select en este caso
		signal(SIGINT,  SIG_DFL);
		signal(SIGTERM, SIG_DFL);
		signal(SIGHUP,  SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		signal(SIGPIPE, SIG_DFL);
		signal(SIGSEGV, SIG_DFL);
		signal(SIGCHLD, SIG_DFL);

		if (setsid() == -1) _exit(1);

		script.slave_fd = open(pty_name, O_RDWR);
		if (script.slave_fd == -1) {
			// open slave failed
			_exit(1);
		}

		ioctl(script.slave_fd, TIOCSCTTY, 0);

		dup2(script.slave_fd, STDIN_FILENO);
		dup2(script.slave_fd, STDOUT_FILENO);
		dup2(script.slave_fd, STDERR_FILENO);

		close(script.master_fd);
		if (script.slave_fd > 2) {
			close(script.slave_fd);
		}

		struct termios term;
		if (tcgetattr(script.slave_fd, &term) == 0) {
			if		(!ft_strcmp(script.options.echo, "never"))		term.c_lflag &= ~ECHO;
			else if (!ft_strcmp(script.options.echo, "always"))		term.c_lflag |= ECHO;
			tcsetattr(script.slave_fd, TCSANOW, &term);
		}

		// Set TERM si no existe
		setenv("TERM", "xterm-256color", 1);

		// Buscar SHELL y usar ese shell
		const char *shell = NULL;
		const char *shell_path = NULL;
		if		(!access(shell, X_OK))		shell_path = shell;
		else if (!access("/bin/sh", X_OK))	shell_path = "/bin/sh";
		else {
			// no shell found
			_exit(1); 
		}

		// Crear envp
		char *args[] = { (char *)shell_path, NULL };
		execvp(args[0], args);	// execve
		// execve() failed
		_exit(1);
	}

	script.shell_pid = pid;
	script.shell_running = 1;

	// Epoll::add(script.master_fd, true, false);	 // select en este caso

	return (0);
}
