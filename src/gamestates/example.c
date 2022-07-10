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

	ALLEGRO_BITMAP *bg_anim, *bg_anim2, *mask1, *mask2, *p1, *p2, *w1, *koniec;

	bool w, s, a, d;
	double x, y;
	bool w2, s2, a2, d2;
	double x2, y2;

	bool show1, shown1;
	bool show2, shown2;
	ALLEGRO_AUDIO_STREAM *player1, *player2, *obj, *win;
	double woncount;

	struct Character *lisek, *smok, *myszka, *drzwi, *transition;

	bool found, won;
};

int Gamestate_ProgressCount = 9;

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta) {
	if (data->won) {
		data->woncount -= delta;
		if (data->woncount < 0.0) {
			game->data->won = true;
		}
	}

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
		if (data->lisek->flipX) {
			data->x += 0.12;
		}
		data->lisek->flipX = false;
		data->lisek->flipY = false;
	}
	if (data->d) {
		data->x += delta / 3.0;
		SwitchSpritesheet(game, data->lisek, "walk");
		if (!data->lisek->flipX) {
			data->x -= 0.12;
		}
		data->lisek->flipX = true;
		data->lisek->flipY = false;
	}

	if (data->w2) { data->y2 -= delta / 3.0; }
	if (data->s2) { data->y2 += delta / 3.0; }
	if (data->a2) {
		data->x2 -= delta / 3.0;
		if (data->smok->flipX) {
			data->x2 += 0.02;
		}
		data->smok->flipX = false;
	}
	if (data->d2) {
		data->x2 += delta / 3.0;
		if (!data->smok->flipX) {
			data->x2 -= 0.02;
		}
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
	AnimateCharacter(game, data->transition, delta, 1.0);

	PrintConsole(game, "lisek: %f %f", GetCharacterX(game, data->lisek), GetCharacterY(game, data->lisek));
	PrintConsole(game, "smok: %f %f", GetCharacterX(game, data->smok), GetCharacterY(game, data->smok));

	ALLEGRO_COLOR color1 = al_get_pixel(data->p1, GetCharacterX(game, data->drzwi), GetCharacterY(game, data->drzwi));
	ALLEGRO_COLOR color2 = al_get_pixel(data->p2, GetCharacterX(game, data->myszka), GetCharacterY(game, data->myszka));

	if (color1.a && color2.a) {
		if (!data->found) {
			al_rewind_audio_stream(data->obj);
			al_set_audio_stream_playing(data->obj, true);
			data->found = true;
		}
	}

	if (IsOnCharacter(game, data->drzwi, GetCharacterX(game, data->myszka) + 60, GetCharacterY(game, data->myszka), false)) {
		PrintConsole(game, "WON");
		if (!data->won) {
			al_rewind_audio_stream(data->win);
			al_set_audio_stream_playing(data->win, true);
			SetCharacterPosition(game, data->transition, GetCharacterX(game, data->myszka) + 60, GetCharacterY(game, data->myszka), 0);
			SelectSpritesheet(game, data->transition, "mikro_max");
			data->won = true;
			data->woncount = 5.0;
		}
	}
}

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	al_set_target_bitmap(data->bg_anim);
	al_clear_to_color(al_map_rgba(255, 255, 255, 255));
	al_draw_bitmap(data->bg, 0, 0, 0);
	// if (!data->found)
	//	DrawCharacter(game, data->drzwi);
	al_draw_filled_rectangle(0, 0, 1920, 1080, al_map_rgba(0, 0, 0, 20));
	al_set_target_bitmap(data->bg_anim2);
	al_clear_to_color(al_map_rgba(255, 255, 255, 255));
	al_draw_bitmap(data->bg2, 0, 0, 0);
	if (!data->found)
		DrawCharacter(game, data->myszka);
	al_draw_filled_rectangle(0, 0, 1920, 1080, al_map_rgba(0, 0, 0, 20));

	al_set_target_bitmap(data->p1);
	al_clear_to_color(al_map_rgba(0, 0, 0, 0));
	if (data->shown1) {
		DrawCharacter(game, data->lisek);
	}
	al_set_target_bitmap(data->p2);
	al_clear_to_color(al_map_rgba(0, 0, 0, 0));
	if (data->shown2) {
		DrawCharacter(game, data->smok);
		// al_draw_bitmap(data->player, 1920 * data->x2, 1080 * data->y2, 0);
	}

	SetFramebufferAsTarget(game);
	al_clear_to_color(al_map_rgba(0, 0, 0, 0));

	float size[2] = {al_get_bitmap_width(data->bg), al_get_bitmap_height(data->bg)};
	float offset[2] = {
		-1920 * ((data->lisek->flipX ? 1.0 : 0.0) + data->x * (data->lisek->flipX ? -1.0 : 1.0)) + sin(al_get_time() / 2.0) * (data->lisek->flipX ? -4 : 4) + al_get_bitmap_width(data->lisek->frame->bitmap) * data->lisek->flipX * data->lisek->scaleX,
		-1080 * ((data->lisek->flipY ? 1.0 : 0.0) + data->y * (data->lisek->flipY ? -1.0 : 1.0)) + cos(al_get_time() / 2.9) * (data->lisek->flipY ? -3 : 3) + al_get_bitmap_height(data->lisek->frame->bitmap) * data->lisek->scaleY / 2.0};
	float offset2[2] = {
		-1920 * ((data->smok->flipX ? 1.0 : 0.0) + data->x2 * (data->smok->flipX ? -1.0 : 1.0)) + sin(al_get_time() / 2.0) * (data->smok->flipX ? -4 : 4) + al_get_bitmap_width(data->smok->frame->bitmap) * data->smok->scaleX / 2.0,
		-1080 * ((data->smok->flipY ? 1.0 : 0.0) + data->y2 * (data->smok->flipY ? -1.0 : 1.0)) + cos(al_get_time() / 2.9) * (data->smok->flipY ? -3 : 3) + al_get_bitmap_height(data->smok->frame->bitmap) * data->smok->scaleY / 2.0};

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
		// DrawCharacter(game, data->drzwi);
	}

	if (data->found) {
		int mx = GetCharacterX(game, data->myszka) + 45;
		int my = GetCharacterY(game, data->myszka);
		int lx = GetCharacterX(game, data->lisek) + (data->lisek->flipX ? 420 : 230) - 60;
		int ly = GetCharacterY(game, data->lisek) - 50;
		int sx = GetCharacterX(game, data->smok) + 45 * (data->smok->flipX ? 1 : -1) - 50;
		int sy = GetCharacterY(game, data->smok) + 50;

		ALLEGRO_COLOR color1 = al_get_pixel(data->p1, mx, my);
		ALLEGRO_COLOR color2 = al_get_pixel(data->p2, mx, my);

		if (color1.a) {
			bool connected = true;

			double a = my - ly;
			double b = lx - mx;
			double c = a * lx + b * ly;

			if (mx != lx) {
				for (int x = fmin(mx, lx); x <= fmax(mx, lx); x++) {
					int y = c / b - a / b * x;

					ALLEGRO_COLOR mask1 = al_get_pixel(data->mask1, x, y);
					if (!mask1.a) {
						connected = false;
						break;
					}
					// al_draw_line(mx, my, x, y, al_map_rgb(0, 0, 0), 1);
				}
			}

			if (mx == lx) {
				for (int y = fmin(my, ly); y <= fmax(my, ly); y++) {
					ALLEGRO_COLOR mask1 = al_get_pixel(data->mask1, mx, y);
					if (!mask1.a) {
						connected = false;
						break;
					}
					// al_draw_line(mx, my, mx, y, al_map_rgb(0, 0, 0), 1);
				}
			}

			if (connected) {
				if (mx != lx) {
					data->myszka->flipX = mx < lx;
				}
				SetCharacterPosition(game, data->myszka, lx - 45, ly, 0);
			}
		}

		if (color2.a) {
			bool connected = true;

			double a = my - sy;
			double b = sx - mx;
			double c = a * sx + b * sy;

			if (mx != sx) {
				for (int x = fmin(mx, sx); x <= fmax(mx, sx); x++) {
					int y = c / b - a / b * x;

					ALLEGRO_COLOR mask2 = al_get_pixel(data->mask2, x, y);
					if (!mask2.a) {
						connected = false;
						break;
					}
					// al_draw_line(mx, my, x, y, al_map_rgb(0, 0, 0), 1);
				}
			}

			if (mx == sx) {
				for (int y = fmin(my, sy); y <= fmax(my, sy); y++) {
					ALLEGRO_COLOR mask2 = al_get_pixel(data->mask2, mx, y);
					if (!mask2.a) {
						connected = false;
						break;
					}
					// al_draw_line(mx, my, mx, y, al_map_rgb(0, 0, 0), 1);
				}
			}

			if (connected) {
				if (mx != sx) {
					data->myszka->flipX = mx < sx;
				}
				SetCharacterPosition(game, data->myszka, sx - 45, sy, 0);
			}
		}

		if (GetCharacterX(game, data->myszka) <= 0) SetCharacterPosition(game, data->myszka, 1, GetCharacterY(game, data->myszka), 0);
		if (GetCharacterY(game, data->myszka) <= 0) SetCharacterPosition(game, data->myszka, GetCharacterX(game, data->myszka), 1, 0);
		if (GetCharacterX(game, data->myszka) >= 1920) SetCharacterPosition(game, data->myszka, 1919, GetCharacterY(game, data->myszka), 0);
		if (GetCharacterY(game, data->myszka) >= 1080) SetCharacterPosition(game, data->myszka, GetCharacterX(game, data->myszka), 1079, 0);
	}

	if (data->won) {
		al_set_target_bitmap(data->w1);
		al_clear_to_color(al_map_rgba(0, 0, 0, 0));
		data->transition->scaleX = data->transition->scaleY = 0.15 * (data->transition->pos + 1);
		if (data->transition->spritesheet->frame_count == 1) {
			data->transition->scaleX = data->transition->scaleY = 2.0;
		}
		DrawCharacter(game, data->transition);

		SetFramebufferAsTarget(game);
		al_use_shader(data->shader);
		al_set_shader_sampler("tex", data->koniec, 1);
		al_set_shader_float_vector("size", 2, size, 1);
		al_set_shader_float("scale", 1.0);
		float offset[2] = {
			0, 0};
		al_set_shader_float_vector("offset", 2, offset, 1);
		al_set_shader_bool("flipX", false);
		al_set_shader_bool("flipY", false);
		al_draw_bitmap(data->w1, 0, 0, 0);
		// DrawCharacter(game, data->transition);
		al_use_shader(NULL);
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
	/*
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_SPACE)) {
		al_rewind_audio_stream(data->win);
		al_set_audio_stream_playing(data->win, true);
		SetCharacterPosition(game, data->transition, GetCharacterX(game, data->drzwi) + 60, GetCharacterY(game, data->drzwi), 0);
		SelectSpritesheet(game, data->transition, "mikro_max");
		data->won = true;
		data->woncount = 5.0;
	}
	*/
}

void* Gamestate_Load(struct Game* game, void (*progress)(struct Game*)) {
	struct GamestateResources* data = calloc(1, sizeof(struct GamestateResources));
	int flags = al_get_new_bitmap_flags();
	al_set_new_bitmap_flags(flags & ~ALLEGRO_MAG_LINEAR); // disable linear scaling for pixelarty appearance
	data->font = al_create_builtin_font();
	progress(game); // report that we progressed with the loading, so the engine can move a progress bar
	al_set_new_bitmap_flags(flags);

	data->mask1 = al_load_bitmap(GetDataFilePath(game, "plansze/maska_1_nowa.png"));
	data->mask2 = al_load_bitmap(GetDataFilePath(game, "plansze/maska_2_nowa.png"));

	data->bg = al_load_bitmap(GetDataFilePath(game, "plansze/nowsze/tlo1a_z_drzwiami.png"));
	data->bg2 = al_load_bitmap(GetDataFilePath(game, "plansze/nowsze/tlo2a_bez_drzwi.png"));
	data->shader = CreateShader(game, GetDataFilePath(game, "shaders/vertex.glsl"), GetDataFilePath(game, "shaders/combine.glsl"));

	data->koniec = al_load_bitmap(GetDataFilePath(game, "ekran_koncowy_nowy.png"));

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

	data->win = al_load_audio_stream(GetDataFilePath(game, "win.flac"), 4, 2048);
	al_set_audio_stream_playing(data->win, false);
	al_attach_audio_stream_to_mixer(data->win, game->audio.fx);
	al_set_audio_stream_gain(data->win, 1.5);
	al_set_audio_stream_playmode(data->win, ALLEGRO_PLAYMODE_ONCE);

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

	data->transition = CreateCharacter(game, "transition");
	RegisterSpritesheet(game, data->transition, "mikro_max");
	RegisterSpritesheet(game, data->transition, "still");
	LoadSpritesheets(game, data->transition, progress);

	data->bg_anim = CreateNotPreservedBitmap(al_get_bitmap_width(data->bg), al_get_bitmap_height(data->bg));
	data->bg_anim2 = CreateNotPreservedBitmap(al_get_bitmap_width(data->bg2), al_get_bitmap_height(data->bg2));

	data->p1 = CreateNotPreservedBitmap(game->viewport.width, game->viewport.height);
	data->p2 = CreateNotPreservedBitmap(game->viewport.width, game->viewport.height);
	data->w1 = CreateNotPreservedBitmap(game->viewport.width, game->viewport.height);
	return data;
}

void Gamestate_Unload(struct Game* game, struct GamestateResources* data) {
	al_destroy_font(data->font);
	al_destroy_bitmap(data->bg);
	al_destroy_bitmap(data->bg2);
	al_destroy_bitmap(data->bg_anim);
	al_destroy_bitmap(data->bg_anim2);
	al_destroy_bitmap(data->player);
	al_destroy_bitmap(data->mask1);
	al_destroy_bitmap(data->mask2);
	al_destroy_bitmap(data->p1);
	al_destroy_bitmap(data->p2);
	al_destroy_bitmap(data->w1);
	al_destroy_bitmap(data->koniec);
	DestroyShader(game, data->shader);
	al_destroy_audio_stream(data->player1);
	al_destroy_audio_stream(data->player2);
	al_destroy_audio_stream(data->obj);
	al_destroy_audio_stream(data->win);
	DestroyCharacter(game, data->lisek);
	DestroyCharacter(game, data->smok);
	DestroyCharacter(game, data->myszka);
	DestroyCharacter(game, data->drzwi);
	DestroyCharacter(game, data->transition);
	free(data);
}

void Gamestate_Start(struct Game* game, struct GamestateResources* data) {
	data->x = 0.2;
	data->y = 0.55;
	data->x2 = 0.65;
	data->y2 = 0.55;
	data->lisek->flipX = true;
	SetCharacterPosition(game, data->myszka, 360, 100, 0.0);
	data->myszka->scaleX = 0.15;
	data->myszka->scaleY = 0.15;
	data->drzwi->scaleX = 0.15;
	data->drzwi->scaleY = 0.15;
	data->lisek->scaleX = 0.32;
	data->lisek->scaleY = 0.32;
	data->smok->scaleX = 0.3;
	data->smok->scaleY = 0.3;
	SetCharacterPosition(game, data->drzwi, 1100, 750, 0.0);
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {}
