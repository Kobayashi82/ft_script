/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 14:09:39 by vzurera-          #+#    #+#             */
/*   Updated: 2026/03/14 11:57:49 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "script.h"

#pragma endregion

#pragma region "Variables"

	t_script script;

#pragma endregion

#pragma region "Main"

	int main(int argc, const char **argv) {
		int ret = 0;
		if ((ret = parse_options(argc, argv))) return (ret - 1);

		return (ret);
	}

#pragma endregion
