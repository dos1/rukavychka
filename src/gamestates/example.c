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
	ALLEGRO_BITMAP *bg, *bg2;
	ALLEGRO_SHADER* shader;

	ALLEGRO_BITMAP* player;

	ALLEGRO_BITMAP *bg_anim, *bg_anim2;

	bool w, s, a, d;
	double x, y;
	bool w2, s2, a2, d2;
	double x2, y2;

	bool show1, shown1;
	bool show2, shown2;
	ALLEGRO_AUDIO_STREAM *player1, *player2, *obj;

	struct Character *lisek, *smok, *myszka, *drzwi;

	bool found;
};

int Gamestate_ProgressCount = 7;

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta) {
	SwitchSpritesheet(game, data->lisek, "idle");
	data->lisek->flipY = false;
	if (data->w) {
		data->y -= delta / 3.0;
		// SwitchSpritesheet(game, data->lisek, "walk2");
		// data->lisek->flipY = false;
		SwitchSpritesheet(game, data->lisek, "walk");
	}
	if (data->s) {
		data->y += delta / 3.0;
		// SwitchSpritesheet(game, data->lisek, "walk2");
		// data->lisek->flipY = true;
		SwitchSpritesheet(game, data->lisek, "walk");
	}
	if (data->a) {
		data->x -= delta / 3.0;
		SwitchSpritesheet(game, data->lisek, "walk");
		data->lisek->flipX = false;
		data->lisek->flipY = false;
	}
	if (data->d) {
		data->x += delta / 3.0;
		SwitchSpritesheet(game, data->lisek, "walk");
		data->lisek->flipX = true;
		data->lisek->flipY = false;
	}

	if (data->w2) { data->y2 -= delta / 3.0; }
	if (data->s2) { data->y2 += delta / 3.0; }
	if (data->a2) {
		data->x2 -= delta / 3.0;
		data->smok->flipX = false;
	}
	if (data->d2) {
		data->x2 += delta / 3.0;
		data->smok->flipX = true;
	}

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

	AnimateCharacter(game, data->lisek, delta, 1.0);
	AnimateCharacter(game, data->smok, delta, 1.0);
	AnimateCharacter(game, data->myszka, delta, 1.0);
	AnimateCharacter(game, data->drzwi, delta, 1.0);

	PrintConsole(game, "lisek: %f %f", GetCharacterX(game, data->lisek), GetCharacterY(game, data->lisek));
	PrintConsole(game, "smok: %f %f", GetCharacterX(game, data->smok), GetCharacterY(game, data->smok));

	int lx = GetCharacterX(game, data->lisek), ly = GetCharacterY(game, data->lisek);
	int sx = GetCharacterX(game, data->smok), sy = GetCharacterY(game, data->smok);

	if (lx > 1300 && lx < 1920 && ly > 140 && ly < 280) {
		if (sx > 200 && sx < 550 && sy > 840 && sy < 1000) {
			if (!data->found) {
				al_rewind_audio_stream(data->obj);
				al_set_audio_stream_playing(data->obj, true);
				data->found = true;
			}
		}
	}
}

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	al_set_target_bitmap(data->bg_anim);
	al_draw_bitmap(data->bg, 0, 0, 0);
	if (!data->found)
		DrawCharacter(game, data->myszka);
	al_draw_filled_rectangle(0, 0, 1920, 1080, al_map_rgba(0, 0, 0, 32));
	al_set_target_bitmap(data->bg_anim2);
	al_draw_bitmap(data->bg2, 0, 0, 0);
	if (!data->found)
		DrawCharacter(game, data->drzwi);
	al_draw_filled_rectangle(0, 0, 1920, 1080, al_map_rgba(0, 0, 0, 32));
	SetFramebufferAsTarget(game);

	float size[2] = {al_get_bitmap_width(data->bg), al_get_bitmap_height(data->bg)};
	float offset[2] = {
		-1920 * ((data->lisek->flipX ? 1.0 : 0.0) + data->x * (data->lisek->flipX ? -1.0 : 1.0)) + sin(al_get_time() / 2.0) * (data->lisek->flipX ? -4 : 4) + al_get_bitmap_width(data->lisek->frame->bitmap) * data->lisek->flipX * data->lisek->scaleX,
		-1080 * ((data->lisek->flipY ? 1.0 : 0.0) + data->y * (data->lisek->flipY ? -1.0 : 1.0)) + cos(al_get_time() / 2.9) * (data->lisek->flipY ? -3 : 3) + al_get_bitmap_height(data->lisek->frame->bitmap) * data->lisek->scaleY / 2.0};
	float offset2[2] = {
		-1920 * ((data->smok->flipX ? 1.0 : 0.0) + data->x2 * (data->smok->flipX ? -1.0 : 1.0)) + sin(al_get_time() / 2.0) * (data->smok->flipX ? -4 : 4) + al_get_bitmap_width(data->smok->frame->bitmap) * data->smok->scaleX / 2.0,
		-1080 * ((data->smok->flipY ? 1.0 : 0.0) + data->y2 * (data->smok->flipY ? -1.0 : 1.0)) + cos(al_get_time() / 2.9) * (data->smok->flipY ? -3 : 3) + al_get_bitmap_height(data->smok->frame->bitmap) * data->smok->scaleY / 2.0};

	al_clear_to_color(al_map_rgba(255, 255, 255, 255));

	al_use_shader(data->shader);
	al_set_shader_sampler("tex", data->bg_anim, 1);
	al_set_shader_float_vector("size", 2, size, 1);
	al_set_shader_float_vector("offset", 2, offset, 1);
	al_set_shader_float("scale", data->lisek->scaleX);
	al_set_shader_bool("flipX", data->lisek->flipX);
	al_set_shader_bool("flipY", data->lisek->flipY);

	SetCharacterPosition(game, data->lisek, 1920 * data->x, 1080 * data->y, 0);
	if (data->shown1) {
		DrawCharacter(game, data->lisek);
		// al_draw_bitmap(data->player, 1920 * data->x, 1080 * data->y, 0);
	}

	al_set_shader_float_vector("offset", 2, offset2, 1);
	al_set_shader_sampler("tex", data->bg_anim2, 1);
	al_set_shader_bool("flipX", data->smok->flipX);
	al_set_shader_bool("flipY", data->smok->flipY);
	al_set_shader_float("scale", data->smok->scaleX);
	SetCharacterPosition(game, data->smok, 1920 * data->x2, 1080 * data->y2, 0);
	if (data->shown2) {
		DrawCharacter(game, data->smok);
		// al_draw_bitmap(data->player, 1920 * data->x2, 1080 * data->y2, 0);
	}

	al_use_shader(NULL);
	if (data->found) {
		DrawCharacter(game, data->myszka);
		DrawCharacter(game, data->drzwi);
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

	data->bg = al_load_bitmap(GetDataFilePath(game, "tlo1.png"));
	data->bg2 = al_load_bitmap(GetDataFilePath(game, "tlo2a.png"));
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
	al_set_audio_stream_gain(data->obj, 2.0);
	al_set_audio_stream_playmode(data->obj, ALLEGRO_PLAYMODE_ONCE);

	data->lisek = CreateCharacter(game, "lisek");
	RegisterSpritesheet(game, data->lisek, "walk");
	RegisterSpritesheet(game, data->lisek, "walk2");
	RegisterSpritesheet(game, data->lisek, "idle");
	LoadSpritesheets(game, data->lisek, progress);

	data->smok = CreateCharacter(game, "smok");
	RegisterSpritesheet(game, data->smok, "smok");
	LoadSpritesheets(game, data->smok, progress);

	data->myszka = CreateCharacter(game, "myszka");
	RegisterSpritesheet(game, data->myszka, "myszka");
	LoadSpritesheets(game, data->myszka, progress);

	data->drzwi = CreateCharacter(game, "myszka");
	RegisterSpritesheet(game, data->drzwi, "myszka");
	LoadSpritesheets(game, data->drzwi, progress);

	data->bg_anim = CreateNotPreservedBitmap(al_get_bitmap_width(data->bg), al_get_bitmap_height(data->bg));
	data->bg_anim2 = CreateNotPreservedBitmap(al_get_bitmap_width(data->bg2), al_get_bitmap_height(data->bg2));
	return data;
}

void Gamestate_Unload(struct Game* game, struct GamestateResources* data) {
	al_destroy_font(data->font);
	al_destroy_bitmap(data->bg);
	al_destroy_bitmap(data->bg2);
	al_destroy_bitmap(data->bg_anim);
	al_destroy_bitmap(data->bg_anim2);
	al_destroy_bitmap(data->player);
	DestroyShader(game, data->shader);
	al_destroy_audio_stream(data->player1);
	al_destroy_audio_stream(data->player2);
	al_destroy_audio_stream(data->obj);
	DestroyCharacter(game, data->lisek);
	DestroyCharacter(game, data->smok);
	DestroyCharacter(game, data->myszka);
	DestroyCharacter(game, data->drzwi);
	free(data);
}

void Gamestate_Start(struct Game* game, struct GamestateResources* data) {
	data->x = 0.2;
	data->y = 0.55;
	data->x2 = 0.65;
	data->y2 = 0.55;
	data->lisek->flipX = true;
	SetCharacterPosition(game, data->myszka, 1920 * 0.9, 1080 * 0.15, 0.0);
	data->myszka->scaleX = 0.15;
	data->myszka->scaleY = 0.15;
	data->drzwi->scaleX = 0.15;
	data->drzwi->scaleY = 0.15;
	data->lisek->scaleX = 0.32;
	data->lisek->scaleY = 0.32;
	data->smok->scaleX = 0.3;
	data->smok->scaleY = 0.3;
	SetCharacterPosition(game, data->drzwi, 1920 * 0.2, 1080 * 0.9, 0.0);
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {}
