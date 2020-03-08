/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012 Guus Sliepen
Copyright (C) 2015-2019 Layla Marchant <diligentcircle@riseup.net>

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

#include "defs.h"
#include "structs.h"

#include "audio.h"
#include "collectable.h"
#include "engine.h"
#include "explosion.h"
#include "game.h"
#include "gfx.h"
#include "player.h"
#include "weapons.h"

/*
Create a new collectable item based on supplied arguments.
*/
void collectable_add(float x, float y, int type, int value, int life)
{
	int r;
	Collectable *collectable;
	int plasma_useless, shield_useless, rockets_useless;
	
	plasma_useless = (((weapons[W_PLAYER_WEAPON].reload[0] >= rate2reload[game.minPlasmaRate]) &&
			(weapons[W_PLAYER_WEAPON].ammo[0] <= game.minPlasmaOutput) &&
			(weapons[W_PLAYER_WEAPON].damage <= game.minPlasmaDamage)) ||
		(player.ammo[0] >= game.maxPlasmaAmmo));
	
	shield_useless = ((game.difficulty == DIFFICULTY_NIGHTMARE) ||
		(player.shield >= player.maxShield));
	
	rockets_useless = ((player.weaponType[1] == W_CHARGER) ||
		(player.weaponType[1] == W_LASER) || (game.maxRocketAmmo <= 0) ||
		(player.ammo[1] >= game.maxRocketAmmo));

	if (type == P_ANYTHING)
	{
		type = P_CASH;

		r = rand() % 9;

		switch (r)
		{
			case 0:
				if ((game.difficulty == DIFFICULTY_ORIGINAL) ||
						(game.difficulty == DIFFICULTY_NIGHTMARE))
				{
					type = P_PLASMA_AMMO;
				}
				else
				{
					if ((!shield_useless) &&
							(CHANCE(2 * (player.maxShield - player.shield) / player.maxShield)))
					{
						type = P_SHIELD;
							
					}
					else if ((!rockets_useless) && (game.maxRocketAmmo > 0) &&
							(CHANCE((game.maxRocketAmmo - player.ammo[1]) / game.maxRocketAmmo)))
					{
						type = P_ROCKET;
					}
					else
					{
						type = P_PLASMA_AMMO;
					}
				}
				break;

			case 1:
				type = P_SHIELD;
				break;

			case 2:
				if ((game.difficulty == DIFFICULTY_ORIGINAL) ||
						(game.difficulty == DIFFICULTY_NIGHTMARE))
				{
					type = P_ROCKET;
				}
				else
				{
					if ((!shield_useless) &&
							(CHANCE(2 * (player.maxShield - player.shield) / player.maxShield)))
					{
						type = P_SHIELD;
					}
					else if ((!plasma_useless) && (game.maxPlasmaAmmo > 0) &&
							(CHANCE((game.maxPlasmaAmmo - player.ammo[0]) / game.maxPlasmaAmmo)))
					{
						type = P_PLASMA_AMMO;
					}
					else
					{
						type = P_ROCKET;
					}
				}
				break;
		}
	}
	else if (type == P_WEAPONS)
	{
		type = P_PLASMA_RATE;

		if ((game.difficulty == DIFFICULTY_NIGHTMARE) ||
			((game.difficulty != DIFFICULTY_EASY) &&
				(game.difficulty != DIFFICULTY_ORIGINAL) &&
				((game.area == MISN_MOEBO) ||
					(game.area == MISN_ELAMALE) ||
					(game.area == MISN_ELLESH) ||
					(game.area == MISN_EARTH))))
		{
			// Deny the Super Charge in Nightmare difficulty, and on bosses.
			r = rand() % 59;
		}
		else
			r = rand() % 61;

		if (r <= 19)
			type = P_PLASMA_DAMAGE;
		else if (r <= 39)
			type = P_PLASMA_SHOT;
		else if (r <= 59)
			type = P_PLASMA_RATE;
		else
			type = P_SUPER;
	}

	if (type == P_SUPER)
		value = MAX(value, 1);

	if (value == 0)
		return; // don't bother!

	// No point in giving the player plasma ammo if the weapons aren't
	// upgraded! Give them money instead. (Except in Classic difficulty.)
	if ((type == P_PLASMA_AMMO) && (game.difficulty != DIFFICULTY_ORIGINAL))
	{
		if (plasma_useless)
		{
			type = P_CASH;
		}
	}

	// If the player has a charge cannon or a laser cannon, don't give them
	// rockets. Causes problems otherwise :)
	if (type == P_ROCKET)
	{
		if ((player.weaponType[1] == W_CHARGER) ||
				(player.weaponType[1] == W_LASER) ||
				(value < 10))
		{
			type = P_CASH;
		}
		else
		{
			value /= 10;
		}
	}

	// Shield bonus is useless if you can't heal; give cash instead.
	if ((type == P_SHIELD) && (game.difficulty != DIFFICULTY_ORIGINAL))
	{
		if  ((game.difficulty == DIFFICULTY_NIGHTMARE) ||
				(player.shield >= player.maxShield))
		{
			type = P_CASH;
		}
	}

	if (game.difficulty == DIFFICULTY_ORIGINAL)
	{
		// Cash is just rare in the original game. You can still grind,
		// just not as much.
		if ((game.area == MISN_INTERCEPTION) && (type == P_CASH))
		{
			if (rand() % 10 > 0)
				return;
		}
	}
	else
	{
		// No cash or ammo on interceptions. Completely stops grinding.
		if ((game.area == MISN_INTERCEPTION) &&
			((type == P_CASH) || (type == P_PLASMA_AMMO) || (type == P_ROCKET)))
		{
			return;
		}
	}

	collectable = malloc(sizeof(*collectable));
	if (collectable == NULL)
	{
		engine_warn("Failed to allocate memory for collectable");
		return;
	}

	collectable->next = NULL;
	collectable->active = 1;
	collectable->x = x;
	collectable->y = y;

	collectable->dx = RANDRANGE(-100, 100);
	collectable->dx /= (game.difficulty == DIFFICULTY_ORIGINAL ? 100. : 200.);

	collectable->dy = RANDRANGE(-100, 100);
	collectable->dy /= (game.difficulty == DIFFICULTY_ORIGINAL ? 100. : 200.);

	collectable->type = type;
	collectable->value = value;
	collectable->life = life;

	switch(type)
	{
		case P_CASH:
			collectable->image = gfx_sprites[SP_PICKUP_MONEY];
			break;

		case P_ROCKET:
			collectable->image = gfx_sprites[SP_PICKUP_ROCKETS];
			break;

		case P_PLASMA_AMMO:
			collectable->image = gfx_sprites[SP_PICKUP_PLASMA];
			break;

		case P_SHIELD:
			collectable->image = gfx_sprites[SP_PICKUP_SHIELD];
			break;

		case P_PLASMA_SHOT:
			collectable->image = gfx_sprites[SP_PICKUP_PLASMA_OUTPUT];
			break;

		case P_PLASMA_RATE:
			collectable->image = gfx_sprites[SP_PICKUP_PLASMA_RATE];
			break;

		case P_PLASMA_DAMAGE:
			collectable->image = gfx_sprites[SP_PICKUP_PLASMA_POWER];
			break;

		case P_CARGO:
			collectable->image = gfx_sprites[SP_CARGO];
			break;

		case P_SUPER:
			collectable->image = gfx_sprites[SP_SUPERCHARGE];
			break;

		case P_MINE:
			collectable->image = gfx_sprites[SP_MINE];
			break;

		case P_SLAVES:
		case P_ESCAPEPOD:
			collectable->image = gfx_sprites[SP_ESCAPE_POD];
			break;

		case P_ORE:
			collectable->image = gfx_sprites[RANDRANGE(SP_ORE, SP_ORE_L)];
			break;
	}

	engine.collectableTail->next = collectable;
	engine.collectableTail = collectable;
}

int collectable_collision(Collectable *collectable, Object *ship)
{
	float x0 = collectable->x;
	float y0 = collectable->y;
	float w0 = collectable->image->w;
	float h0 = collectable->image->h;

	float x2 = ship->x;
	float y2 = ship->y;
	float w1 = ship->image[0]->w;
	float h1 = ship->image[0]->h;

	float x1 = x0 + w0;
	float y1 = y0 + h0;

	float x3 = x2 + w1;
	float y3 = y2 + h1;

	return !(x1<x2 || x3<x0 || y1<y2 || y3<y0);
}

void collectable_explode(Collectable *collectable)
{
	audio_playSound(SFX_EXPLOSION, collectable->x, collectable->y);

	for (int i = 0 ; i < 10 ; i++)
		explosion_add(RANDRANGE(collectable->x - 25, collectable->x + 25),
			RANDRANGE(collectable->y - 25, collectable->y + 25), SP_BIG_EXPLOSION);

	player_checkShockDamage(collectable->x, collectable->y);
}

/*
Returns the number of "good" collectables (all collectables excluding
mines and unneeded refills) that exist.
*/
int collectable_numGood()
{
	Collectable *col = engine.collectableHead->next;
	int n = 0;

	while (col != NULL)
	{
		if ((col->type != P_MINE) && (col->type != P_ORE) &&
				((col->type != P_SHIELD) || (player.shield < player.maxShield)) &&
				((col->type != P_ROCKET) || (player.ammo[1] < game.maxRocketAmmo)) &&
				((col->type != P_PLASMA_AMMO) || (player.ammo[0] < game.maxPlasmaAmmo)) &&
				((col->type != P_PLASMA_SHOT) ||
					(player.ammo[0] < game.maxPlasmaAmmo) ||
					(weapons[W_PLAYER_WEAPON].ammo[0] < game.maxPlasmaOutput)) &&
				((col->type != P_PLASMA_DAMAGE) ||
					(player.ammo[0] < game.maxPlasmaAmmo) ||
					(weapons[W_PLAYER_WEAPON].damage < game.maxPlasmaDamage)) &&
				((col->type != P_PLASMA_RATE) ||
					(player.ammo[0] < game.maxPlasmaAmmo) ||
					(weapons[W_PLAYER_WEAPON].reload[0] > rate2reload[game.maxPlasmaRate])) &&
				((col->type != P_SUPER) ||
					(player.ammo[0] < game.maxPlasmaAmmo) ||
					(weapons[W_PLAYER_WEAPON].ammo[0] < game.maxPlasmaOutput) ||
					(weapons[W_PLAYER_WEAPON].damage < game.maxPlasmaDamage) ||
					(weapons[W_PLAYER_WEAPON].reload[0] > rate2reload[game.maxPlasmaRate])))
		{
			n++;
		}

		col = col->next;
	}

	return n;
}
