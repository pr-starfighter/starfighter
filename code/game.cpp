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

Game currentGame;

void newGame()
{
	currentGame.system = 0;
	currentGame.area = 0;
	currentGame.sfxVolume = 0;
	currentGame.musicVolume = 0;

	if (!engine.useAudio)
	{
		currentGame.useSound = false;
		currentGame.useMusic = false;
	}

	currentGame.autoSaveSlot = -1;

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

	currentGame.maxPlasmaRate = 13;
	currentGame.maxPlasmaOutput = 2;
	currentGame.maxPlasmaDamage = 2;
	currentGame.maxPlasmaAmmo = 100;
	currentGame.maxRocketAmmo = 10;

	currentGame.shieldUnits = 1;

	player.maxShield = 25;
	player.shield = 25;
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
	resetLists();

	setMission(currentGame.area);
	missionBriefScreen();

	initCargo();
	initPlayer();
	initAliens();
	clearInfoLines();

	loadScriptEvents();

	engine.ssx = 0;
	engine.ssy = 0;

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

	graphics.drawBackGround();
	graphics.flushBuffer();

	// Default to no aliens dead...
	engine.allAliensDead = 0;

	engine.keyState[SDLK_LCTRL] = engine.keyState[SDLK_RCTRL] = engine.keyState[SDLK_SPACE] = 0;
	flushInput();

	while (engine.done != 1)
	{
		graphics.updateScreen();

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
						Math::limitFloat(&(engine.musicVolume -= 0.2), 0, 100);
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
				Math::limitFloat(&(engine.musicVolume -= 0.2), 0, 100);
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

		graphics.unBuffer();
		doStarfield();
		doCollectables();
		doBullets();
		doAliens();
		doPlayer();
  		doCargo();
  		doDebris();
		doExplosions();
		doInfo();

		Math::wrapChar(&(--engine.eventTimer), 0, 60);

		while (engine.paused)
		{
			engine.done = checkPauseRequest();
			graphics.delayFrame();
		}

		if ((currentGame.area == 24) && (engine.addAliens > -1))
		{
			if ((rand() % 10) == 0)
				addCollectable(Math::rrand(800, 100), player.y, P_MINE, 25, 180 + rand() % 60);
		}

		if (engine.addAliens > -1)
		{
   		Math::wrapInt(&(--engine.addAliens), 0, currentMission.addAliens);
			if ((engine.addAliens == 0) && (allowableAliens > 0))
			{
				allowableAliens -= addAlien();
			}
		}

		if ((player.shield <= 0) && (engine.missionCompleteTimer == 0))
			engine.missionCompleteTimer = SDL_GetTicks() + 7000;

		// specific to Boss 1
		if ((currentGame.area == 5) && (enemy[WC_BOSS].flags & FL_ESCAPED))
		{
			playSound(SFX_DEATH);
			graphics.clearScreen(graphics.white);
			graphics.updateScreen();
			for (int i = 0 ; i < 300 ; i++)
			{
				SDL_Delay(10);
				if ((rand() % 25) == 0)
					playSound(SFX_EXPLOSION);
			}
			SDL_Delay(1000);
			break;
		}

		graphics.delayFrame();
	}

	graphics.flushBuffer();

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

			if (currentGame.autoSaveSlot > -1)
				saveGame(currentGame.autoSaveSlot + 1);
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

	return rtn;
}
