/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 11:33:39 by vzurera-          #+#    #+#             */
/*   Updated: 2026/03/14 12:02:20 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "script.h"

	#include <signal.h>

#pragma endregion

#pragma region "Handlers"

	#pragma region "SIGINT"

		static void sigint_handler(int sig) {
			raw_mode_disable();
			_exit(128 + sig);
		}

	#pragma endregion

	#pragma region "SIGTERM"

		static void sigterm_handler(int sig) {
			raw_mode_disable();
			_exit(128 + sig);
		}

	#pragma endregion

	#pragma region "SIGUP"

		static void sigup_handler(int sig) {
			raw_mode_disable();
			_exit(128 + sig);
		}

	#pragma endregion

	#pragma region "SIGQUIT"

		static void sigquit_handler(int sig) {
			raw_mode_disable();
			_exit(128 + sig);
		}

	#pragma endregion

	#pragma region "SIGWINCH"

		static void sigwinch_handler(int sig) { (void) sig;
			// window_resized = 1;
		}

	#pragma endregion

	#pragma region "SIGPIPE"

		static void sigpipe_handler(int sig) {
			raw_mode_disable();
			_exit(128 + sig);
		}

	#pragma endregion

	#pragma region "SIGSEV"

		static void sigsev_handler(int sig) {
			raw_mode_disable();
			signal(SIGSEGV, SIG_DFL);
			raise(SIGSEGV);
			_exit(128 + sig);
		}

	#pragma endregion

#pragma endregion

#pragma region "Set"

	int signal_set() {
		int result = 0;

		if (signal(SIGINT,   sigint_handler)	== SIG_ERR) result++;	// Interrupt from keyboard (Ctrl+C)
		if (signal(SIGTERM,  sigterm_handler)	== SIG_ERR) result++;	// Request to terminate the program gracefully (sent by 'kill' or system shutdown)
		if (signal(SIGHUP,   sigup_handler)		== SIG_ERR) result++;	// Terminal hangup or controlling process terminated (often used to reload config)
		if (signal(SIGQUIT,  sigquit_handler)	== SIG_ERR) result++;	// Quit from keyboard (Ctrl+\)
		if (signal(SIGPIPE,  sigpipe_handler)	== SIG_ERR) result++;	// Broken pipe (write to pipe with no readers)
		if (signal(SIGSEGV,  sigsev_handler)	== SIG_ERR) result++;	// Invalid memory reference (segmentation fault)
		if (signal(SIGWINCH, sigwinch_handler)	== SIG_ERR) result++;	// Window size change

		return (result);
	}

#pragma endregion
