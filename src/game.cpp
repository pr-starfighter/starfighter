/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012, 2013 Guus Sliepen
Copyright (C) 2012, 2014, 2015 Julian Marchant

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

Game currentGame;

void newGame()
{
	currentGame.system = 0;
	currentGame.area = MISN_START;
	currentGame.sfxVolume = 0;
	currentGame.musicVolume = 0;

	if (!engine.useAudio)
	{
		engine.useSound = false;
		engine.useMusic = false;
	}

	currentGame.cash = 0;
	currentGame.cashEarned = 0;
	currentGame.shots = 0;
	currentGame.hits = 0;
	currentGame.accuracy = 0;
	currentGame.totalKills = currentGame.wingMate1Kills = currentGame.wingMate2Kills = 0;
	currentGame.totalOtherKills = 0;
	currentGame.hasWingMate1 = currentGame.hasWingMate2 = 0;
	currentGame.wingMate1Ejects = currentGame.wingMate2Ejects = 0;
	currentGame.secondaryMissions = currentGame.secondaryMissionsCompleted = 0;
	currentGame.shieldPickups = currentGame.rocketPickups = currentGame.cellPickups = 0;
	currentGame.powerups = currentGame.minesKilled = currentGame.cargoPickups = 0;

	currentGame.slavesRescued = 0;
	currentGame.experimentalShield = 1000;

	currentGame.timeTaken = 0;

	currentGame.stationedPlanet = -1;
	currentGame.destinationPlanet = -1;
	for (int i = 0 ; i < 10 ; i++)
		currentGame.missionCompleted[i] = 0;
	currentGame.distanceCovered = 0;

	currentGame.minPlasmaRate = 1;
	currentGame.minPlasmaOutput = 1;
	currentGame.minPlasmaDamage = 1;
	currentGame.maxPlasmaRate = 2;
	currentGame.maxPlasmaOutput = 2;
	currentGame.maxPlasmaDamage = 2;
	currentGame.maxPlasmaAmmo = 100;
	currentGame.maxRocketAmmo = 10;

	currentGame.minPlasmaRateLimit = 2;
	currentGame.minPlasmaDamageLimit = 2;
	currentGame.minPlasmaOutputLimit = 2;
	currentGame.maxPlasmaRateLimit = 3;
	currentGame.maxPlasmaDamageLimit = 3;
	currentGame.maxPlasmaOutputLimit = 3;
	currentGame.maxPlasmaAmmoLimit = 250;
	currentGame.maxRocketAmmoLimit = 50;

	player.maxShield = 50;

	switch (currentGame.difficulty)
	{
		case DIFFICULTY_EASY:
			player.maxShield = 100;

			currentGame.minPlasmaRate = 2;
			currentGame.minPlasmaOutput = 2;
			currentGame.minPlasmaDamage = 2;
			currentGame.maxPlasmaRate = 3;
			currentGame.maxPlasmaOutput = 3;
			currentGame.maxPlasmaDamage = 3;
			currentGame.maxPlasmaAmmo = 150;
			currentGame.maxRocketAmmo = 20;

			currentGame.minPlasmaRateLimit = 3;
			currentGame.minPlasmaDamageLimit = 3;
			currentGame.minPlasmaOutputLimit = 3;
			currentGame.maxPlasmaRateLimit = 5;
			currentGame.maxPlasmaDamageLimit = 5;
			currentGame.maxPlasmaOutputLimit = 5;
			break;
		case DIFFICULTY_HARD:
			player.maxShield = 25;
			break;
		case DIFFICULTY_NIGHTMARE:
			player.maxShield = 1;
			currentGame.maxRocketAmmo = 5;
			break;
		case DIFFICULTY_ORIGINAL:
			player.maxShield = 25;

			currentGame.minPlasmaRateLimit = 3;
			currentGame.minPlasmaDamageLimit = 3;
			currentGame.minPlasmaOutputLimit = 3;
			currentGame.maxPlasmaRateLimit = 5;
			currentGame.maxPlasmaDamageLimit = 5;
			currentGame.maxPlasmaOutputLimit = 5;
			break;
	}

	player.shield = player.maxShield;
	player.ammo[0] = 0;
	player.ammo[1] = currentGame.maxRocketAmmo / 2;
	player.weaponType[0] = W_PLAYER_WEAPON;
	player.weaponType[1] = W_ROCKETS;

	initWeapons();
	initMissions();
	initPlanetMissions(currentGame.system);
}

/*
This handles active bullets in a linked list. The current bullet and
previous bullet pointers are first assigned to the main header bullet
and each successive bullet is pulled out. Bullets are moved in their
delta coordinates, with rockets having fire trails added to them. Seperate
collision checks are done for a bullet that belongs to the enemy and one
that belongs to a player. However rockets can hit anyone. Upon an enemy
being killed, mission requirements are checked and collectables are randomly
spawned.
*/
static void game_doBullets()
{
	object *bullet = engine.bulletHead;
	object *prevBullet = engine.bulletHead;
	engine.bulletTail = engine.bulletHead;

	object *alien, *theCargo;

	bool okayToHit = false;
	int old_shield;
	float homingMissileSpeed = 0;

	while (bullet->next != NULL)
	{
		bullet = bullet->next;

		if (bullet->active)
		{
			if (bullet->flags & WF_HOMING)
			{
				if (bullet->target == NULL)
					bullet->target = bullet_getTarget(bullet);

				if (bullet->owner->flags & FL_FRIEND)
					homingMissileSpeed = 0.25;
				else
					homingMissileSpeed = 0.05;
			}

			if (bullet->id == WT_ROCKET)
			{
				addExplosion(bullet->x, bullet->y, E_SMALL_EXPLOSION);
			}
			else if (bullet->id == WT_MICROROCKET)
			{
				addExplosion(bullet->x, bullet->y, E_TINY_EXPLOSION);
			}

			if ((bullet->flags & WF_AIMED))
			{
				blit(bullet->image[0], (int)(bullet->x - bullet->dx),
					(int)(bullet->y - bullet->dy));
			}

			if (bullet->id == WT_CHARGER)
			{
				for (int i = 0 ; i < bullet->damage * 2 ; i++)
					blit(bullet->image[0],
						(int)(bullet->x - RANDRANGE(-(bullet->damage * 2 / 3), 0)),
						(int)(bullet->y + RANDRANGE(-3, 3)));
			}

			blit(bullet->image[0], (int)bullet->x, (int)bullet->y);
			bullet->x += bullet->dx;
			bullet->y += bullet->dy;

			if (bullet->target != NULL)
			{
				if (bullet->x < bullet->target->x)
					LIMIT_ADD(bullet->dx, homingMissileSpeed, -15, 15);
				else if (bullet->x > bullet->target->x)
					LIMIT_ADD(bullet->dx, -homingMissileSpeed, -15, 15);

				//Rocket is (more or less) inline with target. Fly straight
				if ((bullet->x > bullet->target->x - 1) && (bullet->x < bullet->target->x + 5))
					bullet->dx = 0;

				if (bullet->y < bullet->target->y)
					LIMIT_ADD(bullet->dy, homingMissileSpeed, -15, 15);
				else if (bullet->y > bullet->target->y)
					LIMIT_ADD(bullet->dy, -homingMissileSpeed, -15, 15);

				//Rocket is (more or less) inline with target. Fly straight
				if ((bullet->y > bullet->target->y - 1) && (bullet->y < bullet->target->y + 5))
					bullet->dy = 0;

				if ((bullet->target->shield < 1) || (bullet->target->flags & FL_ISCLOAKED))
					bullet->target = NULL;
			}

			bullet->x += engine.ssx + engine.smx;
			bullet->y += engine.ssy + engine.smy;

			for (int i = 0 ; i < ALIEN_MAX ; i++)
			{
				alien = &aliens[i];

				if ((alien->shield < 1) || (!alien->active))
					continue;

				okayToHit = false;

				if ((bullet->flags & WF_FRIEND) && (alien->flags & FL_WEAPCO))
					okayToHit = true;
				if ((bullet->flags & WF_WEAPCO) && (alien->flags & FL_FRIEND))
					okayToHit = true;
				if ((bullet->id == WT_ROCKET) || (bullet->id == WT_LASER) ||
						(bullet->id == WT_CHARGER))
					okayToHit = true;

				if (bullet->owner == alien->owner)
					okayToHit = false;

				if (okayToHit)
				{
					if ((bullet->active) && (collision(bullet, alien)))
					{
						old_shield = alien->shield;

						if (bullet->owner == &player)
						{
							currentGame.hits++;
							if ((alien->classDef == CD_PHOEBE) ||
									(alien->classDef == CD_URSULA))
								getMissFireMessage(alien);
						}

						if (!(alien->flags & FL_IMMORTAL))
						{
							alien_hurt(alien, bullet->owner, bullet->damage,
								(bullet->flags & WF_DISABLE));

							alien->hit = 5;
						}

						if (bullet->id == WT_CHARGER)
						{
							bullet->damage -= old_shield;
							if (bullet->damage <= 0)
							{
								bullet->active = false;
								bullet->shield = 0;
								audio_playSound(SFX_EXPLOSION, bullet->x);
								for (int i = 0 ; i < 10 ; i++)
									addExplosion(bullet->x + RANDRANGE(-35, 35),
										bullet->y + RANDRANGE(-35, 35),
										E_BIG_EXPLOSION);
							}
						}
						else
						{
							bullet->active = false;
							bullet->shield = 0;
						}

						if (bullet->id == WT_ROCKET)
							addExplosion(bullet->x, bullet->y, E_BIG_EXPLOSION);
						else
							addExplosion(bullet->x, bullet->y, E_SMALL_EXPLOSION);
					}
				}
			}

			// Check for bullets hitting player
			if ((bullet->flags & WF_WEAPCO) || (bullet->id == WT_ROCKET) ||
				(bullet->id == WT_LASER) || (bullet->id == WT_CHARGER))
			{
				if (bullet->active && (player.shield > 0) &&
					(bullet->owner != &player) && collision(bullet, &player))
				{
					old_shield = player.shield;

					if ((!engine.cheatShield) || (engine.missionCompleteTimer != 0))
					{
						if ((player.shield > engine.lowShield) &&
								(player.shield - bullet->damage <= engine.lowShield))
							setInfoLine("!!! WARNING: SHIELD LOW !!!", FONT_RED);

						player.shield -= bullet->damage;
						LIMIT(player.shield, 0, player.maxShield);
						player.hit = 5;
					}

					if ((bullet->owner->classDef == CD_PHOEBE) ||
							(bullet->owner->classDef == CD_URSULA))
						getPlayerHitMessage(bullet->owner);

					if (bullet->id == WT_CHARGER)
					{
						bullet->damage -= old_shield;
						if (bullet->damage <= 0)
						{
							bullet->active = false;
							bullet->shield = 0;
							audio_playSound(SFX_EXPLOSION, bullet->x);
							for (int i = 0 ; i < 10 ; i++)
								addExplosion(bullet->x + RANDRANGE(-35, 35),
									bullet->y + RANDRANGE(-35, 35), E_BIG_EXPLOSION);
						}
					}
					else
					{
						bullet->active = false;
						bullet->shield = 0;
					}

					audio_playSound(SFX_HIT, player.x);

					if (bullet->id == WT_ROCKET)
						addExplosion(bullet->x, bullet->y, E_BIG_EXPLOSION);
					else
						addExplosion(bullet->x, bullet->y, E_SMALL_EXPLOSION);
				}
			}
		}

		if ((currentGame.difficulty != DIFFICULTY_EASY) &&
			((bullet->owner == &player) || (bullet->id == WT_ROCKET)))
		{
			for (int j = 0 ; j < 20 ; j++)
			{
				theCargo = &cargo[j];
				if (theCargo->active)
				{
					if (collision(bullet, theCargo))
					{
						bullet->active = false;
						addExplosion(bullet->x, bullet->y, E_SMALL_EXPLOSION);
						audio_playSound(SFX_HIT, theCargo->x);
						if (theCargo->collectType != P_PHOEBE)
						{
							theCargo->active = false;
							audio_playSound(SFX_EXPLOSION, theCargo->x);
							for (int i = 0 ; i < 10 ; i++)
								addExplosion(theCargo->x + RANDRANGE(-15, 15),
									theCargo->y + RANDRANGE(-15, 15),
									E_BIG_EXPLOSION);
							updateMissionRequirements(M_PROTECT_PICKUP,
								P_CARGO, 1);
						}
					}
				}
			}
		}

		// check to see if a bullet (on any side) hits a mine
		checkMineBulletCollisions(bullet);

		bullet->shield--;

		if (bullet->shield < 1)
		{
			if (bullet->flags & WF_TIMEDEXPLOSION)
			{
				audio_playSound(SFX_EXPLOSION, bullet->x);
				for (int i = 0 ; i < 10 ; i++)
					addExplosion(bullet->x + RANDRANGE(-35, 35),
						bullet->y + RANDRANGE(-35, 35), E_BIG_EXPLOSION);

				if (player_checkShockDamage (bullet->x, bullet->y))
					setInfoLine("Warning: Missile Shockwave Damage!!",
						FONT_RED);
			}
			bullet->active = false;
		}

		if (bullet->active)
		{
			prevBullet = bullet;
			engine.bulletTail = bullet;
		}
		else
		{
			prevBullet->next = bullet->next;
			delete bullet;
			bullet = prevBullet;
		}
	}
}

static void game_doAliens()
{
	static float barrierLoop = 0;

	int shapeToUse;
	bool canFire;
	int n;

	barrierLoop += 0.2;

	// A global variable for checking if all the aliens are dead
	engine.allAliensDead = 1;

	for (int i = 0 ; i < ALIEN_MAX ; i++)
	{
		if (aliens[i].active)
		{
			if (aliens[i].shield > 0)
			{
				if ((aliens[i].flags & FL_WEAPCO) && (!(aliens[i].flags & FL_DISABLED)))
					engine.allAliensDead = 0;

				// Set part attributes
				if (aliens[i].owner != &aliens[i])
				{
					aliens[i].face = aliens[i].owner->face;

					if (aliens[i].face == 0)
						aliens[i].x = aliens[i].owner->x - aliens[i].dx;
					else
						aliens[i].x = aliens[i].owner->x + aliens[i].owner->image[0]->w + aliens[i].dx - aliens[i].image[0]->w;

					aliens[i].y = (aliens[i].owner->y + aliens[i].dy);

					if (aliens[i].owner->shield < 1)
					{
						if ((aliens[i].classDef != CD_URANUSBOSSWING1) &&
							(aliens[i].classDef != CD_URANUSBOSSWING2))
						{
							aliens[i].shield = 0;
						}
						else
						{
							aliens[i].flags &= ~FL_IMMORTAL;
							aliens[i].owner = &aliens[i];
							aliens[i].chance[0] = 25;
						}
					}
				}

				canFire = true; // The alien is allowed to fire

				LIMIT_ADD(aliens[i].thinktime, -1, 0, 250);

				if (aliens[i].target->shield < 1)
					aliens[i].target = &aliens[i];

				// Specific to Sid to stop him pissing about(!)
				if ((aliens[i].classDef == CD_SID) &&
						(aliens[i].target->flags & FL_DISABLED))
					aliens[i].target = &aliens[i];

 				if (aliens[i].target == &aliens[i])
				{
					if (engine.missionCompleteTimer == 0)
					{
						alien_searchForTarget(&aliens[i]);
					}
					else
					{
						if (aliens[i].flags & FL_FRIEND)
						{
							aliens[i].target = &player;
							aliens[i].thinktime = 1;
						}
					}
				}

				if ((!(aliens[i].flags & FL_DISABLED)) &&
					(aliens[i].thinktime == 0) && (aliens[i].target != &aliens[i]) &&
					(aliens[i].owner == &aliens[i]))
				{
					if (aliens[i].classDef == CD_KLINE)
						alien_setKlineAI(&aliens[i]);
					else
						alien_setAI(&aliens[i]);

					aliens[i].thinktime = (rand() % 25) * 10;

					// Face direction of movement unless you always face
					// your target(!)
					if (!(aliens[i].flags & FL_ALWAYSFACE))
					{
						aliens[i].face = (aliens[i].dx > 0);
					}

					LIMIT(aliens[i].dx, -aliens[i].speed, aliens[i].speed);
					LIMIT(aliens[i].dy, -aliens[i].speed, aliens[i].speed);

				}

				if (aliens[i].flags & FL_ALWAYSFACE)
				{
					aliens[i].face = 0;
					if (aliens[i].x > aliens[i].target->x) aliens[i].face = 1;
				}

				if ((currentGame.area == MISN_ELLESH) &&
						(aliens[i].classDef == CD_BOSS))
					aliens[i].face = 0;

				if ((aliens[i].flags & FL_DEPLOYDRONES) && ((rand() % 300) == 0))
					alien_addDrone(&aliens[i]);

				if (aliens[i].flags & FL_LEAVESECTOR)
				{
					// Note: The original version of this line incorrectly
					// specified -15 as the *maximum* instead of the
					// *minimum*, which at the time was equivalent to
					// ``aliens[i].dx = -15``.
					LIMIT_ADD(aliens[i].dx, -0.5, -15, 0);
					aliens[i].dy = 0;
					aliens[i].thinktime = 999;
					aliens[i].face = 0;

					if (aliens[i].x >= 5000)
					{
						aliens[i].flags -= FL_LEAVESECTOR;
						aliens[i].flags += FL_ESCAPED;
						aliens[i].active = false;

						if (aliens[i].classDef == CD_CLOAKFIGHTER)
						{
							currentGame.experimentalShield = aliens[i].shield;
							setInfoLine("Experimental Fighter has fled",
								FONT_CYAN);
						}

						aliens[i].shield = 0;
						updateMissionRequirements(M_ESCAPE_TARGET,
							aliens[i].classDef, 1);
					
						if (aliens[i].classDef != CD_CLOAKFIGHTER)
							updateMissionRequirements(M_DESTROY_TARGET_TYPE,
								aliens[i].classDef, 1);
					}
				}

				// This deals with the Experimental Fighter in Mordor
				// (and Kline on the final mission)
				// It can cloak and uncloak at random. When cloaked,
				// its sprite is not displayed. However the engine
				// trail is still visible!
				if ((aliens[i].flags & FL_CANCLOAK) && ((rand() % 500) == 0))
				{
					if (aliens[i].flags & FL_ISCLOAKED)
						aliens[i].flags -= FL_ISCLOAKED;
					else
						aliens[i].flags += FL_ISCLOAKED;
					audio_playSound(SFX_CLOAK, aliens[i].x);
				}

				if (aliens[i].classDef == CD_BARRIER)
				{
					aliens[i].dx = -10 + (sinf(barrierLoop + aliens[i].speed) * 60);
					aliens[i].dy = 20 + (cosf(barrierLoop + aliens[i].speed) * 40);
				}

				if (aliens[i].classDef == CD_MOBILESHIELD)
				{
					LIMIT_ADD(aliens[ALIEN_BOSS].shield, 1, 0,
						aliens[ALIEN_BOSS].maxShield);
				}

				LIMIT_ADD(aliens[i].reload[0], -1, 0, 999);
				LIMIT_ADD(aliens[i].reload[1], -1, 0, 999);

				if ((!(aliens[i].flags & FL_DISABLED)) &&
					(!(aliens[i].flags & FL_NOFIRE)))
				{
					if ((aliens[i].target->shield > 0))
						canFire = alien_checkTarget(&aliens[i]);

					if (((aliens[i].thinktime % 2) == 0) &&
							(aliens[i].flags & FL_FRIEND))
						canFire = alien_enemiesInFront(&aliens[i]);
				}
				else
				{
					canFire = false;
				}

				if ((canFire) && (dev.fireAliens))
				{
					if ((aliens[i].reload[0] == 0) &&
						((rand() % 1000 < aliens[i].chance[0]) ||
							(aliens[i].flags & FL_CONTINUOUS_FIRE)))
					{
						ship_fireBullet(&aliens[i], 0);
					}
					if ((aliens[i].reload[1] == 0) &&
						((rand() % 1000 < aliens[i].chance[1]) ||
							(aliens[i].flags & FL_CONTINUOUS_FIRE)))
					{
						if ((aliens[i].weaponType[1] != W_ENERGYRAY) &&
							(aliens[i].weaponType[1] != W_LASER))
						{
							if (aliens[i].weaponType[1] == W_CHARGER)
								aliens[i].ammo[1] = 50 + rand() % 150;
							ship_fireBullet(&aliens[i], 1);
						}
						else if (aliens[i].weaponType[1] == W_LASER)
						{
							aliens[i].flags += FL_FIRELASER;
						}
						else if ((aliens[i].weaponType[1] == W_ENERGYRAY) &&
							(aliens[i].ammo[0] == 250))
						{
							aliens[i].flags += FL_FIRERAY;
							audio_playSound(SFX_ENERGYRAY, aliens[i].x);
						}
					}
				}

				if (aliens[i].flags & FL_FIRERAY)
				{
					ship_fireRay(&aliens[i]);
				}
				else
				{
					LIMIT_ADD(aliens[i].ammo[0], 1, 0, 250);
				}

				if (aliens[i].flags & FL_FIRELASER)
				{
					ship_fireBullet(&aliens[i], 1);
					if ((rand() % 25) == 0)
						aliens[i].flags -= FL_FIRELASER;
				}

				if (aliens[i].flags & FL_DROPMINES)
				{
					if ((rand() % 150) == 0)
						addCollectable(aliens[i].x, aliens[i].y, P_MINE, 25,
							600 + rand() % 2400);

					// Kline drops mines a lot more often
					if ((&aliens[i] == &aliens[ALIEN_KLINE]))
					{
						if ((rand() % 10) == 0)
							addCollectable(aliens[i].x, aliens[i].y, P_MINE, 25,
								600 + rand() % 2400);
					}
				}

				shapeToUse = aliens[i].imageIndex[aliens[i].face];

				if (aliens[i].hit)
					shapeToUse += SHIP_HIT_INDEX;

				LIMIT_ADD(aliens[i].hit, -1, 0, 100);

				if ((aliens[i].x + aliens[i].image[0]->w > 0) &&
					(aliens[i].x < screen->w) &&
					(aliens[i].y + aliens[i].image[0]->h > 0) &&
					(aliens[i].y < screen->h))
				{
					if ((!(aliens[i].flags & FL_DISABLED)) &&
							(aliens[i].classDef != CD_ASTEROID) &&
							(aliens[i].classDef != CD_ASTEROID2))
						addEngine(&aliens[i]);
					if ((!(aliens[i].flags & FL_ISCLOAKED)) || (aliens[i].hit > 0))
						blit(shipShape[shapeToUse], (int)aliens[i].x,
							(int)aliens[i].y);
					if (aliens[i].flags & FL_DISABLED)
					{
						if ((rand() % 10) == 0)
							addExplosion(aliens[i].x + (rand() % aliens[i].image[0]->w),
								aliens[i].y + (rand() % aliens[i].image[0]->h),
								E_ELECTRICAL);
					}
				}

				if ((currentGame.area == MISN_MARS) && (aliens[i].x < -60))
					aliens[i].active = false;
			}
			else
			{
				aliens[i].shield--;
				if ((aliens[i].x > 0) && (aliens[i].x < screen->w) &&
					(aliens[i].y > 0) && (aliens[i].y < screen->h))
				{
					blit(aliens[i].image[aliens[i].face], (int)aliens[i].x,
						(int)aliens[i].y);
					addExplosion(aliens[i].x + (rand() % aliens[i].image[0]->w),
						aliens[i].y + (rand() % aliens[i].image[0]->h),
						E_BIG_EXPLOSION);
				}
				if (aliens[i].shield < aliens[i].deathCounter)
				{
					aliens[i].active = false;
					if ((aliens[i].classDef == CD_BOSS) ||
							(aliens[i].owner == &aliens[ALIEN_BOSS]) ||
							(aliens[i].flags & FL_FRIEND) ||
							(aliens[i].classDef == CD_ASTEROID) ||
							(aliens[i].classDef == CD_KLINE))
						addDebris((int)aliens[i].x, (int)aliens[i].y,
							aliens[i].maxShield);

					if (aliens[i].classDef == CD_ASTEROID)
					{
						n = 1 + (rand() % 3);
						for (int i = 0 ; i < n ; i++)
							alien_addSmallAsteroid(&aliens[i]);
					}
				}
			}

			// Adjust the movement even whilst exploding
			if ((dev.moveAliens) && (!(aliens[i].flags & FL_NOMOVE)) &&
					(!(aliens[i].flags & FL_DISABLED)))
				alien_move(&aliens[i]);

			if ((currentGame.area != MISN_ELLESH) || (aliens[i].shield < 0))
				aliens[i].x += engine.ssx;

			aliens[i].x += engine.smx;
			aliens[i].y += engine.ssy + engine.smy;
		}
	}
}

int mainGameLoop()
{
	FILE *fp;
	char string[255];
	int index, alienType, placeAttempt;
	int barrierSpeed;

	resetLists();

	setMission(currentGame.area);
	missionBriefScreen();

	initCargo();
	initPlayer();

	// Init aliens
	for (int i = 0 ; i < ALIEN_MAX ; i++)
	{
		aliens[i].active = false;
		aliens[i].shield = -1;
		aliens[i].flags = 0;
	}

	engine.targetIndex = -1;

	strcpy(string, "");
	barrierSpeed = 1;

	sprintf(string, "data/aliens%d.dat", currentGame.area);
	fp = fopen(string, "rb");

	if (fp != NULL)
	{
		while (fscanf(fp, "%d %d ", &index, &alienType) == 2)
		{
			placeAttempt = 0;

			aliens[index] = alien_defs[alienType];
			aliens[index].owner = &aliens[index];
			aliens[index].target = &aliens[index];
			aliens[index].face = rand() % 2;
			aliens[index].active = true;

			/*
			we make 1000 attempts to place this enemy since it is required. If after
			1000 attempts we still haven't managed to place the alien, then it
			simply isn't going to happen and we will just exit the game. The chances
			of this happening are very very low!
			*/
			while (true)
			{
				placeAttempt++;

				if (alien_place(&aliens[index]))
					break;

				if (placeAttempt > 1000)
					showErrorAndExit(2, "");
			}

			if (currentGame.area == MISN_CERADSE)
				addCargo(&aliens[index], P_CARGO);
			else if (currentGame.area == MISN_NEROD)
				addCargo(&aliens[index], P_PHOEBE);

			if (index == ALIEN_KLINE)
			{
				aliens[ALIEN_KLINE].target = &player;
			}

			if (aliens[index].classDef == CD_CLOAKFIGHTER)
			{
				aliens[index].active = false;
				aliens[index].maxShield = aliens[index].shield = 400;
				aliens[index].flags &= ~FL_RUNSAWAY;
				aliens[index].speed = 3;
			}

			if ((aliens[index].classDef == CD_MOBILE_RAY) && (index >= 11))
			{
				aliens[index].active = false;
			}

			if (aliens[index].classDef == CD_FIREFLY)
			{
				aliens[index].active = false;
			}

			if (aliens[index].classDef == CD_BARRIER)
			{
				aliens[index].owner = &aliens[ALIEN_BOSS];
				aliens[index].speed = barrierSpeed;
				barrierSpeed++;
			}

			if ((currentGame.area == MISN_POSWIC) &&
				(aliens[index].classDef == CD_BOSS))
			{
				aliens[index].imageIndex[1] = 29;
				aliens[index].flags |= FL_IMMORTAL;
			}

			if (currentGame.area == MISN_ELLESH)
				aliens[index].flags |= FL_HASMINIMUMSPEED;

			if (currentGame.area == MISN_JUPITER)
			{
				aliens[index].flags = FL_WEAPCO;
				if (index == ALIEN_BOSS)
					aliens[index].chance[1] = 5;
			}
		}

		fclose(fp);

		if (currentGame.area == MISN_MOEBO)
		{
			aliens[ALIEN_BOSS].target = &player;
			aliens[ALIEN_BOSS].x = -screen->w / 2;
			aliens[ALIEN_BOSS].y = screen->h / 2;

			aliens[ALIEN_BOSS_PART1].owner = &aliens[ALIEN_BOSS];
			aliens[ALIEN_BOSS_PART1].target = &player;
			aliens[ALIEN_BOSS_PART1].dx = -25;
			aliens[ALIEN_BOSS_PART1].dy = -21;

			aliens[ALIEN_BOSS_PART2].owner = &aliens[ALIEN_BOSS];
			aliens[ALIEN_BOSS_PART2].target = &player;
			aliens[ALIEN_BOSS_PART2].dx = -20;
			aliens[ALIEN_BOSS_PART2].dy = 37;
		}
		else if ((currentGame.area == MISN_ELAMALE) ||
			(currentGame.area == MISN_FELLON))
		{
			aliens[ALIEN_BOSS].target = &player;
			aliens[ALIEN_BOSS].x = -screen->w / 2;
			aliens[ALIEN_BOSS].y = screen->h / 2;

			aliens[ALIEN_BOSS_PART1].owner = &aliens[ALIEN_BOSS];
			aliens[ALIEN_BOSS_PART1].target = &player;
			aliens[ALIEN_BOSS_PART1].dx = 15;
			aliens[ALIEN_BOSS_PART1].dy = -22;

			aliens[ALIEN_BOSS_PART2].owner = &aliens[ALIEN_BOSS];
			aliens[ALIEN_BOSS_PART2].target = &player;
			aliens[ALIEN_BOSS_PART2].dx = 15;
			aliens[ALIEN_BOSS_PART2].dy = 22;

			aliens[ALIEN_BOSS_PART3].owner = &aliens[ALIEN_BOSS_PART1];
			aliens[ALIEN_BOSS_PART3].target = &player;
			aliens[ALIEN_BOSS_PART3].dx = -35;
			aliens[ALIEN_BOSS_PART3].dy = -12;

			aliens[ALIEN_BOSS_PART4].owner = &aliens[ALIEN_BOSS_PART2];
			aliens[ALIEN_BOSS_PART4].target = &player;
			aliens[ALIEN_BOSS_PART4].dx = -35;
			aliens[ALIEN_BOSS_PART4].dy = 20;

			if (currentGame.area == MISN_FELLON)
			{
				aliens[ALIEN_BOSS].AIType = AI_EVASIVE;

				for (int i = 10 ; i < 15 ; i++)
				{
					aliens[i].imageIndex[0] += 15;
					aliens[i].imageIndex[1] += 15;

					aliens[i].image[0] = shipShape[aliens[i].imageIndex[0]];
					aliens[i].image[1] = shipShape[aliens[i].imageIndex[1]];
				}
			}
		}
		else if (currentGame.area == MISN_URANUS)
		{
			aliens[ALIEN_BOSS].target = &player;
			aliens[ALIEN_BOSS].x = -screen->w / 2;
			aliens[ALIEN_BOSS].y = screen->h / 2;

			aliens[ALIEN_BOSS_PART1].owner = &aliens[ALIEN_BOSS];
			aliens[ALIEN_BOSS_PART1].dy = 20;

			aliens[ALIEN_BOSS_PART2].owner = &aliens[ALIEN_BOSS];
			aliens[ALIEN_BOSS_PART2].dy = -16;
		}
	}

	// specific for Phoebe being captured!
	if (currentGame.area == MISN_NEROD)
		currentGame.hasWingMate1 = 1;

	if (currentGame.area == MISN_ELAMALE)
		aliens[ALIEN_KLINE].active = false;

	for (int i = 0 ; i < engine.maxAliens ; i++)
		alien_add();

	if (currentGame.hasWingMate1)
		alien_addFriendly(ALIEN_PHOEBE);

	if (currentGame.hasWingMate2)
		alien_addFriendly(ALIEN_URSULA);

	if ((currentGame.area == MISN_URUSOR) ||
			(currentGame.area == MISN_POSWIC) ||
			(currentGame.area == MISN_EARTH))
		alien_addFriendly(ALIEN_SID);

	// Disable Wingmates for certain missions
	switch (currentGame.area)
	{
		case MISN_NEROD:
		case MISN_URUSOR:
		case MISN_DORIM:
		case MISN_SIVEDI:
		case MISN_ALMARTHA:
		case MISN_ELLESH:
		case MISN_MARS:
		case MISN_VENUS:
			aliens[ALIEN_PHOEBE].active = false;
			aliens[ALIEN_URSULA].active = false;
			break;
	}

	if (currentGame.area == MISN_DORIM)
	{
		aliens[0].collectChance = 100;
		aliens[0].collectType = P_ESCAPEPOD;
	}

	// Some specifics for interception missions
	if (currentGame.area == MISN_INTERCEPTION)
	{
		if ((currentGame.system > 1) && ((rand() % 5) == 0))
		{
			aliens[ALIEN_KLINE] = alien_defs[CD_KLINE];
			aliens[ALIEN_KLINE].owner = &aliens[ALIEN_KLINE];
			aliens[ALIEN_KLINE].target = &player;
			aliens[ALIEN_KLINE].active = true;
			aliens[ALIEN_KLINE].x = player.x + 1000;
			aliens[ALIEN_KLINE].y = player.y;
			player_setTarget(ALIEN_KLINE);
		}

		if ((currentGame.system == 2) && (currentGame.experimentalShield > 0))
		{
			if ((rand() % 5) > 0)
			{
				aliens[10] = alien_defs[CD_CLOAKFIGHTER];
				aliens[10].owner = &aliens[10];
				aliens[10].target = &aliens[10];
				aliens[10].shield = 1000;
				aliens[10].active = true;
				aliens[10].x = player.x - 1000;
				aliens[10].y = player.y;
				player_setTarget(10);
				aliens[10].shield = currentGame.experimentalShield;
			}
		}
	}

	if (currentGame.area == MISN_VENUS)
	{
		aliens[ALIEN_KLINE].x = player.x + 1000;
		aliens[ALIEN_KLINE].y = player.y;
	}

	for (int i = 0 ; i < ALIEN_MAX ; i++)
	{
		aliens[i].systemPower = aliens[i].maxShield;
		aliens[i].deathCounter = 0 - (aliens[i].maxShield * 3);
		LIMIT(aliens[i].deathCounter, -350, 0);
	}

	// Set target energy meter
	switch (currentGame.area)
	{
		case MISN_MOEBO:
		case MISN_ELAMALE:
		case MISN_ODEON:
		case MISN_FELLON:
		case MISN_POSWIC:
		case MISN_ELLESH:
		case MISN_PLUTO:
		case MISN_NEPTUNE:
		case MISN_URANUS:
		case MISN_JUPITER:
			player_setTarget(ALIEN_BOSS);
			break;
		case MISN_NEROD:
			player_setTarget(ALIEN_PHOEBE);
			break;
		case MISN_ALLEZ:
			player_setTarget(ALIEN_GOODTRANSPORT);
			break;
		case MISN_URUSOR:
			player_setTarget(ALIEN_SID);
			break;
		case MISN_DORIM:
			player_setTarget(0);
			break;
		case MISN_EARTH:
		case MISN_VENUS:
			player_setTarget(ALIEN_KLINE);
			break;
	}

	clearInfoLines();

	loadScriptEvents();

	engine.ssx = 0;
	engine.ssy = 0;
	engine.smx = 0;
	engine.smy = 0;

	engine.done = 0;

	engine.counter = (SDL_GetTicks() + 1000);
	engine.counter2 = (SDL_GetTicks() + 1000);

	engine.missionCompleteTimer = 0;
	engine.musicVolume = 100;

	int rtn = 0;

	int allowableAliens = 999999999;

	for (int i = 0 ; i < 3 ; i++)
	{
		if ((currentMission.primaryType[i] == M_DESTROY_TARGET_TYPE) &&
				(currentMission.target1[i] == CD_ANY))
			allowableAliens = currentMission.targetValue1[i];

		if (currentMission.primaryType[i] == M_DESTROY_ALL_TARGETS)
			allowableAliens = 999999999;
	}

	for (int i = 0 ; i < ALIEN_MAX ; i++)
	{
		if ((aliens[i].active) && (aliens[i].flags & FL_WEAPCO))
		{
			allowableAliens--;
		}
	}

	drawBackGround();
	flushBuffer();

	// Default to no aliens dead...
	engine.allAliensDead = 0;

	engine.keyState[KEY_FIRE] = 0;
	engine.keyState[KEY_ALTFIRE] = 0;
	flushInput();

	while (engine.done != 1)
	{
		updateScreen();

		if ((allMissionsCompleted()) && (engine.missionCompleteTimer == 0))
		{
			engine.missionCompleteTimer = SDL_GetTicks() + 4000;
		}

		if ((missionFailed()) && (engine.missionCompleteTimer == 0))
		{
			if (currentGame.area != MISN_MOEBO)
				engine.missionCompleteTimer = SDL_GetTicks() + 4000;
		}

		if (engine.missionCompleteTimer != 0)
		{
			engine.gameSection = SECTION_INTERMISSION;
			if (player.shield > 0)
			{
				if (SDL_GetTicks() >= engine.missionCompleteTimer)
				{
					if ((!missionFailed()) && (currentGame.area != MISN_VENUS))
					{
						leaveSector();
						if ((engine.done == 2) &&
							(currentGame.area != MISN_DORIM) &&
							(currentGame.area != MISN_SIVEDI))
						{
							if ((aliens[ALIEN_PHOEBE].shield > 0) &&
								(currentGame.area != MISN_EARTH))
							{
								aliens[ALIEN_PHOEBE].x = player.x - 40;
								aliens[ALIEN_PHOEBE].y = player.y - 35;
								aliens[ALIEN_PHOEBE].face = 0;
							}

							if ((aliens[ALIEN_URSULA].shield > 0) &&
								(currentGame.area != MISN_EARTH))
							{
								aliens[ALIEN_URSULA].x = player.x - 40;
								aliens[ALIEN_URSULA].y = player.y + 45;
								aliens[ALIEN_URSULA].face = 0;
							}

							if ((currentGame.area == MISN_URUSOR) ||
								(currentGame.area == MISN_POSWIC))
							{
								aliens[ALIEN_SID].x = player.x - 100;
								aliens[ALIEN_SID].y = player.y;
								aliens[ALIEN_SID].face = 0;
							}
						}
					}
					else if ((currentGame.area == MISN_VENUS) &&
						(engine.musicVolume > 0))
					{
						engine.keyState[KEY_UP] = 0;
						engine.keyState[KEY_DOWN] = 0;
						engine.keyState[KEY_LEFT] = 0;
						engine.keyState[KEY_RIGHT] = 0;
						engine.keyState[KEY_FIRE] = 0;
						engine.keyState[KEY_ALTFIRE] = 0;
						LIMIT_ADD(engine.musicVolume, -0.2, 0, 100);
						audio_setMusicVolume(engine.musicVolume);
					}
					else
					{
						engine.done = 1;
					}
				}
				else
				{
					getPlayerInput();
				}
			}
			else
			{
				LIMIT_ADD(engine.musicVolume, -0.2, 0, 100);
				audio_setMusicVolume(engine.musicVolume);
				if (SDL_GetTicks() >= engine.missionCompleteTimer)
				{
					engine.done = 1;
				}
			}
		}
		else
		{
			getPlayerInput();
		}

		unBuffer();
		doStarfield();
		doCollectables();
		game_doBullets();

		game_doAliens();

		doPlayer();
  		doCargo();
  		doDebris();
		doExplosions();
		doInfo();

		WRAP_ADD(engine.eventTimer, -1, 0, 60);

		if (engine.paused)
		{
			textSurface(22, "PAUSED", -1, screen->h / 2, FONT_WHITE);
			blitText(22);
			updateScreen();

			while (engine.paused)
			{
				engine.done = checkPauseRequest();
				delayFrame();
			}
		}

		if ((currentGame.area == MISN_MARS) && (engine.addAliens > -1))
		{
			if ((rand() % 5) == 0)
				// XXX: The originally specified range for x was [800, 100],
				// which with the original rrand function caused the result
				// returned to be `800 + rand() % -699`. Clearly a mistake,
				// but I'm not entirely sure what the original intention was.
				// For now, I've set the range to [800, 1500], which
				// approximately replicates the original's results.
				addCollectable(RANDRANGE(800, 1500),
					RANDRANGE(-screen->h / 3, (4 * screen->h) / 3), P_MINE, 25,
					180 + rand() % 60);
		}

		if (engine.addAliens > -1)
		{
			WRAP_ADD(engine.addAliens, -1, 0, currentMission.addAliens);
			if ((engine.addAliens == 0) && (allowableAliens > 0))
			{
				allowableAliens -= alien_add();
			}
		}

		if ((player.shield <= 0) && (engine.missionCompleteTimer == 0))
			engine.missionCompleteTimer = SDL_GetTicks() + 7000;

		// specific to Boss 1
		if ((currentGame.area == MISN_MOEBO) &&
			(aliens[ALIEN_BOSS].flags & FL_ESCAPED))
		{
			audio_playSound(SFX_DEATH, aliens[ALIEN_BOSS].x);
			clearScreen(white);
			updateScreen();
			for (int i = 0 ; i < 300 ; i++)
			{
				SDL_Delay(10);
				if ((rand() % 25) == 0)
					audio_playSound(SFX_EXPLOSION, aliens[ALIEN_BOSS].x);
			}
			SDL_Delay(1000);
			break;
		}

		delayFrame();
	}

	flushBuffer();

	if ((player.shield > 0) && (!missionFailed()))
	{
		if (currentGame.area < MISN_VENUS)
			missionFinishedScreen();

		switch (currentGame.area)
		{
			case MISN_MOEBO:
				doCutscene(1);
				doCutscene(2);
				break;
			case MISN_NEROD:
				doCutscene(3);
				break;
			case MISN_ELAMALE:
				doCutscene(4);
				break;
			case MISN_ODEON:
				doCutscene(5);
				break;
			case MISN_ELLESH:
				doCutscene(6);
				break;
			case MISN_VENUS:
				doCredits();
				break;
		}
		
		if (currentGame.area < MISN_VENUS)
		{
			updateSystemStatus();
			saveGame(0);
		}

		rtn = 1;
		
		if (currentGame.area == MISN_VENUS)
			rtn = 0;
	}
	else
	{
		gameover();
		rtn = 0;
	}

	exitPlayer();

	return rtn;
}
