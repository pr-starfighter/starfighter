/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012, 2013 Guus Sliepen
Copyright (C) 2015-2019 Julie Marchant <onpon4@riseup.net>

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

#include <stdio.h>
#include <sys/stat.h>

#include "SDL.h"

#include "defs.h"
#include "structs.h"

#include "engine.h"
#include "game.h"
#include "gfx.h"
#include "intermission.h"
#include "player.h"
#include "save.h"
#include "screen.h"
#include "weapons.h"

static char saveSlot[10][25];

/*
Reads in each save game that it finds and gives it an appropriate
description using the area variable contained in the game binary
data. It returns the slot number (1 - 10) of the most recently
used file. On the title screen, this is used to determine whether
a player can "Continue Current Game" and "Load Saved Game".
*/
int save_initSlots()
{
	char fileName[PATH_MAX];
	int system;
	char stationedName[255];
	int imagePos = screen->h / 3 + 50;
	Game tempGame;
	struct stat fileInfo;
	int modTime = 0;
	int continueSaveIndex = -1;

	FILE *fp;

	//READ SAVE GAME DATA
	for (int i = 0 ; i <= 5 ; i++)
	{
		sprintf(fileName, "%ssave%.2d.sav", engine.configDirectory, i);
		fp = fopen(fileName, "r");
		if (fp != NULL)
		{
			if (fscanf(fp, "%d%*c", &game.saveFormat) < 1)
			{
				printf("Error: Could not determine the version of the save file.\n");
				sprintf(saveSlot[i], "Corrupt Game Data");
			}
			else
			{
				if (i == 0)
				{
					sprintf(saveSlot[i], "AUTOSAVE");
					continueSaveIndex = 0;
				}
				else
				{
					if (fscanf(fp, "%*[^\n]%*c%*[^\n]%*c%d %*d %*d%*c%[^\n]%*c", &system,
							stationedName) < 2)
					{
						sprintf(saveSlot[i], "Corrupt Game Data");
					}
					else
					{
						sprintf(saveSlot[i], "%s, %s", systemNames[system],
							stationedName);
					}
				}

				if (engine.gameSection == SECTION_TITLE)
					gfx_createTextObject(TS_SAVESLOT_0 + i, saveSlot[i], -1,
						imagePos, FONT_WHITE);

				if (stat(fileName, &fileInfo) != -1)
				{
					if (fileInfo.st_mtime > modTime)
						{modTime = fileInfo.st_mtime; continueSaveIndex = i;}
				}
			}
			fclose(fp);
		}
		else
		{
			sprintf(fileName, "%ssave%.2d.dat", engine.configDirectory, i);

			fp = fopen(fileName, "r");
			if (fp == NULL)
			{
				sprintf(saveSlot[i], (i == 0 ? "AUTOSAVE (Empty)" : "Empty"));
				if (engine.gameSection == SECTION_TITLE)
					gfx_createTextObject(TS_SAVESLOT_0 + i, saveSlot[i],
						-1, imagePos, FONT_WHITE);
			}
			else
			{
				if (i == 0)
				{
					sprintf(saveSlot[i], "AUTOSAVE");
					continueSaveIndex = 0;
				}
				else
				{
					if (fread(&tempGame, sizeof(Game), 1, fp) != 1)
					{
						sprintf(saveSlot[i], "Corrupt Game Data");
					}
					else
					{
						sprintf(saveSlot[i], "%s, %s", systemNames[tempGame.system],
							tempGame.stationedName);
					}
				}

				if (engine.gameSection == SECTION_TITLE)
					gfx_createTextObject(TS_SAVESLOT_0 + i, saveSlot[i], -1,
						imagePos, FONT_WHITE);

				if (stat(fileName, &fileInfo) != -1)
				{
					if (fileInfo.st_mtime > modTime)
						{modTime = fileInfo.st_mtime; continueSaveIndex = i;}
				}

				fclose(fp);
			}
		}
		imagePos += 20;
	}

	return continueSaveIndex;
}

/*
Fill in later...
*/
int save_load(int slot)
{
	char filename[PATH_MAX];
	FILE *fp;
	unsigned long timeTaken;

	sprintf(filename, "%ssave%.2d.sav", engine.configDirectory, slot);
	fp = fopen(filename, "r");

	if (fp != NULL)
	{
		if (fscanf(fp, "%d%*c", &game.saveFormat) < 1)
		{
			printf("Error: Could not determine the version of the save file.\n");
			fclose(fp);
			return 0;
		}

		switch (game.saveFormat)
		{
			case 4:
				if ((fscanf(fp, "%d%*c", &game.difficulty) < 1) ||
						(fscanf(fp, "%d %d %d %d %d %d %d %d%*c",
							&game.minPlasmaRateLimit, &game.minPlasmaDamageLimit,
							&game.minPlasmaOutputLimit, &game.maxPlasmaRateLimit,
							&game.maxPlasmaDamageLimit, &game.maxPlasmaOutputLimit,
							&game.maxPlasmaAmmoLimit, &game.maxRocketAmmoLimit) < 8) ||
						(fscanf(fp, "%d %d %d%*c%*[^\n]%*c", &game.system, &game.area,
							&game.stationedPlanet) < 3) ||
						(fscanf(fp, "%d %d%*c", &game.hasWingMate1, &game.hasWingMate2) < 2) ||
						(fscanf(fp, "%d %d %d %d%*c", &player.maxShield,
							&player.ammo[0], &player.ammo[1], &player.weaponType[1]) < 4) ||
						(fscanf(fp, "%d %d %d%*c",
							&weapons[W_PLAYER_WEAPON].ammo[0],
							&weapons[W_PLAYER_WEAPON].damage,
							&weapons[W_PLAYER_WEAPON].reload[0]) < 3) ||
						(fscanf(fp, "%d %d %d %d %d %d %d %d%*c",
							&game.minPlasmaRate, &game.minPlasmaDamage,
							&game.minPlasmaOutput, &game.maxPlasmaRate,
							&game.maxPlasmaDamage, &game.maxPlasmaOutput,
							&game.maxPlasmaAmmo, &game.maxRocketAmmo) < 8) ||
						(fscanf(fp, "%d %d %d %d %d %d %d %d %d %d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d%*c",
							&game.missionCompleted[0], &game.missionCompleted[1],
							&game.missionCompleted[2], &game.missionCompleted[3],
							&game.missionCompleted[4], &game.missionCompleted[5],
							&game.missionCompleted[6], &game.missionCompleted[7],
							&game.missionCompleted[8], &game.missionCompleted[9]) < 10) ||
						(fscanf(fp, "%d%*c", &game.experimentalShield) < 1) ||
						(fscanf(fp, "%d %d%*c", &game.cash, &game.cashEarned) < 2) ||
						(fscanf(fp, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d%*c",
							&game.shots, &game.hits, &game.accuracy, &game.totalKills,
							&game.wingMate1Kills, &game.wingMate2Kills,
							&game.wingMate1Ejects, &game.wingMate2Ejects,
							&game.totalOtherKills, &game.shieldPickups,
							&game.rocketPickups, &game.cellPickups, &game.powerups,
							&game.minesKilled, &game.slavesRescued) < 15) ||
						(fscanf(fp, "%lu%*c", &timeTaken) < 1))
				{
					printf("Warning: Save data is not correctly formatted. Some data may be lost.\n");
				}
				else
				{
					game.timeTaken = (Uint32)(timeTaken);
				}
				game.destinationPlanet = game.stationedPlanet;
				break;
			default:
				printf("Error: Save format version not recognized.\n");
				fclose(fp);
				return 0;
		}

		fclose(fp);
	}
	else
	{
		sprintf(filename, "%ssave%.2d.dat", engine.configDirectory, slot);
		fp = fopen(filename, "rb");

		if (fp == NULL)
			return 0;

		if (fread(&game, sizeof(Game), 1, fp) != 1)
		{
			printf("Save game error. The file was not of the expected format.\n");
			fclose(fp);
			return 0;
		}

		fclose(fp);

		if (game.saveFormat < 2)
			game.difficulty = DIFFICULTY_NORMAL;

		weapons[W_PLAYER_WEAPON] = game.playerWeapon;
		weapons[W_PLAYER_WEAPON].imageIndex[0] = SP_PLASMA_GREEN;
		weapons[W_PLAYER_WEAPON].imageIndex[1] = SP_PLASMA_GREEN;
		player = game.thePlayer;

		save(slot);
		remove(filename);
	}

	// Re-init all the planets in this system...
	intermission_initPlanets(game.system);

	// ... and then override with completition status
	for (int i = 0 ; i < MAX_PLANETS ; i++)
		intermission_planets[i].missionCompleted = game.missionCompleted[i];

	return 1;
}

void save(int slot)
{
	FILE *fp;
	char fileName[PATH_MAX];

	if ((slot < 0) || (slot > 5))
	{
		printf("Error - Saves may only be 0 to 5\n");
		return;
	}

	sprintf(fileName, "%ssave%.2d.sav", engine.configDirectory, slot);
	fp = fopen(fileName, "w");


	game.saveFormat = 4;
	for (int i = 0 ; i < MAX_PLANETS ; i++)
		game.missionCompleted[i] = intermission_planets[i].missionCompleted;

	if (fp != NULL)
	{
		if (fprintf(fp,
				"%d\n"
				"%d\n"
				"%d %d %d %d %d %d %d %d\n"
				"%d %d %d\n"
				"%s\n"
				"%d %d\n"
				"%d %d %d %d\n"
				"%d %d %d\n"
				"%d %d %d %d %d %d %d %d\n"
				"%d %d %d %d %d %d %d %d %d %d 0 0 0 0 0 0 0 0 0 0\n"
				"%d\n"
				"%d %d\n"
				"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n"
				"%lu\n",

				game.saveFormat,

				game.difficulty,

				game.minPlasmaRateLimit, game.minPlasmaDamageLimit,
				game.minPlasmaOutputLimit, game.maxPlasmaRateLimit,
				game.maxPlasmaDamageLimit, game.maxPlasmaOutputLimit,
				game.maxPlasmaAmmoLimit, game.maxRocketAmmoLimit,

				game.system, game.area, game.stationedPlanet,

				intermission_planets[game.stationedPlanet].name,

				game.hasWingMate1, game.hasWingMate2,

				player.maxShield, player.ammo[0], player.ammo[1],
				player.weaponType[1],

				weapons[W_PLAYER_WEAPON].ammo[0], weapons[W_PLAYER_WEAPON].damage,
				weapons[W_PLAYER_WEAPON].reload[0],

				game.minPlasmaRate, game.minPlasmaDamage, game.minPlasmaOutput,
				game.maxPlasmaRate, game.maxPlasmaDamage, game.maxPlasmaOutput,
				game.maxPlasmaAmmo, game.maxRocketAmmo,

				game.missionCompleted[0], game.missionCompleted[1],
				game.missionCompleted[2], game.missionCompleted[3],
				game.missionCompleted[4], game.missionCompleted[5],
				game.missionCompleted[6], game.missionCompleted[7],
				game.missionCompleted[8], game.missionCompleted[9],

				game.experimentalShield,

				game.cash, game.cashEarned,

				game.shots, game.hits, game.accuracy, game.totalKills,
				game.wingMate1Kills, game.wingMate2Kills, game.wingMate1Ejects,
				game.wingMate2Ejects, game.totalOtherKills, game.shieldPickups,
				game.rocketPickups, game.cellPickups, game.powerups,
				game.minesKilled, game.slavesRescued,

				(unsigned long)(game.timeTaken)) <= 0)
		{
			printf("Error Saving Game to Slot %d\n", slot);
		}
		fclose(fp);
	}
	else
	{
		printf("Error Saving Game to Slot %d\n", slot);
	}

	// Recall to update the save slots... (lazy, yes)
	save_initSlots();
	engine.keyState[KEY_FIRE] = 0;
}

void save_createSurface(SDL_Surface *savesSurface, int clickedSlot)
{
	int y = 10;

	gfx_drawRect(savesSurface, 0, 0, 348, 298, 0x00, 0x00, 0x00);

	for (int i = 1 ; i <= 5 ; i++)
	{
		if (clickedSlot == i)
			gfx_drawRect(savesSurface, 5, y, 338, 25, 0x99, 0x00, 0x00);
		else
			gfx_drawRect(savesSurface, 5, y, 338, 25, 0x00, 0x00, 0x99);
		gfx_renderString(saveSlot[i], 70, y + 5, FONT_WHITE, 0, savesSurface);
		y += 30;
	}

	gfx_renderString("*** HELP ***", 120, 170, FONT_WHITE, 0, savesSurface);

	switch (clickedSlot)
	{
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			gfx_drawRect(savesSurface, 5, 265, 100, 25, 0x00, 0x99, 0x00);
			gfx_drawRect(savesSurface, 125, 265, 100, 25, 0x99, 0x99, 0x00);
			gfx_drawRect(savesSurface, 243, 265, 100, 25, 0x99, 0x00, 0x00);
			gfx_renderString("SAVE", 40, 270, FONT_WHITE, 0, savesSurface);
			gfx_renderString("CANCEL", 150, 270, FONT_WHITE, 0, savesSurface);
			gfx_renderString("DELETE", 270, 270, FONT_WHITE, 0, savesSurface);

			gfx_renderString("SAVE will save the game", 17, 200, FONT_WHITE, 0,
				savesSurface);
			gfx_renderString("CANCEL will unselect that slot", 17, 220,
				FONT_WHITE, 0, savesSurface);
			gfx_renderString("DELETE will remove the save", 17, 240,
				FONT_WHITE, 0, savesSurface);
			break;
		case -1:
			gfx_renderString("First click a Save game slot to use", 17, 200,
				FONT_WHITE, 0, savesSurface);
			break;
		case -10:
			gfx_renderString("Game Saved", 130, 200, FONT_WHITE, 0,
				savesSurface);
			break;
		case -11:
			gfx_renderString("Save Deleted", 130, 200, FONT_WHITE, 0,
				savesSurface);
			break;
	}

	engine.keyState[KEY_FIRE] = 0;
}

/*
Displays the save slot available. For use with an interface that
has the cursor enabled. It returns the index number of the slot clicked
so that the function invoking it can perform a load or save on that slot.
*/
int save_showSlots(SDL_Surface *savesSurface, int saveSlot, int x, int y)
{
	int clickedSlot = -1;

	SDL_Rect r;
	r.x = x + 1;
	r.y = y + 15;
	r.w = 348;
	r.h = 25;

	if ((engine.keyState[KEY_FIRE]))
	{
		for (int i = 1 ; i <= 5 ; i++)
		{
			if (game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6,
				r.x, r.y, r.w, r.h))
			{
				clickedSlot = i;
				save_createSurface(savesSurface, i);
			}
			r.y += 30;
		}

		if (game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6,
			x + 15, y + 265, 100, 25))
		{
			save(saveSlot);
			save_createSurface(savesSurface, -10);
		}

		if (game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6,
				x + 135, y + 265, 100, 25))
			save_createSurface(savesSurface, -1);

		if (game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6,
			x + 253, y + 265, 100, 25))
		{
			char filename[PATH_MAX];
			sprintf(filename, "%ssave%.2d.sav", engine.configDirectory,
				saveSlot);
			remove(filename);
			save_initSlots();
			save_createSurface(savesSurface, -11);
		}
	}

	if (clickedSlot > -1)
		saveSlot = clickedSlot;

	return saveSlot;
}
