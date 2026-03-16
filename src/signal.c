/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 11:33:39 by vzurera-          #+#    #+#             */
/*   Updated: 2026/03/16 11:53:07 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "script.h"

	#include <signal.h>							// signal()
	#include <termios.h>						// struct winsize
	#include <sys/ioctl.h>						// ioctl(), TIOCGWINSZ, TIOCSWINSZ
	#include <sys/wait.h>						// waitpid(), WIFEXITED, WIFSIGNALED, WEXITSTATUS, WTERMSIG

#pragma endregion

#pragma region "Name"

	const char *signal_name(int sig) {
		static const char *names[] = {
			"",				// 0
			"SIGHUP",		// 1
			"SIGINT",		// 2
			"SIGQUIT",		// 3
			"SIGILL",		// 4
			"SIGTRAP",		// 5
			"SIGABRT",		// 6
			"SIGBUS",		// 7
			"SIGFPE",		// 8
			"SIGKILL",		// 9
			"SIGUSR1",		// 10
			"SIGSEGV",		// 11
			"SIGUSR2",		// 12
			"SIGPIPE",		// 13
			"SIGALRM",		// 14
			"SIGTERM",		// 15
			"SIGSTKFLT",	// 16
			"SIGCHLD",		// 17
			"SIGCONT",		// 18
			"SIGSTOP",		// 19
			"SIGTSTP",		// 20
			"SIGTTIN",		// 21
			"SIGTTOU",		// 22
			"SIGURG",		// 23
			"SIGXCPU",		// 24
			"SIGXFSZ",		// 25
			"SIGVTALRM",	// 26
			"SIGPROF",		// 27
			"SIGWINCH",		// 28
			"SIGIO",		// 29
			"SIGPWR",		// 30
			"SIGSYS",		// 31
		};

		if (sig < 1 || sig > 31) return ("UNKNOWN");
		return (names[sig]);
	}

#pragma endregion

#pragma region "Handler"

	static void sigchld_handler(int sig) { (void)sig;
		int status;

		if (waitpid(g_script.shell_pid, &status, WNOHANG) > 0) {
			if (WIFEXITED(status)) g_script.exit_code = WEXITSTATUS(status);
			if (WIFSIGNALED(status)) {
				g_script.signal = WTERMSIG(status);
				g_script.exit_code = 128 + g_script.signal;
			}
			g_script.shell_running = 0;
		}
	}

	static void sigwinch_handler(int sig) { (void) sig;
		struct winsize ws;

		if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) != -1)
			ioctl(g_script.master_fd, TIOCSWINSZ, &ws);
	}

#pragma endregion

#pragma region "Set"

	int signal_set() {
		signal(SIGINT,   SIG_IGN);										// Interrupt from keyboard (Ctrl+C)
		signal(SIGTERM,  SIG_IGN);										// Request to terminate the program gracefully (sent by 'kill' or system shutdown)
		signal(SIGHUP,   SIG_IGN);										// Terminal hangup or controlling process terminated (often used to reload config)
		signal(SIGQUIT,  SIG_IGN);										// Quit from keyboard (Ctrl+\)
		signal(SIGPIPE,  SIG_IGN);										// Broken pipe (write to pipe with no readers)
		signal(SIGWINCH, sigwinch_handler);								// Window size change
		if (signal(SIGCHLD,  sigchld_handler) == SIG_ERR) return (1);	// Child process state changed (exited or stopped)

		return (0);
	}

#pragma endregion

#pragma region "Set"

	void signal_reset() {
		signal(SIGINT,   SIG_DFL);
		signal(SIGTERM,  SIG_DFL);
		signal(SIGHUP,   SIG_DFL);
		signal(SIGQUIT,  SIG_DFL);
		signal(SIGPIPE,  SIG_DFL);
		signal(SIGWINCH, SIG_DFL);
		signal(SIGCHLD,  SIG_DFL);
	}

#pragma endregion
