/*
Copyright (C) 2003 Parallel Realities

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "Starfighter.h"

object defEnemy[MAX_DEFALIENS];
object enemy[MAX_ALIENS];

static bool placeAlien(object *theEnemy)
{
	if (rand() % 2 == 0)
		theEnemy->x = rrand(800, 1600);
	else
		theEnemy->x = rrand(-800, 0);

	if (rand() % 2 == 0)
		theEnemy->y = rrand(600, 1200);
	else
		theEnemy->y = rrand(-600, 0);

	if (currentGame.area == 24)
	{
		theEnemy->x = 800;
		theEnemy->y = rrand(200, 400);
	}

	for (int i = 0 ; i < MAX_ALIENS ; i++)
	{
		if ((enemy[i].owner != theEnemy) && (enemy[i].shield > 0))
		{
			if (collision(theEnemy->x, theEnemy->y, theEnemy->image[0]->w, theEnemy->image[0]->h, enemy[i].x, enemy[i].y, enemy[i].image[0]->w, enemy[i].image[0]->h))
				return false;
		}
	}

	return true;
}

/*
This simply pulls back an alien from the array that is
"dead" (no shield) and returns the index number so we can have
a new one.
*/
static int getAlien()
{
	for (int i = 0 ; i < engine.maxAliens ; i++)
	{
		if (!enemy[i].active)
		{
			return i;
		}
	}

	return -1;
}

static void addDrone(object *host)
{
	int index = getAlien();

	if (index == -1)
		return;

	enemy[index] = defEnemy[CD_DRONE];
	enemy[index].active = true;
	enemy[index].face = rand() % 2;
	enemy[index].owner = &enemy[index]; // Most enemies will own themselves
	enemy[index].target = &enemy[index];
	enemy[index].thinktime = (50 + rand() % 50);
	enemy[index].systemPower = enemy[index].maxShield;
	enemy[index].deathCounter = 0 - (enemy[index].maxShield * 3);
	enemy[index].hit = 0;

	enemy[index].x = host->x + rand() % 50;
	enemy[index].y = host->y + rand() % 50;
}

static void addSmallAsteroid(object *host)
{
	if (engine.missionCompleteTimer != 0)
		return;

	int index = -1;
	int debris = 1 + rand() % 10;

	for (int i = 0 ; i < debris ; i++)
		addBullet(&weapon[W_ROCKETS], host, 0, 0);

	for (int i = 10 ; i < 20 ; i++)
		if (!enemy[i].active)
			index = i;

	if (index == -1)
		return;

	if ((rand() % 10) > 3)
	{
		enemy[index] = defEnemy[CD_ASTEROID2];
		enemy[index].imageIndex[0] = enemy[index].imageIndex[1] = 39 + rand() % 2;
		enemy[index].image[0] = graphics.shipShape[enemy[index].imageIndex[0]];
		enemy[index].image[1] = graphics.shipShape[enemy[index].imageIndex[1]];
	}
	else
	{
		enemy[index] = defEnemy[CD_DRONE];
	}

	enemy[index].owner = &enemy[index]; // Most enemies will own themselves
	enemy[index].target = &enemy[index];
	enemy[index].thinktime = 1;
	enemy[index].systemPower = enemy[index].maxShield;
	enemy[index].deathCounter = 0 - (enemy[index].maxShield * 3);
	enemy[index].hit = 0;

	enemy[index].x = host->x;
	enemy[index].y = host->y;
	enemy[index].active = true;
}

bool addAlien()
{
	int index = getAlien();

	if ((index == -1) || (currentGame.area == 23) || (currentGame.area == 26))
		return 0;

	signed char *alienArray;
	signed char numberOfAliens = 1;

	alienArray = new signed char[5];

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
		case 9:
		case 13:
		case 14:
		case 16:
			numberOfAliens = 4;
			alienArray[0] = CD_DUALFIGHTER;
			alienArray[1] = CD_PROTOFIGHTER;
			alienArray[2] = CD_MISSILEBOAT;
			alienArray[3] = CD_AIMFIGHTER;
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
		case 7:
		case 8:
			numberOfAliens = 3;
			alienArray[0] = CD_DUALFIGHTER;
			alienArray[1] = CD_PROTOFIGHTER;
			alienArray[2] = CD_AIMFIGHTER;
			break;
		case 18:
			numberOfAliens = 2;
			alienArray[0] = CD_DUALFIGHTER;
			alienArray[1] = CD_MINER;
			break;
		case 10:
		case 15:
			numberOfAliens = 1;
			alienArray[0] = CD_ASTEROID;
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

	if ((currentGame.area != 10) && (currentGame.area != 15) && (currentGame.area != 24))
	{
		if ((currentGame.system == 1) && (currentGame.area == MAX_MISSIONS - 1))
		{
			if ((rand() % 5) == 0)
				randEnemy = CD_SLAVETRANSPORT;
		}

		if ((rand() % 6) == 0)
			randEnemy = CD_TRANSPORTSHIP;
	}

	delete[] alienArray;

	enemy[index] = defEnemy[randEnemy];
	enemy[index].active = true;
	enemy[index].face = rand() % 2;
	enemy[index].owner = &enemy[index]; // Most enemies will own themselves
	enemy[index].target = &enemy[index];
	enemy[index].thinktime = (50 + rand() % 50);
	enemy[index].systemPower = enemy[index].maxShield;
	enemy[index].deathCounter = 0 - (enemy[index].maxShield * 3);
	enemy[index].hit = 0;

	limitInt(&enemy[index].deathCounter, -250, 0);

	// Attempts to place an alien. If it fails, the alien is deactivated.
	for (int i = 0 ; i < 100 ; i++)
	{
		if (placeAlien(&enemy[index]))
			break;
		enemy[index].active = false;

		return false;
	}

	if (enemy[index].classDef == CD_CARGOSHIP)
		addCargo(&enemy[index], P_CARGO);

	if (enemy[index].classDef == CD_MOBILE_RAY)
		enemy[index].shield = 25;

	if (enemy[index].classDef == CD_ESCORT)
		enemy[index].shield = 50;

	enemy[index].dx = rrand(-2, 2);
	enemy[index].dy = rrand(-2, 2);

	enemy[index].ammo[0] = 0;

	if (currentGame.area == 18)
		enemy[index].flags += FL_HASMINIMUMSPEED;

	return true;
}

static void getPreDefinedAliens()
{
	FILE *fp;
	char string[255];
	strcpy(string, "");

	int index, alienType, placeAttempt;
	int barrierSpeed = 1;

	sprintf(string, "data/aliens%d.dat", currentGame.area);
	#if USEPACK
	int dataLocation = locateDataInPak(string, 0);
	if (dataLocation == -1)
		return;
	fp = fopen(PACKLOCATION, "rb");
	fseek(fp, dataLocation, SEEK_SET);
	#else
	fp = fopen(string, "rb");
	if (fp == NULL)
		return;
	#endif

	fscanf(fp, "%d ", &index);

	while (index != -1)
	{
		placeAttempt = 0;

		fscanf(fp, "%d ", &alienType);

		enemy[index] = defEnemy[alienType];
		enemy[index].owner = &enemy[index];
		enemy[index].target = &enemy[index];
		enemy[index].face = rand() % 2;
		enemy[index].active = true;

		/*
		we make 1000 attempts to place this enemy since it is required. If after 1000 attempts
		we still have managed to place the alien, then it simple isn't going to happen and we
		will just exit the same. The chances of this happening are very very low!
		*/
		while (true)
		{
			placeAttempt++;

			if (placeAlien(&enemy[index]))
				break;

			if (placeAttempt > 1000)
				showErrorAndExit(2, "");
		}

		if (currentGame.area == 2)
			addCargo(&enemy[index], P_CARGO);
		else if (currentGame.area == 7)
			addCargo(&enemy[index], P_PHOEBE);

		if (index == WC_KLINE)
		{
			enemy[WC_KLINE].target = &player;
			if (currentGame.area == 25)
				enemy[WC_KLINE].shield = 500;
		}

		if (enemy[index].classDef == CD_CLOAKFIGHTER)
		{
			enemy[index].active = false;
			enemy[index].maxShield = enemy[index].shield = 400;
			enemy[index].flags -= FL_RUNSAWAY;
			enemy[index].speed = 3;
		}

		if ((enemy[index].classDef == CD_MOBILE_RAY) && (index >= 11))
		{
			enemy[index].active = false;
		}

		if (enemy[index].classDef == CD_FIREFLY)
		{
			enemy[index].active = false;
		}

		if (enemy[index].classDef == CD_BARRIER)
		{
			enemy[index].owner = &enemy[WC_BOSS];
			enemy[index].speed = barrierSpeed;
			barrierSpeed++;
		}

		if ((currentGame.area == 17) && (enemy[index].classDef == CD_BOSS))
		{
			enemy[index].imageIndex[1] = 29;
			enemy[index].flags += FL_IMMORTAL;
		}

		if (currentGame.area == 18)
			enemy[index].flags += FL_HASMINIMUMSPEED;

		if (currentGame.area == 23)
		{
			enemy[index].flags = FL_WEAPCO;
			if (index == WC_BOSS)
				enemy[index].chance[1] = 5;
		}

		fscanf(fp, "%d ", &index);
	}

	fclose(fp);

	if (currentGame.area == 5)
	{
		enemy[WC_BOSS].target = &player;
		enemy[WC_BOSS].x = -400;
		enemy[WC_BOSS].y = 300;

		enemy[13].owner = &enemy[WC_BOSS];
		enemy[13].target = &player;
		enemy[13].dx = -25;
		enemy[13].dy = -21;

		enemy[12].owner = &enemy[WC_BOSS];
		enemy[12].target = &player;
		enemy[12].dx = -20;
		enemy[12].dy = 37;
	}
	else if ((currentGame.area == 11) || (currentGame.area == 14))
	{
		enemy[WC_BOSS].target = &player;
		enemy[WC_BOSS].x = -400;
		enemy[WC_BOSS].y = 300;

		enemy[13].owner = &enemy[WC_BOSS];
		enemy[13].target = &player;
		enemy[13].dx = 15;
		enemy[13].dy = -22;

		enemy[12].owner = &enemy[WC_BOSS];
		enemy[12].target = &player;
		enemy[12].dx = 15;
		enemy[12].dy = 22;

		enemy[11].owner = &enemy[13];
		enemy[11].target = &player;
		enemy[11].dx = -35;
		enemy[11].dy = -12;

		enemy[10].owner = &enemy[12];
		enemy[10].target = &player;
		enemy[10].dx = -35;
		enemy[10].dy = 20;

		if (currentGame.area == 14)
		{
			enemy[WC_BOSS].AIType = AI_EVASIVE;

			for (int i = 10 ; i < 15 ; i++)
			{
				enemy[i].imageIndex[0] += 15;
				enemy[i].imageIndex[1] += 15;

				enemy[i].image[0] = graphics.shipShape[enemy[i].imageIndex[0]];
				enemy[i].image[1] = graphics.shipShape[enemy[i].imageIndex[1]];
			}
		}
	}
	else if (currentGame.area == 21)
	{
		enemy[WC_BOSS].target = &player;
		enemy[WC_BOSS].x = 400;
		enemy[WC_BOSS].y = 300;

		enemy[13].owner = &enemy[WC_BOSS];
		enemy[13].dy = 20;

		enemy[12].owner = &enemy[WC_BOSS];
		enemy[12].dy = -16;
	}
}

static void addFriendly(int type)
{
	if (type != FR_SID)
		enemy[type] = defEnemy[CD_FRIEND];
	else
		enemy[type] = defEnemy[CD_SID];

	enemy[type].owner = &enemy[type];
	enemy[type].target = &enemy[type];
	enemy[type].active = true;

	if (rand() % 2 == 0)
		enemy[type].x = rrand(400, 550);
	else
		enemy[type].x = rrand(250, 400);

	if (rand() % 2 == 0)
		enemy[type].y = rrand(300, 450);
	else
		enemy[type].y = rrand(150, 300);

	if (type == FR_PHOEBE)
		enemy[type].classDef = CD_PHOEBE;

	if (type == FR_URSULA)
		enemy[type].classDef = CD_URSULA;

	// For the sake of it being the final battle :)
	if (currentGame.area == 25)
		enemy[type].flags += FL_IMMORTAL;
}

void setTarget(int index)
{
	engine.targetIndex = index;
	engine.targetShield = 85;
	engine.targetShield /= enemy[index].shield;

	engine.targetArrowTimer = -1;
	if (currentGame.area == 10)
		engine.targetArrowTimer = 0;
}

void initAliens()
{
	for (int i = 0 ; i < MAX_ALIENS ; i++)
	{
		enemy[i].active = false;
		enemy[i].shield = -1;
		enemy[i].flags = 0;
	}

	engine.targetIndex = -1;

	getPreDefinedAliens();

	// specific for Phoebe being captured!
	if (currentGame.area == 7)
		currentGame.hasWingMate1 = 1;

	if (currentGame.area == 11)
		enemy[WC_KLINE].active = false;

	for (int i = 0 ; i < engine.maxAliens ; i++)
		addAlien();

	if (currentGame.hasWingMate1)
		addFriendly(FR_PHOEBE);

	if (currentGame.hasWingMate2)
		addFriendly(FR_URSULA);

	if ((currentGame.area == 9) || (currentGame.area == 17) || (currentGame.area == 25))
		addFriendly(FR_SID);

	// Disable Wingmates for certain missions
	switch (currentGame.area)
	{
		case 7:
		case 9:
		case 10:
		case 15:
		case 16:
		case 18:
		case 24:
		case 26:
			enemy[FR_PHOEBE].active = false;
			enemy[FR_URSULA].active = false;
			break;
	}

	if (currentGame.area == 10)
	{
		enemy[0].collectChance = 100;
		enemy[0].collectType = P_ESCAPEPOD;
	}

	// Some specifics for interception missions
	if (currentGame.area == MAX_MISSIONS - 1)
	{
		if ((currentGame.system > 1) && ((rand() % 5) == 0))
		{
			enemy[WC_KLINE] = defEnemy[CD_KLINE];
			enemy[WC_KLINE].owner = &enemy[WC_KLINE];
			enemy[WC_KLINE].target = &player;
			enemy[WC_KLINE].shield = 100;
			enemy[WC_KLINE].active = true;
			enemy[WC_KLINE].x = player.x + 1000;
			enemy[WC_KLINE].y = player.y;
			setTarget(WC_KLINE);
		}

		if ((currentGame.system == 2) && (currentGame.experimentalShield > 0))
		{
			if ((rand() % 2) == 0)
			{
				enemy[10] = defEnemy[CD_CLOAKFIGHTER];
				enemy[10].owner = &enemy[10];
				enemy[10].target = &enemy[10];
				enemy[10].shield = 1000;
				enemy[10].active = true;
				enemy[10].x = player.x - 1000;
				enemy[10].y = player.y;
				setTarget(10);
				enemy[10].shield = currentGame.experimentalShield;
			}
		}
	}

	if (currentGame.area == 26)
	{
		enemy[WC_KLINE].flags += FL_IMMORTAL;
		enemy[WC_KLINE].flags += FL_NOFIRE;
		enemy[WC_KLINE].flags += FL_NOMOVE;

		enemy[WC_KLINE].x = 600;
		enemy[WC_KLINE].y = 300;

		enemy[WC_KLINE].deathCounter = -250;
		enemy[WC_KLINE].maxShield = 1500;
		enemy[WC_KLINE].shield = 500;
	}

	for (int i = 0 ; i < MAX_ALIENS ; i++)
	{
		enemy[i].systemPower = enemy[i].maxShield;
		enemy[i].deathCounter = 0 - (enemy[i].maxShield * 3);
		limitInt(&enemy[i].deathCounter, -350, 0);
	}

	// Set target energy meter
	switch (currentGame.area)
	{
		case 5:
		case 11:
		case 13:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 23:
			setTarget(WC_BOSS);
			break;
		case 7:
			setTarget(FR_PHOEBE);
			break;
		case 8:
			setTarget(19);
			break;
		case 9:
			setTarget(FR_SID);
			break;
		case 10:
			setTarget(0);
			break;
		case 25:
		case 26:
			setTarget(WC_KLINE);
			break;
		default:
			break;
	}
}

/*
"Looks" for an enemy by picking a randomly active enemy and using them
as a target. If the target is too far away, it will be ignored.
*/
static void searchForTarget(object *theEnemy)
{
	int i;

	if (theEnemy->flags & FL_WEAPCO)
	{
		i = (rand() % 10);

		if (i == 0)
		{
			theEnemy->target = &player;
			return;
		}
	}

	i = rand() % MAX_ALIENS;

	object *targetEnemy = &enemy[i];

	// Tell Sid not to attack craft that are already disabled or can
	// return fire. This will save him from messing about (unless we're on the last mission)
	if ((theEnemy->classDef == CD_SID) && (currentGame.area != 25))
	{
		if ((targetEnemy->flags & FL_DISABLED) || (!(targetEnemy->flags & FL_NOFIRE)))
			return;
	}

	// Tell Phoebe and Ursula not to attack ships that cannot fire or are disabled (unless we're on the last mission)
	if (currentGame.area != 25)
	{
		if ((theEnemy->classDef == CD_PHOEBE) || (theEnemy->classDef == CD_URSULA))
		{
			// Don't attack the boss or we could be here all day(!)
			if (targetEnemy->classDef == CD_BOSS)
				return;

			if ((targetEnemy->flags & FL_DISABLED) || (targetEnemy->flags & FL_NOFIRE))
				return;
		}
	}

	if ((targetEnemy->shield < 1) || (!targetEnemy->active))
		return;

	if ((targetEnemy->flags & FL_WEAPCO) && (theEnemy->flags & FL_WEAPCO))
		return;

	if ((targetEnemy->flags & FL_FRIEND) && (theEnemy->flags & FL_FRIEND))
		return;

	if (abs((int)theEnemy->x - (int)theEnemy->target->x) > 550)
		return;

	if (abs((int)theEnemy->y - (int)theEnemy->target->y) > 400)
		return;

	theEnemy->target = targetEnemy;
}

static int traceTarget(object *theEnemy)
{
	// Do various checks to see if the alien can fire at
	// the target. Start with the most obvious checks.

	// No target
	if (theEnemy->target == theEnemy)
		return 0;

	// Whilst firing a Ray, no other weapons can be fired!
	if (theEnemy->flags & FL_FIRERAY)
		return 0;

	// The target is on the same side as you!
	if ((theEnemy->flags & FL_WEAPCO) && (theEnemy->target->flags & FL_WEAPCO))
		return 0;
	if ((theEnemy->flags & FL_FRIEND) && (theEnemy->target->flags & FL_FRIEND))
		return 0;

	// You're facing the wrong way
	if ((theEnemy->face == 0) && (theEnemy->target->x < theEnemy->x))
		return 0;
	if ((theEnemy->face == 1) && (theEnemy->target->x > theEnemy->x))
		return 0;

	// Slightly more than half a screen away from you
	if (abs((int)theEnemy->x - (int)theEnemy->target->x) > 550)
		return 0;

	if ((theEnemy->flags & FL_AIMS) || (theEnemy->flags & FL_CONTINUOUS_FIRE))
		return 1;

	// Not at the correct vertical height
	if ((theEnemy->y < theEnemy->target->y - 15) || (theEnemy->y > theEnemy->target->y + theEnemy->target->image[0]->h + 15))
		return 0;

	return 1;
}

/*
Currently only used for the allies. Whilst flying around, the allies will fire on
any enemy craft that enter their line of sight.
*/
static int traceView(object *theEnemy)
{
	object *anEnemy = enemy;

	for (int i = 0 ; i < MAX_ALIENS ; i++)
	{
		if ((theEnemy != anEnemy) && (anEnemy->flags & FL_WEAPCO) && (anEnemy->shield > 0))
		{
			if ((theEnemy->y > anEnemy->y - 15) && (theEnemy->y < anEnemy->y + anEnemy->image[0]->h + 15))
			{
				if ((theEnemy->face == 1) && (anEnemy->x < theEnemy->x))
					return 1;
				if ((theEnemy->face == 0) && (anEnemy->x > theEnemy->x))
					return 1;
			}
		}

		anEnemy++;
	}

	return 0;
}

static void moveAndSeparate(object *theEnemy)
{
	bool checkCollisions = true;
	bool hasCollided = false;

	// don't worry about bumping into other craft
	if ((theEnemy->flags & FL_LEAVESECTOR) || (theEnemy->classDef == CD_DRONE) || (currentGame.area == 18))
		checkCollisions = false;

	if (theEnemy->shield < 1)
		checkCollisions = false;

	if (theEnemy->owner == theEnemy)
	{
		if (theEnemy->flags & FL_CIRCLES)
		{
			if (theEnemy->face == 0)
			{
				theEnemy->dx += 0.02;
				theEnemy->dy += 0.02;
			}
			else
			{
				theEnemy->dx -= 0.02;
				theEnemy->dy -= 0.02;
			}

			theEnemy->x -= (sinf(theEnemy->dx) * 4);
			theEnemy->y -= (cosf(theEnemy->dy) * 4);
		}
		else
		{
			theEnemy->x -= theEnemy->dx;
			theEnemy->y -= theEnemy->dy;
		}
	}

	object *anEnemy = enemy;

	if (checkCollisions)
	{
		for (int i = 0 ; i < MAX_ALIENS ; i++)
		{
			if ((theEnemy->flags & FL_LEAVESECTOR) || (theEnemy->classDef == CD_DRONE) || (theEnemy->classDef == CD_ASTEROID2) || (theEnemy->owner == anEnemy->owner) || (theEnemy->owner->owner == anEnemy->owner) || (anEnemy->shield < 1))
			{
				anEnemy++;
				continue;
			}

			if (collision(theEnemy, anEnemy))
			{
				if ((anEnemy->classDef == CD_BARRIER) && (anEnemy->owner != theEnemy))
				{
					theEnemy->shield--;
					theEnemy->hit = 3;
					theEnemy->dx *= -1;
					theEnemy->dy *= -1;
					playSound(SFX_HIT);
				}

				if (anEnemy->owner == anEnemy)
					hasCollided = true;
			}

			anEnemy++;
		}
	}

	// Handle a collision with the player
	if ((player.shield > 0) && (theEnemy->shield > 0) && (checkCollisions))
	{
		if (collision(theEnemy, &player))
		{
			hasCollided = true;

			if (theEnemy->classDef == CD_ASTEROID)
			{
				if (!engine.cheatShield)
					player.shield -= theEnemy->shield;
				theEnemy->shield = 0;
				playSound(SFX_EXPLOSION);
				setInfoLine("Warning: Asteroid Collision Damage!!", FONT_RED);
				player.hit = 5;
				playSound(SFX_HIT);
			}

			if (theEnemy->classDef == CD_ASTEROID2)
			{
				if (!engine.cheatShield)
					player.shield -= theEnemy->shield;
				theEnemy->shield = 0;
				playSound(SFX_EXPLOSION);
				setInfoLine("Warning: Asteroid Collision Damage!!", FONT_RED);
				player.hit = 5;
				playSound(SFX_HIT);
			}

			if (theEnemy->classDef == CD_BARRIER)
			{
				if (!engine.cheatShield)
					player.shield--;
				player.hit = 5;
				playSound(SFX_HIT);
			}
		}
	}

	// Got back to where you originally were...
	if (theEnemy->owner == theEnemy)
	{
		if (hasCollided)
		{
			if (theEnemy->flags & FL_CIRCLES)
			{
				if (theEnemy->face == 0)
				{
					theEnemy->dx -= 0.02;
					theEnemy->dy -= 0.02;
				}
				else
				{
					theEnemy->dx += 0.02;
					theEnemy->dy += 0.02;
				}

				theEnemy->x += (sinf(theEnemy->dx) * 4);
				theEnemy->y += (cosf(theEnemy->dy) * 4);

				theEnemy->thinktime = 0;
			}
			else
			{
				theEnemy->x += theEnemy->dx;
				theEnemy->y += theEnemy->dy;

				theEnemy->dx *= -1;
				theEnemy->dy *= -1;

				theEnemy->dx *= 0.2;
				theEnemy->dy *= 0.2;

				limitInt(&theEnemy->thinktime, 0, 15);
			}
		}
	}
}

/*
Call this whenever a mission requires all the remaining aliens to
automatically die
*/
void killAllAliens()
{
	for (int i = 0 ; i < MAX_ALIENS ; i++)
	{
		if ((enemy[i].flags & FL_WEAPCO) && (enemy[i].active) && (enemy[i].shield > 0))
			enemy[i].shield = 0;
	}
}

void doAliens()
{
	static float barrierLoop = 0;

	barrierLoop += 0.2;

	// A global variable for checking if all the aliens are dead
	engine.allAliensDead = 1;

	bool canFire;
	int shapeToUse;

	object *theEnemy = enemy;

	for (int i = 0 ; i < MAX_ALIENS ; i++)
	{
		if (theEnemy->active)
		{
			if (theEnemy->shield > 0)
			{
				if ((theEnemy->flags & FL_WEAPCO) && (!(theEnemy->flags & FL_DISABLED)))
					engine.allAliensDead = 0;

				// Set part attributes
				if (theEnemy->owner != theEnemy)
				{
					theEnemy->face = theEnemy->owner->face;

					if (theEnemy->face == 0)
						theEnemy->x = theEnemy->owner->x - theEnemy->dx;
					else
						theEnemy->x = theEnemy->owner->x + theEnemy->owner->image[0]->w + theEnemy->dx - theEnemy->image[0]->w;

					theEnemy->y = (theEnemy->owner->y + theEnemy->dy);

					if (theEnemy->owner->shield < 1)
					{
						if ((theEnemy->classDef != CD_URANUSBOSSWING1) && (theEnemy->classDef != CD_URANUSBOSSWING2))
						{
							theEnemy->shield = 0;
						}
						else
						{
							theEnemy->flags -= FL_IMMORTAL;
							theEnemy->owner = theEnemy;
							theEnemy->chance[0] = 25;
						}
					}
				}

				canFire = true; // The alien is allowed to fire

				limitInt(&--theEnemy->thinktime, 0, 250);

				if (theEnemy->target->shield < 1)
					theEnemy->target = theEnemy;

				// Specific to Sid to stop him pissing about(!)
				if ((theEnemy->classDef == CD_SID) && (theEnemy->target->flags & FL_DISABLED))
					theEnemy->target = theEnemy;

 				if (theEnemy->target == theEnemy)
				{
					if (engine.missionCompleteTimer == 0)
					{
						searchForTarget(theEnemy);
					}
					else
					{
						if (theEnemy->flags & FL_FRIEND)
						{
							theEnemy->target = &player;
							theEnemy->thinktime = 1;
						}
					}
				}

				if ((!(theEnemy->flags & FL_DISABLED)) && (theEnemy->thinktime == 0) && (theEnemy->target != theEnemy) && (theEnemy->owner == theEnemy))
				{
					if (theEnemy->classDef != CD_KLINE)
						setEnemyAI(theEnemy);
					else
						setKlineAI(theEnemy);

					theEnemy->thinktime = (rand() % 25) * 10;

					// Face direction of movement unless you always face your target(!)

					if (!(theEnemy->flags & FL_ALWAYSFACE))
					{
						theEnemy->face = 0;
						if (theEnemy->dx > 0) theEnemy->face = 1;
					}

					limitFloat(&theEnemy->dx, 0 - theEnemy->speed, theEnemy->speed);
					limitFloat(&theEnemy->dy, 0 - theEnemy->speed, theEnemy->speed);

				}

				if (theEnemy->flags & FL_ALWAYSFACE)
				{
					theEnemy->face = 0;
					if (theEnemy->x > theEnemy->target->x) theEnemy->face = 1;
				}

				if ((currentGame.area == 18) && (theEnemy->classDef == CD_BOSS))
					theEnemy->face = 0;

				if ((theEnemy->flags & FL_DEPLOYDRONES) && ((rand() % 300) == 0))
					addDrone(theEnemy);

				if (theEnemy->flags & FL_LEAVESECTOR)
				{
					limitFloat(&(theEnemy->dx -= 0.5), 0, -15);
					theEnemy->dy = 0;
					theEnemy->thinktime = 999;
					theEnemy->face = 0;

					if (theEnemy->x >= 5000)
					{
						theEnemy->flags -= FL_LEAVESECTOR;
						theEnemy->flags += FL_ESCAPED;
						theEnemy->active = false;

						if (theEnemy->classDef == CD_CLOAKFIGHTER)
						{
							currentGame.experimentalShield = theEnemy->shield;
							setInfoLine("Experimental Fighter has fled", FONT_CYAN);
						}

						theEnemy->shield = 0;
						updateMissionRequirements(M_ESCAPE_TARGET, theEnemy->classDef, 1);
						
						if (theEnemy->classDef != CD_CLOAKFIGHTER)
							updateMissionRequirements(M_DESTROY_TARGET_TYPE, theEnemy->classDef, 1);
					}
				}

				/*
					This deals with the Experimental Fighter in Mordor (and Kline on the final mission)
					It can cloak and uncloak at random. When cloaked, it's sprite is
					not displayed. However the engine trail is still visible!
				*/
				if ((theEnemy->flags & FL_CANCLOAK) && ((rand() % 500) == 0))
				{
					if (theEnemy->flags & FL_ISCLOAKED)
						theEnemy->flags -= FL_ISCLOAKED;
					else
						theEnemy->flags += FL_ISCLOAKED;
					playSound(SFX_CLOAK);
				}

				// ------------ Barriers ------------------

				if (theEnemy->classDef == CD_BARRIER)
				{
					theEnemy->dx = -10 + (sinf(barrierLoop + theEnemy->speed) * 60);
					theEnemy->dy = 20 + (cosf(barrierLoop + theEnemy->speed) * 40);
				}

				// ----------------------------------------

				// ------------ Mobile Shields ------------

				if (theEnemy->classDef == CD_MOBILESHIELD)
				{
					limitInt(&(++enemy[WC_BOSS].shield), 0, enemy[WC_BOSS].maxShield);
				}

				// ----------------------------------------

				limitCharAdd(&theEnemy->reload[0], -1, 0, 999);
				limitCharAdd(&theEnemy->reload[1], -1, 0, 999);

    			if ((!(theEnemy->flags & FL_DISABLED)) && (!(theEnemy->flags & FL_NOFIRE)))
				{
					if ((theEnemy->target->shield > 0))
						canFire = traceTarget(theEnemy);

					if (((theEnemy->thinktime % 2) == 0) && (theEnemy->flags & FL_FRIEND))
						canFire = traceView(theEnemy);
				}
				else
				{
					canFire = false;
				}

				if ((canFire) && (dev.fireAliens))
				{
					if ((theEnemy->reload[0] == 0) && ((rand() % 1000 < theEnemy->chance[0]) || (theEnemy->flags & FL_CONTINUOUS_FIRE)))
					{
						fireBullet(theEnemy, 0);
					}
					if ((theEnemy->reload[1] == 0) && ((rand() % 1000 < theEnemy->chance[1]) || (theEnemy->flags & FL_CONTINUOUS_FIRE)))
					{
						if ((theEnemy->weaponType[1] != W_ENERGYRAY) && (theEnemy->weaponType[1] != W_LASER))
						{
							if (theEnemy->weaponType[1] == W_CHARGER)
								theEnemy->ammo[1] = 50 + rand() % 150;
							fireBullet(theEnemy, 1);
						}
						else if (theEnemy->weaponType[1] == W_LASER)
						{
							theEnemy->flags += FL_FIRELASER;
						}
						else if ((theEnemy->weaponType[1] == W_ENERGYRAY) && (theEnemy->ammo[0] == 250))
						{
							theEnemy->flags += FL_FIRERAY;
							playSound(SFX_ENERGYRAY);
						}
					}
				}

				// --------------- Ray specific stuff ------------------
				if (theEnemy->flags & FL_FIRERAY)
				{
					fireRay(theEnemy);
				}
				else
				{
					limitCharAdd(&theEnemy->ammo[0], 1, 0, 250);
				}
				// -------------------------------------------------------

				// --------------- Laser specific stuff ------------------
				if (theEnemy->flags & FL_FIRELASER)
				{
					fireBullet(theEnemy, 1);
					if ((rand() % 25) == 0)
						theEnemy->flags -= FL_FIRELASER;
				}
				// -------------------------------------------------------

				// ---------------- Mine specific stuff ------------------

				if (theEnemy->flags & FL_DROPMINES)
					if ((rand() % 150) == 0)
						addCollectable(theEnemy->x, theEnemy->y, P_MINE, 25, 600 + rand() % 2400);

						// Kline drops mines a lot more often
				if ((theEnemy->flags & FL_DROPMINES) && (theEnemy == &enemy[WC_KLINE]))
					if ((rand() % 10) == 0)
						addCollectable(theEnemy->x, theEnemy->y, P_MINE, 25, 600 + rand() % 2400);

				// -------------------------------------------------------

				shapeToUse = theEnemy->imageIndex[theEnemy->face];

				if (theEnemy->hit)
					shapeToUse += SHIP_HIT_INDEX;

				limitCharAdd(&theEnemy->hit, -1, 0, 100);

				if ((theEnemy->x + theEnemy->image[0]->w > 0) && (theEnemy->x < 800) && (theEnemy->y + theEnemy->image[0]->h > 0) && (theEnemy->y < 600))
				{
					if ((!(theEnemy->flags & FL_DISABLED)) && (theEnemy->classDef != CD_ASTEROID) && (theEnemy->classDef != CD_ASTEROID2))
						addEngine(theEnemy);
					if ((!(theEnemy->flags & FL_ISCLOAKED)) || (theEnemy->hit > 0))
						graphics.blit(graphics.shipShape[shapeToUse], (int)theEnemy->x, (int)theEnemy->y);
					if (theEnemy->flags & FL_DISABLED)
					{
						if ((rand() % 10) == 0)
							addExplosion(theEnemy->x + (rand() % theEnemy->image[0]->w), theEnemy->y + (rand() % theEnemy->image[0]->h), E_ELECTRICAL);
					}
				}

				if ((currentGame.area == 24) && (theEnemy->x < -300))
					theEnemy->active = false;
			}
			else
			{
				theEnemy->shield--;
				if ((theEnemy->x > 0) && (theEnemy->x < 800) && (theEnemy->y > 0) && (theEnemy->y < 600))
				{
					graphics.blit(theEnemy->image[theEnemy->face], (int)theEnemy->x, (int)theEnemy->y);
					addExplosion(theEnemy->x + (rand() % theEnemy->image[0]->w), theEnemy->y + (rand() % theEnemy->image[0]->h), E_BIG_EXPLOSION);
				}
				if (theEnemy->shield < theEnemy->deathCounter)
				{
					theEnemy->active = false;
					if ((theEnemy->classDef == CD_BOSS) || (theEnemy->owner == &enemy[WC_BOSS]) || (theEnemy->flags & FL_FRIEND) || (theEnemy->classDef == CD_ASTEROID) || (theEnemy->classDef == CD_KLINE))
						addDebris((int)theEnemy->x, (int)theEnemy->y, theEnemy->maxShield);

					if (theEnemy->classDef == CD_ASTEROID)
					{
						int i = 1 + (rand() % 3);
						for (int j = 0 ; j < i ; j++)
							addSmallAsteroid(theEnemy);
					}
				}
			}

			// Adjust the movement even whilst exploding
			if ((dev.moveAliens) && (!(theEnemy->flags & FL_NOMOVE)) && (!(theEnemy->flags & FL_DISABLED)))
			{
				moveAndSeparate(theEnemy);
			}

			if ((currentGame.area != 18) || (theEnemy->shield < 0))
				theEnemy->x += engine.ssx;

			theEnemy->y += engine.ssy;
		}

		theEnemy++;
	}
}

void setAlienShapes()
{
	for (int i = 0 ; i < MAX_DEFALIENS ; i++)
	{
		if (graphics.shipShape[defEnemy[i].imageIndex[0]] != NULL)
		{
			defEnemy[i].image[0] = graphics.shipShape[defEnemy[i].imageIndex[0]];
			defEnemy[i].image[1] = graphics.shipShape[defEnemy[i].imageIndex[1]];
			defEnemy[i].engineX = defEnemy[i].image[0]->w;
			defEnemy[i].engineY = (defEnemy[i].image[0]->h / 2);
		}
	}
}

#if USEPACK

static void loadAliens()
{
	int dataLocation = locateDataInPak("data/aliens.dat", 1);
	int def, ai, speed, shield, max, image1, image2, weapon1, weapon2, chance1, chance2, score;
	int collectChance, collectType, collectVal, flags;

	FILE *fp;

	fp = fopen(PACKLOCATION, "rb");
	fseek(fp, dataLocation, SEEK_SET);

	for (int i = 0 ; i < MAX_DEFALIENS ; i++)
	{
		fscanf(fp, "%d", &def);
		fscanf(fp, "%d", &ai);
		fscanf(fp, "%d", &speed);
		fscanf(fp, "%d", &shield);
		fscanf(fp, "%d", &max);
		fscanf(fp, "%d", &image1);
		fscanf(fp, "%d", &image2);
		fscanf(fp, "%d", &weapon1);
		fscanf(fp, "%d", &weapon2);
		fscanf(fp, "%d", &chance1);
		fscanf(fp, "%d", &chance2);
		fscanf(fp, "%d", &score);
		fscanf(fp, "%d", &collectChance);
		fscanf(fp, "%d", &collectType);
		fscanf(fp, "%d", &collectVal);
		fscanf(fp, "%d", &flags);

		defEnemy[i].classDef = def;
		defEnemy[i].AIType = ai;
		defEnemy[i].speed = speed;
		defEnemy[i].shield = shield;
		defEnemy[i].maxShield = max;
		defEnemy[i].imageIndex[0] = image1;
		defEnemy[i].imageIndex[1] = image2;
		defEnemy[i].weaponType[0] = weapon1;
		defEnemy[i].weaponType[1] = weapon2;
		defEnemy[i].chance[0] = chance1;
		defEnemy[i].chance[1] = chance2;
		defEnemy[i].score = score;
		defEnemy[i].collectChance = collectChance;
		defEnemy[i].collectType = collectType;
		defEnemy[i].collectValue = collectVal;
		defEnemy[i].flags = flags;
	}

	fclose(fp);
}

void defineAliens(){loadAliens();}

#else

static void saveAliens()
{
	FILE *fp;

	fp = fopen("data/aliens.dat", "wb");
	if (fp == NULL)
	{
		printf("Unable to write Alien Data File\n");
		exit(1);
	}

	for (int i = 0 ; i < MAX_DEFALIENS ; i++)
	{
		fprintf(fp, "%d ", defEnemy[i].classDef);
		fprintf(fp, "%d ", defEnemy[i].AIType);
		fprintf(fp, "%d ", defEnemy[i].speed);
		fprintf(fp, "%d ", defEnemy[i].shield);
		fprintf(fp, "%d ", defEnemy[i].maxShield);
		fprintf(fp, "%d ", defEnemy[i].imageIndex[0]);
		fprintf(fp, "%d ", defEnemy[i].imageIndex[1]);
		fprintf(fp, "%d ", defEnemy[i].weaponType[0]);
		fprintf(fp, "%d ", defEnemy[i].weaponType[1]);
		fprintf(fp, "%d ", defEnemy[i].chance[0]);
		fprintf(fp, "%d ", defEnemy[i].chance[1]);
		fprintf(fp, "%d ", defEnemy[i].score);
		fprintf(fp, "%d ", defEnemy[i].collectChance);
		fprintf(fp, "%d ", defEnemy[i].collectType);
		fprintf(fp, "%d ", defEnemy[i].collectValue);
		fprintf(fp, "%d\n", defEnemy[i].flags);
	}

	// Put an extra line for the PAK file "just in case"
	fprintf(fp, "\n");

	fclose(fp);
}

/*
Will be dumped into a data file at the end of the project
*/
void defineAliens()
{
	// Dual Plasma Fighter.
	defEnemy[CD_DUALFIGHTER].classDef = CD_DUALFIGHTER;
	defEnemy[CD_DUALFIGHTER].AIType = AI_NORMAL;
	defEnemy[CD_DUALFIGHTER].speed = 4;
	defEnemy[CD_DUALFIGHTER].maxShield = 5;
	defEnemy[CD_DUALFIGHTER].shield = 5;
	defEnemy[CD_DUALFIGHTER].imageIndex[0] = 2;
	defEnemy[CD_DUALFIGHTER].imageIndex[1] = 3;
	defEnemy[CD_DUALFIGHTER].weaponType[0] = W_DOUBLE_SHOT;
	defEnemy[CD_DUALFIGHTER].weaponType[1] = W_ROCKETS;
	defEnemy[CD_DUALFIGHTER].chance[0] = 100;
	defEnemy[CD_DUALFIGHTER].chance[1] = 1;
	defEnemy[CD_DUALFIGHTER].score = 25;
	defEnemy[CD_DUALFIGHTER].collectChance = 50;
	defEnemy[CD_DUALFIGHTER].collectType = P_ANYTHING;
	defEnemy[CD_DUALFIGHTER].collectValue = 50;
	defEnemy[CD_DUALFIGHTER].flags = FL_WEAPCO;

	// Missile Boat
	defEnemy[CD_MISSILEBOAT].classDef = CD_MISSILEBOAT;
	defEnemy[CD_MISSILEBOAT].AIType = AI_DEFENSIVE;
	defEnemy[CD_MISSILEBOAT].speed = 2;
	defEnemy[CD_MISSILEBOAT].maxShield = 50;
	defEnemy[CD_MISSILEBOAT].shield = 50;
	defEnemy[CD_MISSILEBOAT].imageIndex[0] = 4;
	defEnemy[CD_MISSILEBOAT].imageIndex[1] = 5;
	defEnemy[CD_MISSILEBOAT].weaponType[0] = W_ROCKETS;
	defEnemy[CD_MISSILEBOAT].weaponType[1] = W_DOUBLE_ROCKETS;
	defEnemy[CD_MISSILEBOAT].chance[0] = 25;
	defEnemy[CD_MISSILEBOAT].chance[1] = 4;
	defEnemy[CD_MISSILEBOAT].score = 250;
	defEnemy[CD_MISSILEBOAT].collectChance = 25;
	defEnemy[CD_MISSILEBOAT].collectType = P_ANYTHING;
	defEnemy[CD_MISSILEBOAT].collectValue = 75;
	defEnemy[CD_MISSILEBOAT].flags = FL_WEAPCO;

	//Prototype fighter
	defEnemy[CD_PROTOFIGHTER].classDef = CD_PROTOFIGHTER;
	defEnemy[CD_PROTOFIGHTER].AIType = AI_DEFENSIVE;
	defEnemy[CD_PROTOFIGHTER].speed = 5;
	defEnemy[CD_PROTOFIGHTER].maxShield = 15;
	defEnemy[CD_PROTOFIGHTER].shield = 15;
	defEnemy[CD_PROTOFIGHTER].imageIndex[0] = 6;
	defEnemy[CD_PROTOFIGHTER].imageIndex[1] = 7;
	defEnemy[CD_PROTOFIGHTER].weaponType[0] = W_TRIPLE_SHOT;
	defEnemy[CD_PROTOFIGHTER].weaponType[1] = P_ANYTHING;
	defEnemy[CD_PROTOFIGHTER].chance[0] = 100;
	defEnemy[CD_PROTOFIGHTER].chance[1] = 1;
	defEnemy[CD_PROTOFIGHTER].score = 50;
	defEnemy[CD_PROTOFIGHTER].collectChance = 50;
	defEnemy[CD_PROTOFIGHTER].collectType = P_ANYTHING;
	defEnemy[CD_PROTOFIGHTER].collectValue = 50;
	defEnemy[CD_PROTOFIGHTER].flags = FL_WEAPCO;

	// Phoebe and Ursula
	defEnemy[CD_FRIEND].classDef = CD_FRIEND;
	defEnemy[CD_FRIEND].AIType = AI_OFFENSIVE;
	defEnemy[CD_FRIEND].speed = 3;
	defEnemy[CD_FRIEND].maxShield = 50;
	defEnemy[CD_FRIEND].shield = 50;
	defEnemy[CD_FRIEND].imageIndex[0] = 20;
	defEnemy[CD_FRIEND].imageIndex[1] = 21;
	defEnemy[CD_FRIEND].weaponType[0] = W_DOUBLE_SHOT;
	defEnemy[CD_FRIEND].weaponType[1] = W_HOMING_MISSILE;
	defEnemy[CD_FRIEND].chance[0] = 100;
	defEnemy[CD_FRIEND].chance[1] = 5;
	defEnemy[CD_FRIEND].score = 0;
	defEnemy[CD_FRIEND].collectChance = 0;
	defEnemy[CD_FRIEND].collectType = P_CASH;
	defEnemy[CD_FRIEND].collectValue = 0;
	defEnemy[CD_FRIEND].flags = FL_FRIEND;

	// Boss 1
	defEnemy[CD_FRIGATE].classDef = CD_BOSS;
	defEnemy[CD_FRIGATE].AIType = AI_NORMAL;
	defEnemy[CD_FRIGATE].speed = 2;
	defEnemy[CD_FRIGATE].maxShield = 550;
	defEnemy[CD_FRIGATE].shield = 550;
	defEnemy[CD_FRIGATE].imageIndex[0] = 8;
	defEnemy[CD_FRIGATE].imageIndex[1] = 9;
	defEnemy[CD_FRIGATE].weaponType[0] = W_MICRO_ROCKETS;
	defEnemy[CD_FRIGATE].weaponType[1] = W_ENERGYRAY;
	defEnemy[CD_FRIGATE].chance[0] = 100;
	defEnemy[CD_FRIGATE].chance[1] = 85;
	defEnemy[CD_FRIGATE].score = 500;
	defEnemy[CD_FRIGATE].collectChance = 100;
	defEnemy[CD_FRIGATE].collectType = P_CASH;
	defEnemy[CD_FRIGATE].collectValue = 250;
	defEnemy[CD_FRIGATE].flags = FL_WEAPCO;

	defEnemy[CD_FRIGATE_WING1].classDef = CD_FRIGATE_WING1;
	defEnemy[CD_FRIGATE_WING1].AIType = AI_NORMAL;
	defEnemy[CD_FRIGATE_WING1].speed = 2;
	defEnemy[CD_FRIGATE_WING1].maxShield = 100;
	defEnemy[CD_FRIGATE_WING1].shield = 100;
	defEnemy[CD_FRIGATE_WING1].imageIndex[0] = 10;
	defEnemy[CD_FRIGATE_WING1].imageIndex[1] = 11;
	defEnemy[CD_FRIGATE_WING1].weaponType[0] = W_TRIPLE_SHOT;
	defEnemy[CD_FRIGATE_WING1].weaponType[1] = W_ROCKETS;
	defEnemy[CD_FRIGATE_WING1].chance[0] = 100;
	defEnemy[CD_FRIGATE_WING1].chance[1] = 10;
	defEnemy[CD_FRIGATE_WING1].score = 500;
	defEnemy[CD_FRIGATE_WING1].collectChance = 100;
	defEnemy[CD_FRIGATE_WING1].collectType = P_ANYTHING;
	defEnemy[CD_FRIGATE_WING1].collectValue = 250;
	defEnemy[CD_FRIGATE_WING1].flags = FL_WEAPCO + FL_DAMAGEOWNER;

	defEnemy[CD_FRIGATE_WING2].classDef = CD_FRIGATE_WING2;
	defEnemy[CD_FRIGATE_WING2].AIType = AI_NORMAL;
	defEnemy[CD_FRIGATE_WING2].speed = 2;
	defEnemy[CD_FRIGATE_WING2].maxShield = 100;
	defEnemy[CD_FRIGATE_WING2].shield = 100;
	defEnemy[CD_FRIGATE_WING2].imageIndex[0] = 12;
	defEnemy[CD_FRIGATE_WING2].imageIndex[1] = 13;
	defEnemy[CD_FRIGATE_WING2].weaponType[0] = W_TRIPLE_SHOT;
	defEnemy[CD_FRIGATE_WING2].weaponType[1] = W_ROCKETS;
	defEnemy[CD_FRIGATE_WING2].chance[0] = 100;
	defEnemy[CD_FRIGATE_WING2].chance[1] = 10;
	defEnemy[CD_FRIGATE_WING2].score = 500;
	defEnemy[CD_FRIGATE_WING2].collectChance = 100;
	defEnemy[CD_FRIGATE_WING2].collectType = P_ANYTHING;
	defEnemy[CD_FRIGATE_WING2].collectValue = 250;
	defEnemy[CD_FRIGATE_WING2].flags = FL_WEAPCO + FL_DAMAGEOWNER;

	// Transport ship
	defEnemy[CD_TRANSPORTSHIP].classDef = CD_TRANSPORTSHIP;
	defEnemy[CD_TRANSPORTSHIP].AIType = AI_EVASIVE;
	defEnemy[CD_TRANSPORTSHIP].speed = 4;
	defEnemy[CD_TRANSPORTSHIP].maxShield = 10;
	defEnemy[CD_TRANSPORTSHIP].shield = 10;
	defEnemy[CD_TRANSPORTSHIP].imageIndex[0] = 14;
	defEnemy[CD_TRANSPORTSHIP].imageIndex[1] = 15;
	defEnemy[CD_TRANSPORTSHIP].weaponType[0] = W_DOUBLE_SHOT;
	defEnemy[CD_TRANSPORTSHIP].weaponType[1] = W_DOUBLE_SHOT;
	defEnemy[CD_TRANSPORTSHIP].chance[0] = 0;
	defEnemy[CD_TRANSPORTSHIP].chance[1] = 0;
	defEnemy[CD_TRANSPORTSHIP].score = 25;
	defEnemy[CD_TRANSPORTSHIP].collectChance = 100;
	defEnemy[CD_TRANSPORTSHIP].collectType = P_WEAPONS;
	defEnemy[CD_TRANSPORTSHIP].collectValue = 30;
	defEnemy[CD_TRANSPORTSHIP].flags = FL_WEAPCO + FL_NOFIRE;

	// Cargo ship
	defEnemy[CD_CARGOSHIP].classDef = CD_CARGOSHIP;
	defEnemy[CD_CARGOSHIP].AIType = AI_EVASIVE;
	defEnemy[CD_CARGOSHIP].speed = 4;
	defEnemy[CD_CARGOSHIP].maxShield = 10;
	defEnemy[CD_CARGOSHIP].shield = 10;
	defEnemy[CD_CARGOSHIP].imageIndex[0] = 22;
	defEnemy[CD_CARGOSHIP].imageIndex[1] = 23;
	defEnemy[CD_CARGOSHIP].weaponType[0] = W_DOUBLE_SHOT;
	defEnemy[CD_CARGOSHIP].weaponType[1] = W_DOUBLE_SHOT;
	defEnemy[CD_CARGOSHIP].chance[0] = 0;
	defEnemy[CD_CARGOSHIP].chance[1] = 0;
	defEnemy[CD_CARGOSHIP].score = 25;
	defEnemy[CD_CARGOSHIP].collectChance = 50;
	defEnemy[CD_CARGOSHIP].collectType = P_ANYTHING;
	defEnemy[CD_CARGOSHIP].collectValue = 100;
	defEnemy[CD_CARGOSHIP].flags = FL_WEAPCO + FL_NOFIRE;

	// Weapco Miner
	defEnemy[CD_MINER].classDef = CD_MINER;
	defEnemy[CD_MINER].AIType = AI_EVASIVE;
	defEnemy[CD_MINER].speed = 4;
	defEnemy[CD_MINER].maxShield = 25;
	defEnemy[CD_MINER].shield = 25;
	defEnemy[CD_MINER].imageIndex[0] = 16;
	defEnemy[CD_MINER].imageIndex[1] = 17;
	defEnemy[CD_MINER].weaponType[0] = W_DOUBLE_SHOT;
	defEnemy[CD_MINER].weaponType[1] = W_DOUBLE_SHOT;
	defEnemy[CD_MINER].chance[0] = 0;
	defEnemy[CD_MINER].chance[1] = 0;
	defEnemy[CD_MINER].score = 100;
	defEnemy[CD_MINER].collectChance = 100;
	defEnemy[CD_MINER].collectType = P_ANYTHING;
	defEnemy[CD_MINER].collectValue = 30;
	defEnemy[CD_MINER].flags = FL_WEAPCO + FL_NOFIRE + FL_DROPMINES;

	// Kline
	defEnemy[CD_KLINE].classDef = CD_KLINE;
	defEnemy[CD_KLINE].AIType = AI_OFFENSIVE;
	defEnemy[CD_KLINE].speed = 5;
	defEnemy[CD_KLINE].maxShield = 750;
	defEnemy[CD_KLINE].shield = 750;
	defEnemy[CD_KLINE].imageIndex[0] = 18;
	defEnemy[CD_KLINE].imageIndex[1] = 19;
	defEnemy[CD_KLINE].weaponType[0] = W_TRIPLE_SHOT;
	defEnemy[CD_KLINE].weaponType[1] = W_MICRO_ROCKETS;
	defEnemy[CD_KLINE].chance[0] = 100;
	defEnemy[CD_KLINE].chance[1] = 2;
	defEnemy[CD_KLINE].score = 0;
	defEnemy[CD_KLINE].collectChance = 0;
	defEnemy[CD_KLINE].collectType = P_ANYTHING;
	defEnemy[CD_KLINE].collectValue = 0;
	defEnemy[CD_KLINE].flags = FL_WEAPCO + FL_CANNOTDIE + FL_ALWAYSFACE + FL_CIRCLES;

	// Aim Fighter
	defEnemy[CD_AIMFIGHTER].classDef = CD_AIMFIGHTER;
	defEnemy[CD_AIMFIGHTER].AIType = AI_NORMAL;
	defEnemy[CD_AIMFIGHTER].speed = 3;
	defEnemy[CD_AIMFIGHTER].maxShield = 15;
	defEnemy[CD_AIMFIGHTER].shield = 15;
	defEnemy[CD_AIMFIGHTER].imageIndex[0] = 8;
	defEnemy[CD_AIMFIGHTER].imageIndex[1] = 9;
	defEnemy[CD_AIMFIGHTER].weaponType[0] = W_AIMED_SHOT;
	defEnemy[CD_AIMFIGHTER].weaponType[1] = W_AIMED_SHOT;
	defEnemy[CD_AIMFIGHTER].chance[0] = 7;
	defEnemy[CD_AIMFIGHTER].chance[1] = 1;
	defEnemy[CD_AIMFIGHTER].score = 50;
	defEnemy[CD_AIMFIGHTER].collectChance = 75;
	defEnemy[CD_AIMFIGHTER].collectType = P_ANYTHING;
	defEnemy[CD_AIMFIGHTER].collectValue = 100;
	defEnemy[CD_AIMFIGHTER].flags = FL_WEAPCO + FL_AIMS;

	// Slave ship
	defEnemy[CD_SLAVETRANSPORT].classDef = CD_SLAVETRANSPORT;
	defEnemy[CD_SLAVETRANSPORT].AIType = AI_EVASIVE;
	defEnemy[CD_SLAVETRANSPORT].speed = 2;
	defEnemy[CD_SLAVETRANSPORT].maxShield = 10;
	defEnemy[CD_SLAVETRANSPORT].shield = 20;
	defEnemy[CD_SLAVETRANSPORT].imageIndex[0] = 10;
	defEnemy[CD_SLAVETRANSPORT].imageIndex[1] = 11;
	defEnemy[CD_SLAVETRANSPORT].weaponType[0] = W_DOUBLE_SHOT;
	defEnemy[CD_SLAVETRANSPORT].weaponType[1] = W_DOUBLE_SHOT;
	defEnemy[CD_SLAVETRANSPORT].chance[0] = 0;
	defEnemy[CD_SLAVETRANSPORT].chance[1] = 0;
	defEnemy[CD_SLAVETRANSPORT].score = 25;
	defEnemy[CD_SLAVETRANSPORT].collectChance = 100;
	defEnemy[CD_SLAVETRANSPORT].collectType = P_SLAVES;
	defEnemy[CD_SLAVETRANSPORT].collectValue = 25;
	defEnemy[CD_SLAVETRANSPORT].flags = FL_WEAPCO + FL_NOFIRE;

	// Good Transport
	defEnemy[CD_GOODTRANSPORT].classDef = CD_GOODTRANSPORT;
	defEnemy[CD_GOODTRANSPORT].AIType = AI_EVASIVE;
	defEnemy[CD_GOODTRANSPORT].speed = 3;
	defEnemy[CD_GOODTRANSPORT].maxShield = 75;
	defEnemy[CD_GOODTRANSPORT].shield = 75;
	defEnemy[CD_GOODTRANSPORT].imageIndex[0] = 12;
	defEnemy[CD_GOODTRANSPORT].imageIndex[1] = 13;
	defEnemy[CD_GOODTRANSPORT].weaponType[0] = W_AIMED_SHOT;
	defEnemy[CD_GOODTRANSPORT].weaponType[1] = W_AIMED_SHOT;
	defEnemy[CD_GOODTRANSPORT].chance[0] = 100;
	defEnemy[CD_GOODTRANSPORT].chance[1] = 100;
	defEnemy[CD_GOODTRANSPORT].score = 0;
	defEnemy[CD_GOODTRANSPORT].collectChance = 0;
	defEnemy[CD_GOODTRANSPORT].collectType = P_ANYTHING;
	defEnemy[CD_GOODTRANSPORT].collectValue = 0;
	defEnemy[CD_GOODTRANSPORT].flags = FL_FRIEND + FL_NOFIRE + FL_AIMS;

	// Sid Wilson
	defEnemy[CD_SID].classDef = CD_SID;
	defEnemy[CD_SID].AIType = AI_NORMAL;
	defEnemy[CD_SID].speed = 3;
	defEnemy[CD_SID].maxShield = 50;
	defEnemy[CD_SID].shield = 50;
	defEnemy[CD_SID].imageIndex[0] = 24;
	defEnemy[CD_SID].imageIndex[1] = 25;
	defEnemy[CD_SID].weaponType[0] = W_IONCANNON;
	defEnemy[CD_SID].weaponType[1] = W_IONCANNON;
	defEnemy[CD_SID].chance[0] = 100;
	defEnemy[CD_SID].chance[1] = 0;
	defEnemy[CD_SID].score = 0;
	defEnemy[CD_SID].collectChance = 0;
	defEnemy[CD_SID].collectType = P_ANYTHING;
	defEnemy[CD_SID].collectValue = 0;
	defEnemy[CD_SID].flags = FL_FRIEND + FL_AIMS;

	// Mining Vessel Boss
	defEnemy[CD_MINEBOSS].classDef = CD_BOSS;
	defEnemy[CD_MINEBOSS].AIType = AI_NORMAL;
	defEnemy[CD_MINEBOSS].speed = 3;
	defEnemy[CD_MINEBOSS].maxShield = 1000;
	defEnemy[CD_MINEBOSS].shield = 1000;
	defEnemy[CD_MINEBOSS].imageIndex[0] = 26;
	defEnemy[CD_MINEBOSS].imageIndex[1] = 27;
	defEnemy[CD_MINEBOSS].weaponType[0] = W_TRIPLE_SHOT;
	defEnemy[CD_MINEBOSS].weaponType[1] = W_SPREADSHOT;
	defEnemy[CD_MINEBOSS].chance[0] = 0;
	defEnemy[CD_MINEBOSS].chance[1] = 0;
	defEnemy[CD_MINEBOSS].score = 1000;
	defEnemy[CD_MINEBOSS].collectChance = 100;
	defEnemy[CD_MINEBOSS].collectType = P_ANYTHING;
	defEnemy[CD_MINEBOSS].collectValue = 255;
	defEnemy[CD_MINEBOSS].flags = FL_WEAPCO + FL_IMMORTAL;

	defEnemy[CD_BOSS2_WING1].classDef = CD_BOSS2_WING1;
	defEnemy[CD_BOSS2_WING1].AIType = AI_NORMAL;
	defEnemy[CD_BOSS2_WING1].speed = 1;
	defEnemy[CD_BOSS2_WING1].maxShield = 250;
	defEnemy[CD_BOSS2_WING1].shield = 250;
	defEnemy[CD_BOSS2_WING1].imageIndex[0] = 28;
	defEnemy[CD_BOSS2_WING1].imageIndex[1] = 29;
	defEnemy[CD_BOSS2_WING1].weaponType[0] = W_TRIPLE_SHOT;
	defEnemy[CD_BOSS2_WING1].weaponType[1] = W_SPREADSHOT;
	defEnemy[CD_BOSS2_WING1].chance[0] = 0;
	defEnemy[CD_BOSS2_WING1].chance[1] = 0;
	defEnemy[CD_BOSS2_WING1].score = 1000;
	defEnemy[CD_BOSS2_WING1].collectChance = 100;
	defEnemy[CD_BOSS2_WING1].collectType = P_ANYTHING;
	defEnemy[CD_BOSS2_WING1].collectValue = 255;
	defEnemy[CD_BOSS2_WING1].flags = FL_WEAPCO + FL_DAMAGEOWNER;

	defEnemy[CD_BOSS2_WING2].classDef = CD_BOSS2_WING2;
	defEnemy[CD_BOSS2_WING2].AIType = AI_NORMAL;
	defEnemy[CD_BOSS2_WING2].speed = 1;
	defEnemy[CD_BOSS2_WING2].maxShield = 500;
	defEnemy[CD_BOSS2_WING2].shield = 500;
	defEnemy[CD_BOSS2_WING2].imageIndex[0] = 30;
	defEnemy[CD_BOSS2_WING2].imageIndex[1] = 31;
	defEnemy[CD_BOSS2_WING2].weaponType[0] = W_TRIPLE_SHOT;
	defEnemy[CD_BOSS2_WING2].weaponType[1] = W_SPREADSHOT;
	defEnemy[CD_BOSS2_WING2].chance[0] = 0;
	defEnemy[CD_BOSS2_WING2].chance[1] = 0;
	defEnemy[CD_BOSS2_WING2].score = 1000;
	defEnemy[CD_BOSS2_WING2].collectChance = 100;
	defEnemy[CD_BOSS2_WING2].collectType = P_ANYTHING;
	defEnemy[CD_BOSS2_WING2].collectValue = 255;
	defEnemy[CD_BOSS2_WING2].flags = FL_WEAPCO + FL_DEPLOYDRONES + FL_DAMAGEOWNER;

	defEnemy[CD_BOSS2_WING3].classDef = CD_BOSS2_WING3;
	defEnemy[CD_BOSS2_WING3].AIType = AI_NORMAL;
	defEnemy[CD_BOSS2_WING3].speed = 1;
	defEnemy[CD_BOSS2_WING3].maxShield = 500;
	defEnemy[CD_BOSS2_WING3].shield = 500;
	defEnemy[CD_BOSS2_WING3].imageIndex[0] = 32;
	defEnemy[CD_BOSS2_WING3].imageIndex[1] = 33;
	defEnemy[CD_BOSS2_WING3].weaponType[0] = W_TRIPLE_SHOT;
	defEnemy[CD_BOSS2_WING3].weaponType[1] = W_SPREADSHOT;
	defEnemy[CD_BOSS2_WING3].chance[0] = 0;
	defEnemy[CD_BOSS2_WING3].chance[1] = 0;
	defEnemy[CD_BOSS2_WING3].score = 1000;
	defEnemy[CD_BOSS2_WING3].collectChance = 100;
	defEnemy[CD_BOSS2_WING3].collectType = P_ANYTHING;
	defEnemy[CD_BOSS2_WING3].collectValue = 255;
	defEnemy[CD_BOSS2_WING3].flags = FL_WEAPCO + FL_DEPLOYDRONES + FL_DAMAGEOWNER;

	defEnemy[CD_BOSS2_WING4].classDef = CD_BOSS2_WING4;
	defEnemy[CD_BOSS2_WING4].AIType = AI_NORMAL;
	defEnemy[CD_BOSS2_WING4].speed = 1;
	defEnemy[CD_BOSS2_WING4].maxShield = 250;
	defEnemy[CD_BOSS2_WING4].shield = 250;
	defEnemy[CD_BOSS2_WING4].imageIndex[0] = 34;
	defEnemy[CD_BOSS2_WING4].imageIndex[1] = 35;
	defEnemy[CD_BOSS2_WING4].weaponType[0] = W_TRIPLE_SHOT;
	defEnemy[CD_BOSS2_WING4].weaponType[1] = W_SPREADSHOT;
	defEnemy[CD_BOSS2_WING4].chance[0] = 0;
	defEnemy[CD_BOSS2_WING4].chance[1] = 0;
	defEnemy[CD_BOSS2_WING4].score = 1000;
	defEnemy[CD_BOSS2_WING4].collectChance = 100;
	defEnemy[CD_BOSS2_WING4].collectType = P_ANYTHING;
	defEnemy[CD_BOSS2_WING4].collectValue = 255;
	defEnemy[CD_BOSS2_WING4].flags = FL_WEAPCO + FL_DAMAGEOWNER;

	// Drone
	defEnemy[CD_DRONE].classDef = CD_DRONE;
	defEnemy[CD_DRONE].AIType = AI_OFFENSIVE;
	defEnemy[CD_DRONE].speed = 8;
	defEnemy[CD_DRONE].maxShield = 5;
	defEnemy[CD_DRONE].shield = 5;
	defEnemy[CD_DRONE].imageIndex[0] = 36;
	defEnemy[CD_DRONE].imageIndex[1] = 37;
	defEnemy[CD_DRONE].weaponType[0] = W_DOUBLE_SHOT;
	defEnemy[CD_DRONE].weaponType[1] = W_LASER;
	defEnemy[CD_DRONE].chance[0] = 100;
	defEnemy[CD_DRONE].chance[1] = 0;
	defEnemy[CD_DRONE].score = 0;
	defEnemy[CD_DRONE].collectChance = 10;
	defEnemy[CD_DRONE].collectType = P_SHIELD;
	defEnemy[CD_DRONE].collectValue = 1;
	defEnemy[CD_DRONE].flags = FL_WEAPCO;

	// Experimental Fighter
	defEnemy[CD_CLOAKFIGHTER].classDef = CD_CLOAKFIGHTER;
	defEnemy[CD_CLOAKFIGHTER].AIType = AI_OFFENSIVE;
	defEnemy[CD_CLOAKFIGHTER].speed = 6;
	defEnemy[CD_CLOAKFIGHTER].maxShield = 1000;
	defEnemy[CD_CLOAKFIGHTER].shield = 1000;
	defEnemy[CD_CLOAKFIGHTER].imageIndex[0] = 10;
	defEnemy[CD_CLOAKFIGHTER].imageIndex[1] = 11;
	defEnemy[CD_CLOAKFIGHTER].weaponType[0] = W_SPREADSHOT;
	defEnemy[CD_CLOAKFIGHTER].weaponType[1] = W_DOUBLE_ROCKETS;
	defEnemy[CD_CLOAKFIGHTER].chance[0] = 100;
	defEnemy[CD_CLOAKFIGHTER].chance[1] = 5;
	defEnemy[CD_CLOAKFIGHTER].score = 550;
	defEnemy[CD_CLOAKFIGHTER].collectChance = 100;
	defEnemy[CD_CLOAKFIGHTER].collectType = P_CASH;
	defEnemy[CD_CLOAKFIGHTER].collectValue = 255;
	defEnemy[CD_CLOAKFIGHTER].flags = FL_WEAPCO + FL_CANCLOAK + FL_RUNSAWAY;

	// Evil Ursula
	defEnemy[CD_EVILURSULA].classDef = CD_EVILURSULA;
	defEnemy[CD_EVILURSULA].AIType = AI_OFFENSIVE;
	defEnemy[CD_EVILURSULA].speed = 5;
	defEnemy[CD_EVILURSULA].maxShield = 500;
	defEnemy[CD_EVILURSULA].shield = 500;
	defEnemy[CD_EVILURSULA].imageIndex[0] = 12;
	defEnemy[CD_EVILURSULA].imageIndex[1] = 13;
	defEnemy[CD_EVILURSULA].weaponType[0] = W_TRIPLE_SHOT;
	defEnemy[CD_EVILURSULA].weaponType[1] = W_HOMING_MISSILE;
	defEnemy[CD_EVILURSULA].chance[0] = 100;
	defEnemy[CD_EVILURSULA].chance[1] = 100;
	defEnemy[CD_EVILURSULA].score = 500;
	defEnemy[CD_EVILURSULA].collectChance = 100;
	defEnemy[CD_EVILURSULA].collectType = P_ESCAPEPOD;
	defEnemy[CD_EVILURSULA].collectValue = 1;
	defEnemy[CD_EVILURSULA].flags = FL_WEAPCO;

	// Mercenary
	defEnemy[CD_KRASS].classDef = CD_KRASS;
	defEnemy[CD_KRASS].AIType = AI_OFFENSIVE;
	defEnemy[CD_KRASS].speed = 5;
	defEnemy[CD_KRASS].maxShield = 1000;
	defEnemy[CD_KRASS].shield = 1000;
	defEnemy[CD_KRASS].imageIndex[0] = 26;
	defEnemy[CD_KRASS].imageIndex[1] = 27;
	defEnemy[CD_KRASS].weaponType[0] = W_SPREADSHOT;
	defEnemy[CD_KRASS].weaponType[1] = W_CHARGER;
	defEnemy[CD_KRASS].chance[0] = 100;
	defEnemy[CD_KRASS].chance[1] = 0;
	defEnemy[CD_KRASS].score = 2000;
	defEnemy[CD_KRASS].collectChance = 100;
	defEnemy[CD_KRASS].collectType = P_ANYTHING;
	defEnemy[CD_KRASS].collectValue = 255;
	defEnemy[CD_KRASS].flags = FL_FRIEND + FL_IMMORTAL;

	// Executive Transport
	defEnemy[CD_EXEC].classDef = CD_BOSS;
	defEnemy[CD_EXEC].AIType = AI_NORMAL;
	defEnemy[CD_EXEC].speed = 5;
	defEnemy[CD_EXEC].maxShield = 1000;
	defEnemy[CD_EXEC].shield = 1000;
	defEnemy[CD_EXEC].imageIndex[0] = 28;
	defEnemy[CD_EXEC].imageIndex[1] = 28;
	defEnemy[CD_EXEC].weaponType[0] = W_SPREADSHOT;
	defEnemy[CD_EXEC].weaponType[1] = W_HOMING_MISSILE;
	defEnemy[CD_EXEC].chance[0] = 0;
	defEnemy[CD_EXEC].chance[1] = 0;
	defEnemy[CD_EXEC].score = 2000;
	defEnemy[CD_EXEC].collectChance = 0;
	defEnemy[CD_EXEC].collectType = P_ANYTHING;
	defEnemy[CD_EXEC].collectValue = 0;
	defEnemy[CD_EXEC].flags = FL_WEAPCO + FL_NOFIRE;

	// Asteroid
	defEnemy[CD_ASTEROID].classDef = CD_ASTEROID;
	defEnemy[CD_ASTEROID].AIType = AI_WANDER;
	defEnemy[CD_ASTEROID].speed = 1;
	defEnemy[CD_ASTEROID].maxShield = 50;
	defEnemy[CD_ASTEROID].shield = 50;
	defEnemy[CD_ASTEROID].imageIndex[0] = 38;
	defEnemy[CD_ASTEROID].imageIndex[1] = 38;
	defEnemy[CD_ASTEROID].weaponType[0] = W_SPREADSHOT;
	defEnemy[CD_ASTEROID].weaponType[1] = W_HOMING_MISSILE;
	defEnemy[CD_ASTEROID].chance[0] = 0;
	defEnemy[CD_ASTEROID].chance[1] = 0;
	defEnemy[CD_ASTEROID].score = 0;
	defEnemy[CD_ASTEROID].collectChance = 25;
	defEnemy[CD_ASTEROID].collectType = P_ORE;
	defEnemy[CD_ASTEROID].collectValue = 1;
	defEnemy[CD_ASTEROID].flags = FL_WEAPCO;

	defEnemy[CD_ASTEROID2].classDef = CD_ASTEROID2;
	defEnemy[CD_ASTEROID2].AIType = AI_WANDER;
	defEnemy[CD_ASTEROID2].speed = 1;
	defEnemy[CD_ASTEROID2].maxShield = 10;
	defEnemy[CD_ASTEROID2].shield = 10;
	defEnemy[CD_ASTEROID2].imageIndex[0] = 39;
	defEnemy[CD_ASTEROID2].imageIndex[1] = 40;
	defEnemy[CD_ASTEROID2].weaponType[0] = W_SPREADSHOT;
	defEnemy[CD_ASTEROID2].weaponType[1] = W_HOMING_MISSILE;
	defEnemy[CD_ASTEROID2].chance[0] = 0;
	defEnemy[CD_ASTEROID2].chance[1] = 0;
	defEnemy[CD_ASTEROID2].score = 0;
	defEnemy[CD_ASTEROID2].collectChance = 25;
	defEnemy[CD_ASTEROID2].collectType = P_ORE;
	defEnemy[CD_ASTEROID2].collectValue = 1;
	defEnemy[CD_ASTEROID2].flags = FL_WEAPCO;

	// Escort
	defEnemy[CD_ESCORT].classDef = CD_ESCORT;
	defEnemy[CD_ESCORT].AIType = AI_NORMAL;
	defEnemy[CD_ESCORT].speed = 3;
	defEnemy[CD_ESCORT].maxShield = 200;
	defEnemy[CD_ESCORT].shield = 200;
	defEnemy[CD_ESCORT].imageIndex[0] = 30;
	defEnemy[CD_ESCORT].imageIndex[1] = 31;
	defEnemy[CD_ESCORT].weaponType[0] = W_LASER;
	defEnemy[CD_ESCORT].weaponType[1] = W_LASER;
	defEnemy[CD_ESCORT].chance[0] = 25;
	defEnemy[CD_ESCORT].chance[1] = 25;
	defEnemy[CD_ESCORT].score = 450;
	defEnemy[CD_ESCORT].collectChance = 100;
	defEnemy[CD_ESCORT].collectType = P_ANYTHING;
	defEnemy[CD_ESCORT].collectValue = 100;
	defEnemy[CD_ESCORT].flags = FL_WEAPCO;

	// Mobile Ray Cannon
	defEnemy[CD_MOBILE_RAY].classDef = CD_MOBILE_RAY;
	defEnemy[CD_MOBILE_RAY].AIType = AI_OFFENSIVE;
	defEnemy[CD_MOBILE_RAY].speed = 5;
	defEnemy[CD_MOBILE_RAY].maxShield = 250;
	defEnemy[CD_MOBILE_RAY].shield = 250;
	defEnemy[CD_MOBILE_RAY].imageIndex[0] = 10;
	defEnemy[CD_MOBILE_RAY].imageIndex[1] = 11;
	defEnemy[CD_MOBILE_RAY].weaponType[0] = W_ENERGYRAY;
	defEnemy[CD_MOBILE_RAY].weaponType[1] = W_ENERGYRAY;
	defEnemy[CD_MOBILE_RAY].chance[0] = 50;
	defEnemy[CD_MOBILE_RAY].chance[1] = 50;
	defEnemy[CD_MOBILE_RAY].score = 1000;
	defEnemy[CD_MOBILE_RAY].collectChance = 75;
	defEnemy[CD_MOBILE_RAY].collectType = P_SHIELD;
	defEnemy[CD_MOBILE_RAY].collectValue = 2;
	defEnemy[CD_MOBILE_RAY].flags = FL_WEAPCO;

	// Rebel Carrier
	defEnemy[CD_REBELCARRIER].classDef = CD_REBELCARRIER;
	defEnemy[CD_REBELCARRIER].AIType = AI_OFFENSIVE;
	defEnemy[CD_REBELCARRIER].speed = 2;
	defEnemy[CD_REBELCARRIER].maxShield = 100;
	defEnemy[CD_REBELCARRIER].shield = 100;
	defEnemy[CD_REBELCARRIER].imageIndex[0] = 32;
	defEnemy[CD_REBELCARRIER].imageIndex[1] = 33;
	defEnemy[CD_REBELCARRIER].weaponType[0] = W_DOUBLE_ROCKETS;
	defEnemy[CD_REBELCARRIER].weaponType[1] = W_MICRO_ROCKETS;
	defEnemy[CD_REBELCARRIER].chance[0] = 50;
	defEnemy[CD_REBELCARRIER].chance[1] = 2;
	defEnemy[CD_REBELCARRIER].score = 0;
	defEnemy[CD_REBELCARRIER].collectChance = 0;
	defEnemy[CD_REBELCARRIER].collectType = P_SHIELD;
	defEnemy[CD_REBELCARRIER].collectValue = 0;
	defEnemy[CD_REBELCARRIER].flags = FL_FRIEND;

	// Pluto Boss
	defEnemy[CD_PLUTOBOSS].classDef = CD_PLUTOBOSS;
	defEnemy[CD_PLUTOBOSS].AIType = AI_OFFENSIVE;
	defEnemy[CD_PLUTOBOSS].speed = 4;
	defEnemy[CD_PLUTOBOSS].maxShield = 500;
	defEnemy[CD_PLUTOBOSS].shield = 500;
	defEnemy[CD_PLUTOBOSS].imageIndex[0] = 12;
	defEnemy[CD_PLUTOBOSS].imageIndex[1] = 13;
	defEnemy[CD_PLUTOBOSS].weaponType[0] = W_DOUBLE_ROCKETS;
	defEnemy[CD_PLUTOBOSS].weaponType[1] = W_MICRO_ROCKETS;
	defEnemy[CD_PLUTOBOSS].chance[0] = 50;
	defEnemy[CD_PLUTOBOSS].chance[1] = 2;
	defEnemy[CD_PLUTOBOSS].score = 1000;
	defEnemy[CD_PLUTOBOSS].collectChance = 0;
	defEnemy[CD_PLUTOBOSS].collectType = P_SHIELD;
	defEnemy[CD_PLUTOBOSS].collectValue = 0;
	defEnemy[CD_PLUTOBOSS].flags = FL_WEAPCO;

	// Pluto Boss Barrier
	defEnemy[CD_BARRIER].classDef = CD_BARRIER;
	defEnemy[CD_BARRIER].AIType = AI_OFFENSIVE;
	defEnemy[CD_BARRIER].speed = 1;
	defEnemy[CD_BARRIER].maxShield = 250;
	defEnemy[CD_BARRIER].shield = 250;
	defEnemy[CD_BARRIER].imageIndex[0] = 32;
	defEnemy[CD_BARRIER].imageIndex[1] = 33;
	defEnemy[CD_BARRIER].weaponType[0] = W_DOUBLE_SHOT;
	defEnemy[CD_BARRIER].weaponType[1] = W_MICRO_ROCKETS;
	defEnemy[CD_BARRIER].chance[0] = 0;
	defEnemy[CD_BARRIER].chance[1] = 0;
	defEnemy[CD_BARRIER].score = 1000;
	defEnemy[CD_BARRIER].collectChance = 100;
	defEnemy[CD_BARRIER].collectType = P_ANYTHING;
	defEnemy[CD_BARRIER].collectValue = 25;
	defEnemy[CD_BARRIER].flags = FL_WEAPCO + FL_NOFIRE;

	// Neptune Boss
	defEnemy[CD_NEPTUNEBOSS].classDef = CD_NEPTUNEBOSS;
	defEnemy[CD_NEPTUNEBOSS].AIType = AI_OFFENSIVE;
	defEnemy[CD_NEPTUNEBOSS].speed = 4;
	defEnemy[CD_NEPTUNEBOSS].maxShield = 800;
	defEnemy[CD_NEPTUNEBOSS].shield = 800;
	defEnemy[CD_NEPTUNEBOSS].imageIndex[0] = 12;
	defEnemy[CD_NEPTUNEBOSS].imageIndex[1] = 13;
	defEnemy[CD_NEPTUNEBOSS].weaponType[0] = W_DOUBLE_SHOT;
	defEnemy[CD_NEPTUNEBOSS].weaponType[1] = W_MICRO_ROCKETS;
	defEnemy[CD_NEPTUNEBOSS].chance[0] = 100;
	defEnemy[CD_NEPTUNEBOSS].chance[1] = 0;
	defEnemy[CD_NEPTUNEBOSS].score = 1000;
	defEnemy[CD_NEPTUNEBOSS].collectChance = 100;
	defEnemy[CD_NEPTUNEBOSS].collectType = P_ANYTHING;
	defEnemy[CD_NEPTUNEBOSS].collectValue = 25;
	defEnemy[CD_NEPTUNEBOSS].flags = FL_WEAPCO;

	// Mobile Shield
	defEnemy[CD_MOBILESHIELD].classDef = CD_MOBILESHIELD;
	defEnemy[CD_MOBILESHIELD].AIType = AI_EVASIVE;
	defEnemy[CD_MOBILESHIELD].speed = 6;
	defEnemy[CD_MOBILESHIELD].maxShield = 150;
	defEnemy[CD_MOBILESHIELD].shield = 150;
	defEnemy[CD_MOBILESHIELD].imageIndex[0] = 34;
	defEnemy[CD_MOBILESHIELD].imageIndex[1] = 35;
	defEnemy[CD_MOBILESHIELD].weaponType[0] = W_DOUBLE_SHOT;
	defEnemy[CD_MOBILESHIELD].weaponType[1] = W_MICRO_ROCKETS;
	defEnemy[CD_MOBILESHIELD].chance[0] = 0;
	defEnemy[CD_MOBILESHIELD].chance[1] = 0;
	defEnemy[CD_MOBILESHIELD].score = 250;
	defEnemy[CD_MOBILESHIELD].collectChance = 100;
	defEnemy[CD_MOBILESHIELD].collectType = P_ANYTHING;
	defEnemy[CD_MOBILESHIELD].collectValue = 25;
	defEnemy[CD_MOBILESHIELD].flags = FL_WEAPCO + FL_NOFIRE;

	// Firefly
	defEnemy[CD_FIREFLY].classDef = CD_FIREFLY;
	defEnemy[CD_FIREFLY].AIType = AI_OFFENSIVE;
	defEnemy[CD_FIREFLY].speed = 5;
	defEnemy[CD_FIREFLY].maxShield = 250;
	defEnemy[CD_FIREFLY].shield = 250;
	defEnemy[CD_FIREFLY].imageIndex[0] = 0;
	defEnemy[CD_FIREFLY].imageIndex[1] = 1;
	defEnemy[CD_FIREFLY].weaponType[0] = W_TRIPLE_SHOT;
	defEnemy[CD_FIREFLY].weaponType[1] = W_DOUBLE_ROCKETS;
	defEnemy[CD_FIREFLY].chance[0] = 100;
	defEnemy[CD_FIREFLY].chance[1] = 5;
	defEnemy[CD_FIREFLY].score = 500;
	defEnemy[CD_FIREFLY].collectChance = 100;
	defEnemy[CD_FIREFLY].collectType = P_ANYTHING;
	defEnemy[CD_FIREFLY].collectValue = 250;
	defEnemy[CD_FIREFLY].flags = FL_WEAPCO;

	// Uranus Boss
	defEnemy[CD_URANUSBOSS].classDef = CD_URANUSBOSS;
	defEnemy[CD_URANUSBOSS].AIType = AI_OFFENSIVE;
	defEnemy[CD_URANUSBOSS].speed = 4;
	defEnemy[CD_URANUSBOSS].maxShield = 750;
	defEnemy[CD_URANUSBOSS].shield = 750;
	defEnemy[CD_URANUSBOSS].imageIndex[0] = 41;
	defEnemy[CD_URANUSBOSS].imageIndex[1] = 42;
	defEnemy[CD_URANUSBOSS].weaponType[0] = W_SPREADSHOT;
	defEnemy[CD_URANUSBOSS].weaponType[1] = W_DOUBLE_ROCKETS;
	defEnemy[CD_URANUSBOSS].chance[0] = 100;
	defEnemy[CD_URANUSBOSS].chance[1] = 5;
	defEnemy[CD_URANUSBOSS].score = 500;
	defEnemy[CD_URANUSBOSS].collectChance = 100;
	defEnemy[CD_URANUSBOSS].collectType = P_ANYTHING;
	defEnemy[CD_URANUSBOSS].collectValue = 250;
	defEnemy[CD_URANUSBOSS].flags = FL_WEAPCO;

	// Uranus Boss Wing 1
	defEnemy[CD_URANUSBOSSWING1].classDef = CD_URANUSBOSSWING1;
	defEnemy[CD_URANUSBOSSWING1].AIType = AI_OFFENSIVE;
	defEnemy[CD_URANUSBOSSWING1].speed = 4;
	defEnemy[CD_URANUSBOSSWING1].maxShield = 250;
	defEnemy[CD_URANUSBOSSWING1].shield = 250;
	defEnemy[CD_URANUSBOSSWING1].imageIndex[0] = 43;
	defEnemy[CD_URANUSBOSSWING1].imageIndex[1] = 44;
	defEnemy[CD_URANUSBOSSWING1].weaponType[0] = W_DOUBLE_ROCKETS;
	defEnemy[CD_URANUSBOSSWING1].weaponType[1] = W_DOUBLE_ROCKETS;
	defEnemy[CD_URANUSBOSSWING1].chance[0] = 5;
	defEnemy[CD_URANUSBOSSWING1].chance[1] = 0;
	defEnemy[CD_URANUSBOSSWING1].score = 500;
	defEnemy[CD_URANUSBOSSWING1].collectChance = 100;
	defEnemy[CD_URANUSBOSSWING1].collectType = P_ANYTHING;
	defEnemy[CD_URANUSBOSSWING1].collectValue = 250;
	defEnemy[CD_URANUSBOSSWING1].flags = FL_WEAPCO + FL_IMMORTAL;

	// Uranus Boss Wing 2
	defEnemy[CD_URANUSBOSSWING2].classDef = CD_URANUSBOSSWING2;
	defEnemy[CD_URANUSBOSSWING2].AIType = AI_OFFENSIVE;
	defEnemy[CD_URANUSBOSSWING2].speed = 4;
	defEnemy[CD_URANUSBOSSWING2].maxShield = 250;
	defEnemy[CD_URANUSBOSSWING2].shield = 250;
	defEnemy[CD_URANUSBOSSWING2].imageIndex[0] = 45;
	defEnemy[CD_URANUSBOSSWING2].imageIndex[1] = 46;
	defEnemy[CD_URANUSBOSSWING2].weaponType[0] = W_DOUBLE_ROCKETS;
	defEnemy[CD_URANUSBOSSWING2].weaponType[1] = W_DOUBLE_ROCKETS;
	defEnemy[CD_URANUSBOSSWING2].chance[0] = 5;
	defEnemy[CD_URANUSBOSSWING2].chance[1] = 0;
	defEnemy[CD_URANUSBOSSWING2].score = 500;
	defEnemy[CD_URANUSBOSSWING2].collectChance = 100;
	defEnemy[CD_URANUSBOSSWING2].collectType = P_ANYTHING;
	defEnemy[CD_URANUSBOSSWING2].collectValue = 250;
	defEnemy[CD_URANUSBOSSWING2].flags = FL_WEAPCO + FL_IMMORTAL;

	saveAliens();
}

#endif
