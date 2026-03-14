/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 11:33:39 by vzurera-          #+#    #+#             */
/*   Updated: 2026/03/14 18:16:56 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "script.h"

	#include <signal.h>

#pragma endregion

#pragma region "Handlers"

	static void sig_handler(int sig) {
		script.signal = sig;
		script.shell_running = 0;
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
		signal(SIGWINCH, sigwinch_handler);		// Window size change
	}

#pragma endregion
