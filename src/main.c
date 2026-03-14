/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 14:09:39 by vzurera-          #+#    #+#             */
/*   Updated: 2026/03/14 19:00:58 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "script.h"

#pragma endregion

#pragma region "Variables"

	t_script script;

#pragma endregion

#pragma region "Set ENV"

	static void set_env(char **env) {
		int i = 0;
		int found_shell = 0;
		int found_term = 0;
		for (; env[i] && i < 253; ++i) {
			if (!found_shell && !ft_strncmp(env[i], "SHELL=", 6)) {
				found_shell = 1;
				if (access(env[i] + 6, X_OK)) {
					script.env[i] = "SHELL=/bin/sh";
					script.shell_path = script.env[i] + 6;
					continue;
				}
				script.shell_path = env[i] + 6;
			}
			if (!found_term  && !ft_strncmp(env[i], "TERM=", 5)) found_term = 1;
			script.env[i] = env[i];
		}
		if (!found_shell) {
			script.env[i] = "SHELL=/bin/sh";
			script.shell_path = script.env[i] + 6;
			i++;
		}
		if (!found_term)  script.env[i++] = "TERM=xterm-256color";
		script.env[i] = NULL;
	}

#pragma endregion

#pragma region "Main"

	int main(int argc, char **argv, char **env) {
		int ret = parse_options(argc, argv);
		if (ret) return (ret - 1);

		set_env(env);
		ret = shell_start();
		if (!ret) ret = raw_mode_enable();
		if (!ret) {
			signal_set();
			ret = select_loop();
		}
		raw_mode_disable();

		return (ret);
	}

#pragma endregion
