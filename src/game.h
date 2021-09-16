/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011 Guus Sliepen
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

#ifndef GAME_H
#define GAME_H

#include "SDL.h"

#include "defs.h"
#include "structs.h"

typedef struct Game_ {
	int saveFormat;

	int difficulty;

	int system;
	int area;
	int musicVolume;
	int sfxVolume;

	int cash;
	int cashEarned;

	int shots;
	int hits;
	int accuracy;
	int hasWingMate1;
	int hasWingMate2;
	int totalKills;
	int wingMate1Kills;
	int wingMate2Kills;
	int wingMate1Ejects;
	int wingMate2Ejects;
	int totalOtherKills;
	int secondaryMissions;
	int secondaryMissionsCompleted;
	int shieldPickups;
	int rocketPickups;
	int cellPickups;
	int powerups;
	int minesKilled;
	int cargoPickups;

	// slaves for Eyananth
	int slavesRescued;

	// remaining shield for experimental fighter
	int experimentalShield;

	// Whether to force interception targets to appear
	int forceMisnTarget;

	Uint32 timeTaken; // In seconds
	int missionCompleted[MAX_PLANETS];

	int stationedPlanet;
	int destinationPlanet;

	char stationedName[STRMAX_SHORT];
	char destinationName[STRMAX_SHORT];
	double distanceCovered;

	int minPlasmaRate;
	int minPlasmaDamage;
	int minPlasmaOutput;
	int maxPlasmaRate;
	int maxPlasmaDamage;
	int maxPlasmaOutput;
	int maxPlasmaAmmo;
	int maxRocketAmmo;

	// Limits on shop upgrades
	int minPlasmaRateLimit;
	int minPlasmaDamageLimit;
	int minPlasmaOutputLimit;
	int maxPlasmaRateLimit;
	int maxPlasmaDamageLimit;
	int maxPlasmaOutputLimit;
	int maxPlasmaAmmoLimit;
	int maxRocketAmmoLimit;

} Game;

extern Game game;
extern char game_systemNames[SYSTEM_MAX][STRMAX_SHORT];

void game_init();
void game_setStars();
void game_doStars();
void game_doExplosions();
void game_delayFrame();
int game_collision(float x0, float y0, int w0, int h0, float x2, float y2, int w1, int h1);
void game_getDifficultyText(char *dest, int difficulty);
int game_mainLoop();

#endif
