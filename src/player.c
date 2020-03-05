/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012, 2013 Guus Sliepen
Copyright (C) 2012, 2015-2019 Layla Marchant <diligentcircle@riseup.net>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 3
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "SDL.h"

#include "defs.h"
#include "structs.h"

#include "alien.h"
#include "audio.h"
#include "engine.h"
#include "colors.h"
#include "game.h"
#include "gfx.h"
#include "info.h"
#include "player.h"
#include "renderer.h"
#include "screen.h"
#include "weapons.h"
#include "window.h"

Object player;
int player_chargerFired = 0;
int player_damageDelay = 0;
int player_resetDamageDelay = 0;

/*
Initialises the player for a new game.
*/
void player_init()
{
	player.active = 1;
	player.x = screen->w / 2;
	player.y = screen->h / 2;
	player.speed = 2;
	player.systemPower = player.maxShield;
	player.face = 0;

	player.image[0] = gfx_shipSprites[SS_FIREFLY];
	player.image[1] = gfx_shipSprites[SS_FIREFLY_L];

	player.engineX = player.image[0]->w;
	player.engineY = (player.image[0]->h / 2);

	player.owner = &player;
	player.flags = FL_FRIEND;

	player.weaponType[0] = W_PLAYER_WEAPON;

	if (weapons[W_PLAYER_WEAPON].ammo[0] < game.minPlasmaOutput)
		weapons[W_PLAYER_WEAPON].ammo[0] = game.minPlasmaOutput;
	if (weapons[W_PLAYER_WEAPON].damage < game.minPlasmaDamage)
		weapons[W_PLAYER_WEAPON].damage = game.minPlasmaDamage;
	if (weapons[W_PLAYER_WEAPON].reload[0] > rate2reload[game.minPlasmaRate])
		weapons[W_PLAYER_WEAPON].reload[0] = rate2reload[game.minPlasmaRate];

	player.hit = 0;

	engine.lowShield = (player.maxShield >= 3) ? (player.maxShield / 3) : 1;
	engine.averageShield = engine.lowShield + engine.lowShield;

	if ((player.weaponType[1] == W_CHARGER) || (player.weaponType[1] == W_LASER))
		player.ammo[1] = 0;
}

void player_setTarget(int index)
{
	engine.targetIndex = index;
	engine.targetShield = 85;
	engine.targetShield /= aliens[index].shield;
}

/*
Attempt to damage the player by ``amount``.  If ``delay`` is specified,
delay any damage dealt by that number of frames (i.e. require that
number of frames of continuous damage before it registers).
Return 1 if damage is inflicted, 0 otherwise.
*/
void player_damage(int amount, int delay)
{
	int oldshield = player.shield;

	player_resetDamageDelay = 0;

	if ((!engine.cheatShield) && (engine.missionCompleteTimer == 0) &&
			((!player.hit) ||
				(game.difficulty == DIFFICULTY_ORIGINAL) ||
				((player.shield != engine.lowShield) &&
					(player.shield != 1))))
	{
		if ((game.difficulty == DIFFICULTY_ORIGINAL) ||
				(player_damageDelay >= delay))
		{
			player.shield -= amount;

			LIMIT(player.shield, 0, player.maxShield);
			player.hit = 5; // Damage flash timer
			audio_playSound(SFX_HIT, player.x, player.y);

			// Damage tiers (not in Classic mode)
			if ((oldshield > engine.lowShield) &&
					(player.shield <= engine.lowShield))
			{
				info_setLine("!!! WARNING: SHIELD LOW !!!", FONT_RED);
				if (game.difficulty != DIFFICULTY_ORIGINAL)
				{
					player.shield = engine.lowShield;
					player_damageDelay = 0;
				}
			}
			else if ((oldshield > 1) && (player.shield <= 1))
			{
				info_setLine("!!! WARNING: SHIELD CRITICAL !!!", FONT_RED);
				if (game.difficulty != DIFFICULTY_ORIGINAL)
				{
					player.shield = 1;
					player_damageDelay = 0;
				}
			}
		}
		else
		{
			player_damageDelay += amount;
		}
	}
}

void player_checkShockDamage(float x, float y)
{
	float distX = fabsf(x - player.x);
	float distY = fabsf(y - player.y);

	// Don't let the player be hurt by an explosion after they have completed
	// all the mission Objectives. That would be *really* annoying!
	if ((engine.cheatShield) || (engine.missionCompleteTimer != 0))
		return;

	if ((distX <= 50) && (distY <= 50))
	{
		if (distX >= 1)
			distX /= 5;

		if (distY >= 1)
			distY /= 5;

		player_damage((int)((10 - distX) + (10 - distY)), 0);
		player.hit = 10;
	}
}

void player_exit()
{
	player_chargerFired = 0;

	if ((player.weaponType[1] == W_CHARGER) || (player.weaponType[1] == W_LASER))
		player.ammo[1] = 0;
}

void player_flushInput()
{
	for (int i = 0; i < KEY_LAST; i++)
		engine.keyState[i] = 0;

	while (SDL_PollEvent(&engine.event)){}
}

static enum keys mapkey(int code) {
	switch (code) {
		case SDLK_UP:
		case SDLK_KP_8:
			return KEY_UP;
		case SDLK_DOWN:
		case SDLK_KP_2:
		case SDLK_KP_5:
			return KEY_DOWN;
		case SDLK_LEFT:
		case SDLK_KP_4:
			return KEY_LEFT;
		case SDLK_RIGHT:
		case SDLK_KP_6:
			return KEY_RIGHT;
		case SDLK_LCTRL:
		case SDLK_RCTRL:
		case SDLK_RETURN:
		case SDLK_z:
		case SDLK_y:
		case SDLK_c:
		case SDLK_a:
		case SDLK_d:
		case SDLK_f:
		case SDLK_SLASH:
		case SDLK_COMMA:
		case SDLK_1:
		case SDLK_3:
		case SDLK_KP_0:
		case SDLK_HOME:
		case SDLK_END:
			return KEY_FIRE;
		case SDLK_SPACE:
		case SDLK_x:
		case SDLK_s:
		case SDLK_PERIOD:
		case SDLK_2:
		case SDLK_KP_1:
		case SDLK_PAGEUP:
		case SDLK_PAGEDOWN:
			return KEY_ALTFIRE;
		case SDLK_LSHIFT:
		case SDLK_RSHIFT:
		case SDLK_LALT:
		case SDLK_RALT:
		case SDLK_KP_7:
		case SDLK_KP_9:
			return KEY_SWITCH;
		case SDLK_p:
		case SDLK_PAUSE:
			return KEY_PAUSE;
		case SDLK_ESCAPE:
		case SDLK_q:
		case SDLK_BACKSPACE:
		case SDLK_DELETE:
			return KEY_ESCAPE;
		case SDLK_F11:
			return KEY_FULLSCREEN;
		default:
			return KEY_DUMMY;
	}
}

void player_getInput()
{
	static int prevjoyup, prevjoydown, prevjoyleft, prevjoyright;
	int joyup, joydown, joyleft, joyright;
	static int px = -1, py = -1;
	int x, y, w, h;

	while (SDL_PollEvent(&engine.event))
	{
		switch (engine.event.type)
		{
			case SDL_QUIT:
				exit(0);
				break;

			case SDL_MOUSEBUTTONDOWN:
				if (engine.gameSection == SECTION_INTERMISSION)
				{
					if (engine.event.button.button == SDL_BUTTON_LEFT) engine.keyState[KEY_FIRE] = 1;
					if (engine.event.button.button == SDL_BUTTON_RIGHT) engine.keyState[KEY_ALTFIRE] = 1;
				}
				break;

			case SDL_KEYDOWN:
				if (!engine.event.key.repeat)
				{
					engine.keyState[mapkey(engine.event.key.keysym.sym)] = 1;

					if (engine.gameSection != SECTION_GAME)
						engine.paused = 0;
				}
				break;

			case SDL_KEYUP:
				if (engine.event.key.keysym.sym != SDLK_p)
					engine.keyState[mapkey(engine.event.key.keysym.sym)] = 0;
				break;

			case SDL_JOYBUTTONDOWN:
			case SDL_JOYBUTTONUP:
				switch (engine.event.jbutton.button)
				{
					case 0:
					case 3:
						engine.keyState[KEY_ALTFIRE] = engine.event.jbutton.state;
						break;
					case 1:
					case 2:
						engine.keyState[KEY_FIRE] = engine.event.jbutton.state;
						break;
					case 4:
					case 6:
						engine.keyState[KEY_ESCAPE] = engine.event.jbutton.state;
						break;
					case 5:
					case 7:
					case 8:
						engine.keyState[KEY_SWITCH] = engine.event.jbutton.state;
						break;
					case 9:
						if (engine.event.jbutton.state)
							engine.keyState[KEY_PAUSE] = 1;
						break;
				}
				break;

			case SDL_JOYHATMOTION:
				engine.keyState[KEY_UP]    = engine.event.jhat.value & SDL_HAT_UP;
				engine.keyState[KEY_DOWN]  = engine.event.jhat.value & SDL_HAT_DOWN;
				engine.keyState[KEY_LEFT]  = engine.event.jhat.value & SDL_HAT_LEFT;
				engine.keyState[KEY_RIGHT] = engine.event.jhat.value & SDL_HAT_RIGHT;
				break;

			case SDL_JOYAXISMOTION:
				if (engine.event.jaxis.axis & 1) {
					joyup = engine.event.jaxis.value < -16384;
					joydown = engine.event.jaxis.value >= 16384;
					if (joyup != prevjoyup)
						engine.keyState[KEY_UP] = prevjoyup = joyup;
					if (joydown != prevjoydown)
						engine.keyState[KEY_DOWN] = prevjoydown = joydown;
				} else {
					joyleft = engine.event.jaxis.value < -16384;
					joyright = engine.event.jaxis.value >= 16384;
					if (joyleft != prevjoyleft)
						engine.keyState[KEY_LEFT] = prevjoyleft = joyleft;
					if (joyright != prevjoyright)
						engine.keyState[KEY_RIGHT] = prevjoyright = joyright;
				}
				break;

			case SDL_WINDOWEVENT:
				if (engine.event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				{
					screen_adjustDimensions(engine.event.window.data1, engine.event.window.data2);
					renderer_reset();
					gfx_scaleBackground();
					screen_clear(black);
					renderer_update();
					screen_clear(black);
					screen_addBuffer(0, 0, screen->w, screen->h);
				}

				if (engine.autoPause &&
						(engine.event.window.event == SDL_WINDOWEVENT_FOCUS_LOST))
					engine.paused = 1;
				break;
		}

		if (engine.keyState[KEY_FULLSCREEN])
		{
			engine_setFullscreen(!engine.fullScreen);
			engine.keyState[KEY_FULLSCREEN] = 0;
		}
	}

	if (engine.gameSection == SECTION_INTERMISSION)
	{
		// Get the current mouse position
		SDL_GetMouseState(&x, &y);
		SDL_GetWindowSize(window, &w, &h);
		x = screen->w * x / w;
		y = screen->h * y / h;
		if (px == x && py == y) {
			if(engine.keyState[KEY_UP] && engine.cursor_y > 0)
				engine.cursor_y -= 4;
			if(engine.keyState[KEY_DOWN] && engine.cursor_y < screen->h - 4)
				engine.cursor_y += 4;
			if(engine.keyState[KEY_LEFT] && engine.cursor_x > 0)
				engine.cursor_x -= 4;
			if(engine.keyState[KEY_RIGHT] && engine.cursor_x < screen->w - 4)
				engine.cursor_x += 4;
		} else {
			engine.cursor_x = px = x;
			engine.cursor_y = py = y;
		}
	}

}

void player_leaveSector()
{
	engine.keyState[KEY_UP] = 0;
	engine.keyState[KEY_DOWN] = 0;
	engine.keyState[KEY_LEFT] = 0;
	engine.keyState[KEY_RIGHT] = 0;
	engine.keyState[KEY_FIRE] = 0;
	engine.keyState[KEY_ALTFIRE] = 0;

	if (engine.done == 0)
		engine.done = 3;

	if (engine.done == 3)
	{
		player.face = 0;
		if (player.x > -100)
		{
			player.x += engine.ssx;
			engine.ssx -= 1;
			if (player.y > screen->h / 2)
				player.y--;
			if (player.y < screen->h / 2)
				player.y++;
		}

		if (player.x <= -100)
		{
			engine.done = 2;
			audio_playSound(SFX_FLY, screen->w / 2, screen->h / 2);
		}
	}

	if (engine.done == 2)
	{
		player.face = 0;
		player.x += WARP_SPEED;
		engine.ssx -= 0.2;
		if (player.x > (2 * screen->w))
			engine.done = 1;
	}
}

