/*! \file common.c
 *  \brief Common stuff that can be used by all gamestates.
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
#include <libsuperderpy.h>

void Compositor(struct Game* game) {
	struct Gamestate* tmp = GetNextGamestate(game, NULL);

	if (al_get_time() - game->data->lasttime > 0.4) {
		game->data->posx = rand() / (float)RAND_MAX * 880;
		game->data->posy = rand() / (float)RAND_MAX * 1175;
		game->data->lasttime = al_get_time();
	}

	ClearToColor(game, al_map_rgb(255, 255, 255));

	ALLEGRO_COLOR tint = al_map_rgba_f(1.0, 1.0, 1.0, 1.0);

	if (game->loading.shown) {
		al_draw_tinted_bitmap(GetGamestateFramebuffer(game, GetGamestate(game, NULL)), tint, game->clip_rect.x, game->clip_rect.y, 0);
		tmp = NULL;
	}

	while (tmp) {
		if (IsGamestateVisible(game, tmp)) {
			al_draw_tinted_bitmap(GetGamestateFramebuffer(game, tmp), tint, game->clip_rect.x, game->clip_rect.y, 0);
		}
		tmp = GetNextGamestate(game, tmp);
	}

	al_use_shader(game->data->shader);
	al_set_clipping_rectangle(game->clip_rect.x, game->clip_rect.y - game->clip_rect.h * 0.05555, game->clip_rect.w, game->clip_rect.h * 1.1111);
	al_draw_scaled_bitmap(game->data->tex, 0, 0, al_get_bitmap_width(game->data->tex), al_get_bitmap_height(game->data->tex),
		game->clip_rect.x - game->data->posx / 1920.0 * (double)game->clip_rect.w, game->clip_rect.y - game->data->posy / 1920.0 * (double)game->clip_rect.h,
		al_get_bitmap_width(game->data->tex) / 1920.0 * (double)game->clip_rect.w * 0.4,
		al_get_bitmap_height(game->data->tex) / 1080.0 * (double)game->clip_rect.h * 0.4, 0);
	al_reset_clipping_rectangle();
	al_use_shader(NULL);

	if (game->data->won) {
		double col = 1.0 - pow(al_get_sample_instance_gain(game->data->sample_instance) / 0.32, 0.5);
		al_draw_filled_rectangle(0, 0, al_get_display_width(game->display), al_get_display_height(game->display),
			al_map_rgba_f(0, 0, 0, col));
	}
}

bool GlobalEventHandler(struct Game* game, ALLEGRO_EVENT* ev) {
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_M)) {
		ToggleMute(game);
	}

	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_F)) {
		ToggleFullscreen(game);
	}

	return false;
}

void PreLogic(struct Game* game, double delta) {
	if (game->data->won) {
		if (al_get_sample_instance_gain(game->data->sample_instance) == 0.00) {
			SetBackgroundColor(game, al_map_rgb(0, 0, 0));
			al_rest(1.0);
			QuitGame(game, false);
		}
		al_set_sample_instance_gain(game->data->sample_instance, fmax(0.00, al_get_sample_instance_gain(game->data->sample_instance) - delta / 16.0));
	} else {
		al_set_sample_instance_gain(game->data->sample_instance, fmin(0.32, al_get_sample_instance_gain(game->data->sample_instance) + delta / 8.0));
	}
}

struct CommonResources* CreateGameData(struct Game* game) {
	struct CommonResources* data = calloc(1, sizeof(struct CommonResources));
	data->tex = al_load_bitmap(GetDataFilePath(game, "tex.jpg"));
	data->shader = CreateShader(game, GetDataFilePath(game, "shaders/vertex.glsl"), GetDataFilePath(game, "shaders/alpha.glsl"));

	data->sample = al_load_sample(GetDataFilePath(game, "synth.flac"));
	data->sample_instance = al_create_sample_instance(data->sample);
	al_attach_sample_instance_to_mixer(data->sample_instance, game->audio.music);
	al_set_sample_instance_playmode(data->sample_instance, ALLEGRO_PLAYMODE_LOOP);
	al_set_sample_instance_gain(data->sample_instance, 0.0);

	return data;
}

void DestroyGameData(struct Game* game) {
	al_destroy_bitmap(game->data->tex);
	al_destroy_sample_instance(game->data->sample_instance);
	al_destroy_sample(game->data->sample);
	DestroyShader(game, game->data->shader);
	free(game->data);
}
