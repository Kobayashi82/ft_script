/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 11:33:39 by vzurera-          #+#    #+#             */
/*   Updated: 2026/03/15 19:11:17 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "script.h"

	#include <signal.h>
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

	static void sig_handler(int sig) {
		script.signal = sig;
		script.shell_running = 0;
	}

	static void sigchld_handler(int sig) { (void)sig;
		int status;
		
		if (waitpid(script.shell_pid, &status, WNOHANG) > 0) {
			if (WIFEXITED(status))		script.exit_code = WEXITSTATUS(status);
			if (WIFSIGNALED(status))	script.exit_code = 128 + WTERMSIG(status);
			script.shell_running = 0;
		}
	}

	static void sigwinch_handler(int sig) { (void) sig;
		// window_resized = 1;
	}

#pragma endregion

#pragma region "Set"

	void signal_set() {
		signal(SIGINT,   sig_handler);			// Interrupt from keyboard (Ctrl+C)
		signal(SIGTERM,  sig_handler);			// Request to terminate the program gracefully (sent by 'kill' or system shutdown)
		signal(SIGHUP,   sig_handler);			// Terminal hangup or controlling process terminated (often used to reload config)
		signal(SIGQUIT,  sig_handler);			// Quit from keyboard (Ctrl+\)
		signal(SIGPIPE,  sig_handler);			// Broken pipe (write to pipe with no readers)
		signal(SIGCHLD,  sigchld_handler);		// 
		signal(SIGWINCH, sigwinch_handler);		// Window size change
	}

#pragma endregion
