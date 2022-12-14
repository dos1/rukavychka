/*! \file main.c
 *  \brief Main file of Rukavychka.
 */
/*
 * Copyright (c) Sebastian Krzyszkowiak <dos@dosowisko.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "common.h"
#include "defines.h"
#include <libsuperderpy.h>
#include <signal.h>
#include <stdio.h>

static _Noreturn void derp(int sig) {
	ssize_t __attribute__((unused)) n = write(STDERR_FILENO, "Segmentation fault\nI just don't know what went wrong!\n", 54);
	abort();
}

int main(int argc, char** argv) {
#ifdef __vita__
	scePowerSetGpuClockFrequency(222);
#endif

	signal(SIGSEGV, derp);

	srand(time(NULL));

	al_set_org_name(LIBSUPERDERPY_VENDOR);
	al_set_app_name(LIBSUPERDERPY_GAMENAME_PRETTY);

	struct Game* game = libsuperderpy_init(argc, argv, LIBSUPERDERPY_GAMENAME,
		(struct Params){
			1920,
			1080,
			.handlers = {
				.event = GlobalEventHandler,
				.destroy = DestroyGameData,
				.prelogic = PreLogic,
				.postdraw = PostDraw,
			},
			.show_loading_on_launch = true,
			.bg_color = al_map_rgb(255, 255, 255),
		});
	if (!game) { return 1; }

	LoadGamestate(game, "holypangolin");
	LoadGamestate(game, "game");
	StartGamestate(game, "holypangolin");

	game->data = CreateGameData(game);

#ifdef __EMSCRIPTEN__
	EM_ASM({
		document.getElementById('loading').style.display = 'none';
	});
#endif

	return libsuperderpy_run(game);
}
