/*! \file example.c
 *  \brief Example gamestate.
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

#include "../common.h"
#include <libsuperderpy.h>

struct GamestateResources {
	ALLEGRO_FONT* font;
	ALLEGRO_BITMAP* bg;
	ALLEGRO_SHADER* shader;
	int blink_counter;

	ALLEGRO_BITMAP* player;

	bool w, s, a, d;
	double x, y;
	bool w2, s2, a2, d2;
	double x2, y2;

	bool show1, shown1;
	bool show2, shown2;
	ALLEGRO_AUDIO_STREAM *player1, *player2, *obj;
};

int Gamestate_ProgressCount = 1;

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta) {
	data->blink_counter++;
	if (data->blink_counter >= 60) {
		data->blink_counter = 0;
	}

	if (data->w) data->y -= delta / 3.0;
	if (data->s) data->y += delta / 3.0;
	if (data->a) data->x -= delta / 3.0;
	if (data->d) data->x += delta / 3.0;

	if (data->w2) data->y2 -= delta / 3.0;
	if (data->s2) data->y2 += delta / 3.0;
	if (data->a2) data->x2 -= delta / 3.0;
	if (data->d2) data->x2 += delta / 3.0;

	if (data->show1 && !data->shown1) {
		data->shown1 = true;
		al_rewind_audio_stream(data->player1);
		al_set_audio_stream_playing(data->player1, true);
	}
	if (data->show2 && !data->shown2) {
		data->shown2 = true;
		al_rewind_audio_stream(data->player2);
		al_set_audio_stream_playing(data->player2, true);
	}
}

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	float size[2] = {al_get_bitmap_width(data->bg), al_get_bitmap_height(data->bg)};
	float offset[2] = {-1920 * data->x + sin(al_get_time() / 2.0) * 4, -1080 * data->y + cos(al_get_time() / 2.9) * 3};
	float offset2[2] = {-1920 * data->x2 + sin(al_get_time() / 2.0) * 4, -1080 * data->y2 + cos(al_get_time() / 2.9) * 3};
	al_clear_to_color(al_map_rgba(0, 0, 0, 0));

	al_use_shader(data->shader);
	al_set_shader_sampler("tex", data->bg, 1);
	al_set_shader_float_vector("size", 2, size, 1);
	al_set_shader_float_vector("offset", 2, offset, 1);

	if (data->shown1) {
		al_draw_bitmap(data->player, 1920 * data->x, 1080 * data->y, 0);
	}

	al_set_shader_float_vector("offset", 2, offset2, 1);
	if (data->shown2) {
		al_draw_bitmap(data->player, 1920 * data->x2, 1080 * data->y2, 0);
	}

	al_use_shader(NULL);

	if (data->blink_counter < 50) {
		//	al_draw_text(data->font, al_map_rgb(0, 0, 0), game->viewport.width / 2.0, game->viewport.height / 2.0,
		//	ALLEGRO_ALIGN_CENTRE, "Nothing to see here, move along!");
	}
}

void Gamestate_ProcessEvent(struct Game* game, struct GamestateResources* data, ALLEGRO_EVENT* ev) {
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {
		UnloadCurrentGamestate(game); // mark this gamestate to be stopped and unloaded
		// When there are no active gamestates, the engine will quit.
	}

	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_W)) {
		data->w = true;
		data->show1 = true;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_A)) {
		data->a = true;
		data->show1 = true;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_S)) {
		data->s = true;
		data->show1 = true;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_D)) {
		data->d = true;
		data->show1 = true;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_UP) && (ev->keyboard.keycode == ALLEGRO_KEY_W)) {
		data->w = false;
		data->show1 = true;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_UP) && (ev->keyboard.keycode == ALLEGRO_KEY_A)) {
		data->a = false;
		data->show1 = true;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_UP) && (ev->keyboard.keycode == ALLEGRO_KEY_S)) {
		data->s = false;
		data->show1 = true;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_UP) && (ev->keyboard.keycode == ALLEGRO_KEY_D)) {
		data->d = false;
		data->show1 = true;
	}

	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_UP)) {
		data->w2 = true;
		data->show2 = true;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_LEFT)) {
		data->a2 = true;
		data->show2 = true;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_DOWN)) {
		data->s2 = true;
		data->show2 = true;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_RIGHT)) {
		data->d2 = true;
		data->show2 = true;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_UP) && (ev->keyboard.keycode == ALLEGRO_KEY_UP)) {
		data->w2 = false;
		data->show2 = true;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_UP) && (ev->keyboard.keycode == ALLEGRO_KEY_LEFT)) {
		data->a2 = false;
		data->show2 = true;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_UP) && (ev->keyboard.keycode == ALLEGRO_KEY_DOWN)) {
		data->s2 = false;
		data->show2 = true;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_UP) && (ev->keyboard.keycode == ALLEGRO_KEY_RIGHT)) {
		data->d2 = false;
		data->show2 = true;
	}
}

void* Gamestate_Load(struct Game* game, void (*progress)(struct Game*)) {
	struct GamestateResources* data = calloc(1, sizeof(struct GamestateResources));
	int flags = al_get_new_bitmap_flags();
	al_set_new_bitmap_flags(flags & ~ALLEGRO_MAG_LINEAR); // disable linear scaling for pixelarty appearance
	data->font = al_create_builtin_font();
	progress(game); // report that we progressed with the loading, so the engine can move a progress bar
	al_set_new_bitmap_flags(flags);

	data->bg = al_load_bitmap(GetDataFilePath(game, "bg.jpg"));
	data->shader = CreateShader(game, GetDataFilePath(game, "shaders/vertex.glsl"), GetDataFilePath(game, "shaders/combine.glsl"));

	data->player = al_load_bitmap(GetDataFilePath(game, "player.png"));

	data->player1 = al_load_audio_stream(GetDataFilePath(game, "player2.flac"), 4, 2048);
	al_set_audio_stream_playing(data->player1, false);
	al_attach_audio_stream_to_mixer(data->player1, game->audio.fx);
	al_set_audio_stream_gain(data->player1, 1.5);
	al_set_audio_stream_playmode(data->player1, ALLEGRO_PLAYMODE_ONCE);

	data->player2 = al_load_audio_stream(GetDataFilePath(game, "player1.flac"), 4, 2048);
	al_set_audio_stream_playing(data->player2, false);
	al_attach_audio_stream_to_mixer(data->player2, game->audio.fx);
	al_set_audio_stream_gain(data->player2, 1.0);
	al_set_audio_stream_playmode(data->player2, ALLEGRO_PLAYMODE_ONCE);

	data->obj = al_load_audio_stream(GetDataFilePath(game, "obj.flac"), 4, 2048);
	al_set_audio_stream_playing(data->obj, false);
	al_attach_audio_stream_to_mixer(data->obj, game->audio.fx);
	al_set_audio_stream_gain(data->obj, 1.5);
	al_set_audio_stream_playmode(data->obj, ALLEGRO_PLAYMODE_ONCE);

	return data;
}

void Gamestate_Unload(struct Game* game, struct GamestateResources* data) {
	al_destroy_font(data->font);
	free(data);
}

void Gamestate_Start(struct Game* game, struct GamestateResources* data) {
	data->blink_counter = 0;
	data->x = 0.2;
	data->y = 0.55;
	data->x2 = 0.65;
	data->y2 = 0.55;
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {}
