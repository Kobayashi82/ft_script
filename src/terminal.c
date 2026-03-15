/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   terminal.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 11:50:25 by vzurera-          #+#    #+#             */
/*   Updated: 2026/03/15 19:03:19 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "script.h"

#pragma endregion

#pragma region "Enable"

	int raw_mode_enable() {
		if (!script.raw_enabled) {
			if (tcgetattr(STDIN_FILENO, &script.original_termios) == -1) {
				write(STDERR_FILENO, "Failed to get terminal attributes\n", 34);
				return (1);
			}

			struct termios raw = script.original_termios;
			raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
			raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
			raw.c_cflag |= (CS8);
			raw.c_oflag &= ~(OPOST);
			raw.c_cc[VMIN] = 1;
			raw.c_cc[VTIME] = 0;

			if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
				write(STDERR_FILENO, "Failed to set raw mode\n", 23);
				return (1);
			}

			script.raw_enabled = 1;
		}

		return (0);
	}

#pragma endregion

#pragma region "Disable"

	void raw_mode_disable() {
		if (script.raw_enabled) {
			tcsetattr(STDIN_FILENO, TCSAFLUSH, &script.original_termios);
			write(STDOUT_FILENO, "\n", 1);
			script.raw_enabled = 0;
		}
	}

#pragma endregion
