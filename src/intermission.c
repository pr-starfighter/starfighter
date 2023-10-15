/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012, 2013 Guus Sliepen
Copyright (C) 2012, 2015-2023 Diligent Circle <diligentcircle@riseup.net>

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

#include <libintl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"

#include "colors.h"
#include "defs.h"
#include "structs.h"

#include "audio.h"
#include "engine.h"
#include "game.h"
#include "gfx.h"
#include "intermission.h"
#include "player.h"
#include "renderer.h"
#include "save.h"
#include "screen.h"
#include "shop.h"
#include "weapons.h"
#include "window.h"

Planet intermission_planets[MAX_PLANETS];

void intermission_initPlanets(int system)
{
	for (int i = 0 ; i < MAX_PLANETS ; i++)
	{
		intermission_planets[i].missionNumber = -1; // no mission for this planet
		intermission_planets[i].missionCompleted = 1;
	}

	switch(system)
	{
		// Spirit
		case SYSTEM_SPIRIT:
			intermission_planets[PLANET_HAIL].missionNumber = MISN_HAIL;
			intermission_planets[PLANET_HAIL].missionCompleted = 0;

			intermission_planets[PLANET_CERADSE].missionNumber = MISN_CERADSE;
			intermission_planets[PLANET_CERADSE].missionCompleted = 0;

			intermission_planets[PLANET_HINSTAG].missionNumber = MISN_HINSTAG;
			intermission_planets[PLANET_HINSTAG].missionCompleted = 0;

			intermission_planets[PLANET_JOLDAR].missionNumber = MISN_JOLDAR;
			intermission_planets[PLANET_JOLDAR].missionCompleted = 0;

			intermission_planets[PLANET_MOEBO].missionNumber = MISN_MOEBO;
			intermission_planets[PLANET_MOEBO].missionCompleted = -1;

			break;

		// Eyananth
		case SYSTEM_EYANANTH:
			intermission_planets[PLANET_NEROD].missionNumber = MISN_NEROD;
			intermission_planets[PLANET_NEROD].missionCompleted = 0;

			intermission_planets[PLANET_ALLEZ].missionNumber = MISN_ALLEZ;
			intermission_planets[PLANET_ALLEZ].missionCompleted = 0;

			intermission_planets[PLANET_URUSOR].missionNumber = MISN_URUSOR;
			intermission_planets[PLANET_URUSOR].missionCompleted = -1;

			intermission_planets[PLANET_DORIM].missionNumber = MISN_DORIM;
			intermission_planets[PLANET_DORIM].missionCompleted = -1;

			intermission_planets[PLANET_ELAMALE].missionNumber = MISN_ELAMALE;
			intermission_planets[PLANET_ELAMALE].missionCompleted = -2;

			// This one is for the slaves
			intermission_planets[PLANET_RESCUESLAVES].missionNumber = MISN_RESCUESLAVES;
			intermission_planets[PLANET_RESCUESLAVES].missionCompleted = 0;

			break;

		// Mordor
		case SYSTEM_MORDOR:
			intermission_planets[PLANET_ODEON].missionNumber = MISN_ODEON;
			intermission_planets[PLANET_ODEON].missionCompleted = 0;

			intermission_planets[PLANET_FELLON].missionNumber = MISN_FELLON;
			intermission_planets[PLANET_FELLON].missionCompleted = 0;

			intermission_planets[PLANET_SIVEDI].missionNumber = MISN_SIVEDI;
			intermission_planets[PLANET_SIVEDI].missionCompleted = -1;

			intermission_planets[PLANET_ALMARTHA].missionNumber = MISN_ALMARTHA;
			intermission_planets[PLANET_ALMARTHA].missionCompleted = -1;

			intermission_planets[PLANET_POSWIC].missionNumber = MISN_POSWIC;
			intermission_planets[PLANET_POSWIC].missionCompleted = -2;

			intermission_planets[PLANET_ELLESH].missionNumber = MISN_ELLESH;
			intermission_planets[PLANET_ELLESH].missionCompleted = -3;

			// This one is for the experimental fighter
			intermission_planets[PLANET_CLOAKFIGHTER].missionNumber = MISN_CLOAKFIGHTER;
			intermission_planets[PLANET_CLOAKFIGHTER].missionCompleted = 0;

			break;

		// Sol
		case SYSTEM_SOL:
			intermission_planets[PLANET_PLUTO].missionNumber = MISN_PLUTO;
			intermission_planets[PLANET_PLUTO].missionCompleted = 0;

			intermission_planets[PLANET_NEPTUNE].missionNumber = MISN_NEPTUNE;
			intermission_planets[PLANET_NEPTUNE].missionCompleted = 0;

			intermission_planets[PLANET_URANUS].missionNumber = MISN_URANUS;
			intermission_planets[PLANET_URANUS].missionCompleted = 0;

			intermission_planets[PLANET_SATURN].missionNumber = MISN_SATURN;
			intermission_planets[PLANET_SATURN].missionCompleted = -1;

			intermission_planets[PLANET_JUPITER].missionNumber = MISN_JUPITER;
			intermission_planets[PLANET_JUPITER].missionCompleted = -2;

			intermission_planets[PLANET_MARS].missionNumber = MISN_MARS;
			intermission_planets[PLANET_MARS].missionCompleted = -3;

			intermission_planets[PLANET_EARTH].missionNumber = MISN_EARTH;
			intermission_planets[PLANET_EARTH].missionCompleted = -4;

			intermission_planets[PLANET_VENUS].missionNumber = MISN_VENUS;
			intermission_planets[PLANET_VENUS].missionCompleted = -5;

			break;
	}
}

void intermission_unlockPlanets()
{
	for (int i = 0 ; i < MAX_PLANETS ; i++)
	{
		if ((intermission_planets[i].missionCompleted == 0) && (intermission_planets[i].missionNumber != -1))
			return;
	}

	for (int i = 0 ; i < MAX_PLANETS ; i++)
	{
		if (intermission_planets[i].missionCompleted < 0)
			intermission_planets[i].missionCompleted++;
	}
}

void intermission_updateSystemStatus()
{
	if (game.area == MISN_START)
	{
		game.stationedPlanet = 0;
		game.area = 1;
		intermission_initPlanets(game.system);
	}
	else if (game.area == MISN_MOEBO)
	{
		game.stationedPlanet = 0;
		game.system = 1;
		game.area = MISN_RESCUESLAVES;
		game.forceMisnTarget = 1;
		intermission_initPlanets(game.system);

		if (game.difficulty == DIFFICULTY_ORIGINAL)
			player.maxShield = 50;
	}
	else if (game.area == MISN_ELAMALE)
	{
		game.stationedPlanet = 0;
		game.system = 2;
		game.area = MISN_CLOAKFIGHTER;
		game.forceMisnTarget = 1;
		intermission_initPlanets(game.system);

		if (game.difficulty == DIFFICULTY_ORIGINAL)
			player.maxShield = 75;
	}
	else if (game.area == MISN_ELLESH)
	{
		game.stationedPlanet = 8;
		game.system = 3;
		game.area = MISN_PLUTO;
		intermission_initPlanets(game.system);

		if (game.difficulty == DIFFICULTY_ORIGINAL)
			player.maxShield = 100;
	}
	else // Update the mission for the planet
	{
		intermission_planets[game.stationedPlanet].missionCompleted = 1;
	}

	strcpy(game.destinationName, _("No Destination"));
	game.destinationPlanet = game.stationedPlanet;
}

/*
Drives the cursor. Is used by some other screens too
*/
static void intermission_doCursor()
{
	player_getInput();

	LIMIT(engine.cursor_x, 10, screen->w - 10 - gfx_sprites[SP_CURSOR]->w);
	LIMIT(engine.cursor_y, 10, screen->h - 10 - gfx_sprites[SP_CURSOR]->h);
	screen_blit(gfx_sprites[SP_CURSOR], engine.cursor_x, engine.cursor_y);
}

/*
Sets the player's current status information lines. These are the lines
that are scrolled up the screen when the player clicks on Current Status
These are set only once.
*/
static void intermission_setStatusLines()
{
	char string[STRMAX_SHORT];
	char difficulty[STRMAX_SHORT];
	long timeTaken = game.timeTaken;

	game_getDifficultyText(difficulty, game.difficulty);

	/// Status Screen text
	/// Retain "%s" as-is.  It is replaced with the current difficulty.
	snprintf(string, STRMAX_SHORT, _("Difficulty : %s"), difficulty);
	gfx_createTextObject(TS_STATUS_DIFFICULTY, string, 0, 0, FONT_WHITE);

	/// Status Screen text
	/// Retain "%d" as-is.  It is replaced with the number of shots fired.
	snprintf(string, STRMAX_SHORT, _("Shots Fired : %d"), game.shots);
	gfx_createTextObject(TS_SHOTS_FIRED, string, 0, 0, FONT_WHITE);

	/// Status Screen text
	/// Retain "%d" as-is.  It is replaced with the number of hits scored.
	snprintf(string, STRMAX_SHORT, _("Hits Scored : %d"), game.hits);
	gfx_createTextObject(TS_HITS_SCORED, string, 0, 0, FONT_WHITE);

	/// Status Screen text
	/// Retain "%d" as-is.  It is replaced with the firing accuracy as a percentage.
	/// Note: To use the "%" symbol, you must enter "%%", as you can see in
	/// the English version.
	snprintf(string, STRMAX_SHORT, _("Accuracy : %d%%"), game.accuracy);
	gfx_createTextObject(TS_ACCURACY, string, 0, 0, FONT_WHITE);

	/// Status Screen text
	/// Retain "%d" as-is.  It is replaced with the number of kills.
	snprintf(string, STRMAX_SHORT, _("Enemies Killed by Others : %d"), game.totalOtherKills);
	gfx_createTextObject(TS_OTHER_KILLS, string, 0, 0, FONT_WHITE);

	/// Retain "%d" as-is.  It is replaced with the amount of cash earned.
	snprintf(string, STRMAX_SHORT, _("Total Cash Earned : %d"), game.cashEarned);
	gfx_createTextObject(TS_CASH_EARNED, string, 0, 0, FONT_WHITE);

	gfx_createTextObject(TS_CHRIS_HEADER, _("*** Chris ***"), 0, 0, FONT_WHITE);

	/// Status Screen text
	/// Retain "%d" as-is.  It is replaced with the number of kills.
	snprintf(string, STRMAX_SHORT, _("Enemies Killed : %d"), game.totalKills);
	gfx_createTextObject(TS_CHRIS_KILLS, string, 0, 0, FONT_WHITE);

	/// Status Screen text
	/// Retain "%d" as-is.  It is replaced with the number of shield restores picked up.
	snprintf(string, STRMAX_SHORT, _("Shield Restores Picked Up : %d"), game.shieldPickups);
	gfx_createTextObject(TS_CHRIS_SHIELD_PICKUPS, string, 0, 0, FONT_WHITE);

	/// Status Screen text
	/// Retain "%d" as-is.  It is replaced with the number of plasma cells picked up.
	snprintf(string, STRMAX_SHORT, _("Plasma Cells Picked Up : %d"), game.cellPickups);
	gfx_createTextObject(TS_CHRIS_PLASMA_PICKUPS, string, 0, 0, FONT_WHITE);

	/// Status Screen text
	/// Retain "%d" as-is.  It is replaced with the number of rockets picked up.
	snprintf(string, STRMAX_SHORT, _("Rockets Picked Up : %d"), game.rocketPickups);
	gfx_createTextObject(TS_CHRIS_ROCKET_PICKUPS, string, 0, 0, FONT_WHITE);

	/// Status Screen text
	/// Retain "%d" as-is.  It is replaced with the number of powerups picked up.
	snprintf(string, STRMAX_SHORT, _("Powerups Picked Up : %d"), game.powerups);
	gfx_createTextObject(TS_CHRIS_POWERUP_PICKUPS, string, 0, 0, FONT_WHITE);

	/// Status Screen text
	/// Retain "%d" as-is.  It is replaced with the number of mines destroyed.
	snprintf(string, STRMAX_SHORT, _("Mines Destroyed : %d"), game.minesKilled);
	gfx_createTextObject(TS_CHRIS_MINES_KILLED, string, 0, 0, FONT_WHITE);

	/// Status Screen text
	/// Retain "%d" as-is.  It is replaced with the number of slaves rescued.
	snprintf(string, STRMAX_SHORT, _("Slaves Rescued : %d"), game.slavesRescued);
	gfx_createTextObject(TS_CHRIS_SLAVES_RESCUED, string, 0, 0, FONT_WHITE);

	if (game.hasWingMate1)
	{
		gfx_createTextObject(TS_PHOEBE_HEADER, _("*** Phoebe ***"), 0, 0, FONT_WHITE);

		/// Status Screen text
		/// Retain "%d" as-is.  It is replaced with the number of kills.
		snprintf(string, STRMAX_SHORT, _("Enemies Killed : %d"), game.wingMate1Kills);
		gfx_createTextObject(TS_PHOEBE_KILLS, string, 0, 0, FONT_WHITE);

		/// Retain
		/// Status Screen text "%d" as-is.  It is replaced with the number of ejections.
		snprintf(string, STRMAX_SHORT, _("Ejections : %d"), game.wingMate1Ejects);
		gfx_createTextObject(TS_PHOEBE_DEATHS, string, 0, 0, FONT_WHITE);
	}
	else
	{
		gfx_createTextObject(TS_PHOEBE_HEADER, "", 0, 0, FONT_WHITE);
		gfx_createTextObject(TS_PHOEBE_KILLS, "", 0, 0, FONT_WHITE);
		gfx_createTextObject(TS_PHOEBE_DEATHS, "", 0, 0, FONT_WHITE);
	}

	if (game.hasWingMate2)
	{
		/// Status Screen text
		gfx_createTextObject(TS_URSULA_HEADER, _("*** Ursula ***"), 0, 0, FONT_WHITE);

		/// Status Screen text
		/// Retain "%d" as-is.  It is replaced with the number of kills.
		snprintf(string, STRMAX_SHORT, _("Enemies Killed : %d"), game.wingMate2Kills);
		gfx_createTextObject(TS_URSULA_KILLS, string, 0, 0, FONT_WHITE);

		/// Status Screen text
		/// Retain "%d" as-is.  It is replaced with the number of ejections.
		snprintf(string, STRMAX_SHORT, _("Ejections : %d"), game.wingMate2Ejects);
		gfx_createTextObject(TS_URSULA_DEATHS, string, 0, 0, FONT_WHITE);
	}
	else
	{
		gfx_createTextObject(TS_URSULA_HEADER, "", 0, 0, FONT_WHITE);
		gfx_createTextObject(TS_URSULA_KILLS, "", 0, 0, FONT_WHITE);
		gfx_createTextObject(TS_URSULA_DEATHS, "", 0, 0, FONT_WHITE);
	}

	gfx_createTextObject(TS_STATUS_HEADER, _("Current Status"), 0, 0, FONT_WHITE);

	/// Status Screen footer (indicates the total time the game has been played)
	/// "%ld" (which represents hours) and "%02ld" sequences (which
	/// represent minutes and seconds, respectively) must remain and
	/// stay in the same order relative to each other.  The ":"s
	/// between them can be changed to other characters if desired,
	/// e.g. this would be acceptable:
	///
	///     "Total Time : %ld hours, %02ld minutes, %02ld seconds"
	///
	/// If you are familiar with printf formatting, you may also change
	/// the formatting of any of these as long as the "ld" type remains.
	/// For example, the "%02ld" sequences may be changed to "%ld" if
	/// you wish to not force two digits to be filled in (e.g. to render
	/// the number 3 as "3" instead of "03").
	snprintf(string, STRMAX_SHORT, _("Total Time : %ld:%02ld:%02ld"), timeTaken / 3600, (timeTaken / 60) % 60, timeTaken % 60);
	gfx_createTextObject(TS_STATUS_FOOTER, string, 0, 0, FONT_WHITE);
}

/*
Sets the names and stats of the planets within the current system.
*/
static void intermission_setPlanets()
{
	for (int i = 0 ; i < MAX_PLANETS ; i++)
	{
		intermission_planets[i].dist = -1;
		strcpy(intermission_planets[i].name, "");
		intermission_planets[i].image = NULL;
		intermission_planets[i].messageMission = -1;
		intermission_planets[i].messageSlot = -1;
		intermission_planets[i].faceImage = -1;
		strcpy(intermission_planets[i].subject, "ERROR");
	}

	switch (game.system)
	{
		case SYSTEM_SPIRIT:
			intermission_planets[PLANET_HAIL].dist = 3;
			strcpy(intermission_planets[PLANET_HAIL].name, _("Hail"));
			intermission_planets[PLANET_HAIL].image = gfx_sprites[SP_PLANET_GREEN];

			intermission_planets[PLANET_CERADSE].dist = 6;
			strcpy(intermission_planets[PLANET_CERADSE].name, _("Ceradse"));
			intermission_planets[PLANET_CERADSE].image = gfx_sprites[SP_PLANET_BLUE];

			intermission_planets[PLANET_HINSTAG].dist = 6;
			strcpy(intermission_planets[PLANET_HINSTAG].name, _("Hinstag"));
			intermission_planets[PLANET_HINSTAG].image = gfx_sprites[SP_PLANET_RED];

			intermission_planets[PLANET_JOLDAR].dist = 4;
			strcpy(intermission_planets[PLANET_JOLDAR].name, _("Joldar"));
			intermission_planets[PLANET_JOLDAR].image = gfx_sprites[SP_PLANET_GREEN];

			intermission_planets[PLANET_MOEBO].dist = 8;
			strcpy(intermission_planets[PLANET_MOEBO].name, _("Moebo"));
			intermission_planets[PLANET_MOEBO].image = gfx_sprites[SP_PLANET_ORANGE];

			intermission_planets[PLANET_HAIL].messageMission = MISN_HAIL;
			intermission_planets[PLANET_HAIL].messageSlot = 0;
			intermission_planets[PLANET_HAIL].faceImage = FS_KRASS;
			/// Spirit, Hail mission summary
			strcpy(intermission_planets[PLANET_HAIL].subject, _("Destroy WEAPCO training ground"));

			intermission_planets[PLANET_CERADSE].messageMission = MISN_CERADSE;
			intermission_planets[PLANET_CERADSE].messageSlot = 1;
			intermission_planets[PLANET_CERADSE].faceImage = FS_SID;
			/// Spirit, Ceradse mission summary
			strcpy(intermission_planets[PLANET_CERADSE].subject, _("Collect 6 cargo pods"));

			intermission_planets[PLANET_HINSTAG].messageMission = MISN_HINSTAG;
			intermission_planets[PLANET_HINSTAG].messageSlot = 2;
			intermission_planets[PLANET_HINSTAG].faceImage = FS_SID;
			/// Spirit, Hinstag mission summary
			strcpy(intermission_planets[PLANET_HINSTAG].subject, _("Destroy 5 WEAPCO missile boats"));

			intermission_planets[PLANET_JOLDAR].messageMission = MISN_JOLDAR;
			intermission_planets[PLANET_JOLDAR].messageSlot = 3;
			intermission_planets[PLANET_JOLDAR].faceImage = FS_SID;
			/// Spirit, Joldar mission summary
			strcpy(intermission_planets[PLANET_JOLDAR].subject, _("Clear the mine field around Joldar"));

			intermission_planets[PLANET_MOEBO].messageMission = MISN_MOEBO;
			intermission_planets[PLANET_MOEBO].messageSlot = 0;
			intermission_planets[PLANET_MOEBO].faceImage = FS_SID;
			/// Spirit, Moebo mission summary
			strcpy(intermission_planets[PLANET_MOEBO].subject, _("Destroy WEAPCO frigate"));

			break;

		case SYSTEM_EYANANTH:
			strcpy(intermission_planets[PLANET_RESCUESLAVES].name, _("WEAPCO interceptions"));

			intermission_planets[PLANET_NEROD].dist = 3;
			strcpy(intermission_planets[PLANET_NEROD].name, _("Nerod"));
			intermission_planets[PLANET_NEROD].image = gfx_sprites[SP_PLANET_GREEN];

			intermission_planets[PLANET_ALLEZ].dist = 6;
			strcpy(intermission_planets[PLANET_ALLEZ].name, _("Allez"));
			intermission_planets[PLANET_ALLEZ].image = gfx_sprites[SP_PLANET_BLUE];

			intermission_planets[PLANET_URUSOR].dist = 6;
			strcpy(intermission_planets[PLANET_URUSOR].name, _("Urusor"));
			intermission_planets[PLANET_URUSOR].image = gfx_sprites[SP_PLANET_RED];

			intermission_planets[PLANET_DORIM].dist = 4;
			strcpy(intermission_planets[PLANET_DORIM].name, _("Dorim"));
			intermission_planets[PLANET_DORIM].image = gfx_sprites[SP_PLANET_GREEN];

			intermission_planets[PLANET_ELAMALE].dist = 8;
			strcpy(intermission_planets[PLANET_ELAMALE].name, _("Elamale"));
			intermission_planets[PLANET_ELAMALE].image = gfx_sprites[SP_PLANET_ORANGE];

			intermission_planets[PLANET_RESCUESLAVES].messageMission = MISN_RESCUESLAVES;
			intermission_planets[PLANET_RESCUESLAVES].messageSlot = 0;
			intermission_planets[PLANET_RESCUESLAVES].faceImage = FS_SID;
			/// Eyananth, interception mission summary
			strcpy(intermission_planets[PLANET_RESCUESLAVES].subject, _("Rescue slaves"));

			intermission_planets[PLANET_NEROD].messageMission = MISN_NEROD;
			intermission_planets[PLANET_NEROD].messageSlot = 1;
			intermission_planets[PLANET_NEROD].faceImage = FS_PHOEBE;
			/// Eyananth, Nerod mission summary
			strcpy(intermission_planets[PLANET_NEROD].subject, _("SOS"));

			intermission_planets[PLANET_ALLEZ].messageMission = MISN_ALLEZ;
			intermission_planets[PLANET_ALLEZ].messageSlot = 2;
			intermission_planets[PLANET_ALLEZ].faceImage = FS_SID;
			/// Eyananth, Allez mission summary
			strcpy(intermission_planets[PLANET_ALLEZ].subject, _("Assist medical supply craft"));

			intermission_planets[PLANET_URUSOR].messageMission = MISN_URUSOR;
			intermission_planets[PLANET_URUSOR].messageSlot = 0;
			intermission_planets[PLANET_URUSOR].faceImage = FS_SID;
			/// Eyananth, Urusor mission summary
			strcpy(intermission_planets[PLANET_URUSOR].subject, _("Capture five WEAPCO supply craft"));

			intermission_planets[PLANET_DORIM].messageMission = MISN_DORIM;
			intermission_planets[PLANET_DORIM].messageSlot = 1;
			intermission_planets[PLANET_DORIM].faceImage = FS_SID;
			/// Eyananth, Dorim mission summary
			strcpy(intermission_planets[PLANET_DORIM].subject, _("Find WEAPCO scientist"));

			intermission_planets[PLANET_ELAMALE].messageMission = MISN_ELAMALE;
			intermission_planets[PLANET_ELAMALE].messageSlot = 0;
			intermission_planets[PLANET_ELAMALE].faceImage = FS_PHOEBE;
			/// Eyananth, Elamale mission summary
			strcpy(intermission_planets[PLANET_ELAMALE].subject, _("Destroy WEAPCO Ore Mining craft"));

			break;

		case SYSTEM_MORDOR:
			strcpy(intermission_planets[PLANET_CLOAKFIGHTER].name, _("WEAPCO interceptions"));

			intermission_planets[PLANET_ODEON].dist = 3;
			strcpy(intermission_planets[PLANET_ODEON].name, _("Odeon"));
			intermission_planets[PLANET_ODEON].image = gfx_sprites[SP_PLANET_GREEN];

			intermission_planets[PLANET_FELLON].dist = 6;
			strcpy(intermission_planets[PLANET_FELLON].name, _("Fellon"));
			intermission_planets[PLANET_FELLON].image = gfx_sprites[SP_PLANET_BLUE];

			intermission_planets[PLANET_SIVEDI].dist = 6;
			strcpy(intermission_planets[PLANET_SIVEDI].name, _("Sivedi"));
			intermission_planets[PLANET_SIVEDI].image = gfx_sprites[SP_PLANET_RED];

			intermission_planets[PLANET_ALMARTHA].dist = 4;
			strcpy(intermission_planets[PLANET_ALMARTHA].name, _("Almartha"));
			intermission_planets[PLANET_ALMARTHA].image = gfx_sprites[SP_PLANET_GREEN];

			intermission_planets[PLANET_POSWIC].dist = 4;
			strcpy(intermission_planets[PLANET_POSWIC].name, _("Poswic"));
			intermission_planets[PLANET_POSWIC].image = gfx_sprites[SP_PLANET_ORANGE];

			intermission_planets[PLANET_ELLESH].dist = 8;
			strcpy(intermission_planets[PLANET_ELLESH].name, _("Ellesh"));
			intermission_planets[PLANET_ELLESH].image = gfx_sprites[SP_PLANET_GREEN];

			intermission_planets[PLANET_CLOAKFIGHTER].messageMission = MISN_CLOAKFIGHTER;
			intermission_planets[PLANET_CLOAKFIGHTER].messageSlot = 0;
			intermission_planets[PLANET_CLOAKFIGHTER].faceImage = FS_SID;
			/// Mordor, incerception mission summary
			strcpy(intermission_planets[PLANET_CLOAKFIGHTER].subject, _("Destroy experimental fighter"));

			intermission_planets[PLANET_ODEON].messageMission = MISN_ODEON;
			intermission_planets[PLANET_ODEON].messageSlot = 1;
			intermission_planets[PLANET_ODEON].faceImage = FS_PHOEBE;
			/// Mordor, Odeon mission summary
			strcpy(intermission_planets[PLANET_ODEON].subject, _("Rescue Ursula"));

			intermission_planets[PLANET_FELLON].messageMission = MISN_FELLON;
			intermission_planets[PLANET_FELLON].messageSlot = 2;
			intermission_planets[PLANET_FELLON].faceImage = FS_SID;
			/// Mordor, Fellon mission summary
			strcpy(intermission_planets[PLANET_FELLON].subject, _("Assist rebel forces"));

			intermission_planets[PLANET_SIVEDI].messageMission = MISN_SIVEDI;
			intermission_planets[PLANET_SIVEDI].messageSlot = 0;
			intermission_planets[PLANET_SIVEDI].faceImage = FS_SID;
			/// Mordor, Sivedi mission summary
			strcpy(intermission_planets[PLANET_SIVEDI].subject, _("Mine ore from asteroid belt"));

			intermission_planets[PLANET_ALMARTHA].messageMission = MISN_ALMARTHA;
			intermission_planets[PLANET_ALMARTHA].messageSlot = 1;
			intermission_planets[PLANET_ALMARTHA].faceImage = FS_KRASS;
			/// Mordor, Almartha mission summary
			strcpy(intermission_planets[PLANET_ALMARTHA].subject, _("Create a diversion"));

			intermission_planets[PLANET_POSWIC].messageMission = MISN_POSWIC;
			intermission_planets[PLANET_POSWIC].messageSlot = 0;
			intermission_planets[PLANET_POSWIC].faceImage = FS_URSULA;
			/// Mordor, Poswic mission summary
			strcpy(intermission_planets[PLANET_POSWIC].subject, _("Capture WEAPCO executive transport"));

			intermission_planets[PLANET_ELLESH].messageMission = MISN_ELLESH;
			intermission_planets[PLANET_ELLESH].messageSlot = 0;
			intermission_planets[PLANET_ELLESH].faceImage = FS_PHOEBE;
			/// Mordor, Ellesh mission summary
			strcpy(intermission_planets[PLANET_ELLESH].subject, _("Destroy WEAPCO executive transport"));

			break;

		case SYSTEM_SOL:
			intermission_planets[PLANET_MERCURY].dist = 3;
			strcpy(intermission_planets[PLANET_MERCURY].name, _("Mercury"));
			intermission_planets[PLANET_MERCURY].image = gfx_sprites[SP_PLANET_RED];

			intermission_planets[PLANET_VENUS].dist = 4;
			strcpy(intermission_planets[PLANET_VENUS].name, _("Venus"));
			intermission_planets[PLANET_VENUS].image = gfx_sprites[SP_PLANET_ORANGE];

			intermission_planets[PLANET_EARTH].dist = 4;
			strcpy(intermission_planets[PLANET_EARTH].name, _("Earth"));
			intermission_planets[PLANET_EARTH].image = gfx_sprites[SP_PLANET_BLUE];

			intermission_planets[PLANET_MARS].dist = 4;
			strcpy(intermission_planets[PLANET_MARS].name, _("Mars"));
			intermission_planets[PLANET_MARS].image = gfx_sprites[SP_PLANET_RED];

			intermission_planets[PLANET_JUPITER].dist = 6;
			strcpy(intermission_planets[PLANET_JUPITER].name, _("Jupiter"));
			intermission_planets[PLANET_JUPITER].image = gfx_sprites[SP_PLANET_ORANGE];

			intermission_planets[PLANET_SATURN].dist = 4;
			strcpy(intermission_planets[PLANET_SATURN].name, _("Saturn"));
			intermission_planets[PLANET_SATURN].image = gfx_sprites[SP_PLANET_GREEN];

			intermission_planets[PLANET_URANUS].dist = 4;
			strcpy(intermission_planets[PLANET_URANUS].name, _("Uranus"));
			intermission_planets[PLANET_URANUS].image = gfx_sprites[SP_PLANET_BLUE];

			intermission_planets[PLANET_NEPTUNE].dist = 4;
			strcpy(intermission_planets[PLANET_NEPTUNE].name, _("Neptune"));
			intermission_planets[PLANET_NEPTUNE].image = gfx_sprites[SP_PLANET_BLUE];

			intermission_planets[PLANET_PLUTO].dist = 4;
			strcpy(intermission_planets[PLANET_PLUTO].name, _("Pluto"));
			intermission_planets[PLANET_PLUTO].image = gfx_sprites[SP_PLANET_BLUE];

			intermission_planets[PLANET_PLUTO].messageMission = MISN_PLUTO;
			intermission_planets[PLANET_PLUTO].messageSlot = 0;
			intermission_planets[PLANET_PLUTO].faceImage = FS_SID;
			/// Sol, Pluto mission summary
			strcpy(intermission_planets[PLANET_PLUTO].subject, _("Secure Pluto"));

			intermission_planets[PLANET_NEPTUNE].messageMission = MISN_NEPTUNE;
			intermission_planets[PLANET_NEPTUNE].messageSlot = 1;
			intermission_planets[PLANET_NEPTUNE].faceImage = FS_SID;
			/// Sol, Neptune mission summary
			strcpy(intermission_planets[PLANET_NEPTUNE].subject, _("Secure Neptune"));

			intermission_planets[PLANET_URANUS].messageMission = MISN_URANUS;
			intermission_planets[PLANET_URANUS].messageSlot = 2;
			intermission_planets[PLANET_URANUS].faceImage = FS_SID;
			/// Sol, Uranus mission summary
			strcpy(intermission_planets[PLANET_URANUS].subject, _("Secure Uranus"));

			intermission_planets[PLANET_SATURN].messageMission = MISN_SATURN;
			intermission_planets[PLANET_SATURN].messageSlot = 0;
			intermission_planets[PLANET_SATURN].faceImage = FS_SID;
			/// Sol, Saturn mission summary
			strcpy(intermission_planets[PLANET_SATURN].subject, _("Destroy outer defense system"));

			intermission_planets[PLANET_JUPITER].messageMission = MISN_JUPITER;
			intermission_planets[PLANET_JUPITER].messageSlot = 0;
			intermission_planets[PLANET_JUPITER].faceImage = FS_SID;
			/// Sol, Jupiter mission summary
			strcpy(intermission_planets[PLANET_JUPITER].subject, _("Investigate distress call"));

			intermission_planets[PLANET_MARS].messageMission = MISN_MARS;
			intermission_planets[PLANET_MARS].messageSlot = 0;
			intermission_planets[PLANET_MARS].faceImage = FS_SID;
			/// Sol, Mars mission summary
			strcpy(intermission_planets[PLANET_MARS].subject, _("Navigate asteroid belt"));

			intermission_planets[PLANET_EARTH].messageMission = MISN_EARTH;
			intermission_planets[PLANET_EARTH].messageSlot = 0;
			intermission_planets[PLANET_EARTH].faceImage = FS_CHRIS;
			/// Sol, Earth mission summary
			strcpy(intermission_planets[PLANET_EARTH].subject, _("Take back Earth"));

			intermission_planets[PLANET_VENUS].messageMission = MISN_VENUS;
			intermission_planets[PLANET_VENUS].messageSlot = 0;
			intermission_planets[PLANET_VENUS].faceImage = FS_SID;
			/// Sol, Venus mission summary
			strcpy(intermission_planets[PLANET_VENUS].subject, _("Defeat Kline"));

			break;
	}
}

/*
Spins the planets around the sun, spaced according to their dist value
as defined in intermission_setPlanets(). Moving the cursor over the planet
will show their name and their current status
*/
static int intermission_showSystem(float pos, int selectable)
{
	int h = MIN(screen->h * 5 / 6, screen->w);
	SDL_Rect r;
	int printedName = 0;
	int planet = 0;
	int planetSpace = intermission_planets[planet].dist;
	int rtn = 0;

	// Blit the sun
	screen_blit(gfx_sprites[SP_SUN], screen->w / 2 - 30, h / 2 - 30);

	for (int i = h / 10 ; i < h * 3 / 5 ; i+= planetSpace * h / 100)
	{
		pos *= 0.75;

		gfx_drawCircle(screen->w / 2, h / 2, i, screen, darkGrey);

		r.x = (int)(screen->w / 2 + (sinf(pos) * i));
		r.y = (int)(h / 2 + (cosf(pos) * i));
		r.w = h / 50;
		r.h = h / 50;

		r.x -= (intermission_planets[planet].image->w / 2);
		r.y -= (intermission_planets[planet].image->h / 2);
		screen_blit(intermission_planets[planet].image, r.x, r.y);

		if (selectable
				&& game_collision(engine.cursor_x + 13, engine.cursor_y + 13,
					6, 6, r.x, r.y, intermission_planets[planet].image->w,
					intermission_planets[planet].image->h))
		{
			if (!printedName)
			{
				screen_renderUnicode(intermission_planets[planet].name, -1, screen->h - 25, FONT_WHITE);
				printedName = 1;
			}
			if ((engine.keyState[KEY_FIRE]))
			{
				game.destinationPlanet = planet;
				strcpy(game.destinationName, intermission_planets[game.destinationPlanet].name);
				rtn = 1;
				engine.keyState[KEY_FIRE] = 0;
			}
		}

		planet++;
		if (intermission_planets[planet].dist == -1)
			break;
		planetSpace = intermission_planets[planet].dist;
	}

	return rtn;
}

/*
Scrolls the player's current information up the screen. When
the specified status line reaches a certain Y value, the entire
list is reset and the information lines begin again from the bottom
(in other words, they loop around).
*/
static void intermission_showStatus(SDL_Surface *infoSurface)
{
	int status_x = (screen->w - infoSurface->w) / 2;
	int status_y = (screen->h - infoSurface->h) / 2 - 40;
	int y;
	float speed = 0.25;

	if(engine.keyState[KEY_DOWN])
		speed = 1;
	else if(engine.keyState[KEY_UP])
		speed = -1;

	screen_blit(infoSurface, status_x, status_y);

	for (int i = TS_STATUS_HEADER + 1 ; i < TS_STATUS_FOOTER ; i++)
	{
		gfx_textSprites[i].y -= speed;
		if ((gfx_textSprites[i].y > 10) && (gfx_textSprites[i].y < infoSurface->h))
			screen_blitText(i, status_x + 25, status_y - 10);
	}

	if (gfx_textSprites[TS_STATUS_FOOTER - 1].y < -5)
	{
		y = infoSurface->h;
		for (int i = TS_STATUS_HEADER + 1 ; i < TS_STATUS_FOOTER ; i++)
		{
			y += 20;
			if ((i == TS_CHRIS_HEADER) || (i == TS_PHOEBE_HEADER)
					|| (i == TS_URSULA_HEADER))
				y += 25;

			gfx_textSprites[i].y = y;
		}
	}

	screen_drawRect(status_x, status_y, infoSurface->w, 20, 0x00, 0x00, 0x99);

	screen_drawRect(status_x, status_y + infoSurface->h - 10, infoSurface->w, 20, 0x00, 0x00, 0x99);

	screen_blitText(TS_STATUS_HEADER, -1, status_y + 3);
	screen_blitText(TS_STATUS_FOOTER, -1, status_y + infoSurface->h - 6);
}

static void intermission_createCommsSurface(SDL_Surface *comms)
{
	int yStart;
	int yOffset;

	engine.commsSection = 0;

	gfx_drawRect(comms, 0, 0, comms->w - 1, comms->h - 1, 0x00, 0x00, 0x25);

	gfx_renderUnicode(_("+++ CURRENT MISSIONS +++"), -1, 15, FONT_GREEN, 0, comms);
	/// Brief instructions for how to review a mission conversation, shown below the
	/// CURRENT MISSIONS header.
	gfx_renderUnicode(_("click for info"), -1, 35, FONT_WHITE, 0, comms);

	yStart = 60;

	for (int i = 0 ; i < MAX_PLANETS ; i++)
	{
		if ((intermission_planets[i].messageSlot != -1) && (intermission_planets[i].missionCompleted == 0))
		{
			yOffset = intermission_planets[i].messageSlot * 60;
			gfx_drawRect(comms, 0, yStart + yOffset, comms->w - 1, 55, 0x00, 0x00, 0x77);
			gfx_blit(gfx_faceSprites[intermission_planets[i].faceImage], 20, yStart + 5 + yOffset, comms);
			gfx_renderUnicode(intermission_planets[i].name, 80, yStart + 5 + yOffset, FONT_WHITE, 0, comms);
			gfx_renderUnicode(intermission_planets[i].subject, 80, yStart + 25 + yOffset, FONT_CYAN, 0, comms);
		}
	}
}

static int intermission_renderDialog(SDL_Surface *comms, int y, int face, const char *string)
{
	int newY;
	gfx_blit(gfx_faceSprites[face], 10, y, comms);
	newY = gfx_renderUnicode(string, 80, y, FONT_WHITE, 1, comms) + MENU_SPACING;
	if (newY < y + 60)
		newY += (60 - (newY - y));
	return newY;
}

static void intermission_createMissionDetailSurface(SDL_Surface *comms, int missionSlot)
{
	char string[STRMAX];
	int y = 10;
	int misn = -1;

	for (int i = 0 ; i < MAX_PLANETS ; i++)
	{
		if ((intermission_planets[i].messageSlot == missionSlot) && (intermission_planets[i].missionCompleted == 0))
		{
			misn = intermission_planets[i].messageMission;
		}
	}

	if (misn == -1)
		return;

	gfx_drawRect(comms, 0, 0, comms->w - 1, comms->h - 1, 0x00, 0x00, 0x25);

	switch (misn)
	{
		case MISN_HAIL:
			/// Mission dialog: Spirit, Hail (Krass Tyler)
			strcpy(string, _("Hey, boy! You still owe me money for the Firefly I stole for you! But instead, I want you to go to the WEAPCO training ground and destroy all the craft there."));
			y = intermission_renderDialog(comms, y, FS_KRASS, string);

			/// Mission dialog: Spirit, Hail (Chris Bainfield)
			strcpy(string, _("Oh? That's the job I contracted you to do, was it not?"));
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			/// Mission dialog: Spirit, Hail (Krass Tyler)
			strcpy(string, _("I know, but this way we can resolve your debt right now. Do this job, and also collect $500, and we will call it quits. And if you die... well, I guess the ship was not worth stealing! HA HA HA!"));
			y = intermission_renderDialog(comms, y, FS_KRASS, string);

			/// Mission dialog: Spirit, Hail (Chris Bainfield)
			strcpy(string, _("As usual, you take me too lightly, Krass."));
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			break;

		case MISN_CERADSE:
			/// Mission dialog: Spirit, Ceradse (Chris Bainfield)
			strcpy(string, _("Hey, Sid, what's up?"));
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			/// Mission dialog: Spirit, Ceradse (Sid Wilson)
			strcpy(string, _("Chris, I've intercepted a communication from WEAPCO. Seems they're transporting some medical supplies around Ceradse. We need to get hold of those pods to save some lives!"));
			y = intermission_renderDialog(comms, y, FS_SID, string);

			/// Mission dialog: Spirit, Ceradse (Chris Bainfield)
			strcpy(string, _("How many do we need?"));
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			/// Mission dialog: Spirit, Ceradse (Sid Wilson)
			strcpy(string, _("All six, Chris! If you lose even a single one, thousands of people could perish in Spirit within the next few months."));
			y = intermission_renderDialog(comms, y, FS_SID, string);

			break;

		case MISN_HINSTAG:
			/// Mission dialog: Spirit, Hinstag (Chris Bainfield)
			strcpy(string, _("Wow! Missile boats?"));
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			/// Mission dialog: Spirit, Hinstag (Sid Wilson)
			strcpy(string, _("Yup. Looks like WEAPCO is starting to take notice of your actions."));
			y = intermission_renderDialog(comms, y, FS_SID, string);

			/// Mission dialog: Spirit, Hinstag (Chris Bainfield)
			strcpy(string, _("Awesome! This will really put the Firefly's fighting ability to the test!"));
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			/// Mission dialog: Spirit, Hinstag (Sid Wilson)
			strcpy(string, _("Please be careful, Chris. A single missile boat carries enough rockets to level most major cities. Try not to face them head-on, and keep your distance."));
			y = intermission_renderDialog(comms, y, FS_SID, string);

			break;

		case MISN_JOLDAR:
			/// Mission dialog: Spirit, Joldar (Sid Wilson)
			strcpy(string, _("We're going to have to get rid of the mine deployment unit around Joldar. The minefield is stopping interplanetary traffic."));
			y = intermission_renderDialog(comms, y, FS_SID, string);

			/// Mission dialog: Spirit, Joldar (Chris Bainfield)
			strcpy(string, _("Are any fighters around to keep me entertained?"));
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			/// Mission dialog: Spirit, Joldar (Sid Wilson)
			strcpy(string, _("Not at the moment, but that doesn't mean they won't turn up. Be very careful of those mines! They'll only explode when they encounter a ship that's not transmitting a WEAPCO signal. Shoot them down if they get in your way."));
			y = intermission_renderDialog(comms, y, FS_SID, string);

			break;

		case MISN_MOEBO:
			/// Mission dialog: Spirit, Moebo (Sid Wilson)
			strcpy(string, _("We've got a major problem here! WEAPCO has decided to stop our resistance by destroying Spirit! The explosion will incinerate everything in the system! You've got to destroy that frigate before it gets in range!"));
			y = intermission_renderDialog(comms, y, FS_SID, string);

			/// Mission dialog: Spirit, Moebo (Chris Bainfield)
			strcpy(string, _("Damn! I'll get right on it, then!"));
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			/// Mission dialog: Spirit, Moebo (Sid Wilson)
			strcpy(string, _("We're all counting on you, Chris! But remember, they didn't call that thing \"Star Killer\" just because it sounded nice!"));
			y = intermission_renderDialog(comms, y, FS_SID, string);

			break;

		case MISN_RESCUESLAVES:
			/// Mission dialog: Eyananth, interceptions (Sid Wilson)
			/// "%d" must be retained as-is. It is replaced with the number of slaves that
			/// need to be rescued.
			snprintf(string, STRMAX, _("As you know, WEAPCO has many slaves in this system. If we free a large number of them, it might help to spark a rebellion. I estimate that we will need to rescue around %d to make a difference."), SLAVE_RESCUE_TARGET);
			y = intermission_renderDialog(comms, y, FS_SID, string);

			/// Mission dialog: Eyananth, interceptions (Chris Bainfield)
			strcpy(string, _("Most of the slaves are working in ore mines, aren't they?"));
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			/// Mission dialog: Eyananth, interceptions (Sid Wilson)
			strcpy(string, _("Yes, but attacking the mines directly would be dangerous. You'd be better off intercepting slave transports. What you'll have to do is fly around and see if you can intercept a WEAPCO patrol. Of course, they might not be escorting any slave units, so be careful!"));
			y = intermission_renderDialog(comms, y, FS_SID, string);

			break;

		case MISN_NEROD:
			/// Mission dialog: Eyananth, Nerod (Phoebe Lexx)
			strcpy(string, _("Help! This is an SOS! Can anyone hear me?!"));
			y = intermission_renderDialog(comms, y, FS_PHOEBE, string);

			/// Mission dialog: Eyananth, Nerod (Chris Bainfield)
			strcpy(string, _("I'm hearing you loud and clear! What's up?"));
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			/// Mission dialog: Eyananth, Nerod (Phoebe Lexx)
			strcpy(string, _("Oh, thank God! I was intercepted by a large WEAPCO force near Nerod! I'm in need of assistance!"));
			y = intermission_renderDialog(comms, y, FS_PHOEBE, string);

			/// Mission dialog: Eyananth, Nerod (Chris Bainfield)
			strcpy(string, _("I'm on my way!"));
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			break;

		case MISN_ALLEZ:
			/// Mission dialog: Eyananth, Allez (Sid Wilson)
			strcpy(string, _("I've just received another SOS. This one is coming from a supply craft carrying essential medical supplies."));
			y = intermission_renderDialog(comms, y, FS_SID, string);

			/// Mission dialog: Eyananth, Allez (Chris Bainfield)
			strcpy(string, _("Alright, tell 'em I'm on my way."));
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			break;

		case MISN_URUSOR:
			/// Mission dialog: Eyananth, Urusor (Sid Wilson)
			strcpy(string, _("I need some resources before we leave. It'll make life a lot easier in Mordor. Problem is that WEAPCO hoards these parts."));
			y = intermission_renderDialog(comms, y, FS_SID, string);

			/// Mission dialog: Eyananth, Urusor (Chris Bainfield)
			strcpy(string, _("Where can we get them, then?"));
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			/// Mission dialog: Eyananth, Urusor (Sid Wilson)
			strcpy(string, _("There's a big shipment of them nearby. I can disable the supply craft carrying them; I just need you to give me some cover while I do it."));
			y = intermission_renderDialog(comms, y, FS_SID, string);

			/// Mission dialog: Eyananth, Urusor (Chris Bainfield)
			strcpy(string, _("You got it!"));
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			break;

		case MISN_DORIM:
			/// Mission dialog: Eyananth, Dorim (Sid Wilson)
			strcpy(string, _("A WEAPCO scientist just ran off in an escape pod and hid in the asteroid belt. If we capture him, we may be able to get some information about Mordor."));
			y = intermission_renderDialog(comms, y, FS_SID, string);

			/// Mission dialog: Eyananth, Dorim (Chris Bainfield)
			strcpy(string, _("Alright, I'll go look for him. I guess I'll grab some ore along the way."));
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			break;

		case MISN_ELAMALE:
			/// Mission dialog: Eyananth, Elamale (Sid Wilson)
			strcpy(string, _("I've received word that the slaves we rescued have started a rebellion. Looks like the plan worked."));
			y = intermission_renderDialog(comms, y, FS_SID, string);

			/// Mission dialog: Eyananth, Elamale (Phoebe Lexx)
			strcpy(string, _("WEAPCO has an automated mining ship in orbit around Elamale. How about we take it out and cause some confusion?"));
			y = intermission_renderDialog(comms, y, FS_PHOEBE, string);

			/// Mission dialog: Eyananth, Elamale (Chris Bainfield)
			strcpy(string, _("I like that idea!"));
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			/// Mission dialog: Eyananth, Elamale (Sid Wilson)
			strcpy(string, _("It'll work, but be careful."));
			y = intermission_renderDialog(comms, y, FS_SID, string);

			break;

		case MISN_CLOAKFIGHTER:
			/// Mission dialog: Mordor, interceptions (Chris Bainfield)
			strcpy(string, _("What have you found out about that experimental fighter?"));
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			/// Mission dialog: Mordor, interceptions (Sid Wilson)
			strcpy(string, _("It's got some kind of cloaking device that makes it invisible to radar. Could prove hard to track down."));
			y = intermission_renderDialog(comms, y, FS_SID, string);

			/// Mission dialog: Mordor, interceptions (Chris Bainfield)
			strcpy(string, _("I'll just have to run around the system until I find it."));
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			/// Mission dialog: Mordor, interceptions (Sid Wilson)
			strcpy(string, _("It's likely to run away if you engage it in battle, so try and do as much damage to it as possible."));
			y = intermission_renderDialog(comms, y, FS_SID, string);

			break;

		case MISN_ODEON:
			/// Mission dialog: Mordor, Odeon (Phoebe Lexx)
			strcpy(string, _("I've located my sister's ship currently in orbit around Odeon. She's ignoring my hails though."));
			y = intermission_renderDialog(comms, y, FS_PHOEBE, string);

			/// Mission dialog: Mordor, Odeon (Sid Wilson)
			strcpy(string, _("Something's off here. She seems to be travelling freely with a WEAPCO group."));
			y = intermission_renderDialog(comms, y, FS_SID, string);

			/// Mission dialog: Mordor, Odeon (Chris Bainfield)
			strcpy(string, _("Do you think she's turned traitor?"));
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			/// Mission dialog: Mordor, Odeon (Phoebe Lexx)
			strcpy(string, _("No way. She hates WEAPCO with a passion."));
			y = intermission_renderDialog(comms, y, FS_PHOEBE, string);

			/// Mission dialog: Mordor, Odeon (Sid Wilson)
			strcpy(string, _("She must be under some kind of mind control. I've heard of WEAPCO developing a new \"AI training program\" recently. We'd better rescue her!"));
			y = intermission_renderDialog(comms, y, FS_SID, string);

			break;

		case MISN_FELLON:
			/// Mission dialog: Mordor, Fellon (Sid Wilson)
			strcpy(string, _("A rebel group has organized a counter strike. If we can help them secure a victory, it will be a real boost to morale."));
			y = intermission_renderDialog(comms, y, FS_SID, string);

			/// Mission dialog: Mordor, Fellon (Chris Bainfield)
			strcpy(string, _("Awesome! Let's do it!"));
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			/// Mission dialog: Mordor, Fellon (Sid Wilson)
			strcpy(string, _("Just make sure the rebel ships don't all get destroyed."));
			y = intermission_renderDialog(comms, y, FS_SID, string);

			break;

		case MISN_SIVEDI:
			/// Mission dialog: Mordor, Sivedi (Sid Wilson)
			strcpy(string, _("Seems like taking out that WEAPCO mining ship wasn't such a good idea. The ore it collected is needed in weapons production."));
			y = intermission_renderDialog(comms, y, FS_SID, string);

			/// Mission dialog: Mordor, Sivedi (Chris Bainfield)
			strcpy(string, _("Damn! I guess that means I'll have to mine some myself, then, huh?"));
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			/// Mission dialog: Mordor, Sivedi (Sid Wilson)
			strcpy(string, _("Yes. Be careful, Chris. Your weapons weren't designed for that sort of work, after all."));
			y = intermission_renderDialog(comms, y, FS_SID, string);

			break;

		case MISN_ALMARTHA:
			/// Mission dialog: Mordor, Almartha (Chris Bainfield)
			strcpy(string, _("Hey, Krass! I need you to help us out with something. Phoebe and Ursula are taking out key WEAPCO plants. Can you help me create a diversion by wreaking havoc a little bit away from that?"));
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			/// Mission dialog: Mordor, Almartha (Krass Tyler)
			strcpy(string, _("Sure, I can help you out, boy. But I'll be needing my fee..."));
			y = intermission_renderDialog(comms, y, FS_KRASS, string);

			break;

		case MISN_POSWIC:
			/// Mission dialog: Mordor, Poswic (Ursula Lexx)
			strcpy(string, _("My memory is finally back. Here's something interesting: just before I was captured, I found out that WEAPCO is transporting several important executives to Poswic."));
			y = intermission_renderDialog(comms, y, FS_URSULA, string);

			/// Mission dialog: Mordor, Poswic (Sid Wilson)
			strcpy(string, _("We can't let a rare opportunity like this slip through our fingers! I'll need some cover so I can disable that ship."));
			y = intermission_renderDialog(comms, y, FS_SID, string);

			/// Mission dialog: Mordor, Poswic (Chris Bainfield)
			strcpy(string, _("I've got you covered, Sid!"));
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			break;

		case MISN_ELLESH:
			/// Mission dialog: Mordor, Ellesh (Chris Bainfield)
			strcpy(string, _("Phoebe, I need you to keep an eye on things here. I'm going after that ship!"));
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			/// Mission dialog: Mordor, Ellesh (Phoebe Lexx)
			strcpy(string, _("Are you sure you can catch up to it?"));
			y = intermission_renderDialog(comms, y, FS_PHOEBE, string);

			/// Mission dialog: Mordor, Ellesh (Chris Bainfield)
			strcpy(string, _("Absolutely. One thing that's really nice about the Firefly is its speed. I'll see you in a bit!"));
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			break;

		case MISN_PLUTO:
		case MISN_NEPTUNE:
		case MISN_URANUS:
			/// Mission dialog: Sol, Pluto/Neptune/Uranus (Sid Wilson)
			strcpy(string, _("We've got to start from the outside and work our way in. That will give us less chance of being flanked during the final operation."));
			y = intermission_renderDialog(comms, y, FS_SID, string);

			/// Mission dialog: Sol, Pluto/Neptune/Uranus (Phoebe Lexx)
			strcpy(string, _("Sounds like a plan, Sid!"));
			y = intermission_renderDialog(comms, y, FS_PHOEBE, string);

			/// Mission dialog: Sol, Pluto/Neptune/Uranus (Ursula Lexx)
			strcpy(string, _("Better safe than sorry, I guess."));
			y = intermission_renderDialog(comms, y, FS_URSULA, string);

			/// Mission dialog: Sol, Pluto/Neptune/Uranus (Chris Bainfield)
			strcpy(string, _("Boring, but I guess you're right, Sid, as usual."));
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			break;

		case MISN_SATURN:
			/// Mission dialog: Sol, Saturn (Chris Bainfield)
			strcpy(string, _("WEAPCO has set up a highly dangerous defense line between Saturn and Uranus. We'll need to take it out."));
			y = intermission_renderDialog(comms, y, FS_SID, string);

			/// Mission dialog: Sol, Saturn (Ursula Lexx)
			strcpy(string, _("What kind of defense system?"));
			y = intermission_renderDialog(comms, y, FS_URSULA, string);

			/// Mission dialog: Sol, Saturn (Chris Bainfield)
			strcpy(string, _("Several mobile Energy Ray cannons, not unlike the weapon used by the Star Killer back in Spirit."));
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			/// Mission dialog: Sol, Saturn (Phoebe Lexx)
			strcpy(string, _("Best check my ejection system, then!"));
			y = intermission_renderDialog(comms, y, FS_PHOEBE, string);

			break;

		case MISN_JUPITER:
			/// Mission dialog: Sol, Jupiter (Sid Wilson)
			strcpy(string, _("While you were gone I picked up a distress call coming from around Jupiter."));
			y = intermission_renderDialog(comms, y, FS_SID, string);

			/// Mission dialog: Sol, Jupiter (Ursula Lexx)
			strcpy(string, _("Who would be sending out a distress call within Sol?"));
			y = intermission_renderDialog(comms, y, FS_URSULA, string);

			/// Mission dialog: Sol, Jupiter (Chris Bainfield)
			strcpy(string, _("Let's check it out. Even if it's a trap, I think we can handle it."));
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			break;

		case MISN_MARS:
			/// Mission dialog: Sol, Mars (Sid Wilson)
			strcpy(string, _("Chris, we've got a small problem. WEAPCO has deployed a minefield in the asteroid belt. We'll need you to clear a way through."));
			y = intermission_renderDialog(comms, y, FS_SID, string);

			/// Mission dialog: Sol, Mars (Chris Bainfield)
			strcpy(string, _("Alright. I'll radio in once I've cleared a safe path."));
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			break;

		case MISN_EARTH:
			/// Mission dialog: Sol, Earth (Chris Bainfield)
			strcpy(string, _("Okay people, this is the big one. We go in fast and we go in hard. Don't hold back and hit them with everything we've got!"));
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			/// Mission dialog: Sol, Earth (Sid Wilson)
			strcpy(string, _("We've come too far to turn back now. None of us better die out there!"));
			y = intermission_renderDialog(comms, y, FS_SID, string);

			/// Mission dialog: Sol, Earth (Phoebe Lexx)
			strcpy(string, _("Right with you, Chris!"));
			y = intermission_renderDialog(comms, y, FS_PHOEBE, string);

			/// Mission dialog: Sol, Earth (Ursula Lexx)
			strcpy(string, _("WEAPCO'll regret sticking probes into my head!"));
			y = intermission_renderDialog(comms, y, FS_URSULA, string);

			break;

		case MISN_VENUS:
			/// Mission dialog: Sol, Venus (Chris Bainfield)
			strcpy(string, _("Kethlan has run off to Venus. I'm going after him."));
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			/// Mission dialog: Sol, Venus (Sid Wilson)
			strcpy(string, _("Be careful, Chris. We've won the war, but it would be a real shame if you died now!"));
			y = intermission_renderDialog(comms, y, FS_SID, string);

			break;

		default:
			strcpy(string, "Hey, why am I talking to myself? This shouldn't happen! Clearly, this must be a bug.");
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			snprintf(string, STRMAX, "I should go to pr-starfighter.github.io and report this bug there. In that report, I should mention that the mission number is %d.", misn);
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);

			strcpy(string, "Wait, what am I still talking into empty space for? It's not like anyone can hear me...");
			y = intermission_renderDialog(comms, y, FS_CHRIS, string);
	}

	engine.commsSection = 1;
}

static void intermission_doComms(SDL_Surface *comms, int x, int y)
{
	if ((engine.keyState[KEY_FIRE]))
	{
		if (engine.commsSection == 0)
		{
			for (int i = 0 ; i < 4 ; i++)
			{
				if (game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, x, y + 45 + (i * 60), 430, 50))
				{
					intermission_createMissionDetailSurface(comms, i);
					engine.keyState[KEY_FIRE] = 0;
				}
			}
		}
		else
		{
			if (game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, x, y, comms->w, comms->h))
			{
				intermission_createCommsSurface(comms);
				engine.keyState[KEY_FIRE] = 0;
			}
		}
	}
}

static void intermission_createOptions(SDL_Surface *optionsSurface)
{
	SDL_FillRect(optionsSurface, NULL, black);

	gfx_drawRect(optionsSurface, 0, 0, optionsSurface->w - 2, optionsSurface->h - 2, 0x00, 0x00, 0x44);

	gfx_renderUnicode(_("++ OPTIONS ++"), 105, 8, FONT_WHITE, 0, optionsSurface);

	gfx_drawRect(optionsSurface, 190, 45, 50, 22, 0x00, 0x00, 0x00);
	gfx_drawRect(optionsSurface, 250, 45, 50, 22, 0x00, 0x00, 0x00);
	gfx_drawRect(optionsSurface, 20, 45, 150, 22, 0x00, 0x00, 0x00);
	if (engine.useSound)
		gfx_drawRect(optionsSurface, 190, 45, 50, 22, 0xff, 0x00, 0x00);
	else
		gfx_drawRect(optionsSurface, 250, 45, 50, 22, 0xff, 0x00, 0x00);
	gfx_renderUnicode(_("ON"), 207, 50, FONT_WHITE, 0, optionsSurface);
	gfx_renderUnicode(_("OFF"), 263, 50, FONT_WHITE, 0, optionsSurface);
	gfx_renderUnicode(_("SOUND"), 30, 50, FONT_WHITE, 0, optionsSurface);

	gfx_drawRect(optionsSurface, 190, 95, 50, 22, 0x00, 0x00, 0x00);
	gfx_drawRect(optionsSurface, 250, 95, 50, 22, 0x00, 0x00, 0x00);
	gfx_drawRect(optionsSurface, 20, 95, 150, 22, 0x00, 0x00, 0x00);
	if (engine.useMusic)
		gfx_drawRect(optionsSurface, 190, 95, 50, 22, 0xff, 0x00, 0x00);
	else
		gfx_drawRect(optionsSurface, 250, 95, 50, 22, 0xff, 0x00, 0x00);
	gfx_renderUnicode(_("ON"), 207, 100, FONT_WHITE, 0, optionsSurface);
	gfx_renderUnicode(_("OFF"), 263, 100, FONT_WHITE, 0, optionsSurface);
	gfx_renderUnicode(_("MUSIC"), 30, 100, FONT_WHITE, 0, optionsSurface);

 	gfx_drawRect(optionsSurface, 190, 145, 50, 22, 0x00, 0x00, 0x00);
	gfx_drawRect(optionsSurface, 250, 145, 50, 22, 0x00, 0x00, 0x00);
	gfx_drawRect(optionsSurface, 20, 145, 150, 22, 0x00, 0x00, 0x00);
	if (engine.fullScreen)
		gfx_drawRect(optionsSurface, 190, 145, 50, 22, 0xff, 0x00, 0x00);
	else
		gfx_drawRect(optionsSurface, 250, 145, 50, 22, 0xff, 0x00, 0x00);
	gfx_renderUnicode(_("ON"), 207, 150, FONT_WHITE, 0, optionsSurface);
	gfx_renderUnicode(_("OFF"), 263, 150, FONT_WHITE, 0, optionsSurface);
	gfx_renderUnicode(_("FULLSCREEN"), 30, 150, FONT_WHITE, 0, optionsSurface);
}

static void intermission_doOptions(SDL_Surface *optionsSurface, int x, int y)
{
	if ((engine.keyState[KEY_FIRE]))
	{
		if (game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, x + 187, y + 42, 45, 22))
			engine.useSound = 1;

		if (game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, x + 248, y + 42, 45, 22))
			engine.useSound = 0;


		if (game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, x + 187, y + 92, 45, 22))
		{
			engine.useMusic = 1;
#ifdef OLD_MUSIC
			audio_playMusic("music/3DParadise.mod", -1);
#else
			audio_playMusic("music/through_space.ogg", -1);
#endif
		}

		if (game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, x + 248, y + 92, 45, 22))
		{
			engine.useMusic = 0;
			audio_haltMusic();
		}

		if (game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, x + 187, y + 142, 45, 22))
		{
			if (!engine.fullScreen)
			{
				engine_setFullscreen(1);
			}
		}

		if (game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, x + 248, y + 142, 45, 22))
		{
			if (engine.fullScreen)
			{
				engine_setFullscreen(0);
			}
		}
	}
	intermission_createOptions(optionsSurface);
}

/*
Controls the entire intermission screen. This simply draws a background,
stars, gridlines and the icons at the bottom of the screen. Will call
(and continue to call) the specified functions when the player has
selected an icon.
*/
int intermission()
{
	char string[STRMAX_SHORT];

	SDL_Rect r;
	SDL_Rect destRect;
	int distance = 0;
	double interceptionChance;

	int section = 1;

	int x;
	int y;
	int w;

	float orbit_pos = 300;
	int movePlanets = 1;
	int saveSlot = -1;

	int rtn = 0;

	int redrawBackground = 1;

	gfx_free();

	intermission_unlockPlanets(); // double check just to make sure!

	// Tell the game we are not in a mission so
	// do not perform certain keyboard actions
	engine.gameSection = SECTION_INTERMISSION;

	screen_clear(black);
	renderer_update();
	screen_clear(black);

	save_initSlots();

	gfx_loadBackground(systemBackground[game.system]);

	engine.cursor_x = screen->w / 2;
	engine.cursor_y = screen->h / 2;
	gfx_sprites[SP_CURSOR] = gfx_loadImage("gfx/cursor.png");

	// Icons
	gfx_sprites[SP_START_MISSION] = gfx_loadImage("gfx/icon1.png");
	gfx_sprites[SP_MAP] = gfx_loadImage("gfx/icon2.png");
	gfx_sprites[SP_STATUS] = gfx_loadImage("gfx/icon3.png");
	gfx_sprites[SP_SAVE] = gfx_loadImage("gfx/icon4.png");
	gfx_sprites[SP_SHOP] = gfx_loadImage("gfx/icon5.png");
	gfx_sprites[SP_COMM] = gfx_loadImage("gfx/icon6.png");
	gfx_sprites[SP_OPTIONS] = gfx_loadImage("gfx/icon7.png");
	gfx_sprites[SP_EXIT] = gfx_loadImage("gfx/icon8.png");
	gfx_sprites[SP_PLASMA_MAX_OUTPUT] = gfx_loadImage("gfx/icon9.png");
	gfx_sprites[SP_PLASMA_MAX_POWER] = gfx_loadImage("gfx/icon10.png");
	gfx_sprites[SP_PLASMA_MAX_RATE] = gfx_loadImage("gfx/icon11.png");
	gfx_sprites[SP_PLASMA_AMMO] = gfx_loadImage("gfx/icon12.png");
	gfx_sprites[SP_ROCKET_AMMO] = gfx_loadImage("gfx/icon13.png");
	gfx_sprites[SP_PLASMA_MIN_OUTPUT] = gfx_loadImage("gfx/icon14.png");
	gfx_sprites[SP_PLASMA_MIN_POWER] = gfx_loadImage("gfx/icon15.png");
	gfx_sprites[SP_PLASMA_MIN_RATE] = gfx_loadImage("gfx/icon16.png");
	gfx_sprites[SP_PLASMA_MAX_AMMO] = gfx_loadImage("gfx/icon17.png");
	gfx_sprites[SP_ROCKET_MAX_AMMO] = gfx_loadImage("gfx/icon18.png");
	gfx_sprites[SP_DOUBLE_ROCKETS] = gfx_loadImage("gfx/icon19.png");
	gfx_sprites[SP_MICRO_ROCKETS] = gfx_loadImage("gfx/icon20.png");
	gfx_sprites[SP_LASER] = gfx_loadImage("gfx/icon21.png");
	gfx_sprites[SP_HOMING_MISSILE] = gfx_loadImage("gfx/icon22.png");
	gfx_sprites[SP_CHARGER] = gfx_loadImage("gfx/icon23.png");
	gfx_sprites[SP_DOUBLE_HOMING_MISSILES] = gfx_loadImage("gfx/icon24.png");
	gfx_sprites[SP_MICRO_HOMING_MISSILES] = gfx_loadImage("gfx/icon25.png");
	gfx_sprites[SP_GOTO] = gfx_loadImage("gfx/icon26.png");
	gfx_sprites[SP_BUY] = gfx_loadImage("gfx/buyIcon.png");
	gfx_sprites[SP_SELL] = gfx_loadImage("gfx/sellIcon.png");
	gfx_sprites[SP_FIREFLY] = gfx_loadImage("gfx/firefly1.png");

	// Planets 30 - 39
	gfx_sprites[SP_SUN] = gfx_loadImage("gfx/planet_sun.png");
	gfx_sprites[SP_PLANET_GREEN] = gfx_loadImage("gfx/planet_green.png");
	gfx_sprites[SP_PLANET_BLUE] = gfx_loadImage("gfx/planet_blue.png");
	gfx_sprites[SP_PLANET_RED] = gfx_loadImage("gfx/planet_red.png");
	gfx_sprites[SP_PLANET_ORANGE] = gfx_loadImage("gfx/planet_orange.png");

	// Faces
	gfx_faceSprites[FS_CHRIS] = gfx_loadImage("gfx/face_chris.png");
	gfx_faceSprites[FS_SID] = gfx_loadImage("gfx/face_sid.png");
	gfx_faceSprites[FS_KRASS] = gfx_loadImage("gfx/face_krass.png");
	gfx_faceSprites[FS_PHOEBE] = gfx_loadImage("gfx/face_phoebe.png");
	gfx_faceSprites[FS_URSULA] = gfx_loadImage("gfx/face_ursula.png");
	gfx_faceSprites[FS_KLINE] = gfx_loadImage("gfx/face_kline.png");
	gfx_faceSprites[FS_CREW] = gfx_loadImage("gfx/face_crew.png");

	engine.done = ENGINE_RUNNING;
	engine.keyState[KEY_FIRE] = 0;
	engine.ssx = 0;
	engine.ssy = 0;

	intermission_setStatusLines();
	shop_init();
	intermission_setPlanets();

	SDL_Surface *statsSurface = gfx_createAlphaRect(DEFAULT_SCREEN_WIDTH * 7 / 8, DEFAULT_SCREEN_HEIGHT - 200, 0x00, 0x00, 0x99);
	SDL_Surface *savesSurface = gfx_createSurface(350, 300);
	SDL_Surface *optionsSurface = gfx_createSurface(320, 240);
	SDL_Surface *commsSurface = gfx_createSurface(450, 336);

	save_createSurface(savesSurface, -1);
	intermission_createOptions(optionsSurface);
	intermission_createCommsSurface(commsSurface);

	// Remove the Supercharge, if it is there
	if ((game.difficulty > DIFFICULTY_EASY)
		&& (game.difficulty != DIFFICULTY_ORIGINAL))
	{
		weapons[W_PLAYER_WEAPON].reload[0] = MAX(
			weapons[W_PLAYER_WEAPON].reload[0],
			rate2reload[game.maxPlasmaRate]);
		weapons[W_PLAYER_WEAPON].ammo[0] = MIN(weapons[W_PLAYER_WEAPON].ammo[0],
			game.maxPlasmaOutput);
		weapons[W_PLAYER_WEAPON].damage = MIN(weapons[W_PLAYER_WEAPON].damage,
			game.maxPlasmaDamage);
	}

	switch (game.system)
	{
		case SYSTEM_SPIRIT:
			if (game.difficulty == DIFFICULTY_ORIGINAL)
				interceptionChance = 0;
			else
				interceptionChance = 1. / 600.;
			break;
		case SYSTEM_EYANANTH:
			interceptionChance = 1. / 300.;
			break;
		case SYSTEM_MORDOR:
			interceptionChance = 1. / 150.;
			break;
		case SYSTEM_SOL:
			// There is no chance of being interceptted after the final attack on Earth
			if ((game.system == SYSTEM_SOL) && (intermission_planets[2].missionCompleted))
				interceptionChance = 0;
			else
				interceptionChance = 1. / 100.;
			break;
		default:
			interceptionChance = 0;
	}

	if ((engine.useAudio) && (engine.useMusic))
#ifdef OLD_MUSIC
		audio_playMusic("music/3DParadise.mod", -1);
#else
		audio_playMusic("music/through_space.ogg", -1);
#endif

	/// Retain "%s" as-is.  It is replaced with the current system name.
	snprintf(string, STRMAX_SHORT, _("System : %s"), game_systemNames[game.system]);
	gfx_createTextObject(TS_CURRENT_SYSTEM, string, 0, 0, FONT_WHITE);

	gfx_createTextObject(TS_INFO_START_MISSION, _("Start Next Mission"), 0, 0, FONT_WHITE);
	gfx_createTextObject(TS_INFO_GOTO, _("Go to Destination Planet"), 0, 0, FONT_WHITE);
	gfx_createTextObject(TS_INFO_MAP, _("View System Map"), 0, 0, FONT_WHITE);
	gfx_createTextObject(TS_INFO_STATUS, _("Current Status"), 0, 0, FONT_WHITE);
	gfx_createTextObject(TS_INFO_SAVE_GAME, _("Save Game"), 0, 0, FONT_WHITE);
	gfx_createTextObject(TS_INFO_SHOP, _("Upgrade FIREFLY"), 0, 0, FONT_WHITE);
	gfx_createTextObject(TS_INFO_COMMS, _("Missions"), 0, 0, FONT_WHITE);
	gfx_createTextObject(TS_INFO_OPTIONS, _("Options"), 0, 0, FONT_WHITE);
	gfx_createTextObject(TS_INFO_EXIT, _("Exit to Title Screen"), 0, 0, FONT_WHITE);

	/// Retain "%s" as-is.  It is replaced with the name of the planet
	/// the player is currently stationed on.
	snprintf(string, STRMAX_SHORT, _("Stationed At: %s"), intermission_planets[game.stationedPlanet].name);
	gfx_createTextObject(TS_CURRENT_PLANET, string, 90, 0, FONT_WHITE);

	if (game.destinationPlanet > -1)
	{
		/// Retain "%s" as-is.  It is replaced with the name of the planet
		/// the player's destination is currently set to.
		snprintf(string, STRMAX_SHORT, _("Destination: %s"), intermission_planets[game.destinationPlanet].name);
	}
	else
	{
		strcpy(string, _("Destination: None"));
	}
	gfx_createTextObject(TS_DEST_PLANET, string, 0, 0, FONT_WHITE);

	if (game.distanceCovered > 0)
		section = 0;
	else
		player.shield = player.maxShield;

	player_flushInput();
	engine.keyState[KEY_FIRE] = engine.keyState[KEY_ALTFIRE] = 0;
	engine.done = ENGINE_RUNNING;

	while (engine.done == ENGINE_RUNNING)
	{
		renderer_update();

		if (redrawBackground)
		{
			screen_drawBackground();
			redrawBackground = 0;
		}
		else
		{
			screen_unBuffer();
		}

  		game_doStars();

		r.x = 0;
		r.y = 0;
		r.h = screen->h;
		r.w = 1;
		for (int i = 40 ; i < screen->w ; i+= 40)
		{
			r.x = i;
			SDL_FillRect(screen, &r, darkerBlue);
		}

		r.x = 0;
		r.y = 0;
		r.h = 1;
		r.w = screen->w;
		for (int i = 40 ; i < screen->h ; i+= 40)
		{
			r.y = i;
			SDL_FillRect(screen, &r, darkerBlue);
		}


		if (CHANCE(1. / 500))
		{
			engine.ssx = RANDRANGE(-100, 100);
			engine.ssy = RANDRANGE(-100, 100);
			engine.ssx /= 100;
			engine.ssy /= 100;
		}

		screen_blitText(TS_CURRENT_SYSTEM, -1, 15);

		switch(section)
		{
			case 0:
				if (game.stationedPlanet == game.destinationPlanet)
				{
					game.area = intermission_planets[game.stationedPlanet].missionNumber;
					rtn = 2;
					engine.done = ENGINE_CLOSING;
				}
				else
				{
					distance = abs(game.stationedPlanet - game.destinationPlanet);
					if (interceptionChance <= 0)
						distance = 1;

					distance = (5 / distance);
					if (distance < 1)
						distance = 1;

					gfx_createTextObject(TS_CURRENT_PLANET, intermission_planets[game.stationedPlanet].name,
						135, 0, FONT_WHITE);
					gfx_createTextObject(TS_DEST_PLANET, intermission_planets[game.destinationPlanet].name,
						85, 0, FONT_WHITE);

					section = 8;

					destRect.x = 180;
					destRect.y = screen->h - 90;
					destRect.w = 1;
					if (game.distanceCovered > 0)
						destRect.w = (int)game.distanceCovered;
					destRect.h = 20;
				}
				break;

			case 1:
				if (engine.keyState[KEY_ALTFIRE])
				{
					movePlanets = !movePlanets;
					engine.keyState[KEY_ALTFIRE] = 0;
				}

				if (movePlanets)
				{
					orbit_pos += 0.01;
				}

				if (intermission_showSystem(orbit_pos, 1))
				{
					/// Retain "%s" as-is.  It is replaced with the name of the planet
					/// the player's destination is currently set to.
					snprintf(string, STRMAX_SHORT, _("Destination: %s"), intermission_planets[game.destinationPlanet].name);
					gfx_createTextObject(TS_DEST_PLANET, string, 0, 0, FONT_WHITE);
				}

				screen_blitText(TS_CURRENT_PLANET, 0, screen->h - 120);
				if (game.stationedPlanet != game.destinationPlanet)
					screen_blitText(TS_DEST_PLANET, screen->w - 250, screen->h - 120);
				break;

			case 2:
				intermission_showStatus(statsSurface);
				break;

			case 3:
				x = screen->w / 2 - savesSurface->w / 2;
				y = INTERMISSION_YCENTER - savesSurface->h / 2;
				screen_blit(savesSurface, x, y);
				saveSlot = save_showSlots(savesSurface, saveSlot, x, y);
				break;

			case 4:
				shop_show();
				break;

			case 5:
				x = screen->w / 2 - commsSurface->w / 2;
				y = INTERMISSION_YCENTER - commsSurface->h / 2;
				screen_blit(commsSurface, x, y);
				intermission_doComms(commsSurface, x, y);
				break;

			case 6:
				x = screen->w / 2 - optionsSurface->w / 2;
				y = INTERMISSION_YCENTER - optionsSurface->h / 2;
				screen_blit(optionsSurface, x, y);
				intermission_doOptions(optionsSurface, x, y);
				break;

			case 7:
				rtn = 0;
				engine.done = ENGINE_CLOSING;
				break;

			case 8:
				intermission_showSystem(orbit_pos, 0);

				screen_blit(intermission_planets[game.stationedPlanet].image, 150, screen->h - 90);
				screen_blitText(TS_CURRENT_PLANET, 0, screen->h - 120);
				screen_blit(intermission_planets[game.destinationPlanet].image, screen->w - 150, screen->h - 90);
				screen_blitText(TS_DEST_PLANET, screen->w - 250, screen->h - 120);

				game.distanceCovered += distance * (screen->w - 350) / 450.;
				destRect.w = (int)game.distanceCovered;
				SDL_FillRect(screen, &destRect, red);

				if (game.distanceCovered >= screen->w - 350)
				{
					game.stationedPlanet = game.destinationPlanet;
					game.distanceCovered = 0;
					player.shield = player.maxShield;
					/// Retain "%s" as-is.  It is replaced with the name of the planet
					/// the player's destination is currently set to.
					snprintf(string, STRMAX_SHORT, _("Stationed At: %s"),
						intermission_planets[game.stationedPlanet].name);
					gfx_createTextObject(TS_CURRENT_PLANET, string, 90, 0, FONT_WHITE);
					section = 1;
					redrawBackground = 1;
					save(0);
				}
				else if (interceptionChance > 0)
				{
					if (CHANCE(interceptionChance))
					{
						game.area = MISN_INTERCEPTION;
						rtn = 2;
						engine.done = ENGINE_CLOSING;
					}
				}

				break;
		}

		screen_addBuffer(screen->w / 2 - 100, screen->h - 25, 200, 15);

		if (section != 8)
		{
			x = screen->w / 16;
			y = screen->h - 80;
			w = screen->w - 2 * x - 32;
			if ((game.stationedPlanet == game.destinationPlanet)
					&& (!intermission_planets[game.stationedPlanet].missionCompleted))
				screen_blit(gfx_sprites[SP_START_MISSION], x, y);
			else if (game.stationedPlanet != game.destinationPlanet)
				screen_blit(gfx_sprites[SP_GOTO], x, y);

			screen_blit(gfx_sprites[SP_MAP], x + w / 7, y);
			screen_blit(gfx_sprites[SP_STATUS], x + 2 * w / 7, y);
			screen_blit(gfx_sprites[SP_SAVE], x + 3 * w / 7, y);
			screen_blit(gfx_sprites[SP_SHOP], x + 4 * w / 7, y);
			screen_blit(gfx_sprites[SP_COMM], x + 5 * w / 7, y);
			screen_blit(gfx_sprites[SP_OPTIONS], x + 6 * w / 7, y);
			screen_blit(gfx_sprites[SP_EXIT], x + w, y);

			if (game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, x, y, 32, 32)
					&& ((game.stationedPlanet != game.destinationPlanet)
						|| (!intermission_planets[game.stationedPlanet].missionCompleted)))
			{
				if (game.stationedPlanet == game.destinationPlanet)
					screen_blitText(TS_INFO_START_MISSION, -1, screen->h - 25);
				else
					screen_blitText(TS_INFO_GOTO, -1, screen->h - 25);

				if ((engine.keyState[KEY_FIRE]))
				{
					redrawBackground = 1;
					section = 0;
					engine.keyState[KEY_FIRE] = 0;
				}
			}
			else if (game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, x + w / 7, y, 32, 32))
			{
				screen_blitText(TS_INFO_MAP, -1, screen->h - 25);

				if ((engine.keyState[KEY_FIRE]))
				{
					redrawBackground = 1;
					section = 1;
					engine.keyState[KEY_FIRE] = 0;
				}
			}
			else if (game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, x + 2 * w / 7, y, 32, 32))
			{
				screen_blitText(TS_INFO_STATUS, -1, screen->h - 25);

				if ((engine.keyState[KEY_FIRE]))
				{
					redrawBackground = 1;
					section = 2;
					engine.keyState[KEY_FIRE] = 0;
				}
			}
			else if (game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, x + 3 * w / 7, y, 32, 32))
			{
				screen_blitText(TS_INFO_SAVE_GAME, -1, screen->h - 25);

				if ((engine.keyState[KEY_FIRE]))
				{
					redrawBackground = 1;
					section = 3;
					engine.keyState[KEY_FIRE] = 0;
				}
			}
			else if (game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, x + 4 * w / 7, y, 32, 32))
			{
				screen_blitText(TS_INFO_SHOP, -1, screen->h - 25);

				if ((engine.keyState[KEY_FIRE]))
				{
					redrawBackground = 1;
					section = 4;
					engine.keyState[KEY_FIRE] = 0;
				}
			}
			else if (game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, x + 5 * w / 7, y, 32, 32))
			{
				screen_blitText(TS_INFO_COMMS, -1, screen->h - 25);

				if ((engine.keyState[KEY_FIRE]))
				{
					redrawBackground = 1;
					section = 5;
					intermission_createCommsSurface(commsSurface);
					engine.keyState[KEY_FIRE] = 0;
				}
			}
			else if (game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, x + 6 * w / 7, y, 32, 32))
			{
				screen_blitText(TS_INFO_OPTIONS, -1, screen->h - 25);

				if ((engine.keyState[KEY_FIRE]))
				{
					redrawBackground = 1;
					section = 6;
					engine.keyState[KEY_FIRE] = 0;
				}
			}
			else if (game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, x + w, y, 32, 32))
			{
				screen_blitText(TS_INFO_EXIT, -1, screen->h - 25);

				if ((engine.keyState[KEY_FIRE]))
				{
					redrawBackground = 1;
					section = 7;
					engine.keyState[KEY_FIRE] = 0;
				}
			}
		}

		engine.keyState[KEY_FIRE] = 0;
		engine.keyState[KEY_ALTFIRE] = 0;
		intermission_doCursor();

		game_delayFrame();
	}

	audio_haltMusic();
	SDL_FreeSurface(statsSurface);
	SDL_FreeSurface(savesSurface);
	SDL_FreeSurface(optionsSurface);
	SDL_FreeSurface(commsSurface);

	if (game.distanceCovered == 0)
		player.shield = player.maxShield;

	return rtn;
}

