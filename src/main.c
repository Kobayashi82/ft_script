/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 14:09:39 by vzurera-          #+#    #+#             */
/*   Updated: 2026/03/12 15:54:29 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "script.h"
	#include "options.h"
	#include "utils.h"

#pragma endregion

#pragma region "Variables"


#pragma endregion

#pragma region "Main"

	int main(int argc, const char **argv) {
		t_script script;

		ft_memset(&script, 0, sizeof(t_script));

		int ret = 0;
		if ((ret = options_parse(&script.options, argc, argv))) return (ret - 1);


		return (ret);
	}

#pragma endregion
