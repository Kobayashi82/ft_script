/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   terminal.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 11:50:25 by vzurera-          #+#    #+#             */
/*   Updated: 2026/03/16 13:51:41 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "script.h"

	#include <sys/ioctl.h>

#pragma endregion

#pragma region "Enable"

	int raw_mode_enable() {
		struct termios t;
		if (ioctl(STDIN_FILENO, TCGETS, &t) == -1) return (0);

		if (!g_script.raw_enabled) {
			if (ioctl(STDIN_FILENO, TCGETS, &g_script.original_termios) == -1) {
				write(STDERR_FILENO, "Failed to get terminal attributes\n", 34);
				return (1);
			}

			struct termios raw = g_script.original_termios;
			raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
			raw.c_iflag &= ~(BRKINT | INPCK | ISTRIP | IXON);
			raw.c_cflag |= (CS8);
			raw.c_cc[VMIN] = 1;
			raw.c_cc[VTIME] = 0;

			if (ioctl(STDIN_FILENO, TCSETSF, &raw) == -1) {
				write(STDERR_FILENO, "Failed to set raw mode\n", 23);
				return (1);
			}

			g_script.raw_enabled = 1;
		}

		return (0);
	}

#pragma endregion

#pragma region "Disable"

	int raw_mode_disable() {
		int ret = 0;

		if (g_script.raw_enabled) {
			if (ioctl(STDIN_FILENO, TCSETSF, &g_script.original_termios) == -1) {
				write(STDERR_FILENO, "Failed to disable raw mode\n", 27);
				ret = 1;
			}
			g_script.raw_enabled = 0;
		}

		return (ret);
	}

#pragma endregion
