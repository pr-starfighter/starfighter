/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012 Guus Sliepen
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

object alien_defs[CD_MAX];
object aliens[MAX_ALIENS];

/*
This simply pulls back an alien from the array that is
"dead" (no shield) and returns the index number so we can have
a new one.
*/
static int alien_getFreeIndex()
{
	for (int i = 0 ; i < engine.maxAliens ; i++)
	{
		if (!aliens[i].active)
		{
			return i;
		}
	}

	return -1;
}

bool alien_add()
{
	int index = alien_getFreeIndex();

	if ((index == -1) || (currentGame.area == 23) || (currentGame.area == 26))
		return 0;

	signed char *alienArray;
	signed char numberOfAliens = 1;

	alienArray = new signed char[8];

	switch(currentGame.area)
	{
		case 0:
		case 3:
		case 11:
			numberOfAliens = 1;
			alienArray[0] = CD_DUALFIGHTER;
			break;
		case 1:
		case 2:
		case 4:
		case 5:
			numberOfAliens = 2;
			alienArray[0] = CD_DUALFIGHTER;
			alienArray[1] = CD_PROTOFIGHTER;
			break;
		case 7:
		case 8:
			numberOfAliens = 3;
			alienArray[0] = CD_DUALFIGHTER;
			alienArray[1] = CD_PROTOFIGHTER;
			alienArray[2] = CD_AIMFIGHTER;
			break;
		case 9:
			// This is the mission where you need to disable cargo ships.
			// Missiles are extremely bad in this mission, not because
			// of the damage they do to you, but because they tend to
			// accidentally destroy the cargo ships. Therefore, ships
			// with missiles (dual fighters and missile boats) are
			// excluded from this mission.
			numberOfAliens = 2;
			alienArray[0] = CD_PROTOFIGHTER;
			alienArray[1] = CD_AIMFIGHTER;
			break;
		case 10:
		case 15:
			numberOfAliens = 1;
			alienArray[0] = CD_ASTEROID;
			break;
		case 13:
		case 14:
		case 16:
			numberOfAliens = 4;
			alienArray[0] = CD_DUALFIGHTER;
			alienArray[1] = CD_PROTOFIGHTER;
			alienArray[2] = CD_MISSILEBOAT;
			alienArray[3] = CD_AIMFIGHTER;
			break;
		case 18:
			numberOfAliens = 2;
			alienArray[0] = CD_DUALFIGHTER;
			alienArray[1] = CD_MINER;
			break;
		case 25:
			numberOfAliens = 6;
			alienArray[0] = CD_DUALFIGHTER;
			alienArray[1] = CD_PROTOFIGHTER;
			alienArray[2] = CD_MISSILEBOAT;
			alienArray[3] = CD_AIMFIGHTER;
			alienArray[4] = CD_ESCORT;
			alienArray[5] = CD_MOBILE_RAY;
			break;
		case 22:
			numberOfAliens = 2;
			alienArray[0] = CD_AIMFIGHTER;
			alienArray[1] = CD_DUALFIGHTER;
			break;
		case 24:
			numberOfAliens = 2;
			alienArray[0] = CD_ASTEROID;
			alienArray[1] = CD_ASTEROID2;
			break;
		case MAX_MISSIONS - 1:
			numberOfAliens = 3;
			alienArray[0] = CD_DUALFIGHTER;
			alienArray[1] = CD_MISSILEBOAT;
			alienArray[2] = CD_AIMFIGHTER;
			if (currentGame.system == 2)
			{
				numberOfAliens = 4;
				alienArray[3] = CD_PROTOFIGHTER;
			}
			break;
		default:
			numberOfAliens = 1;
			alienArray[0] = CD_DUALFIGHTER;
			break;
	}

	signed char randEnemy = alienArray[rand() % numberOfAliens];

	if ((currentGame.area != 10) && (currentGame.area != 15) &&
		(currentGame.area != 24))
	{
		if ((currentGame.system == 1) && (currentGame.area == MAX_MISSIONS - 1))
		{
			if ((rand() % 5) == 0)
				randEnemy = CD_SLAVETRANSPORT;
		}

		if ((currentGame.area != MAX_MISSIONS - 1) &&
			((currentGame.maxPlasmaRate > currentGame.minPlasmaRate) ||
				(currentGame.maxPlasmaOutput > currentGame.minPlasmaOutput) ||
				(currentGame.maxPlasmaDamage > currentGame.minPlasmaDamage)))
		{
			if ((rand() % 6) == 0)
				randEnemy = CD_TRANSPORTSHIP;
		}
	}

	delete[] alienArray;

	aliens[index] = alien_defs[randEnemy];
	aliens[index].active = true;
	aliens[index].face = rand() % 2;
	aliens[index].owner = &aliens[index]; // Most enemies will own themselves
	aliens[index].target = &aliens[index];
	aliens[index].thinktime = (50 + rand() % 50);
	aliens[index].systemPower = aliens[index].maxShield;
	aliens[index].deathCounter = 0 - (aliens[index].maxShield * 3);
	aliens[index].hit = 0;

	limitInt(&aliens[index].deathCounter, -250, 0);

	// Attempts to place an alien. If it fails, the alien is deactivated.
	for (int i = 0 ; i < 100 ; i++)
	{
		if (alien_place(&aliens[index]))
			break;
		aliens[index].active = false;

		return false;
	}

	if (aliens[index].classDef == CD_CARGOSHIP)
		addCargo(&aliens[index], P_CARGO);

	if (aliens[index].classDef == CD_MOBILE_RAY)
		aliens[index].shield = 25;

	if (aliens[index].classDef == CD_ESCORT)
		aliens[index].shield = 50;

	aliens[index].dx = rrand(-2, 2);
	aliens[index].dy = rrand(-2, 2);

	aliens[index].ammo[0] = 0;

	if (currentGame.area == 18)
		aliens[index].flags |= FL_HASMINIMUMSPEED;

	return true;
}

void alien_addDrone(object *hostAlien)
{
	int index = alien_getFreeIndex();

	if (index == -1)
		return;

	aliens[index] = alien_defs[CD_DRONE];
	aliens[index].active = true;
	aliens[index].face = rand() % 2;
	aliens[index].owner = &aliens[index]; // Most enemies will own themselves
	aliens[index].target = &aliens[index];
	aliens[index].thinktime = (50 + rand() % 50);
	aliens[index].systemPower = aliens[index].maxShield;
	aliens[index].deathCounter = 0 - (aliens[index].maxShield * 3);
	aliens[index].hit = 0;

	aliens[index].x = hostAlien->x + rand() % 50;
	aliens[index].y = hostAlien->y + rand() % 50;
}

void alien_addSmallAsteroid(object *hostAlien)
{
	if (engine.missionCompleteTimer != 0)
		return;

	int index = -1;
	int debris = 1 + rand() % 10;

	for (int i = 0 ; i < debris ; i++)
		addBullet(&weapon[W_ROCKETS], hostAlien, 0, 0);

	for (int i = 10 ; i < 20 ; i++)
		if (!aliens[i].active)
			index = i;

	if (index == -1)
		return;

	if ((rand() % 10) > 3)
	{
		aliens[index] = alien_defs[CD_ASTEROID2];
		aliens[index].imageIndex[0] = aliens[index].imageIndex[1] = 39 + rand() % 2;
		aliens[index].image[0] = shipShape[aliens[index].imageIndex[0]];
		aliens[index].image[1] = shipShape[aliens[index].imageIndex[1]];
	}
	else
	{
		aliens[index] = alien_defs[CD_DRONE];
	}

	aliens[index].owner = &aliens[index]; // Most enemies will own themselves
	aliens[index].target = &aliens[index];
	aliens[index].thinktime = 1;
	aliens[index].systemPower = aliens[index].maxShield;
	aliens[index].deathCounter = 0 - (aliens[index].maxShield * 3);
	aliens[index].hit = 0;

	aliens[index].x = hostAlien->x;
	aliens[index].y = hostAlien->y;
	aliens[index].active = true;
}

void alien_addFriendly(int type)
{
	if (type != FR_SID)
		aliens[type] = alien_defs[CD_FRIEND];
	else
		aliens[type] = alien_defs[CD_SID];

	aliens[type].owner = &aliens[type];
	aliens[type].target = &aliens[type];
	aliens[type].active = true;

	if (rand() % 2 == 0)
		aliens[type].x = rrand((int)(screen->w / 2), (int)(screen->w / 2) + 150);
	else
		aliens[type].x = rrand((int)(screen->w / 2) - 150, (int)(screen->w / 2));

	if (rand() % 2 == 0)
		aliens[type].y = rrand((int)(screen->h / 2), (int)(screen->h / 2) + 150);
	else
		aliens[type].y = rrand((int)(screen->h / 2) - 150, (int)(screen->h / 2));

	if (type == FR_PHOEBE)
		aliens[type].classDef = CD_PHOEBE;

	if (type == FR_URSULA)
		aliens[type].classDef = CD_URSULA;

	// For the sake of it being the final battle :)
	if (currentGame.area == 25)
		aliens[type].flags |= FL_IMMORTAL;
}

bool alien_place(object *alien)
{
	if (rand() % 2 == 0)
		alien->x = rrand(screen->w, screen->w * 2);
	else
		alien->x = rrand(-screen->w, 0);

	if (rand() % 2 == 0)
		alien->y = rrand(screen->h, screen->h * 2);
	else
		alien->y = rrand(-screen->h, 0);

	if (currentGame.area == 24)
	{
		alien->x = screen->w;
		alien->y = rrand(screen->h / 3, (2 * screen->h) / 3);
	}

	for (int i = 0 ; i < MAX_ALIENS ; i++)
	{
		if ((aliens[i].owner != alien) && (aliens[i].shield > 0))
		{
			if (collision(alien->x, alien->y, alien->image[0]->w,
					alien->image[0]->h, aliens[i].x, aliens[i].y,
					aliens[i].image[0]->w, aliens[i].image[0]->h))
				return false;
		}
	}

	return true;
}

void alien_setAI(object *alien)
{
	// Make friendly craft generally concentrate on smaller fighters
	if ((alien->flags & FL_FRIEND) && (alien->target == &aliens[WC_BOSS]))
	{
		if ((rand() % 5) == 0)
		{
			alien->target = alien;
			alien->thinktime = 0;
			return;
		}
	}

	int i = rand() % 10;
	float tx = alien->target->x;
	float ty = alien->target->y;

	int chase = 0; // Chance in 10 of chasing player
	int area = 0; // Chance in 10 of moving to an area around the player
	int stop = 0; // Chance in 10 of hanging back
	int point = 0; // Size of area alien will move into

	switch (alien->AIType)
	{
		case AI_NORMAL:
			chase = 3;
			point = 6;
			stop = 9;
			area = 250;
			break;
		case AI_OFFENSIVE:
			chase = 7;
			point = 8;
			stop = 9;
			area = 50;
			break;
		case AI_DEFENSIVE:
			chase = 2;
			point = 6;
			stop = 8;
			area = 300;
			break;
		case AI_EVASIVE:
			chase = 1;
			point = 8;
			stop = 9;
			area = 600;
			break;
		case AI_WANDER:
			chase = -1;
			point = 0;
			stop = 10;
			area = 1200;
			break;
	}

	if (i <= chase)
	{
		// Chase the target
		alien->dx = ((alien->x - tx) / ((300 / alien->speed)  + rand() % 100));
		alien->dy = ((alien->y - ty) / ((300 / alien->speed)  + rand() % 100));
		return;
	}
	else if ((i >= point) && (i <= stop))
	{
		// Fly to a random point around the target
		tx += (rand() % area - (rand() % area * 2));
		ty += (rand() % area - (rand() % area * 2));
		alien->dx = ((alien->x - tx) / ((300 / alien->speed)  + rand() % 100));
		alien->dy = ((alien->y - ty) / ((300 / alien->speed)  + rand() % 100));
		return;
	}
	else
	{
		// Hang back
		alien->dx = 0;
		alien->dy = 0;
		return;
	}
}

void alien_setKlineAttackMethod(object *alien)
{
	alien->maxShield -= 500;

	if (alien->maxShield >= 1000)
	{
		setRadioMessage(FACE_KLINE, "Very good, Bainfield. Now let's get a little more serious...", 1);
		alien->weaponType[0] = W_SPREADSHOT;
		alien->chance[1] = 40;
	}
	else if (alien->maxShield >= 500)
	{
		setRadioMessage(FACE_KLINE, "Your ability to stay alive irritates me!! Try dodging some of these!!", 1);
		alien->weaponType[0] = W_DIRSHOCKMISSILE;
		alien->weaponType[1] = W_DIRSHOCKMISSILE;
		alien->chance[0] = 2;
		alien->chance[1] = 2;
		alien->flags |= FL_AIMS;
	}
	else
	{
		setRadioMessage(FACE_KLINE, "ENOUGH!! THIS ENDS NOW!!!", 1);
		alien->weaponType[0] = W_AIMED_SHOT;
		alien->weaponType[1] = W_MICRO_HOMING_MISSILES;
		alien->flags |= FL_CANCLOAK;
		alien->chance[0] = 100;
		alien->chance[1] = 2;
		alien->flags &= ~FL_CANNOTDIE;
	}

	alien->shield = 500;
}

/*
This AI is exclusively for Kline.
*/
void alien_setKlineAI(object *alien)
{
	// Weapon type change
	if ((rand() % 3) == 0)
	{
		if (currentGame.area != 26)
		{
			alien->flags &= ~FL_AIMS;

			switch(rand() % 2)
			{
				case 0:
					alien->weaponType[0] = W_TRIPLE_SHOT;
					break;
				case 1:
					alien->weaponType[0] = W_AIMED_SHOT;
					alien->flags |= FL_AIMS;
					break;
			}
		}
	}

	alien->flags &= ~(FL_CIRCLES | FL_CONTINUOUS_FIRE | FL_DROPMINES);

	switch(rand() % 10)
	{
		case 0:
			if ((alien->weaponType[0] != W_DIRSHOCKMISSILE) &&
					(alien->weaponType[1] != W_MICRO_HOMING_MISSILES))
				alien->flags |= FL_CONTINUOUS_FIRE;
			alien->dx = ((alien->x - alien->target->x) /
				((300 / alien->speed)  + rand() % 100));
			alien->dy = ((alien->y - alien->target->y) /
				((300 / alien->speed)  + rand() % 100));
			break;
		case 1:
		case 2:
			// Kline only attacks then he is ready!
			if ((!(alien->flags & FL_NOFIRE)) && (currentGame.area == 11))
				alien->flags |= FL_DROPMINES;
			break;
		case 3:
		case 4:
			alien->flags |= FL_CIRCLES;
			break;
		default:
			alien_setAI(alien);
			break;
	}
}

/*
"Looks" for an enemy by picking a randomly active enemy and using them
as a target. If the target is too far away, it will be ignored.
*/
void alien_searchForTarget(object *alien)
{
	int i;

	if (alien->flags & FL_WEAPCO)
	{
		i = (rand() % 10);

		if (i == 0)
		{
			alien->target = &player;
			return;
		}
	}

	i = rand() % MAX_ALIENS;

	object *targetEnemy = &aliens[i];

	// Tell Sid not to attack craft that are already disabled or can
	// return fire. This will save him from messing about (unless we're on the last mission)
	if ((alien->classDef == CD_SID) && (currentGame.area != 25))
	{
		if ((targetEnemy->flags & FL_DISABLED) || (!(targetEnemy->flags & FL_NOFIRE)))
			return;
	}

	// Tell Phoebe and Ursula not to attack ships that cannot fire or are disabled (unless we're on the last mission)
	if (currentGame.area != 25)
	{
		if ((alien->classDef == CD_PHOEBE) || (alien->classDef == CD_URSULA))
		{
			// Don't attack the boss or we could be here all day(!)
			if (targetEnemy->classDef == CD_BOSS)
				return;

			if ((targetEnemy->flags & FL_DISABLED) ||
					(targetEnemy->flags & FL_NOFIRE))
				return;
		}
	}

	if ((targetEnemy->shield < 1) || (!targetEnemy->active))
		return;

	if ((targetEnemy->flags & FL_WEAPCO) && (alien->flags & FL_WEAPCO))
		return;

	if ((targetEnemy->flags & FL_FRIEND) && (alien->flags & FL_FRIEND))
		return;

	if (abs((int)alien->x - (int)alien->target->x) > 550)
		return;

	if (abs((int)alien->y - (int)alien->target->y) > 400)
		return;

	alien->target = targetEnemy;
}

/*
Do various checks to see if the alien can fire at the target.
*/
int alien_checkTarget(object *alien)
{
	// No target
	if (alien->target == alien)
		return 0;

	// Whilst firing a Ray, no other weapons can be fired!
	if (alien->flags & FL_FIRERAY)
		return 0;

	// The target is on the same side as you!
	if ((alien->flags & FL_WEAPCO) && (alien->target->flags & FL_WEAPCO))
		return 0;
	if ((alien->flags & FL_FRIEND) && (alien->target->flags & FL_FRIEND))
		return 0;

	// You're facing the wrong way
	if ((alien->face == 0) && (alien->target->x < alien->x))
		return 0;
	if ((alien->face == 1) && (alien->target->x > alien->x))
		return 0;

	// Slightly more than half a screen away from you
	if (abs((int)alien->x - (int)alien->target->x) > 550)
		return 0;

	if ((alien->flags & FL_AIMS) || (alien->flags & FL_CONTINUOUS_FIRE))
		return 1;

	// Not at the correct vertical height
	if ((alien->y < alien->target->y - 15) ||
			(alien->y > alien->target->y + alien->target->image[0]->h + 15))
		return 0;

	return 1;
}

/*
Currently only used for the allies. Whilst flying around, the allies will fire on
any enemy craft that enter their line of sight.
*/
int alien_enemiesInFront(object *alien)
{
	object *anEnemy = aliens;

	for (int i = 0 ; i < MAX_ALIENS ; i++)
	{
		if ((alien != anEnemy) && (anEnemy->flags & FL_WEAPCO) &&
			(anEnemy->shield > 0))
		{
			if ((alien->y > anEnemy->y - 15) &&
				(alien->y < anEnemy->y + anEnemy->image[0]->h + 15))
			{
				if ((alien->face == 1) && (anEnemy->x < alien->x))
					return 1;
				if ((alien->face == 0) && (anEnemy->x > alien->x))
					return 1;
			}
		}

		anEnemy++;
	}

	return 0;
}

void alien_move(object *alien)
{
	bool checkCollisions;

	if ((alien->flags & FL_LEAVESECTOR) || (alien->shield < 1))
		checkCollisions = false;
	else
		checkCollisions = true;

	if (alien->owner == alien)
	{
		if (alien->flags & FL_CIRCLES)
		{
			if (alien->face == 0)
			{
				alien->dx += 0.02;
				alien->dy += 0.02;
			}
			else
			{
				alien->dx -= 0.02;
				alien->dy -= 0.02;
			}

			alien->x -= (sinf(alien->dx) * 4);
			alien->y -= (cosf(alien->dy) * 4);
		}
		else
		{
			alien->x -= alien->dx;
			alien->y -= alien->dy;
		}
	}

	object *anEnemy = aliens;

	if (checkCollisions)
	{
		for (int i = 0 ; i < MAX_ALIENS ; i++)
		{
			if ((alien->flags & FL_LEAVESECTOR) ||
				(alien->classDef == CD_DRONE) ||
				(alien->classDef == CD_ASTEROID2) ||
				(alien->owner == anEnemy->owner) ||
				(alien->owner->owner == anEnemy->owner) ||
				(anEnemy->shield < 1))
			{
				anEnemy++;
				continue;
			}

			if (collision(alien, anEnemy))
			{
				if ((anEnemy->classDef == CD_BARRIER) &&
					(anEnemy->owner != alien))
				{
					alien->shield--;
					alien->hit = 3;
					alien->dx *= -1;
					alien->dy *= -1;
					playSound(SFX_HIT, alien->x);
				}
			}

			anEnemy++;
		}
	}

	// Handle a collision with the player
	if ((player.shield > 0) && (alien->shield > 0) && (checkCollisions))
	{
		if (collision(alien, &player))
		{
			if (alien->classDef == CD_ASTEROID)
			{
				if (!engine.cheatShield)
					player.shield -= alien->shield;
				alien->shield = 0;
				playSound(SFX_EXPLOSION, alien->x);
				setInfoLine("Warning: Asteroid Collision Damage!!", FONT_RED);
				player.hit = 5;
				playSound(SFX_HIT, player.x);
			}

			if (alien->classDef == CD_ASTEROID2)
			{
				if (!engine.cheatShield)
					player.shield -= alien->shield;
				alien->shield = 0;
				playSound(SFX_EXPLOSION, alien->x);
				setInfoLine("Warning: Asteroid Collision Damage!!", FONT_RED);
				player.hit = 5;
				playSound(SFX_HIT, player.x);
			}

			if (alien->classDef == CD_BARRIER)
			{
				if (!engine.cheatShield)
					player.shield--;
				player.hit = 5;
				playSound(SFX_HIT, player.x);
			}
		}
	}
}
