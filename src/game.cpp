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
	currentGame.area = 0;
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
			currentGame.maxPlasmaRate = 1;
			currentGame.maxPlasmaOutput = 1;
			currentGame.maxPlasmaDamage = 1;
			currentGame.maxRocketAmmo = 5;
			break;
		default:
			player.maxShield = 50;
	}

	player.shield = player.maxShield;
	player.ammo[0] = 0;
	player.ammo[1] = 5;
	player.weaponType[0] = W_PLAYER_WEAPON;
	player.weaponType[1] = W_ROCKETS;

	initWeapons();
	initMissions();
	initPlanetMissions(currentGame.system);
}

int mainGameLoop()
{
	static float barrierLoop = 0;

	bool canFire;
	int shapeToUse;
	FILE *fp;
	char string[255];
	int index, alienType, placeAttempt;
	int barrierSpeed;
	int n;

	resetLists();

	setMission(currentGame.area);
	missionBriefScreen();

	initCargo();
	initPlayer();

	// Init aliens
	for (int i = 0 ; i < MAX_ALIENS ; i++)
	{
		enemy[i].active = false;
		enemy[i].shield = -1;
		enemy[i].flags = 0;
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

			enemy[index] = defEnemy[alienType];
			enemy[index].owner = &enemy[index];
			enemy[index].target = &enemy[index];
			enemy[index].face = rand() % 2;
			enemy[index].active = true;

			/*
			we make 1000 attempts to place this enemy since it is required. If after
			1000 attempts we still haven't managed to place the alien, then it
			simply isn't going to happen and we will just exit the game. The chances
			of this happening are very very low!
			*/
			while (true)
			{
				placeAttempt++;

				if (alien_place(&enemy[index]))
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
				enemy[index].flags &= ~FL_RUNSAWAY;
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
				enemy[index].flags |= FL_IMMORTAL;
			}

			if (currentGame.area == 18)
				enemy[index].flags |= FL_HASMINIMUMSPEED;

			if (currentGame.area == 23)
			{
				enemy[index].flags = FL_WEAPCO;
				if (index == WC_BOSS)
					enemy[index].chance[1] = 5;
			}
		}

		fclose(fp);

		if (currentGame.area == 5)
		{
			enemy[WC_BOSS].target = &player;
			enemy[WC_BOSS].x = -screen->w / 2;
			enemy[WC_BOSS].y = screen->h / 2;

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
			enemy[WC_BOSS].x = -screen->w / 2;
			enemy[WC_BOSS].y = screen->h / 2;

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

					enemy[i].image[0] = shipShape[enemy[i].imageIndex[0]];
					enemy[i].image[1] = shipShape[enemy[i].imageIndex[1]];
				}
			}
		}
		else if (currentGame.area == 21)
		{
			enemy[WC_BOSS].target = &player;
			enemy[WC_BOSS].x = -screen->w / 2;
			enemy[WC_BOSS].y = screen->h / 2;

			enemy[13].owner = &enemy[WC_BOSS];
			enemy[13].dy = 20;

			enemy[12].owner = &enemy[WC_BOSS];
			enemy[12].dy = -16;
		}
	}

	// specific for Phoebe being captured!
	if (currentGame.area == 7)
		currentGame.hasWingMate1 = 1;

	if (currentGame.area == 11)
		enemy[WC_KLINE].active = false;

	for (int i = 0 ; i < engine.maxAliens ; i++)
		alien_add();

	if (currentGame.hasWingMate1)
		alien_addFriendly(FR_PHOEBE);

	if (currentGame.hasWingMate2)
		alien_addFriendly(FR_URSULA);

	if ((currentGame.area == 9) || (currentGame.area == 17) ||
			(currentGame.area == 25))
		alien_addFriendly(FR_SID);

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
			player_setTarget(WC_KLINE);
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
				player_setTarget(10);
				enemy[10].shield = currentGame.experimentalShield;
			}
		}
	}

	if (currentGame.area == 26)
	{
		enemy[WC_KLINE].flags |= FL_IMMORTAL | FL_NOFIRE | FL_NOMOVE;

		enemy[WC_KLINE].x = screen->w * 2 / 3;
		enemy[WC_KLINE].y = screen->h * 2 / 3;

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
			player_setTarget(WC_BOSS);
			break;
		case 7:
			player_setTarget(FR_PHOEBE);
			break;
		case 8:
			player_setTarget(19);
			break;
		case 9:
			player_setTarget(FR_SID);
			break;
		case 10:
			player_setTarget(0);
			break;
		case 25:
		case 26:
			player_setTarget(WC_KLINE);
			break;
		default:
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
		if ((currentMission.primaryType[i] == M_DESTROY_TARGET_TYPE) && (currentMission.target1[i] == CD_ANY))
			allowableAliens = currentMission.targetValue1[i];

		if (currentMission.primaryType[i] == M_DESTROY_ALL_TARGETS)
			allowableAliens = 999999999;
	}

	for (int i = 0 ; i < MAX_ALIENS ; i++)
	{
		if ((enemy[i].active) && (enemy[i].flags & FL_WEAPCO))
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
			if (currentGame.area != 5)
				engine.missionCompleteTimer = SDL_GetTicks() + 4000;
		}

		if (engine.missionCompleteTimer != 0)
		{
			engine.gameSection = SECTION_INTERMISSION;
			if (player.shield > 0)
			{
				if (SDL_GetTicks() >= engine.missionCompleteTimer)
				{
					if ((!missionFailed()) && (currentGame.area != 26))
					{
						leaveSector();
						if ((engine.done == 2) && (currentGame.area != 10) && (currentGame.area != 15))
						{
							if ((enemy[FR_PHOEBE].shield > 0) && (currentGame.area != 25))
							{
								enemy[FR_PHOEBE].x = player.x - 40;
								enemy[FR_PHOEBE].y = player.y - 35;
								enemy[FR_PHOEBE].face = 0;
							}

							if ((enemy[FR_URSULA].shield > 0) && (currentGame.area != 25))
							{
								enemy[FR_URSULA].x = player.x - 40;
								enemy[FR_URSULA].y = player.y + 45;
								enemy[FR_URSULA].face = 0;
							}

							if ((currentGame.area == 9) || (currentGame.area == 17))
							{
								enemy[FR_SID].x = player.x - 100;
								enemy[FR_SID].y = player.y;
								enemy[FR_SID].face = 0;
							}
						}
					}
					else if ((currentGame.area == 26) && (engine.musicVolume > 0))
					{
						limitFloat(&(engine.musicVolume -= 0.2), 0, 100);
						Mix_VolumeMusic((int)engine.musicVolume);
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
				limitFloat(&(engine.musicVolume -= 0.2), 0, 100);
				Mix_VolumeMusic((int)engine.musicVolume);
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
		doBullets();

		barrierLoop += 0.2;

		// A global variable for checking if all the aliens are dead
		engine.allAliensDead = 1;

		object *alien = enemy;

		for (int i = 0 ; i < MAX_ALIENS ; i++)
		{
			if (alien->active)
			{
				if (alien->shield > 0)
				{
					if ((alien->flags & FL_WEAPCO) && (!(alien->flags & FL_DISABLED)))
						engine.allAliensDead = 0;

					// Set part attributes
					if (alien->owner != alien)
					{
						alien->face = alien->owner->face;

						if (alien->face == 0)
							alien->x = alien->owner->x - alien->dx;
						else
							alien->x = alien->owner->x + alien->owner->image[0]->w + alien->dx - alien->image[0]->w;

						alien->y = (alien->owner->y + alien->dy);

						if (alien->owner->shield < 1)
						{
							if ((alien->classDef != CD_URANUSBOSSWING1) &&
								(alien->classDef != CD_URANUSBOSSWING2))
							{
								alien->shield = 0;
							}
							else
							{
								alien->flags &= ~FL_IMMORTAL;
								alien->owner = alien;
								alien->chance[0] = 25;
							}
						}
					}

					canFire = true; // The alien is allowed to fire

					limitInt(&--alien->thinktime, 0, 250);

					if (alien->target->shield < 1)
						alien->target = alien;

					// Specific to Sid to stop him pissing about(!)
					if ((alien->classDef == CD_SID) &&
							(alien->target->flags & FL_DISABLED))
						alien->target = alien;

	 				if (alien->target == alien)
					{
						if (engine.missionCompleteTimer == 0)
						{
							alien_searchForTarget(alien);
						}
						else
						{
							if (alien->flags & FL_FRIEND)
							{
								alien->target = &player;
								alien->thinktime = 1;
							}
						}
					}

					if ((!(alien->flags & FL_DISABLED)) &&
						(alien->thinktime == 0) && (alien->target != alien) &&
						(alien->owner == alien))
					{
						if (alien->classDef == CD_KLINE)
							alien_setKlineAI(alien);
						else
							alien_setAI(alien);

						alien->thinktime = (rand() % 25) * 10;

						// Face direction of movement unless you always face
						// your target(!)
						if (!(alien->flags & FL_ALWAYSFACE))
						{
							alien->face = (alien->dx > 0);
						}

						limitFloat(&alien->dx, 0 - alien->speed, alien->speed);
						limitFloat(&alien->dy, 0 - alien->speed, alien->speed);

					}

					if (alien->flags & FL_ALWAYSFACE)
					{
						alien->face = 0;
						if (alien->x > alien->target->x) alien->face = 1;
					}

					if ((currentGame.area == 18) && (alien->classDef == CD_BOSS))
						alien->face = 0;

					if ((alien->flags & FL_DEPLOYDRONES) && ((rand() % 300) == 0))
						alien_addDrone(alien);

					if (alien->flags & FL_LEAVESECTOR)
					{
						limitFloat(&(alien->dx -= 0.5), 0, -15);
						alien->dy = 0;
						alien->thinktime = 999;
						alien->face = 0;

						if (alien->x >= 5000)
						{
							alien->flags -= FL_LEAVESECTOR;
							alien->flags += FL_ESCAPED;
							alien->active = false;

							if (alien->classDef == CD_CLOAKFIGHTER)
							{
								currentGame.experimentalShield = alien->shield;
								setInfoLine("Experimental Fighter has fled",
									FONT_CYAN);
							}

							alien->shield = 0;
							updateMissionRequirements(M_ESCAPE_TARGET,
								alien->classDef, 1);
						
							if (alien->classDef != CD_CLOAKFIGHTER)
								updateMissionRequirements(M_DESTROY_TARGET_TYPE,
									alien->classDef, 1);
						}
					}

					// This deals with the Experimental Fighter in Mordor
					// (and Kline on the final mission)
					// It can cloak and uncloak at random. When cloaked,
					// its sprite is not displayed. However the engine
					// trail is still visible!
					if ((alien->flags & FL_CANCLOAK) && ((rand() % 500) == 0))
					{
						if (alien->flags & FL_ISCLOAKED)
							alien->flags -= FL_ISCLOAKED;
						else
							alien->flags += FL_ISCLOAKED;
						playSound(SFX_CLOAK, alien->x);
					}

					if (alien->classDef == CD_BARRIER)
					{
						alien->dx = -10 + (sinf(barrierLoop + alien->speed) * 60);
						alien->dy = 20 + (cosf(barrierLoop + alien->speed) * 40);
					}

					if (alien->classDef == CD_MOBILESHIELD)
					{
						limitInt(&(++enemy[WC_BOSS].shield), 0, enemy[WC_BOSS].maxShield);
					}

					limitCharAdd(&alien->reload[0], -1, 0, 999);
					limitCharAdd(&alien->reload[1], -1, 0, 999);

					if ((!(alien->flags & FL_DISABLED)) &&
						(!(alien->flags & FL_NOFIRE)))
					{
						if ((alien->target->shield > 0))
							canFire = alien_checkTarget(alien);

						if (((alien->thinktime % 2) == 0) &&
								(alien->flags & FL_FRIEND))
							canFire = alien_enemiesInFront(alien);
					}
					else
					{
						canFire = false;
					}

					if ((canFire) && (dev.fireAliens))
					{
						if ((alien->reload[0] == 0) &&
							((rand() % 1000 < alien->chance[0]) ||
								(alien->flags & FL_CONTINUOUS_FIRE)))
						{
							fireBullet(alien, 0);
						}
						if ((alien->reload[1] == 0) &&
							((rand() % 1000 < alien->chance[1]) ||
								(alien->flags & FL_CONTINUOUS_FIRE)))
						{
							if ((alien->weaponType[1] != W_ENERGYRAY) &&
								(alien->weaponType[1] != W_LASER))
							{
								if (alien->weaponType[1] == W_CHARGER)
									alien->ammo[1] = 50 + rand() % 150;
								fireBullet(alien, 1);
							}
							else if (alien->weaponType[1] == W_LASER)
							{
								alien->flags += FL_FIRELASER;
							}
							else if ((alien->weaponType[1] == W_ENERGYRAY) &&
								(alien->ammo[0] == 250))
							{
								alien->flags += FL_FIRERAY;
								playSound(SFX_ENERGYRAY, alien->x);
							}
						}
					}

					if (alien->flags & FL_FIRERAY)
					{
						fireRay(alien);
					}
					else
					{
						limitCharAdd(&alien->ammo[0], 1, 0, 250);
					}

					if (alien->flags & FL_FIRELASER)
					{
						fireBullet(alien, 1);
						if ((rand() % 25) == 0)
							alien->flags -= FL_FIRELASER;
					}

					if (alien->flags & FL_DROPMINES)
					{
						if ((rand() % 150) == 0)
							addCollectable(alien->x, alien->y, P_MINE, 25,
								600 + rand() % 2400);

						// Kline drops mines a lot more often
						if ((alien == &enemy[WC_KLINE]))
						{
							if ((rand() % 10) == 0)
								addCollectable(alien->x, alien->y, P_MINE, 25,
									600 + rand() % 2400);
						}
					}

					shapeToUse = alien->imageIndex[alien->face];

					if (alien->hit)
						shapeToUse += SHIP_HIT_INDEX;

					limitCharAdd(&alien->hit, -1, 0, 100);

					if ((alien->x + alien->image[0]->w > 0) &&
						(alien->x < screen->w) &&
						(alien->y + alien->image[0]->h > 0) &&
						(alien->y < screen->h))
					{
						if ((!(alien->flags & FL_DISABLED)) &&
								(alien->classDef != CD_ASTEROID) &&
								(alien->classDef != CD_ASTEROID2))
							addEngine(alien);
						if ((!(alien->flags & FL_ISCLOAKED)) || (alien->hit > 0))
							blit(shipShape[shapeToUse], (int)alien->x,
								(int)alien->y);
						if (alien->flags & FL_DISABLED)
						{
							if ((rand() % 10) == 0)
								addExplosion(alien->x + (rand() % alien->image[0]->w), alien->y + (rand() % alien->image[0]->h), E_ELECTRICAL);
						}
					}

					if ((currentGame.area == 24) && (alien->x < -300))
						alien->active = false;
				}
				else
				{
					alien->shield--;
					if ((alien->x > 0) && (alien->x < screen->w) &&
						(alien->y > 0) && (alien->y < screen->h))
					{
						blit(alien->image[alien->face], (int)alien->x, (int)alien->y);
						addExplosion(alien->x + (rand() % alien->image[0]->w), alien->y + (rand() % alien->image[0]->h), E_BIG_EXPLOSION);
					}
					if (alien->shield < alien->deathCounter)
					{
						alien->active = false;
						if ((alien->classDef == CD_BOSS) ||
								(alien->owner == &enemy[WC_BOSS]) ||
								(alien->flags & FL_FRIEND) ||
								(alien->classDef == CD_ASTEROID) ||
								(alien->classDef == CD_KLINE))
							addDebris((int)alien->x, (int)alien->y,
								alien->maxShield);

						if (alien->classDef == CD_ASTEROID)
						{
							n = 1 + (rand() % 3);
							for (int i = 0 ; i < n ; i++)
								alien_addSmallAsteroid(alien);
						}
					}
				}

				// Adjust the movement even whilst exploding
				if ((dev.moveAliens) && (!(alien->flags & FL_NOMOVE)) &&
						(!(alien->flags & FL_DISABLED)))
					alien_move(alien);

				if ((currentGame.area != 18) || (alien->shield < 0))
					alien->x += engine.ssx;

				alien->x += engine.smx;
				alien->y += engine.ssy + engine.smy;
			}

			alien++;
		}

		doPlayer();
  		doCargo();
  		doDebris();
		doExplosions();
		doInfo();

		wrapChar(&(--engine.eventTimer), 0, 60);

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

		if ((currentGame.area == 24) && (engine.addAliens > -1))
		{
			if ((rand() % 10) == 0)
				addCollectable(rrand(800, 100), player.y, P_MINE, 25,
					180 + rand() % 60);
		}

		if (engine.addAliens > -1)
		{
   		wrapInt(&(--engine.addAliens), 0, currentMission.addAliens);
			if ((engine.addAliens == 0) && (allowableAliens > 0))
			{
				allowableAliens -= alien_add();
			}
		}

		if ((player.shield <= 0) && (engine.missionCompleteTimer == 0))
			engine.missionCompleteTimer = SDL_GetTicks() + 7000;

		// specific to Boss 1
		if ((currentGame.area == 5) && (enemy[WC_BOSS].flags & FL_ESCAPED))
		{
			playSound(SFX_DEATH, enemy[WC_BOSS].x);
			clearScreen(white);
			updateScreen();
			for (int i = 0 ; i < 300 ; i++)
			{
				SDL_Delay(10);
				if ((rand() % 25) == 0)
					playSound(SFX_EXPLOSION, enemy[WC_BOSS].x);
			}
			SDL_Delay(1000);
			break;
		}

		delayFrame();
	}

	flushBuffer();

	if ((player.shield > 0) && (!missionFailed()))
	{
		if (currentGame.area < 26)
			missionFinishedScreen();

		switch (currentGame.area)
		{
			case 5:
				doCutscene(1);
				doCutscene(2);
				break;
			case 7:
				doCutscene(3);
				break;
			case 11:
				doCutscene(4);
				break;
			case 13:
				doCutscene(5);
				break;
			case 18:
				doCutscene(6);
				break;
			case 26:
				doCredits();
				break;
		}
		
		if (currentGame.area < 26)
		{
			updateSystemStatus();
			saveGame(0);
		}

		rtn = 1;
		
		if (currentGame.area == 26)
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
