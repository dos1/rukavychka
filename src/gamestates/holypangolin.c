/*! \file holypangolin.c
 *  \brief Rukavychka's Holy Pangolin logo.
 */
/*
 * Copyright (c) Sebastian Krzyszkowiak <dos@dosowisko.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "../common.h"
#include <libsuperderpy.h>

#define NEXT_GAMESTATE "game"
#define SKIP_GAMESTATE NEXT_GAMESTATE

struct GamestateResources {
	ALLEGRO_BITMAP* bmp;
	ALLEGRO_BITMAP* bg;
	ALLEGRO_SHADER* shader;
	double counter;
	ALLEGRO_AUDIO_STREAM* monkeys;
};

int Gamestate_ProgressCount = 1;

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta) {
	data->counter += delta * 60;
	if (data->counter > 60 * 6) {
		SwitchCurrentGamestate(game, NEXT_GAMESTATE);
	}
}

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	float offset[2] = {0.1521875 / 0.66 - 20 * data->counter / (60.0 * 6.0) / game->viewport.width * 0.66 * 2.0, -15 * data->counter / (60.0 * 6.0) / game->viewport.height * 0.66 * 2.0};
	float scale[2] = {0.66, 0.66};

	al_clear_to_color(al_map_rgba(0, 0, 0, 0));
	al_use_shader(data->shader);
	al_set_shader_sampler("tex", data->bg, 1);
	al_set_shader_float_vector("offset", 2, offset, 1);
	al_set_shader_float_vector("scale", 2, scale, 1);
	al_set_shader_float("brightness", 1.0);
	al_set_shader_float("saturation", 1.2);

	double pos = pow(sin(data->counter / (15 * 7.5)), 0.9);
	al_draw_tinted_scaled_bitmap(data->bmp, al_map_rgba_f(pos, pos, pos, pos), 0, 0, al_get_bitmap_width(data->bmp), al_get_bitmap_height(data->bmp), game->viewport.width * 0.165 + (game->viewport.width - (game->viewport.height * 0.66)) / 4.0, game->viewport.height * 0.165, game->viewport.height * 0.66, game->viewport.height * 0.66, 0);

	al_use_shader(NULL);
	//	al_draw_filled_rectangle(0, 0, game->viewport.width, game->viewport.height, al_map_rgba_f(1 - pos / 280.0, 1 - pos / 280.0, 1 - pos / 280.0, 1 - pos / 280.0));
}

void Gamestate_ProcessEvent(struct Game* game, struct GamestateResources* data, ALLEGRO_EVENT* ev) {
	if (((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) || (ev->type == ALLEGRO_EVENT_TOUCH_END)) {
		SwitchCurrentGamestate(game, SKIP_GAMESTATE);
	}
}

void* Gamestate_Load(struct Game* game, void (*progress)(struct Game*)) {
	struct GamestateResources* data = malloc(sizeof(struct GamestateResources));
	data->bmp = al_load_bitmap(GetDataFilePath(game, "holypangolin.png"));
	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar

	data->monkeys = al_load_audio_stream(GetDataFilePath(game, "holypangolin.flac"), 4, 2048);
	al_set_audio_stream_playing(data->monkeys, false);
	al_attach_audio_stream_to_mixer(data->monkeys, game->audio.fx);
	al_set_audio_stream_gain(data->monkeys, 0.75);

	data->bg = al_load_bitmap(GetDataFilePath(game, "bg.jpg"));
	data->shader = CreateShader(game, GetDataFilePath(game, "shaders/vertex.glsl"), GetDataFilePath(game, "shaders/combine.glsl"));

	return data;
}

void Gamestate_Unload(struct Game* game, struct GamestateResources* data) {
	al_destroy_bitmap(data->bmp);
	al_destroy_bitmap(data->bg);
	DestroyShader(game, data->shader);
	al_destroy_audio_stream(data->monkeys);
	free(data);
}

void Gamestate_Start(struct Game* game, struct GamestateResources* data) {
	data->counter = 0;
	al_rewind_audio_stream(data->monkeys);
	// al_set_audio_stream_playing(data->monkeys, true);

	al_play_sample_instance(game->data->sample_instance);
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {
	al_set_audio_stream_playing(data->monkeys, false);
}

void Gamestate_Pause(struct Game* game, struct GamestateResources* data) {
	al_set_audio_stream_playing(data->monkeys, false);
}

void Gamestate_Resume(struct Game* game, struct GamestateResources* data) {
	// al_set_audio_stream_playing(data->monkeys, true);
}
