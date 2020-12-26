/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012 Guus Sliepen
Copyright (C) 2015-2020 The Diligent Circle <diligentcircle@riseup.net>

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

#include "alien.h"
#include "engine.h"
#include "game.h"
#include "gfx.h"
#include "player.h"

void bullet_add(Object *theWeapon, Object *attacker, int y, int dy)
{
	Object *bullet;
	int imageIndex;
	int tempX, tempY, steps;
	int min_damage;

	bullet = malloc(sizeof(*bullet));
	if (bullet == NULL)
	{
		engine_warn("Failed to allocate memory for bullet");
		return;
	}

	if (attacker == &player)
		game.shots++;

	bullet->next = NULL;
	bullet->active = 1;
	bullet->x = attacker->x + (attacker->image[0]->w / 2) -
		(theWeapon->image[0]->w * attacker->face);
	bullet->y = attacker->y + y;
	bullet->flags = theWeapon->flags;
	bullet->shield = 300; // bullets live for (approximately) 5 seconds

	// Timed explosions live between 1 and 3 seconds
	if (bullet->flags & WF_TIMEDEXPLOSION)
		bullet->shield = RANDRANGE(60, 180);

	if (attacker->face == 0)
	{
		bullet->dx = theWeapon->speed;
		if ((game.area == MISN_ELLESH)
				|| (game.area == MISN_MARS))
			bullet->dx += fabsf(engine.ssx + engine.smx);
	}
	else
	{
		bullet->dx = -theWeapon->speed;
	}

	if (bullet->flags & WF_VARIABLE_SPEED)
	{
		bullet->dx = RANDRANGE(100, 200) / 10;
		if (attacker->face == 1)
			bullet->dx = -bullet->dx;
	}

	bullet->dy = dy;

	if (bullet->flags & WF_SCATTER)
	{
		bullet->dy = RANDRANGE(-200, 200) / 200;
	}

	if (attacker->flags & FL_WEAPCO)
		bullet->flags |= WF_WEAPCO;
	else
		bullet->flags |= WF_FRIEND;

	bullet->owner = attacker->owner;

	bullet->id = theWeapon->id;

	bullet->damage = theWeapon->damage;

	// Adjustments to damage for Classic difficulty (to match original)
	if (game.difficulty == DIFFICULTY_ORIGINAL)
	{
		if (bullet->id == WT_MICROROCKET)
			bullet->damage = 3;
		else if (bullet->id == WT_SPREAD)
			bullet->damage = 1;
	}

	if (bullet->id == WT_CHARGER)
	{
		if (game.difficulty == DIFFICULTY_ORIGINAL)
		{
			bullet->damage = attacker->ammo[1];
			min_damage = 50;
		}
		else
		{
			bullet->damage = attacker->ammo[1] / 2;
			min_damage = 15;
		}

		if (bullet->damage < min_damage)
		{
			bullet->damage = 1;
			bullet->id = WT_PLASMA;
		}
	}

	bullet->target = NULL;

	if (attacker->flags & FL_FRIEND)
		imageIndex = 0;
	else
		imageIndex = 1;

	// Use the enemy's images if applicable
	if (bullet->id != WT_ROCKET)
		bullet->image[0] = theWeapon->image[imageIndex];
	else
		bullet->image[0] = theWeapon->image[attacker->face];

	if (bullet->flags & WF_AIMED)
	{
		tempX = (int)fabsf(attacker->target->x - attacker->x);
		tempY = (int)fabsf(attacker->target->y - attacker->y);
		steps = MAX(tempX, tempY);

		if (steps < 12)
			steps = 12;

		if (!(bullet->flags & WF_TIMEDEXPLOSION))
			steps /= 8;
		else
			steps /= RANDRANGE(6, 11);

		tempX = (int)(attacker->target->x - attacker->x);
		tempY = (int)(attacker->target->y - attacker->y);

		bullet->dx = tempX / steps;
		bullet->dy = tempY / steps;
	}

	if (attacker->classDef == CD_ASTEROID)
	{
		bullet->dx = RANDRANGE(-20, 20);
		bullet->dy = RANDRANGE(-20, 20);
		bullet->image[0] = gfx_sprites[SP_SMALL_EXPLOSION];
	}

	engine.bulletTail->next = bullet;
	engine.bulletTail = bullet;
}

/*
Used for homing missiles. When a missile is active and it is told to home in
on an enemy, it will attempt to randomly grab one every frame if it does not
already have a target. If the target it is currently chasing is killed, it will
begin to look for a new one (done in doBullets()). The homing missile will make
one attempt per call (one call per frame) to find a suitable target. If the target
it picks is dead or outside the screen range, then it returns NULL. A suitable
target will be returned as the Object address.
*/
Object *bullet_getTarget(Object *bullet)
{
	int i;

	if (bullet->owner->flags & FL_WEAPCO)
	{
		if (CHANCE(0.1))
			return &player;
	}

	i = rand() % ALIEN_MAX;

	if ((aliens[i].shield < 1) || (!aliens[i].active))
		return NULL;

	if (aliens[i].flags & FL_IMMORTAL)
		return NULL;

	if ((bullet->owner->flags & FL_WEAPCO) && (aliens[i].flags & FL_WEAPCO))
		return NULL;

	if ((bullet->owner->flags & FL_FRIEND) && (aliens[i].flags & FL_FRIEND))
		return NULL;

	if (abs((int)bullet->x - (int)aliens[i].target->x) > 800)
		return NULL;

	if (abs((int)bullet->y - (int)aliens[i].target->y) > 200)
		return NULL;

	return &aliens[i];
}

int bullet_collision(Object *bullet, Object *ship)
{
	float x0 = bullet->x;
	float y0 = bullet->y;
	float w0 = bullet->image[0]->w;
	float h0 = bullet->image[0]->h;

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
