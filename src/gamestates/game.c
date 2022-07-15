/*! \file game.c
 *  \brief Rukavychka gameplay.
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
	ALLEGRO_BITMAP *bg, *bg2;
	ALLEGRO_SHADER* shader;

	ALLEGRO_BITMAP *bg_anim2, *mask1, *mask2, *koniec;

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

int Gamestate_ProgressCount = 18;

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta) {
	if (data->won) {
		data->woncount -= delta;
		if (data->woncount < 0.0) {
			game->data->won = true;
		}
	}

	data->lisek->flipY = false;
	if (data->w) {
		data->y -= delta * 1.777 / 4.0;
		// SwitchSpritesheet(game, data->lisek, "walk2");
		// data->lisek->flipY = false;
	}
	if (data->s) {
		data->y += delta * 1.777 / 4.0;
		// SwitchSpritesheet(game, data->lisek, "walk2");
		// data->lisek->flipY = true;
	}
	if (data->a) {
		data->x -= delta / 4.0;
		data->lisek->flipX = false;
		data->lisek->flipY = false;
	}
	if (data->d) {
		data->x += delta / 4.0;
		data->lisek->flipX = true;
		data->lisek->flipY = false;
	}

	if (data->w || data->a || data->s || data->d) {
		SwitchSpritesheet(game, data->lisek, "walk");
	} else {
		SwitchSpritesheet(game, data->lisek, "idle");
	}

	if (data->w2) {
		data->y2 -= delta * 1.777 / 4.0;
	}
	if (data->s2) {
		data->y2 += delta * 1.777 / 4.0;
	}
	if (data->a2) {
		data->x2 -= delta / 4.0;
		data->smok->flipX = false;
	}
	if (data->d2) {
		data->x2 += delta / 4.0;
		data->smok->flipX = true;
	}

	if (data->show1 && !data->shown1) {
		data->shown1 = true;
		PrintConsole(game, "lisek visible");
		al_rewind_audio_stream(data->player1);
		al_set_audio_stream_playing(data->player1, true);
	}
	if (data->show2 && !data->shown2) {
		data->shown2 = true;
		PrintConsole(game, "smok visible");
		al_rewind_audio_stream(data->player2);
		al_set_audio_stream_playing(data->player2, true);
	}

	AnimateCharacter(game, data->lisek, delta, 1.0);
	AnimateCharacter(game, data->smok, delta, 1.0);
	AnimateCharacter(game, data->myszka, delta, 1.0);
	AnimateCharacter(game, data->drzwi, delta, 1.0);
	AnimateCharacter(game, data->transition, delta, 1.0);

	// PrintConsole(game, "lisek: %f %f", GetCharacterX(game, data->lisek), GetCharacterY(game, data->lisek));
	// PrintConsole(game, "smok: %f %f", GetCharacterX(game, data->smok), GetCharacterY(game, data->smok));

	if (IsOnCharacter(game, data->lisek, GetCharacterX(game, data->drzwi), GetCharacterY(game, data->drzwi), true) &&
		IsOnCharacter(game, data->smok, GetCharacterX(game, data->myszka), GetCharacterY(game, data->myszka), true)) {
		if (!data->found) {
			PrintConsole(game, "found!");
			al_rewind_audio_stream(data->obj);
			al_set_audio_stream_playing(data->obj, true);
			data->found = true;
		}
	}

	if (IsOnCharacter(game, data->drzwi, GetCharacterX(game, data->myszka) + 60, GetCharacterY(game, data->myszka), false)) {
		if (!data->won) {
			PrintConsole(game, "WON");
			al_rewind_audio_stream(data->win);
			al_set_audio_stream_playing(data->win, true);
			SetCharacterPosition(game, data->transition, GetCharacterX(game, data->myszka) + 60, GetCharacterY(game, data->myszka), 0);
			SelectSpritesheet(game, data->transition, "mikro_max");
			data->won = true;
			data->woncount = 5.0;
		}
	}

	if (data->x < -0.23) data->x = -0.23;
	if (data->x > 1.25) data->x = 1.25;
	if (data->y < -0.32) data->y = -0.32;
	if (data->y > 1.21) data->y = 1.21;

	if (data->x2 < -0.19) data->x2 = -0.19;
	if (data->x2 > 1.19) data->x2 = 1.19;
	if (data->y2 < -0.16) data->y2 = -0.16;
	if (data->y2 > 1.25) data->y2 = 1.25;

	if (data->found) {
		int mx = GetCharacterX(game, data->myszka);
		int my = GetCharacterY(game, data->myszka);
		int lx = GetCharacterX(game, data->lisek);
		int ly = GetCharacterY(game, data->lisek);
		int sx = GetCharacterX(game, data->smok);
		int sy = GetCharacterY(game, data->smok);

		if (IsOnCharacter(game, data->lisek, mx, my, true)) {
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
				SetCharacterPosition(game, data->myszka, lx, ly, 0);
			}
		}

		if (IsOnCharacter(game, data->smok, mx, my, true)) {
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
				SetCharacterPosition(game, data->myszka, sx, sy, 0);
			}
		}

		if (GetCharacterX(game, data->myszka) <= 0) SetCharacterPosition(game, data->myszka, 1, GetCharacterY(game, data->myszka), 0);
		if (GetCharacterY(game, data->myszka) <= 0) SetCharacterPosition(game, data->myszka, GetCharacterX(game, data->myszka), 1, 0);
		if (GetCharacterX(game, data->myszka) >= 1920) SetCharacterPosition(game, data->myszka, 1919, GetCharacterY(game, data->myszka), 0);
		if (GetCharacterY(game, data->myszka) >= 1080) SetCharacterPosition(game, data->myszka, GetCharacterX(game, data->myszka), 1079, 0);
	}
}

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	if (!data->found) {
		al_set_target_bitmap(data->bg_anim2);
		al_clear_to_color(al_map_rgba(255, 255, 255, 255));
		al_draw_bitmap(data->bg2, 0, 0, 0);
		if (!data->found)
			DrawCharacter(game, data->myszka);

		SetFramebufferAsTarget(game);
	}
	ClearToColor(game, al_map_rgba(255, 255, 255, 255));

	float scale[2] = {1.0, 1.0};
	float offset[2] = {
		sin(al_get_time() / 2.0) * 4.0 / 1920.0,
		cos(al_get_time() / 2.9) * 3.0 / 1080.0,
	};

	al_use_shader(data->shader);
	al_set_shader_sampler("tex", data->bg, 1);
	al_set_shader_float_vector("offset", 2, offset, 1);
	al_set_shader_float_vector("scale", 2, scale, 1);
	al_set_shader_float("saturation", 1.2);
	al_set_shader_float("brightness", 0.922);

	SetCharacterPosition(game, data->lisek, 1920 * data->x, 1080 * data->y, 0);

	if (data->shown1) {
		DrawCharacter(game, data->lisek);
	}

	al_set_shader_sampler("tex", data->found ? data->bg2 : data->bg_anim2, 1);
	SetCharacterPosition(game, data->smok, 1920 * data->x2, 1080 * data->y2, 0);
	if (data->shown2) {
		DrawCharacter(game, data->smok);
	}

	al_use_shader(NULL);
	if (data->found) {
		DrawCharacter(game, data->myszka);
		// DrawCharacter(game, data->drzwi);
	}

	if (data->won) {
		al_use_shader(data->shader);
		al_set_shader_sampler("tex", data->koniec, 1);
		float offset0[2] = {0, 0};
		al_set_shader_float_vector("offset", 2, offset0, 1);
		al_set_shader_float_vector("scale", 2, scale, 1);
		al_set_shader_float("brightness", 1.0);
		al_set_shader_float("saturation", 1.2);

		data->transition->scaleX = data->transition->scaleY = 0.30 * (data->transition->pos + 1);
		if (data->transition->spritesheet->frame_count == 1) {
			data->transition->scaleX = data->transition->scaleY = 4.0;
		}
		DrawCharacter(game, data->transition);

		al_use_shader(NULL);
	}

	/*
	DrawDebugCharacter(game, data->myszka);
	DrawDebugCharacter(game, data->drzwi);
	DrawDebugCharacter(game, data->smok);
	DrawDebugCharacter(game, data->lisek);
	DrawDebugCharacter(game, data->transition);
	*/
}

void Gamestate_ProcessEvent(struct Game* game, struct GamestateResources* data, ALLEGRO_EVENT* ev) {
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {
#ifndef __EMSCRIPTEN__
		UnloadCurrentGamestate(game); // mark this gamestate to be stopped and unloaded
		// When there are no active gamestates, the engine will quit.
#endif
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

#ifdef ALLEGRO_WITH_XWINDOWS
	if (ev->type == ALLEGRO_EVENT_JOYSTICK_AXIS && (ev->joystick.stick == 0 || ev->joystick.stick == 3)) {
#else
	if (ev->type == ALLEGRO_EVENT_JOYSTICK_AXIS && ev->joystick.stick == 0) {
#endif
		if (ev->joystick.axis == 1) {
			if (ev->joystick.pos < -0.1) {
				data->w = true;
				data->show1 = true;
			} else {
				data->w = false;
			}
			if (ev->joystick.pos > 0.1) {
				data->s = true;
				data->show1 = true;
			} else {
				data->s = false;
			}
		}
		if (ev->joystick.axis == 0) {
			if (ev->joystick.pos < -0.1) {
				data->a = true;
				data->show1 = true;
			} else {
				data->a = false;
			}
			if (ev->joystick.pos > 0.1) {
				data->d = true;
				data->show1 = true;
			} else {
				data->d = false;
			}
		}
	}

	if (ev->type == ALLEGRO_EVENT_JOYSTICK_AXIS && ev->joystick.stick == 1) {
		if (ev->joystick.axis == 1) {
			if (ev->joystick.pos < -0.1) {
				data->w2 = true;
				data->show2 = true;
			} else {
				data->w2 = false;
			}
			if (ev->joystick.pos > 0.1) {
				data->s2 = true;
				data->show2 = true;
			} else {
				data->s2 = false;
			}
		} else {
			if (ev->joystick.pos < -0.1) {
				data->a2 = true;
				data->show2 = true;
			} else {
				data->a2 = false;
			}
			if (ev->joystick.pos > 0.1) {
				data->d2 = true;
				data->show2 = true;
			} else {
				data->d2 = false;
			}
		}
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
	al_set_new_bitmap_flags((flags | ALLEGRO_MEMORY_BITMAP) & ~(ALLEGRO_VIDEO_BITMAP | ALLEGRO_CONVERT_BITMAP)); // use memory bitmap for faster pixel access
	data->mask1 = al_load_bitmap(GetDataFilePath(game, "plansze/maska_1_nowa.png"));
	progress(game);
	data->mask2 = al_load_bitmap(GetDataFilePath(game, "plansze/maska_2_nowa.png"));
	progress(game);
	al_set_new_bitmap_flags(flags);

	data->bg = al_load_bitmap(GetDataFilePath(game, "plansze/nowsze/tlo1a_z_drzwiami.png"));
	progress(game);
	data->bg2 = al_load_bitmap(GetDataFilePath(game, "plansze/nowsze/tlo2a_bez_drzwi.png"));
	progress(game);
	data->shader = CreateShader(game, GetDataFilePath(game, "shaders/vertex.glsl"), GetDataFilePath(game, "shaders/combine.glsl"));
	progress(game);

	data->koniec = al_load_bitmap(GetDataFilePath(game, "ekran_koncowy_nowy.png"));
	progress(game);

	data->player1 = al_load_audio_stream(GetDataFilePath(game, "player2.flac"), 4, 2048);
	al_set_audio_stream_playing(data->player1, false);
	al_attach_audio_stream_to_mixer(data->player1, game->audio.fx);
	al_set_audio_stream_gain(data->player1, 1.5);
	al_set_audio_stream_playmode(data->player1, ALLEGRO_PLAYMODE_ONCE);
	progress(game);

	data->player2 = al_load_audio_stream(GetDataFilePath(game, "player1.flac"), 4, 2048);
	al_set_audio_stream_playing(data->player2, false);
	al_attach_audio_stream_to_mixer(data->player2, game->audio.fx);
	al_set_audio_stream_gain(data->player2, 1.0);
	al_set_audio_stream_playmode(data->player2, ALLEGRO_PLAYMODE_ONCE);
	progress(game);

	data->obj = al_load_audio_stream(GetDataFilePath(game, "obj.flac"), 4, 2048);
	al_set_audio_stream_playing(data->obj, false);
	al_attach_audio_stream_to_mixer(data->obj, game->audio.fx);
	al_set_audio_stream_gain(data->obj, 2.0);
	al_set_audio_stream_playmode(data->obj, ALLEGRO_PLAYMODE_ONCE);
	progress(game);

	data->win = al_load_audio_stream(GetDataFilePath(game, "win.flac"), 4, 2048);
	al_set_audio_stream_playing(data->win, false);
	al_attach_audio_stream_to_mixer(data->win, game->audio.fx);
	al_set_audio_stream_gain(data->win, 1.5);
	al_set_audio_stream_playmode(data->win, ALLEGRO_PLAYMODE_ONCE);
	progress(game);

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

	data->bg_anim2 = CreateNotPreservedBitmap(al_get_bitmap_width(data->bg2), al_get_bitmap_height(data->bg2));

	return data;
}

void Gamestate_Unload(struct Game* game, struct GamestateResources* data) {
	al_destroy_bitmap(data->bg);
	al_destroy_bitmap(data->bg2);
	al_destroy_bitmap(data->bg_anim2);
	al_destroy_bitmap(data->mask1);
	al_destroy_bitmap(data->mask2);
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
	data->x = 0.3;
	data->y = 0.6;
	data->x2 = 0.7;
	data->y2 = 0.6;
	data->lisek->flipX = true;
	SetCharacterPosition(game, data->myszka, 400, 70, 0.0);
	SetCharacterPosition(game, data->drzwi, 1090, 740, 0.0);
	data->myszka->scaleX = 0.5;
	data->myszka->scaleY = 0.5;
	data->drzwi->scaleX = 0.5;
	data->drzwi->scaleY = 0.5;
	data->lisek->scaleX = 0.64;
	data->lisek->scaleY = 0.64;
	data->smok->scaleX = 0.6;
	data->smok->scaleY = 0.6;
	data->found = false;
	data->won = false;
	data->shown1 = false;
	data->shown2 = false;
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {}

void Gamestate_Reload(struct Game* game, struct GamestateResources* data) {
	data->bg_anim2 = CreateNotPreservedBitmap(al_get_bitmap_width(data->bg2), al_get_bitmap_height(data->bg2));
}
