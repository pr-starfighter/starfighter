/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012, 2013 Guus Sliepen
Copyright (C) 2012, 2015, 2016 onpon4 <onpon4@riseup.net>

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

// XXX: Magic number
Planet systemPlanet[10];
mission currentMission;
static mission missions[MISN_MAX];

void initPlanetMissions(int system)
{
	// XXX: Magic number
	for (int i = 0 ; i < 10 ; i++)
	{
		systemPlanet[i].missionNumber = -1; // no mission for this planet
		systemPlanet[i].missionCompleted = 1;
	}

	switch(system)
	{
		// Spirit
		case SYSTEM_SPIRIT:
			systemPlanet[PLANET_HAIL].missionNumber = MISN_HAIL;
			systemPlanet[PLANET_HAIL].missionCompleted = 0;

			systemPlanet[PLANET_CERADSE].missionNumber = MISN_CERADSE;
			systemPlanet[PLANET_CERADSE].missionCompleted = 0;

			systemPlanet[PLANET_HINSTAG].missionNumber = MISN_HINSTAG;
			systemPlanet[PLANET_HINSTAG].missionCompleted = 0;

			systemPlanet[PLANET_JOLDAR].missionNumber = MISN_JOLDAR;
			systemPlanet[PLANET_JOLDAR].missionCompleted = 0;

			systemPlanet[PLANET_MOEBO].missionNumber = MISN_MOEBO;
			systemPlanet[PLANET_MOEBO].missionCompleted = -1;

			break;

		// Eyananth
		case SYSTEM_EYANANTH:
			systemPlanet[PLANET_NEROD].missionNumber = MISN_NEROD;
			systemPlanet[PLANET_NEROD].missionCompleted = 0;

			systemPlanet[PLANET_ALLEZ].missionNumber = MISN_ALLEZ;
			systemPlanet[PLANET_ALLEZ].missionCompleted = 0;

			systemPlanet[PLANET_URUSOR].missionNumber = MISN_URUSOR;
			systemPlanet[PLANET_URUSOR].missionCompleted = -1;

			systemPlanet[PLANET_DORIM].missionNumber = MISN_DORIM;
			systemPlanet[PLANET_DORIM].missionCompleted = -1;

			systemPlanet[PLANET_ELAMALE].missionNumber = MISN_ELAMALE;
			systemPlanet[PLANET_ELAMALE].missionCompleted = -2;

			// This one is for the slaves
			systemPlanet[PLANET_RESCUESLAVES].missionNumber = MISN_RESCUESLAVES;
			systemPlanet[PLANET_RESCUESLAVES].missionCompleted = 0;

			break;

		// Mordor
		case SYSTEM_MORDOR:
			systemPlanet[PLANET_ODEON].missionNumber = MISN_ODEON;
			systemPlanet[PLANET_ODEON].missionCompleted = 0;

			systemPlanet[PLANET_FELLON].missionNumber = MISN_FELLON;
			systemPlanet[PLANET_FELLON].missionCompleted = 0;

			systemPlanet[PLANET_SIVEDI].missionNumber = MISN_SIVEDI;
			systemPlanet[PLANET_SIVEDI].missionCompleted = -1;

			systemPlanet[PLANET_ALMARTHA].missionNumber = MISN_ALMARTHA;
			systemPlanet[PLANET_ALMARTHA].missionCompleted = -1;

			systemPlanet[PLANET_POSWIC].missionNumber = MISN_POSWIC;
			systemPlanet[PLANET_POSWIC].missionCompleted = -2;

			systemPlanet[PLANET_ELLESH].missionNumber = MISN_ELLESH;
			systemPlanet[PLANET_ELLESH].missionCompleted = -3;

			// This one is for the experimental fighter
			systemPlanet[PLANET_CLOAKFIGHTER].missionNumber = MISN_CLOAKFIGHTER;
			systemPlanet[PLANET_CLOAKFIGHTER].missionCompleted = 0;

			break;

		// Sol
		case SYSTEM_SOL:
			systemPlanet[PLANET_PLUTO].missionNumber = MISN_PLUTO;
			systemPlanet[PLANET_PLUTO].missionCompleted = 0;

			systemPlanet[PLANET_NEPTUNE].missionNumber = MISN_NEPTUNE;
			systemPlanet[PLANET_NEPTUNE].missionCompleted = 0;

			systemPlanet[PLANET_URANUS].missionNumber = MISN_URANUS;
			systemPlanet[PLANET_URANUS].missionCompleted = 0;

			systemPlanet[PLANET_SATURN].missionNumber = MISN_SATURN;
			systemPlanet[PLANET_SATURN].missionCompleted = -1;

			systemPlanet[PLANET_JUPITER].missionNumber = MISN_JUPITER;
			systemPlanet[PLANET_JUPITER].missionCompleted = -2;

			systemPlanet[PLANET_MARS].missionNumber = MISN_MARS;
			systemPlanet[PLANET_MARS].missionCompleted = -3;

			systemPlanet[PLANET_EARTH].missionNumber = MISN_EARTH;
			systemPlanet[PLANET_EARTH].missionCompleted = -4;

			systemPlanet[PLANET_VENUS].missionNumber = MISN_VENUS;
			systemPlanet[PLANET_VENUS].missionCompleted = -5;

			break;
	}
}

void checkForBossMission()
{
	// XXX: Magic number
	for (int i = 0 ; i < 10 ; i++)
	{
		if ((systemPlanet[i].missionCompleted == 0) && (systemPlanet[i].missionNumber != -1))
			return;
	}

	for (int i = 0 ; i < 10 ; i++)
	{
		if (systemPlanet[i].missionCompleted < 0)
			systemPlanet[i].missionCompleted++;
	}
}

void updateSystemStatus()
{
	if (game.area == MISN_START)
	{
		game.stationedPlanet = 0;
		game.area = 1;
		strcpy(game.stationedName, "Hail");
		initPlanetMissions(game.system);
	}
	else if (game.area == MISN_MOEBO)
	{
		game.stationedPlanet = 0;
		game.system = 1;
		game.area = MISN_RESCUESLAVES;
		strcpy(game.stationedName, "Nerod");
		initPlanetMissions(game.system);

		if (game.difficulty == DIFFICULTY_ORIGINAL)
			player.maxShield = 50;
	}
	else if (game.area == MISN_ELAMALE)
	{
		game.stationedPlanet = 0;
		game.system = 2;
		game.area = MISN_CLOAKFIGHTER;
		strcpy(game.stationedName, "Odeon");
		initPlanetMissions(game.system);

		if (game.difficulty == DIFFICULTY_ORIGINAL)
			player.maxShield = 75;
	}
	else if (game.area == MISN_ELLESH)
	{
		game.stationedPlanet = 8;
		game.system = 3;
		game.area = MISN_PLUTO;
		strcpy(game.stationedName, "Pluto");
		initPlanetMissions(game.system);

		if (game.difficulty == DIFFICULTY_ORIGINAL)
			player.maxShield = 100;
	}
	else // Update the mission for the planet
	{
		systemPlanet[game.stationedPlanet].missionCompleted = 1;
	}

	strcpy(game.destinationName, "None");
	game.destinationPlanet = game.stationedPlanet;
}

/*
Mission Completed Variables:

0  : Not Completed
1  : Completed
2  : Just Completed
3  : Constraint
-1 : Mission Failed
-2 : Just Failed

Timer Variable:
1+ : Time in minutes
-1 : Time up
-2 : No timer
*/
static void clearAllMissions()
{
	for (int m = 0 ; m < MISN_MAX ; m++)
	{
		for (int i = 0 ; i < 3 ; i++)
		{
			strcpy(missions[m].primaryObjective[i], "");
			missions[m].primaryType[i] = M_NONE;
			missions[m].target1[i] = -1;
			missions[m].targetValue1[i] = -1;
			missions[m].timeLimit1[i] = -2;
			missions[m].completed1[i] = 1;
		}

		for (int i = 0 ; i < 3 ; i++)
		{
			strcpy(missions[m].secondaryObjective[i], "");
			missions[m].secondaryType[i] = M_NONE;
			missions[m].target2[i] = -1;
			missions[m].targetValue2[i] = -1;
			missions[m].timeLimit2[i] = -2;
			missions[m].completed2[i] = 1;
		}

		missions[m].addAliens = -1;
	}
}

/*
Sets the currentMission object to the mission number the player
is currently on. Timing is assigned if it is required. The rate
at which to add enemies in this mission is also set.
*/
void setMission(int mission)
{
	currentMission = missions[mission];
	engine.minutes = currentMission.timeLimit1[0];

	for (int i = 0 ; i < 3 ; i++)
	{
		if (currentMission.timeLimit1[i] > engine.minutes)
			engine.minutes = currentMission.timeLimit1[i];
		if (currentMission.timeLimit2[i] > engine.minutes)
			engine.minutes = currentMission.timeLimit2[i];

		if (currentMission.completed1[i] == OB_INCOMPLETE)
			currentMission.remainingObjectives1++;
		if (currentMission.completed2[i] == OB_INCOMPLETE)
			currentMission.remainingObjectives1++;
	}

	engine.addAliens = currentMission.addAliens;

	if (engine.minutes > -1)
	{
		engine.timeMission = 1;
		engine.seconds = 0;
	}

	engine.counter2 = 0;
	engine.timeTaken = 0;
}

/*
Call this whenever a mission requires all the remaining aliens to
automatically die
*/
static void mission_killAllEnemies()
{
	for (int i = 0 ; i < ALIEN_MAX ; i++)
	{
		if ((aliens[i].flags & FL_WEAPCO) && (aliens[i].active) &&
				(aliens[i].shield > 0))
			aliens[i].shield = 0;
	}
}

void checkTimer()
{
	for (int i = 0 ; i < 3 ; i++)
	{
		if ((currentMission.timeLimit1[i] == -1) && (currentMission.completed1[i] == OB_INCOMPLETE))
			currentMission.completed1[i] = OB_JUST_FAILED;
	}

	for (int i = 0 ; i < 3 ; i++)
	{
		if ((currentMission.timeLimit2[i] == -1) && (currentMission.completed2[i] == OB_INCOMPLETE))
			currentMission.completed2[i] = OB_JUST_FAILED;
	}

	// Find out if there are any uncompleted missions that require the timer
	engine.timeMission = 0;
	for (int i = 0 ; i < 3 ; i++)
	{
		if ((currentMission.timeLimit1[i] > -1) && (currentMission.completed1[i] == OB_INCOMPLETE))
			engine.timeMission = 1;
		if ((currentMission.timeLimit2[i] > -1) && (currentMission.completed2[i] == OB_INCOMPLETE))
			engine.timeMission = 1;
	}

	// specific to Spirit Boss
	if ((game.area == MISN_MOEBO) &&
			(currentMission.completed1[0] < OB_INCOMPLETE))
		engine.timeMission = 1;

	// specific to the Asteroid belt
	if ((game.area == MISN_MARS) &&
		(currentMission.completed1[0] < OB_INCOMPLETE))
	{
		currentMission.completed1[0] = OB_COMPLETED;
		mission_killAllEnemies();
		engine.addAliens = -1;
		setInfoLine("*** All Primary Objectives Completed ***", FONT_GREEN);
	}
}

static void evaluateRequirement(int type, int id, int *completed, int *targetValue, int fontColor)
{
	char message[25];

	if ((*targetValue <= 0) && (type != M_PROTECT_TARGET) &&
			(type != M_PROTECT_PICKUP))
	{
		*completed = OB_JUST_COMPLETED;
		checkTimer();
		if ((game.area == MISN_URUSOR) && (type == M_DISABLE_TARGET))
			setRadioMessage(FS_SID, "All vessels disabled!", 1);
	}
	else
	{
		strcpy(message, "");
		switch(type)
		{
			case M_COLLECT:
				switch(id)
				{
					case P_CASH:
						sprintf(message, "Collect $%d more...", *targetValue);
						if ((rand() % 2) == 0)
							sprintf(message, "$%d more to go...", *targetValue);
						break;
					case P_CARGO:
						sprintf(message, "Collect %d more...", *targetValue);
						if ((rand() % 2) == 0)
							sprintf(message, "%d more to go...", *targetValue);
						break;
					case P_ORE:
						sprintf(message, "Collect %d more...", *targetValue);
						if ((rand() % 2) == 0)
							sprintf(message, "%d more to go...", *targetValue);
						break;
				}
				break;
			case M_PROTECT_PICKUP:
				*completed = OB_JUST_FAILED;
				switch(id)
				{
					case P_CARGO:
						sprintf(message, "Cargo pod destroyed!");
						if (game.area == MISN_CERADSE) // Get lectured by Sid
							setRadioMessage(FS_SID, "Chris, we needed that pod! I told you that we couldn't afford to lose a single one!", 1);
						break;
					case P_ESCAPEPOD:
						sprintf(message, "Escape Pod lost!");
						if (game.area == MISN_ODEON) // Get lectured by Phoebe
							setRadioMessage(FS_PHOEBE, "No... Ursula...", 1);
						break;
				}
				break;
			case M_PROTECT_TARGET:
				if (*targetValue <= 0)
				{
					*completed = OB_JUST_FAILED;
					switch (game.area)
					{
						case MISN_NEROD:
							setRadioMessage(FS_SID, "Dammit, Chris! We just lost her!", 1);
							break;
						case MISN_ALLEZ:
							setRadioMessage(FS_CREW, "Noooo!! Hull bre...", 1);
							break;
						case MISN_URUSOR:
							setRadioMessage(FS_SID, "Chris, we've got to disable them, not destroy them!!", 1);
							break;
					}
				}
				break;
			case M_DESTROY_TARGET_TYPE:
				if ((*targetValue <= 10) || (*targetValue % 10 == 0))
				{
					if (CHANCE(0.5))
						sprintf(message, "%d more to go...", *targetValue);
					else
						sprintf(message, "Destroy %d more...", *targetValue);
				}
				break;
			case M_DISABLE_TARGET:
				sprintf(message, "Disable %d more...", *targetValue);
				break;
		}

		if (strcmp(message, "") != 0)
			setInfoLine(message, fontColor);
	}
}

void updateMissionRequirements(int type, int id, int value)
{
	// Can't complete missions if you're dead!
	if (player.shield <= 0)
		return;

	char message[25];
	char matched = 0;

	// We don't need to worry here since if Sid dies,
	// you will automatically fail the mission(!)
	if ((type == M_DESTROY_TARGET_TYPE) && (id == CD_SID))
	{
		setInfoLine("Sid has been killed!!", FONT_RED);
		setRadioMessage(FS_CHRIS, "Sid... I... I'm sorry...", 1);
		currentMission.completed1[0] = OB_JUST_FAILED;
	}

	for (int i = 0 ; i < 3 ; i++)
	{
		if ((currentMission.completed1[i] == OB_INCOMPLETE) || (currentMission.completed1[i] == OB_CONDITION))
		{
			if ((currentMission.primaryType[i] == type) &&
					((currentMission.target1[i] == id) ||
						(currentMission.target1[i] == CD_ANY)))
			{
				matched = 1;
				currentMission.targetValue1[i] -= value;
				evaluateRequirement(type, id, &currentMission.completed1[i], &currentMission.targetValue1[i], FONT_CYAN);
			}
		}
	}

	// Don't evaluate secondary objectives at the same time!
	if (matched)
		return;

	for (int i = 0 ; i < 3 ; i++)
	{
		if ((currentMission.completed2[i] == OB_INCOMPLETE) || (currentMission.completed2[i] == OB_CONDITION))
		{
			if ((currentMission.secondaryType[i] == type) &&
					((currentMission.target2[i] == id) ||
						(currentMission.target2[i] == CD_ANY)))
			{
				currentMission.targetValue2[i] -= value;
				evaluateRequirement(type, id, &currentMission.completed2[i], &currentMission.targetValue2[i], FONT_YELLOW);
				return;
			}
		}
	}

	// Special Case - Interceptions
	if (game.area == MISN_INTERCEPTION)
	{
		if ((type == M_COLLECT) && (id == P_SLAVES))
		{
			if (systemPlanet[9].missionCompleted == 0)
			{
				if (game.slavesRescued >= 250)
				{
					setInfoLine("*** Slaves Rescued - Mission Completed ***", FONT_GREEN);
					systemPlanet[9].missionCompleted = 1;
				}
				else
				{
					sprintf(message, "Rescue %d more...", 250 - game.slavesRescued);
					setInfoLine(message, FONT_CYAN);
				}
			}
		}

		if ((type == M_DESTROY_TARGET_TYPE) && (id == CD_CLOAKFIGHTER))
		{
			setInfoLine("*** Experimental Fighter Destroyed - Mission Completed ***", FONT_GREEN);
			systemPlanet[9].missionCompleted = 1;
			setRadioMessage(FS_CHRIS, "That's one less suprise that WEAPCO can spring on us!", 1);
			game.experimentalShield = 0;
		}
	}
}

/*
This is only used as few times in the game.
Missions 11 and 23 to be exact!
*/
static int revealHiddenObjectives()
{
	int allDone = 1;
	char string[255] = "";

	for (int i = 0 ; i < 3 ; i++)
	{
		if (currentMission.completed1[i] == OB_HIDDEN)
		{
			currentMission.completed1[i] = OB_INCOMPLETE;
			sprintf(string, "New Objective - %s", currentMission.primaryObjective[i]);
			setInfoLine(string, FONT_CYAN);
			allDone = 0;
		}
	}

	if (!allDone)
	{
		// Activate Kline!! :)
		if (game.area == MISN_ELAMALE)
		{
			mission_killAllEnemies();
			events_sync();
			aliens[ALIEN_KLINE].active = true;
			aliens[ALIEN_KLINE].x = player.x + 1000;
			aliens[ALIEN_KLINE].y = player.y;
			aliens[ALIEN_KLINE].flags |= FL_IMMORTAL | FL_NOFIRE;
			player_setTarget(ALIEN_KLINE);
			audio_playMusic("music/last_cyber_dance.ogg", -1);
		}
	}

	return allDone;
}

bool allMissionsCompleted()
{
	for (int i = 0 ; i < 3 ; i++)
	{
		if (currentMission.completed1[i] == OB_INCOMPLETE)
		{
			if ((currentMission.primaryType[i] == M_DESTROY_ALL_TARGETS) && (engine.allAliensDead) && (currentMission.remainingObjectives1 + currentMission.remainingObjectives2 == 1))
			{
				currentMission.completed1[i] = OB_JUST_COMPLETED;
				checkTimer();
			}
		}
	}

	for (int i = 0 ; i < 3 ; i++)
	{
		if (currentMission.completed2[i] == OB_INCOMPLETE)
		{
			if ((currentMission.secondaryType[i] == M_DESTROY_ALL_TARGETS) && (engine.allAliensDead) && (currentMission.remainingObjectives1 + currentMission.remainingObjectives2 == 1))
			{
				currentMission.completed2[i] = OB_JUST_COMPLETED;
				checkTimer();
			}
		}
	}

	for (int i = 0 ; i < 3 ; i++)
	{
		if (currentMission.completed1[i] == OB_JUST_COMPLETED)
		{
			if (currentMission.remainingObjectives1 > 1)
			{
				if ((game.area != MISN_POSWIC) || (i != 1))
					setInfoLine("*** Primary Objective Completed ***", FONT_GREEN);
				else
					setInfoLine(">>> Primary Objective Failed <<<", FONT_RED);
				currentMission.completed1[i] = OB_COMPLETED;
			}
			else
			{
				if (game.area != MISN_INTERCEPTION)
					setInfoLine("*** All Primary Objectives Completed ***", FONT_GREEN);
				else
					setInfoLine("*** Interception Destroyed ***", FONT_GREEN);
				currentMission.completed1[i] = OB_COMPLETED;

				// do some area specific things
				if ((game.area == MISN_MOEBO) ||
					(game.area == MISN_DORIM) ||
					(game.area == MISN_ELLESH) ||
					(game.area == MISN_MARS))
				{
					if (currentMission.remainingObjectives2 == 0)
					{
						mission_killAllEnemies();
						engine.addAliens = -1;
					}
				}

				if (game.area == MISN_EARTH)
					setRadioMessage(FS_CHRIS, "You guys stay here and keep things under control. I'm going after Kethlan!", 1);
			}
		}

		if (currentMission.completed2[i] == OB_JUST_COMPLETED)
		{
			if (currentMission.remainingObjectives2 > 1)
			{
				setInfoLine("*** Secondary Objective Completed ***", FONT_GREEN);
				currentMission.completed2[i] = OB_COMPLETED;
			}
			else
			{
				setInfoLine("*** All Secondary Objectives Completed ***", FONT_GREEN);
				currentMission.completed2[i] = OB_COMPLETED;

				// do some area specific things
				if ((game.area == MISN_DORIM) &&
					(currentMission.remainingObjectives1 == 0))
				{
					mission_killAllEnemies();
					engine.addAliens = -1;
				}
			}
		}

		if (currentMission.completed1[i] == OB_JUST_FAILED)
		{
			setInfoLine(">>> MISSION FAILED <<<", FONT_RED);
			currentMission.completed1[i] = OB_FAILED;
		}

		if (currentMission.completed2[i] == OB_JUST_FAILED)
		{
			setInfoLine(">>> Secondary Objective Failed <<<", FONT_RED);
			currentMission.completed2[i] = OB_FAILED;
		}
	}

	int remaining;
	bool add = false;
	bool allDone = true;

	// Zero objective list for a recount
	currentMission.remainingObjectives1 = currentMission.remainingObjectives2 = 0;

	for (int i = 0 ; i < 3 ; i++)
	{
		if (currentMission.primaryType[i] != M_NONE)
		{
			if (currentMission.completed1[i] == OB_INCOMPLETE)
			{
				currentMission.remainingObjectives1++;
				if (currentMission.primaryType[i] == M_DESTROY_ALL_TARGETS)
					add = true;
				allDone = false;
			}

			if (currentMission.completed1[i] < OB_INCOMPLETE)
				return false;
		}
		if (currentMission.secondaryType[i] != M_NONE)
		{
			if (currentMission.completed2[i] == OB_INCOMPLETE)
			{
				currentMission.remainingObjectives2++;
				if (currentMission.secondaryType[i] == M_DESTROY_ALL_TARGETS)
					add = true;
				allDone = false;
			}
		}
	}

	if (allDone)
		allDone = revealHiddenObjectives();

	remaining = currentMission.remainingObjectives1 + currentMission.remainingObjectives2;

	// We've only got one objective left and it's destroy all targets,
	// so stop adding aliens (otherwise it might be impossible to finish!)
	if ((add) && (remaining == 1))
		engine.addAliens = -1;

	return allDone;
}

bool missionFailed()
{
	for (int i = 0 ; i < 3 ; i++)
	{
		if (currentMission.completed1[i] < OB_INCOMPLETE)
		{
			return true;
		}
	}

	return false;
}

static void drawBriefScreen()
{
	SDL_Rect r = {0, 0, (uint16_t)screen->w, 2};

	for (int i = 0 ; i < (int)(screen->h / 4) - 30 ; i++)
	{
		r.y = (i * 2) + 62; // Not a typo; a black gap is left in the middle if it's 60.
		SDL_FillRect(screen, &r, SDL_MapRGB(screen->format, 0, i, 0));
		r.y = (screen->h - (i * 2) - 60);
		SDL_FillRect(screen, &r, SDL_MapRGB(screen->format, 0, i, 0));
	}

	screen_drawRect(140, 70, 500, 20, 0x00, 0x77, 0x00);
	screen_drawRect(140, 90, 500, 130, 0x00, 0x33, 0x00);
	screen_renderString("Primary Objectives", 150, 74, FONT_WHITE);

	for (int i = 0 ; i < 3 ; i++)
	{
		if ((currentMission.primaryType[i] != M_NONE) && (currentMission.completed1[i] != OB_HIDDEN))
		{
			screen_renderString(currentMission.primaryObjective[i], 160, 114 + (i * 30), FONT_WHITE);
		}
	}

	if (currentMission.secondaryType[0] != M_NONE)
	{
		screen_drawRect(140, 230, 500, 20, 0x00, 0x77, 0x77);
		screen_drawRect(140, 250, 500, 130, 0x00, 0x33, 0x33);
		screen_renderString("Secondary Objectives", 150, 234, FONT_WHITE);

		for (int i = 0 ; i < 3 ; i++)
		{
			if (currentMission.secondaryType[i] != M_NONE)
			{
				screen_renderString(currentMission.secondaryObjective[i], 160, 274 + (i * 30), FONT_WHITE);
				game.secondaryMissions++;
			}
		}
	}

	screen_drawRect(140, 390, 500, 20, 0x77, 0x77, 0x00);
	screen_drawRect(140, 410, 500, 130, 0x33, 0x33, 0x00);
	screen_renderString("Additional Information", 150, 394, FONT_WHITE);
}

/*
Simply displays a screen with all the mission information on it, pulled
back from the data stored in the currentMission object. The music for the
mission begins playing here.
*/
void missionBriefScreen()
{
	screen_clear(black);
	renderer_update();

	if (game.area != MISN_INTERCEPTION)
	{
		screen_clear(black);
		drawBriefScreen();

		if (currentMission.timeLimit1[0] > 0)
		{
			char temp[50];
			if (game.area != MISN_MARS)
				sprintf(temp, "TIME LIMIT: %d minutes", currentMission.timeLimit1[0]);
			else
				sprintf(temp, "SURVIVAL FOR %d minutes", currentMission.timeLimit1[0]);
			screen_renderString(temp, -1, 500, FONT_RED);
		}

		switch (game.area)
		{
			case MISN_URUSOR:
			case MISN_DORIM:
			case MISN_SIVEDI:
			case MISN_ALMARTHA:
			case MISN_ELLESH:
			case MISN_MARS:
			case MISN_VENUS:
				screen_renderString("Phoebe Lexx will not be present", 160, 420, FONT_WHITE);
				if (game.hasWingMate2)
					screen_renderString("Ursula Lexx will not be present", 160, 450, FONT_WHITE);
				break;
		}

		if ((game.area == MISN_URUSOR) ||
				(game.area == MISN_POSWIC) ||
				(game.area == MISN_EARTH))
			screen_renderString("Sid Wilson will join you on this mission", 160, 480, FONT_WHITE);

		renderer_update();
	}

	loadGameGraphics();
	gfx_createTextObject(TS_SHIELD, "Shield", screen->w / 32, screen->h - 50, FONT_WHITE);
	gfx_createTextObject(TS_PLASMA_T, "Plasma:", screen->w * 5 / 16, screen->h - 50, FONT_WHITE);

	if (player.weaponType[1] == W_CHARGER)
		gfx_createTextObject(TS_AMMO_T, "Charge", screen->w / 2, screen->h - 50, FONT_WHITE);
	else if (player.weaponType[1] == W_LASER)
		gfx_createTextObject(TS_AMMO_T, "Heat", screen->w / 2 + 20, screen->h - 50, FONT_WHITE);
	else
		gfx_createTextObject(TS_AMMO_T, "Rockets:", screen->w / 2, screen->h - 50, FONT_WHITE);

	gfx_createTextObject(TS_TARGET, "Target", screen->w * 11 / 16, screen->h - 50, FONT_WHITE);
	gfx_createTextObject(TS_TARGET_SID, "Sid", screen->w * 11 / 16 + 27, screen->h - 50, FONT_WHITE);
	gfx_createTextObject(TS_TARGET_PHOEBE, "Phoebe", screen->w * 11 / 16, screen->h - 50, FONT_WHITE);
	gfx_createTextObject(TS_TARGET_KLINE, "Kline", screen->w * 11 / 16 + 9, screen->h - 50, FONT_WHITE);
	gfx_createTextObject(TS_CASH_T, "Cash: $", 25, 20, FONT_WHITE);
	gfx_createTextObject(TS_OBJECTIVES_T, "Objectives Remaining:", screen->w - 250, 20, FONT_WHITE);
	gfx_createTextObject(TS_TIME_T, "Time Remaining - ", screen->w / 2 - 140, 20, FONT_WHITE);
	gfx_createTextObject(TS_POWER, "Power", screen->w / 32, screen->h - 30, FONT_WHITE);
	gfx_createTextObject(TS_OUTPUT, "Output", screen->w * 5 / 16, screen->h - 30, FONT_WHITE);
	gfx_createTextObject(TS_COOLER, "Cooler", screen->w * 97 / 160, screen->h - 30, FONT_WHITE);
	audio_playRandomTrack();

	if (game.area != MISN_INTERCEPTION)
	{
		screen_renderString("PRESS ENTER TO CONTINUE...", -1, 550, FONT_WHITE);

		renderer_update();

		flushInput();
		engine.done = 0;
		engine.keyState[KEY_FIRE] = 0;
		engine.keyState[KEY_ALTFIRE] = 0;
		engine.keyState[KEY_ESCAPE] = 0;

		while (true)
		{
			game_delayFrame();
			getPlayerInput();
			if ((engine.keyState[KEY_FIRE]) || (engine.keyState[KEY_ALTFIRE]) ||
					(engine.keyState[KEY_ESCAPE]))
				break;
		}

		screen_clear(black);
		renderer_update();
		screen_clear(black);
	}

	engine.gameSection = SECTION_GAME;
}

/*
Display a screen showing all the information from the mission
the player has just done. This includes objectives that have been
completed and failed. A mission timer is also displayed at the bottom
of the screen.
*/
void missionFinishedScreen()
{
	int shield_bonus;
	char temp[100];

	if (game.area != MISN_INTERCEPTION)
	{
		screen_clear(black);
		renderer_update();

		if (game.shots > 0)
			game.accuracy = (game.hits * 100) / game.shots;

		screen_clear(black);
		drawBriefScreen();

		for (int i = 0 ; i < 3 ; i++)
		{
			if (currentMission.primaryType[i] != M_NONE)
			{
				if ((game.area != MISN_POSWIC) || (i != 1))
					screen_renderString("COMPLETED", 550, 114 + (i * 30), FONT_GREEN);
				else
					screen_renderString("FAILED", 550, 114 + (i * 30), FONT_RED);
			}
		}

		if (currentMission.secondaryType[0] != M_NONE)
		{
			for (int i = 0 ; i < 3 ; i++)
			{
				if (currentMission.secondaryType[i] != M_NONE)
				{
					strcpy(temp, currentMission.secondaryObjective[i]);
					if (currentMission.completed2[i] >= OB_COMPLETED)
					{
						screen_renderString("COMPLETED", 550, 274 + (i * 30), FONT_GREEN);
						game.secondaryMissionsCompleted++;
					}
					else
					{
						screen_renderString("FAILED", 550, 274 + (i * 30), FONT_RED);
					}
				}
			}
		}

		if (currentMission.remainingObjectives1 + currentMission.remainingObjectives2 == 0)
		{
			shield_bonus = player.shield * 10;
			sprintf(temp, "Shield Bonus: $%.3d", shield_bonus);
			screen_renderString(temp, -1, 430, FONT_WHITE);
			game.cash += shield_bonus;
			game.cashEarned += shield_bonus;
		}

		game.timeTaken += engine.timeTaken;

		snprintf(temp, sizeof temp, "Mission Time: %2lu:%02lu:%02lu",
			(unsigned long)(engine.timeTaken / 3600),
			(unsigned long)((engine.timeTaken / 60) % 60),
			(unsigned long)(engine.timeTaken % 60));

		screen_renderString(temp, -1, 500, FONT_WHITE);

		// Do some mission specific stuff here...
		if (game.area == MISN_HAIL)
			game.cash -= 500;
		else if (game.area == MISN_ODEON)
			game.hasWingMate2 = 1;
		else if (game.area == MISN_ALMARTHA)
			game.cash -= 2000;

		checkForBossMission();

		renderer_update();

		flushInput();
		engine.done = 0;
		engine.keyState[KEY_FIRE] = 0;

		while (true)
		{
			game_delayFrame();
			getPlayerInput();
			if ((engine.keyState[KEY_FIRE]))
				break;
		}
	}

	// Stop people from "selling" Laser ammo as rockets.
	if (player.weaponType[1] == W_LASER)
		player.ammo[1] = 1;

	audio_haltMusic();
}

/*
This is where all the missions are defined.
*/
void initMissions()
{
	clearAllMissions();

	// Seconds to wait between attempting to add an enemy
	int SOMETIMES    = 40 * 60;
	int NORMAL       = 15 * 60;
	int FREQUENT     = 5 * 60;
	int ALWAYS       = 1 * 60;
	int NEVER        = -1;


	sprintf(missions[MISN_START].primaryObjective[0],
		"Escape from WEAPCO Persuit");
	missions[MISN_START].primaryType[0] = M_DESTROY_ALL_TARGETS;
	missions[MISN_START].completed1[0] = OB_INCOMPLETE;


	sprintf(missions[MISN_HAIL].primaryObjective[0],
		"Collect $500 to pay Mercenary for FIREFLY");
	missions[MISN_HAIL].primaryType[0] = M_COLLECT;
	missions[MISN_HAIL].target1[0] = P_CASH;
	missions[MISN_HAIL].targetValue1[0] = 500;
	missions[MISN_HAIL].completed1[0] = OB_INCOMPLETE;

	sprintf(missions[MISN_HAIL].primaryObjective[1],
		"Destroy all remaining WEAPCO fighters");
	missions[MISN_HAIL].primaryType[1] = M_DESTROY_ALL_TARGETS;
	missions[MISN_HAIL].completed1[1] = OB_INCOMPLETE;

	missions[MISN_HAIL].addAliens = FREQUENT;


	sprintf(missions[MISN_CERADSE].primaryObjective[0],
		"Collect 6 Cargo Pods");
	missions[MISN_CERADSE].primaryType[0] = M_COLLECT;
	missions[MISN_CERADSE].target1[0] = P_CARGO;
	missions[MISN_CERADSE].targetValue1[0] = 6;
	missions[MISN_CERADSE].completed1[0] = OB_INCOMPLETE;

	sprintf(missions[MISN_CERADSE].primaryObjective[1],
		"Do not destroy *ANY* Cargo Pods");
	missions[MISN_CERADSE].primaryType[1] = M_PROTECT_PICKUP;
	missions[MISN_CERADSE].target1[1] = P_CARGO;
	missions[MISN_CERADSE].targetValue1[1] = 0;
	missions[MISN_CERADSE].completed1[1] = OB_CONDITION;

	sprintf(missions[MISN_CERADSE].secondaryObjective[0],
		"Destroy all remaining WEAPCO fighters");
	missions[MISN_CERADSE].secondaryType[0] = M_DESTROY_ALL_TARGETS;
	missions[MISN_CERADSE].completed2[0] = OB_INCOMPLETE;

	missions[MISN_CERADSE].addAliens = FREQUENT;


	sprintf(missions[MISN_HINSTAG].primaryObjective[0],
		"Destroy 5 WEAPCO Missile Boats");
	missions[MISN_HINSTAG].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[MISN_HINSTAG].target1[0] = CD_MISSILEBOAT;
	missions[MISN_HINSTAG].targetValue1[0] = 5;
	missions[MISN_HINSTAG].completed1[0] = OB_INCOMPLETE;

	sprintf(missions[MISN_HINSTAG].secondaryObjective[0],
		"Destroy all remaining WEAPCO fighters");
	missions[MISN_HINSTAG].secondaryType[0] = M_DESTROY_ALL_TARGETS;
	missions[MISN_HINSTAG].completed2[0] = OB_INCOMPLETE;

	missions[MISN_HINSTAG].addAliens = NORMAL;


	sprintf(missions[MISN_JOLDAR].primaryObjective[0],
		"Destroy 9 WEAPCO Miners");
	missions[MISN_JOLDAR].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[MISN_JOLDAR].target1[0] = CD_MINER;
	missions[MISN_JOLDAR].targetValue1[0] = 9;
	missions[MISN_JOLDAR].completed1[0] = OB_INCOMPLETE;

	sprintf(missions[MISN_JOLDAR].secondaryObjective[0],
		"Destroy all remaining WEAPCO fighters");
	missions[MISN_JOLDAR].secondaryType[0] = M_DESTROY_ALL_TARGETS;
	missions[MISN_JOLDAR].completed2[0] = OB_INCOMPLETE;

	missions[MISN_JOLDAR].addAliens = NORMAL;


	sprintf(missions[MISN_MOEBO].primaryObjective[0],
		"Destroy WEAPCO Frigate");
	missions[MISN_MOEBO].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[MISN_MOEBO].target1[0] = CD_BOSS;
	missions[MISN_MOEBO].targetValue1[0] = 1;
	missions[MISN_MOEBO].completed1[0] = OB_INCOMPLETE;
	
	missions[MISN_MOEBO].timeLimit1[0] = 3;

	missions[MISN_MOEBO].addAliens = SOMETIMES;


	sprintf(missions[MISN_NEROD].primaryObjective[0], "Rescue Phoebe Lexx");
	missions[MISN_NEROD].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[MISN_NEROD].target1[0] = CD_CARGOSHIP;
	missions[MISN_NEROD].targetValue1[0] = 1;
	missions[MISN_NEROD].completed1[0] = OB_INCOMPLETE;

	sprintf(missions[MISN_NEROD].primaryObjective[1],
		"Do not allow Phoebe to be killed");
	missions[MISN_NEROD].primaryType[1] = M_PROTECT_TARGET;
	missions[MISN_NEROD].target1[1] = CD_PHOEBE;
	missions[MISN_NEROD].targetValue1[1] = 0;
	missions[MISN_NEROD].completed1[1] = OB_CONDITION;

	sprintf(missions[MISN_NEROD].primaryObjective[2],
		"Destroy all WEAPCO forces");
	missions[MISN_NEROD].primaryType[2] = M_DESTROY_TARGET_TYPE;
	missions[MISN_NEROD].target1[2] = CD_ANY;
	missions[MISN_NEROD].targetValue1[2] = 35;
	missions[MISN_NEROD].completed1[2] = OB_INCOMPLETE;

	missions[MISN_NEROD].addAliens = ALWAYS;


	sprintf(missions[MISN_ALLEZ].primaryObjective[0],
		"Assist medical supply craft");
	missions[MISN_ALLEZ].primaryType[0] = M_ESCAPE_TARGET;
	missions[MISN_ALLEZ].target1[0] = CD_GOODTRANSPORT;
	missions[MISN_ALLEZ].targetValue1[0] = 0;
	missions[MISN_ALLEZ].completed1[0] = OB_INCOMPLETE;

	sprintf(missions[MISN_ALLEZ].primaryObjective[1],
		"Do not allow supply craft to be destroyed");
	missions[MISN_ALLEZ].primaryType[1] = M_PROTECT_TARGET;
	missions[MISN_ALLEZ].target1[1] = CD_GOODTRANSPORT;
	missions[MISN_ALLEZ].targetValue1[1] = 0;
	missions[MISN_ALLEZ].completed1[1] = OB_CONDITION;
	
	sprintf(missions[MISN_ALLEZ].secondaryObjective[0],
		"Destroy all remaining WEAPCO fighters");
	missions[MISN_ALLEZ].secondaryType[0] = M_DESTROY_ALL_TARGETS;
	missions[MISN_ALLEZ].completed2[0] = OB_INCOMPLETE;

	missions[MISN_ALLEZ].addAliens = FREQUENT;


	sprintf(missions[MISN_URUSOR].primaryObjective[0],
		"Disable five WEAPCO supply craft");
	missions[MISN_URUSOR].primaryType[0] = M_DISABLE_TARGET;
	missions[MISN_URUSOR].target1[0] = CD_CARGOSHIP;
	missions[MISN_URUSOR].targetValue1[0] = 5;
	missions[MISN_URUSOR].completed1[0] = OB_INCOMPLETE;

	sprintf(missions[MISN_URUSOR].primaryObjective[1],
		"Destroy all remaining WEAPCO fighters");
	missions[MISN_URUSOR].primaryType[1] = M_DESTROY_ALL_TARGETS;
	missions[MISN_URUSOR].completed1[1] = OB_INCOMPLETE;

	sprintf(missions[MISN_URUSOR].primaryObjective[2],
		"Protect supply craft AND Sid Wilson");
	missions[MISN_URUSOR].primaryType[2] = M_PROTECT_TARGET;
	missions[MISN_URUSOR].target1[2] = CD_CARGOSHIP;
	missions[MISN_URUSOR].targetValue1[2] = 0;
	missions[MISN_URUSOR].completed1[2] = OB_CONDITION;

	missions[MISN_URUSOR].addAliens = FREQUENT;


	sprintf(missions[MISN_DORIM].primaryObjective[0],
		"Locate doctor's escape pod");
	missions[MISN_DORIM].primaryType[0] = M_COLLECT;
	missions[MISN_DORIM].target1[0] = P_ESCAPEPOD;
	missions[MISN_DORIM].targetValue1[0] = 1;
	missions[MISN_DORIM].completed1[0] = OB_INCOMPLETE;

	sprintf(missions[MISN_DORIM].primaryObjective[1],
		"Do not destroy doctor's escape pod");
	missions[MISN_DORIM].primaryType[1] = M_PROTECT_PICKUP;
	missions[MISN_DORIM].target1[1] = P_ESCAPEPOD;
	missions[MISN_DORIM].targetValue1[1] = 1; // DONE ON PURPOSE!! DO NOT CHANGE THIS!!!!
	missions[MISN_DORIM].completed1[1] = OB_CONDITION;

	sprintf(missions[MISN_DORIM].secondaryObjective[0],
		"Collect 10 pieces of Ore");
	missions[MISN_DORIM].secondaryType[0] = M_COLLECT;
	missions[MISN_DORIM].target2[0] = P_ORE;
	missions[MISN_DORIM].targetValue2[0] = 10;
	missions[MISN_DORIM].completed2[0] = OB_INCOMPLETE;

	missions[MISN_DORIM].addAliens = ALWAYS;

	missions[MISN_DORIM].timeLimit1[0] = 3;
	missions[MISN_DORIM].timeLimit2[0] = 3;


	sprintf(missions[MISN_ELAMALE].primaryObjective[0],
		"Destroy WEAPCO ore mining craft");
	missions[MISN_ELAMALE].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[MISN_ELAMALE].target1[0] = CD_BOSS;
	missions[MISN_ELAMALE].targetValue1[0] = 1;
	missions[MISN_ELAMALE].completed1[0] = OB_INCOMPLETE;

	sprintf(missions[MISN_ELAMALE].secondaryObjective[0],
		"Save present slaves");
	missions[MISN_ELAMALE].secondaryType[0] = M_PROTECT_PICKUP;
	missions[MISN_ELAMALE].target2[0] = P_SLAVES;
	missions[MISN_ELAMALE].completed2[0] = OB_CONDITION;

	sprintf(missions[MISN_ELAMALE].primaryObjective[1], "Battle Kline");
	missions[MISN_ELAMALE].primaryType[1] = M_ESCAPE_TARGET;
	missions[MISN_ELAMALE].target1[1] = CD_KLINE;
	missions[MISN_ELAMALE].targetValue1[1] = 1;
	missions[MISN_ELAMALE].completed1[1] = OB_HIDDEN;

	missions[MISN_ELAMALE].addAliens = NEVER;


	sprintf(missions[MISN_ODEON].primaryObjective[0], "Destroy Ursula's ship");
	missions[MISN_ODEON].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[MISN_ODEON].target1[0] = CD_EVILURSULA;
	missions[MISN_ODEON].targetValue1[0] = 0;
	missions[MISN_ODEON].completed1[0] = OB_INCOMPLETE;

	sprintf(missions[MISN_ODEON].primaryObjective[1],
		"Capture Ursula's escape pod");
	missions[MISN_ODEON].primaryType[1] = M_COLLECT;
	missions[MISN_ODEON].target1[1] = P_ESCAPEPOD;
	missions[MISN_ODEON].targetValue1[1] = 1;
	missions[MISN_ODEON].completed1[1] = OB_INCOMPLETE;

	sprintf(missions[MISN_ODEON].primaryObjective[2], "Do not kill Ursula");
	missions[MISN_ODEON].primaryType[2] = M_PROTECT_PICKUP;
	missions[MISN_ODEON].target1[2] = P_ESCAPEPOD;
	missions[MISN_ODEON].targetValue1[2] = 0;
	missions[MISN_ODEON].completed1[2] = OB_CONDITION;

	sprintf(missions[MISN_ODEON].secondaryObjective[0],
		"Destroy all remaining WEAPCO fighters");
	missions[MISN_ODEON].secondaryType[0] = M_DESTROY_ALL_TARGETS;
	missions[MISN_ODEON].completed2[0] = OB_INCOMPLETE;

	missions[MISN_ODEON].addAliens = FREQUENT;


	sprintf(missions[MISN_FELLON].primaryObjective[0],
		"Assist attack on WEAPCO ore mining craft");
	missions[MISN_FELLON].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[MISN_FELLON].target1[0] = CD_BOSS;
	missions[MISN_FELLON].targetValue1[0] = 1;
	missions[MISN_FELLON].completed1[0] = OB_INCOMPLETE;

	sprintf(missions[MISN_FELLON].primaryObjective[1],
		"At least 1 rebel craft must survive");
	missions[MISN_FELLON].primaryType[1] = M_PROTECT_TARGET;
	missions[MISN_FELLON].target1[1] = CD_REBELCARRIER;
	missions[MISN_FELLON].targetValue1[1] = 2;
	missions[MISN_FELLON].completed1[1] = OB_CONDITION;

	sprintf(missions[MISN_FELLON].primaryObjective[2],
		"Destroy all present WEAPCO forces");
	missions[MISN_FELLON].primaryType[2] = M_DESTROY_ALL_TARGETS;
	missions[MISN_FELLON].completed1[2] = OB_INCOMPLETE;

	missions[MISN_FELLON].addAliens = ALWAYS;


	sprintf(missions[MISN_SIVEDI].primaryObjective[0],
		"Collect 25 pieces of Ore");
	missions[MISN_SIVEDI].primaryType[0] = M_COLLECT;
	missions[MISN_SIVEDI].target1[0] = P_ORE;
	missions[MISN_SIVEDI].targetValue1[0] = 25;
	missions[MISN_SIVEDI].completed1[0] = OB_INCOMPLETE;

	sprintf(missions[MISN_SIVEDI].secondaryObjective[0],
		"Collect 25 pieces of Ore");
	missions[MISN_SIVEDI].secondaryType[0] = M_COLLECT;
	missions[MISN_SIVEDI].target2[0] = P_ORE;
	missions[MISN_SIVEDI].targetValue2[0] = 25;
	missions[MISN_SIVEDI].completed2[0] = OB_INCOMPLETE;

	missions[MISN_SIVEDI].addAliens = ALWAYS;


	sprintf(missions[MISN_ALMARTHA].primaryObjective[0],
		"Collect $2000 to pay mercenary");
	missions[MISN_ALMARTHA].primaryType[0] = M_COLLECT;
	missions[MISN_ALMARTHA].target1[0] = P_CASH;
	missions[MISN_ALMARTHA].targetValue1[0] = 2000;
	missions[MISN_ALMARTHA].completed1[0] = OB_INCOMPLETE;

	sprintf(missions[MISN_ALMARTHA].primaryObjective[1],
		"Destroy all remaining WEAPCO fighters");
	missions[MISN_ALMARTHA].primaryType[1] = M_DESTROY_ALL_TARGETS;
	missions[MISN_ALMARTHA].completed1[1] = OB_INCOMPLETE;

	missions[MISN_ALMARTHA].addAliens = ALWAYS;


	sprintf(missions[MISN_POSWIC].primaryObjective[0], "Destroy escorts");
	missions[MISN_POSWIC].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[MISN_POSWIC].target1[0] = CD_ESCORT;
	missions[MISN_POSWIC].targetValue1[0] = 5;
	missions[MISN_POSWIC].completed1[0] = OB_INCOMPLETE;

	sprintf(missions[MISN_POSWIC].primaryObjective[1],
		"Disable executive transport");
	missions[MISN_POSWIC].primaryType[1] = M_ESCAPE_TARGET;
	missions[MISN_POSWIC].target1[1] = CD_BOSS;
	missions[MISN_POSWIC].targetValue1[1] = 1;
	missions[MISN_POSWIC].completed1[1] = OB_INCOMPLETE;

	sprintf(missions[MISN_POSWIC].primaryObjective[2],
		"Destroy all remaining WEAPCO fighters");
	missions[MISN_POSWIC].primaryType[2] = M_DESTROY_ALL_TARGETS;
	missions[MISN_POSWIC].completed1[2] = OB_INCOMPLETE;

	missions[MISN_POSWIC].addAliens = NORMAL;


	sprintf(missions[MISN_ELLESH].primaryObjective[0],
		"Destroy executive transport");
	missions[MISN_ELLESH].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[MISN_ELLESH].target1[0] = CD_BOSS;
	missions[MISN_ELLESH].targetValue1[0] = 1;
	missions[MISN_ELLESH].completed1[0] = OB_INCOMPLETE;

	missions[MISN_ELLESH].addAliens = ALWAYS;


	sprintf(missions[MISN_PLUTO].primaryObjective[0],
		"Destroy planetary guardian");
	missions[MISN_PLUTO].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[MISN_PLUTO].target1[0] = CD_PLUTOBOSS;
	missions[MISN_PLUTO].targetValue1[0] = 1;
	missions[MISN_PLUTO].completed1[0] = OB_INCOMPLETE;

	sprintf(missions[MISN_PLUTO].primaryObjective[1],
		"Destroy all remaining WEAPCO fighters");
	missions[MISN_PLUTO].primaryType[1] = M_DESTROY_ALL_TARGETS;
	missions[MISN_PLUTO].completed1[1] = OB_INCOMPLETE;

	missions[MISN_PLUTO].timeLimit1[0] = 5;
	missions[MISN_PLUTO].timeLimit1[1] = 5;

	missions[MISN_PLUTO].addAliens = ALWAYS;


	sprintf(missions[MISN_NEPTUNE].primaryObjective[0],
		"Destroy planetary guardian");
	missions[MISN_NEPTUNE].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[MISN_NEPTUNE].target1[0] = CD_NEPTUNEBOSS;
	missions[MISN_NEPTUNE].targetValue1[0] = 1;
	missions[MISN_NEPTUNE].completed1[0] = OB_INCOMPLETE;

	sprintf(missions[MISN_NEPTUNE].primaryObjective[1],
		"Destroy all remaining WEAPCO fighters");
	missions[MISN_NEPTUNE].primaryType[1] = M_DESTROY_ALL_TARGETS;
	missions[MISN_NEPTUNE].completed1[1] = OB_INCOMPLETE;

	missions[MISN_NEPTUNE].timeLimit1[0] = 5;
	missions[MISN_NEPTUNE].timeLimit1[1] = 5;

	missions[MISN_NEPTUNE].addAliens = ALWAYS;


	sprintf(missions[MISN_URANUS].primaryObjective[0],
		"Destroy all present WEAPCO forces");
	missions[MISN_URANUS].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[MISN_URANUS].target1[0] = CD_URANUSBOSS;
	missions[MISN_URANUS].targetValue1[0] = 1;
	missions[MISN_URANUS].completed1[0] = OB_INCOMPLETE;

	sprintf(missions[MISN_URANUS].primaryObjective[1],
		"Destroy all remaining WEAPCO fighters");
	missions[MISN_URANUS].primaryType[1] = M_DESTROY_ALL_TARGETS;
	missions[MISN_URANUS].completed1[1] = OB_INCOMPLETE;

	missions[MISN_URANUS].timeLimit1[0] = 5;
	missions[MISN_URANUS].timeLimit1[1] = 5;

	missions[MISN_URANUS].addAliens = ALWAYS;


	sprintf(missions[MISN_SATURN].primaryObjective[0],
		"Destroy outer defence systems");
	missions[MISN_SATURN].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[MISN_SATURN].target1[0] = CD_MOBILE_RAY;
	missions[MISN_SATURN].targetValue1[0] = 6;
	missions[MISN_SATURN].completed1[0] = OB_INCOMPLETE;

	sprintf(missions[MISN_SATURN].primaryObjective[1],
		"Destroy all remaining WEAPCO craft");
	missions[MISN_SATURN].primaryType[1] = M_DESTROY_ALL_TARGETS;
	missions[MISN_SATURN].completed1[1] = OB_INCOMPLETE;

	missions[MISN_SATURN].addAliens = NORMAL;


	sprintf(missions[MISN_JUPITER].primaryObjective[0],
		"Investigate distress call");
	missions[MISN_JUPITER].primaryType[0] = M_DESTROY_ALL_TARGETS;
	missions[MISN_JUPITER].completed1[0] = OB_CONDITION;

	sprintf(missions[MISN_JUPITER].primaryObjective[1], "Defeat Krass Tyler");
	missions[MISN_JUPITER].primaryType[1] = M_DESTROY_TARGET_TYPE;
	missions[MISN_JUPITER].target1[1] = CD_KRASS;
	missions[MISN_JUPITER].targetValue1[1] = 1;
	missions[MISN_JUPITER].completed1[1] = OB_HIDDEN;

	sprintf(missions[MISN_JUPITER].primaryObjective[2],
		"Destroy Krass' support group");
	missions[MISN_JUPITER].primaryType[2] = M_DESTROY_ALL_TARGETS;
	missions[MISN_JUPITER].target1[1] = CD_FIREFLY;
	missions[MISN_JUPITER].targetValue1[1] = 4;
	missions[MISN_JUPITER].completed1[2] = OB_HIDDEN;

	missions[MISN_JUPITER].addAliens = ALWAYS;


	sprintf(missions[MISN_MARS].primaryObjective[0], "Navigate asteroid belt");
	missions[MISN_MARS].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[MISN_MARS].target1[0] = CD_BOSS;
	missions[MISN_MARS].targetValue1[0] = 1;
	missions[MISN_MARS].completed1[0] = OB_INCOMPLETE;

	missions[MISN_MARS].timeLimit1[0] = 2;

	missions[MISN_MARS].addAliens = ALWAYS;


	sprintf(missions[MISN_EARTH].primaryObjective[0],
		"Destroy WEAPCO frontline forces");
	missions[MISN_EARTH].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[MISN_EARTH].target1[0] = CD_ANY;
	missions[MISN_EARTH].targetValue1[0] = 100;
	missions[MISN_EARTH].completed1[0] = OB_INCOMPLETE;

	missions[MISN_EARTH].addAliens = ALWAYS;


	sprintf(missions[MISN_VENUS].primaryObjective[0], "Defeat Kline");
	missions[MISN_VENUS].primaryType[0] = M_DESTROY_ALL_TARGETS;
	missions[MISN_VENUS].completed1[0] = OB_INCOMPLETE;


	strcpy(missions[MISN_INTERCEPTION].primaryObjective[0], "");
	missions[MISN_INTERCEPTION].primaryType[0] = M_DESTROY_ALL_TARGETS;
	missions[MISN_INTERCEPTION].completed1[0] = OB_INCOMPLETE;
}
