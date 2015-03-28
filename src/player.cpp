/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012, 2013 Guus Sliepen
Copyright (C) 2012, 2015 Julian Marchant

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

#include "Starfighter.h"

object player;
static bool charger_fired = false;

/*
Initialises the player for a new game.
*/
void initPlayer()
{
	player.active = true;
	player.x = screen->w / 2;
	player.y = screen->h / 2;
	player.speed = 2;
	player.systemPower = player.maxShield;
	player.face = 0;

	player.image[0] = shipShape[0];
	player.image[1] = shipShape[1];

	player.engineX = player.image[0]->w;
	player.engineY = (player.image[0]->h / 2);

	player.owner = &player;
	player.flags = FL_FRIEND;

	player.weaponType[0] = W_PLAYER_WEAPON;

	if(weapon[W_PLAYER_WEAPON].ammo[0] < currentGame.minPlasmaOutput)
		weapon[W_PLAYER_WEAPON].ammo[0] = currentGame.minPlasmaOutput;
	if(weapon[W_PLAYER_WEAPON].damage < currentGame.minPlasmaDamage)
		weapon[W_PLAYER_WEAPON].damage = currentGame.minPlasmaDamage;
	if(weapon[W_PLAYER_WEAPON].reload[0] > rate2reload[currentGame.minPlasmaRate])
		weapon[W_PLAYER_WEAPON].reload[0] = rate2reload[currentGame.minPlasmaRate];

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

void exitPlayer()
{
	charger_fired = false;

	if ((player.weaponType[1] == W_CHARGER) || (player.weaponType[1] == W_LASER))
		player.ammo[1] = 0;
}

void doPlayer()
{
	// This causes the motion to slow
	engine.ssx *= 0.99;
	engine.ssy *= 0.99;

	engine.smx = 0;
	engine.smy = 0;

	int shapeToUse;
	float cd;
	float cc;
	bool xmoved = false;
	bool ymoved = false;

	if (player.shield > -100)
	{
		if (player.shield > 0)
		{
			if ((engine.keyState[KEY_FIRE]))
				ship_fireBullet(&player, 0);

			if ((engine.keyState[KEY_ALTFIRE]) && (player.weaponType[1] != W_NONE))
			{
				if ((player.weaponType[1] != W_CHARGER) &&
					(player.weaponType[1] != W_LASER) && (player.ammo[1] > 0))
				{
					ship_fireBullet(&player, 1);
				}

				if (player.weaponType[1] == W_LASER)
				{
					if (player.ammo[1] < 100)
					{
						ship_fireBullet(&player, 1);
						player.ammo[1] += 2;
						if (player.ammo[1] >= 100)
						{
							player.ammo[1] = 200;
							setInfoLine("Laser Overheat!!", FONT_WHITE);
						}
					}
				}
			}

			if (player.weaponType[1] == W_CHARGER)
			{
				if (engine.keyState[KEY_ALTFIRE] &&
					((currentGame.difficulty == DIFFICULTY_ORIGINAL) ||
						!(engine.keyState[KEY_FIRE])))
				{
					if (!charger_fired)
					{
						if (currentGame.difficulty == DIFFICULTY_ORIGINAL)
						{
							LIMIT_ADD(player.ammo[1], 1, 0, 200);
						}
						else
						{
							LIMIT_ADD(player.ammo[1], 1, 0, 150);
							if (player.ammo[1] >= 150)
							{
								ship_fireBullet(&player, 1);
								player.ammo[1] = 0;
								charger_fired = true;
							}
						}
					}
				}
				else
				{
					if (player.ammo[1] > 0)
						ship_fireBullet(&player, 1);
					player.ammo[1] = 0;
					charger_fired = false;
				}
			}

			if ((engine.keyState[KEY_SWITCH]))
			{
				if ((weapon[W_PLAYER_WEAPON].ammo[0] >= 3) &&
					(weapon[W_PLAYER_WEAPON].ammo[0] <= currentGame.maxPlasmaOutput))
				{
					weapon[W_PLAYER_WEAPON].flags ^= WF_SPREAD;

					if (weapon[W_PLAYER_WEAPON].flags & WF_SPREAD)
					{
						setInfoLine("Weapon set to Spread", FONT_WHITE);
					}
					else
					{
						setInfoLine("Weapon set to Concentrate", FONT_WHITE);
					}
				}

				engine.keyState[KEY_SWITCH] = 0;
			}

			LIMIT_ADD(player.reload[0], -1, 0, 999);
			LIMIT_ADD(player.reload[1], -1, 0, 999);

			if (engine.keyState[KEY_UP])
			{
				player.y -= player.speed;
				engine.ssy += 0.1;
				ymoved = true;
			}

			if (engine.keyState[KEY_DOWN])
			{
				player.y += player.speed;
				engine.ssy -= 0.1;
				ymoved = true;
			}

			if (engine.keyState[KEY_LEFT])
			{
				player.x -= player.speed;
				engine.ssx += 0.1;
				player.face = 1;
				xmoved = true;
			}

			if (engine.keyState[KEY_RIGHT])
			{
				player.x += player.speed;
				engine.ssx -= 0.1;
				player.face = 0;
				xmoved = true;
			}

			if (engine.keyState[KEY_ESCAPE])
			{
				if ((engine.done == 0) && (engine.gameSection == SECTION_GAME) &&
					(currentMission.remainingObjectives1 == 0))
				{
					audio_playSound(SFX_FLY, screen->w / 2);
					engine.done = 2;
					engine.missionCompleteTimer = (SDL_GetTicks() - 1);
				}
			}

			if (engine.keyState[KEY_PAUSE])
			{
				engine.paused = true;
				engine.keyState[KEY_PAUSE] = 0;
			}

			if (((currentGame.area == 18) && (aliens[ALIEN_BOSS].shield > 0)) ||
				(currentGame.area == 24))
			{
				player.face = 0;
				xmoved = true;
				ymoved = true;
			}

			if (engine.done == 0)
			{
				if (xmoved)
				{
					if (player.x < xViewBorder)
					{
						engine.smx += xViewBorder - player.x;
						player.x = xViewBorder;
					}
					else if (player.x > screen->w - xViewBorder)
					{
						engine.smx += (screen->w - xViewBorder) - player.x;
						player.x = screen->w - xViewBorder;
					}
				}
				else if (currentGame.difficulty != DIFFICULTY_ORIGINAL)
				{
					cd = player.x - screen->w / 2;
					if (cd < 0)
					{
						cc = MAX(cd / 10, MAX(0, engine.ssx) - cameraMaxSpeed);
						player.x -= cc;
						engine.smx -= cc;
					}
					else if (cd > 0)
					{
						cc = MIN(cd / 10, cameraMaxSpeed + MIN(0, engine.ssx));
						player.x -= cc;
						engine.smx -= cc;
					}
				}

				if (ymoved)
				{
					if (player.y < yViewBorder)
					{
						engine.smy += yViewBorder - player.y;
						player.y = yViewBorder;
					}
					else if (player.y > screen->h - yViewBorder)
					{
						engine.smy += (screen->h - yViewBorder) - player.y;
						player.y = screen->h - yViewBorder;
					}
				}
				else if (currentGame.difficulty != DIFFICULTY_ORIGINAL)
				{
					cd = player.y - screen->h / 2;
					if (cd < 0)
					{
						cc = MAX(cd / 10, MAX(0, engine.ssy) - cameraMaxSpeed);
						player.y -= cc;
						engine.smy -= cc;
					}
					else if (cd > 0)
					{
						cc = MIN(cd / 10, cameraMaxSpeed + MIN(0, engine.ssy));
						player.y -= cc;
						engine.smy -= cc;
					}
				}
			}

			if ((player.maxShield <= 1) || (player.shield > engine.lowShield))
				addEngine(&player);

			shapeToUse = player.face;

			if (player.hit)
				shapeToUse += SHIP_HIT_INDEX;

			LIMIT_ADD(player.hit, -1, 0, 100);

			blit(shipShape[shapeToUse], (int)player.x, (int)player.y);
			if ((player.maxShield > 1) && (player.shield <= engine.lowShield) &&
					(rand() % 5 < 1))
				addExplosion(player.x + rrand(-10, 10), player.y + rrand(-10, 20), E_SMOKE);
		}
		else
		{
			player.active = false;
			player.shield--;
			if (player.shield == -1)
			{
				if ((currentGame.hasWingMate1) || (aliens[ALIEN_KLINE].active))
					getPlayerDeathMessage();

				// Make it look like the ships are all still moving...
				if (currentGame.area == 18)
				{
					for (int i = 0 ; i < ALIEN_MAX ; i++)
						aliens[i].flags |= FL_LEAVESECTOR;
				}

				audio_playSound(SFX_DEATH, player.x);
				audio_playSound(SFX_EXPLOSION, player.x);
			}

			engine.keyState[KEY_UP] = engine.keyState[KEY_DOWN] = engine.keyState[KEY_LEFT] = engine.keyState[KEY_RIGHT] = 0;
			if ((rand() % 3) == 0)
				addExplosion(player.x + rrand(-10, 10), player.y + rrand(-10, 10), E_BIG_EXPLOSION);
			if (player.shield == -99)
				addDebris((int)player.x, (int)player.y, player.maxShield);
		}
	}

	LIMIT(engine.ssx, -cameraMaxSpeed, cameraMaxSpeed);
	LIMIT(engine.ssy, -cameraMaxSpeed, cameraMaxSpeed);

	// Specific for the mission were you have to chase the Executive Transport
	if ((currentGame.area == 18) && (aliens[ALIEN_BOSS].shield > 0) &&
		(player.shield > 0))
	{
		engine.ssx = -6;
		engine.ssy = 0;
	}
	
	if (currentGame.area == 24)
	{
		engine.ssx = -6;
		engine.ssy = 0;
	}

	player.dx = engine.ssx;
	player.dy = engine.ssy;
}

void flushInput()
{
	for (int i = 0; i < KEY_LAST; i++)
		engine.keyState[i] = 0;

	while (SDL_PollEvent(&engine.event)){}
}

static enum keys mapkey(uint32_t code) {
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
		case SDLK_c:
		case SDLK_a:
		case SDLK_d:
		case SDLK_f:
		case SDLK_SLASH:
		case SDLK_COMMA:
		case SDLK_1:
		case SDLK_3:
		case SDLK_KP_0:
			return KEY_FIRE;
		case SDLK_SPACE:
		case SDLK_x:
		case SDLK_s:
		case SDLK_PERIOD:
		case SDLK_2:
		case SDLK_KP_1:
			return KEY_ALTFIRE;
		case SDLK_LSHIFT:
		case SDLK_RSHIFT:
		case SDLK_LALT:
		case SDLK_RALT:
		case SDLK_KP_7:
		case SDLK_KP_9:
			return KEY_SWITCH;
		case SDLK_p:
			return KEY_PAUSE;
		case SDLK_ESCAPE:
		case SDLK_q:
		case SDLK_BACKSPACE:
			return KEY_ESCAPE;
		case SDLK_F11:
			return KEY_FULLSCREEN;
		default:
			return KEY_DUMMY;
	}
}

void getPlayerInput()
{
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
				engine.keyState[mapkey(engine.event.key.keysym.sym)] = 1;

				if (engine.gameSection != SECTION_GAME)
					engine.paused = false;

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
				static bool prevjoyup, prevjoydown, prevjoyleft, prevjoyright;
				if (engine.event.jaxis.axis & 1) {
					bool joyup = engine.event.jaxis.value < -16384;
					bool joydown = engine.event.jaxis.value >= 16384;
					if(joyup != prevjoyup)
						engine.keyState[KEY_UP] = prevjoyup = joyup;
					if(joydown != prevjoydown)
						engine.keyState[KEY_DOWN] = prevjoydown = joydown;
				} else {
					bool joyleft = engine.event.jaxis.value < -16384;
					bool joyright = engine.event.jaxis.value >= 16384;
					if(joyleft != prevjoyleft)
						engine.keyState[KEY_LEFT] = prevjoyleft = joyleft;
					if(joyright != prevjoyright)
						engine.keyState[KEY_RIGHT] = prevjoyright = joyright;
				}
				break;

			case SDL_WINDOWEVENT:
				if(engine.event.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
					engine.paused = 1;
				break;
		}

		if (engine.keyState[KEY_FULLSCREEN])
		{
			engine.fullScreen = !engine.fullScreen;
			SDL_SetWindowFullscreen(window, engine.fullScreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
			engine.keyState[KEY_FULLSCREEN] = 0;
		}
	}

	if (engine.gameSection == SECTION_INTERMISSION)
	{
		// Get the current mouse position
		static int px = -1, py = -1;
		int x, y;
		SDL_GetMouseState(&x, &y);
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

void leaveSector()
{
	engine.keyState[KEY_UP] = engine.keyState[KEY_DOWN] = engine.keyState[KEY_LEFT] = engine.keyState[KEY_RIGHT] = 0;
	engine.keyState[KEY_FIRE] = engine.keyState[KEY_ALTFIRE] = 0;

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
			audio_playSound(SFX_FLY, screen->w / 2);
		}
	}

	if (engine.done == 2)
	{
		player.face = 0;
		player.x += 12;
		engine.ssx -= 0.2;
		if (player.x > (2 * screen->w))
			engine.done = 1;
	}
}

