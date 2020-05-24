/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012 Guus Sliepen
Copyright (C) 2015-2020 Layla Marchant <diligentcircle@riseup.net>

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

#include <stdlib.h>

#include "SDL.h"

#include "defs.h"
#include "structs.h"

#include "alien.h"
#include "audio.h"
#include "bullet.h"
#include "engine.h"
#include "explosion.h"
#include "game.h"
#include "info.h"
#include "player.h"
#include "screen.h"
#include "weapons.h"

int ship_collision(Object *ship, Object *otherShip)
{
	float x0 = ship->x;
	float y0 = ship->y;
	float w0 = ship->image[0]->w;
	float h0 = ship->image[0]->h;

	float x2 = otherShip->x;
	float y2 = otherShip->y;
	float w1 = otherShip->image[0]->w;
	float h1 = otherShip->image[0]->h;

	float x1 = x0 + w0;
	float y1 = y0 + h0;

	float x3 = x2 + w1;
	float y3 = y2 + h1;

	return !(x1<x2 || x3<x0 || y1<y2 || y3<y0);
}

/*
Fill in later...
*/
void ship_fireBullet(Object *ship, int weaponIndex)
{
	int y = (ship->image[0]->h) / 5;
	Object *theWeapon = &weapons[ship->weaponType[weaponIndex]];

	if (ship->reload[weaponIndex] > 0)
		return;

	// Remove some ammo from the player
	if ((ship == &player) && (player.weaponType[weaponIndex] != W_LASER) &&
			(player.ammo[weaponIndex] > 0) && (!engine.cheatAmmo))
		player.ammo[weaponIndex]--;

	switch(theWeapon->id)
	{
		case WT_PLASMA:
		case WT_SPREAD:
		case WT_DIRECTIONAL:
			audio_playSound(SFX_PLASMA, ship->x, ship->y);
			break;
		case WT_ROCKET:
			audio_playSound(SFX_MISSILE, ship->x, ship->y);
			break;
		case WT_LASER:
			audio_playSound(SFX_LASER, ship->x, ship->y);
			break;
		case WT_CHARGER:
			audio_playSound(SFX_PLASMA3, ship->x, ship->y);
			break;
	}

	if (theWeapon->flags & WF_SPREAD && theWeapon->ammo[0] >= 3)
	{
		if (game.difficulty == DIFFICULTY_ORIGINAL)
		{
			bullet_add(theWeapon, ship, y * 1, -2);
			bullet_add(theWeapon, ship, y * 5, 2);
		}
		else
		{
			bullet_add(theWeapon, ship, y * 2, -1);
			bullet_add(theWeapon, ship, y * 4, 1);
		}

		if (theWeapon->ammo[0] != 4)
			bullet_add(theWeapon, ship, y * 3, 0);
		else
		{
			bullet_add(theWeapon, ship, y * 2, 0);
			bullet_add(theWeapon, ship, y * 4, 0);
		}

		if (theWeapon->ammo[0] == 5)
		{
			if (game.difficulty == DIFFICULTY_ORIGINAL)
			{
				bullet_add(theWeapon, ship, y * 2, -1);
				bullet_add(theWeapon, ship, y * 4, 1);
			}
			else
			{
				bullet_add(theWeapon, ship, y * 1, -2);
				bullet_add(theWeapon, ship, y * 5, 2);
			}
		}
	}
	else
	{
		if (theWeapon->ammo[0] & 1)
			bullet_add(theWeapon, ship, y * 3, 0);

		if (theWeapon->ammo[0] >= 2)
		{
			bullet_add(theWeapon, ship, y * 2, 0);
			bullet_add(theWeapon, ship, y * 4, 0);
		}

		if (theWeapon->ammo[0] >= 4)
		{
			bullet_add(theWeapon, ship, y * 1, 0);
			bullet_add(theWeapon, ship, y * 5, 0);
		}
	}

	// Reset the weapon reload time. Double it if it is not friendly or
	// a boss or Kline
	ship->reload[weaponIndex] = theWeapon->reload[0];
	if ((ship->flags & FL_WEAPCO) && (ship != &aliens[ALIEN_BOSS]) &&
			(ship != &aliens[ALIEN_KLINE]) && (theWeapon->id != W_LASER))
		ship->reload[weaponIndex] *= 2;

	if ((ship == &player) && (weaponIndex == 0))
	{
		if (player.ammo[weaponIndex] <= 0)
		{
			weapons[W_PLAYER_WEAPON].ammo[0] = game.minPlasmaOutput;
			weapons[W_PLAYER_WEAPON].damage = game.minPlasmaDamage;
			weapons[W_PLAYER_WEAPON].reload[0] = rate2reload[game.minPlasmaRate];
		}
	}
}

/*
Fill in later...
*/
void ship_fireRay(Object *ship)
{
	SDL_Rect ray;

	if (ship->face == 0)
	{
		ray.x = (int)(ship->x + ship->image[0]->w);
	}
	else
	{
		ray.x = (int)(ship->x - 800);
	}
	ray.y = (int)(ship->y + ship->engineY - 1);
	ray.h = 3;
	ray.w = 800;

	int red = SDL_MapRGB(screen->format, rand() % 256, 0x00, 0x00);
	SDL_FillRect(screen, &ray, red);
	screen_addBuffer(ray.x, ray.y, ray.w, ray.h);

	if (ship != &player)
	{
		if (player.shield > 0)
		{
			if (game_collision(player.x, player.y, player.image[0]->w,
						player.image[0]->h, ray.x, ray.y, ray.w, ray.h) &&
					(!engine.cheatShield) && (engine.missionCompleteTimer == 0))
			{
				player_damage(1, RAY_DAMAGE_DELAY);
				explosion_add(player.x, player.y, SP_SMALL_EXPLOSION);
			}
		}
	}

	for (int i = 0 ; i < ALIEN_MAX ; i++)
	{
		if (aliens[i].flags & FL_IMMORTAL)
			continue;

		if ((aliens[i].shield > 0) && (ship != &aliens[i]) &&
			(ship->classDef != aliens[i].classDef))
		{
			if (game_collision(aliens[i].x, aliens[i].y, aliens[i].image[0]->w,
					aliens[i].image[0]->h, ray.x, ray.y, ray.w, ray.h))
			{
				alien_hurt(&aliens[i], ship->owner, 1, 0);
			}
		}
	}

	ship->ammo[0]--;
	if (ship->ammo[0] < 1)
		ship->flags &= ~FL_FIRERAY;
}
