/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012, 2013 Guus Sliepen
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

// God, I hate this file! :((

Planet systemPlanet[10];
mission currentMission;
static mission missions[MAX_MISSIONS];

void initPlanetMissions(signed char system)
{
	for (int i = 0 ; i < 10 ; i++)
	{
		systemPlanet[i].missionNumber = -1; // no mission for this planet
		systemPlanet[i].missionCompleted = 1;
	}

	switch(system)
	{
		// Spirit
		case 0:
			for (int i = 0 ; i < 4 ; i++)
			{
				systemPlanet[i].missionNumber = i + 1;
				systemPlanet[i].missionCompleted = 0;
			}

			systemPlanet[4].missionNumber = 5;
			systemPlanet[4].missionCompleted = -1;

			break;

		// Eyananth
		case 1:
			systemPlanet[0].missionNumber = 7;
			systemPlanet[0].missionCompleted = 0;

			systemPlanet[1].missionNumber = 8;
			systemPlanet[1].missionCompleted = 0;

			systemPlanet[2].missionNumber = 9;
			systemPlanet[2].missionCompleted = -1;

			systemPlanet[3].missionNumber = 10;
			systemPlanet[3].missionCompleted = -1;

			systemPlanet[4].missionNumber = 11;
			systemPlanet[4].missionCompleted = -2;

			// This one is for the slaves
			systemPlanet[9].missionNumber = 6;
			systemPlanet[9].missionCompleted = 0;

			break;

		// Mordor
		case 2:
			systemPlanet[0].missionNumber = 13;
			systemPlanet[0].missionCompleted = 0;

			systemPlanet[1].missionNumber = 14;
			systemPlanet[1].missionCompleted = 0;

			systemPlanet[2].missionNumber = 15;
			systemPlanet[2].missionCompleted = -1;

			systemPlanet[3].missionNumber = 16;
			systemPlanet[3].missionCompleted = -1;

			systemPlanet[4].missionNumber = 17;
			systemPlanet[4].missionCompleted = -2;

			systemPlanet[5].missionNumber = 18;
			systemPlanet[5].missionCompleted = -3;

			// This one is for the experimental fighter
			systemPlanet[9].missionNumber = 12;
			systemPlanet[9].missionCompleted = 0;

			break;

		// Sol
		case 3:
			systemPlanet[8].missionNumber = 19;
			systemPlanet[8].missionCompleted = 0;

			systemPlanet[7].missionNumber = 20;
			systemPlanet[7].missionCompleted = 0;

			systemPlanet[6].missionNumber = 21;
			systemPlanet[6].missionCompleted = 0;

			systemPlanet[5].missionNumber = 22;
			systemPlanet[5].missionCompleted = -1;

			systemPlanet[4].missionNumber = 23;
			systemPlanet[4].missionCompleted = -2;

			systemPlanet[3].missionNumber = 24;
			systemPlanet[3].missionCompleted = -3;

			systemPlanet[2].missionNumber = 25;
			systemPlanet[2].missionCompleted = -4;

			systemPlanet[1].missionNumber = 26;
			systemPlanet[1].missionCompleted = -5;

			break;
	}
}

void checkForBossMission()
{
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
	if (currentGame.area == 0)
	{
		currentGame.stationedPlanet = 0;
		currentGame.area = 1;
		strcpy(currentGame.stationedName, "Hail");
		initPlanetMissions(currentGame.system);
	}
	else if (currentGame.area == 5)
	{
		currentGame.stationedPlanet = 0;
		currentGame.system = 1;
		currentGame.area = 6;
		strcpy(currentGame.stationedName, "Nerod");
		initPlanetMissions(currentGame.system);
	}
	else if (currentGame.area == 11)
	{
		currentGame.stationedPlanet = 0;
		currentGame.system = 2;
		currentGame.area = 12;
		strcpy(currentGame.stationedName, "Odeon");
		initPlanetMissions(currentGame.system);
	}
	else if (currentGame.area == 18)
	{
		currentGame.stationedPlanet = 8;
		currentGame.system = 3;
		currentGame.area = 19;
		strcpy(currentGame.stationedName, "Pluto");
		initPlanetMissions(currentGame.system);
	}
	else // Update the mission for the planet
	{
		systemPlanet[currentGame.stationedPlanet].missionCompleted = 1;
	}

	strcpy(currentGame.destinationName, "None");
	currentGame.destinationPlanet = currentGame.stationedPlanet;
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
	for (int m = 0 ; m < MAX_MISSIONS ; m++)
	{
		for (int i = 0 ; i < 3 ; i++)
		{
			strcpy(missions[m].primaryObjective[i], "");
			missions[m].primaryType[i] = NONE;
			missions[m].target1[i] = -1;
			missions[m].targetValue1[i] = -1;
			missions[m].timeLimit1[i] = -2;
			missions[m].completed1[i] = 1;
		}

		for (int i = 0 ; i < 3 ; i++)
		{
			strcpy(missions[m].secondaryObjective[i], "");
			missions[m].secondaryType[i] = NONE;
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

		if (currentMission.completed1[i] == 0)
			currentMission.remainingObjectives1++;
		if (currentMission.completed2[i] == 0)
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

void checkTimer()
{
	for (int i = 0 ; i < 3 ; i++)
	{
		if ((currentMission.timeLimit1[i] == -1) && (currentMission.completed1[i] == OB_INCOMPLETE))
			currentMission.completed1[i] = -2;
	}

	for (int i = 0 ; i < 3 ; i++)
	{
		if ((currentMission.timeLimit2[i] == -1) && (currentMission.completed2[i] == OB_INCOMPLETE))
			currentMission.completed2[i] = -2;
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
	if ((currentGame.area == 5) && (currentMission.completed1[0] < OB_INCOMPLETE))
		engine.timeMission = 1;

	// specific to the Asteroid belt
	if ((currentGame.area == 24) && (currentMission.completed1[0] < OB_INCOMPLETE))
	{
		currentMission.completed1[0] = OB_COMPLETED;
		killAllAliens();
		engine.addAliens = -1;
		setInfoLine("*** All Primary Objectives Completed ***", FONT_GREEN);
	}
}

static void evaluateRequirement(int type, int id, int *completed, int *targetValue, int fontColor)
{
	char message[25];

	if ((*targetValue <= 0) && (type != M_PROTECT_TARGET) && (type != M_PROTECT_PICKUP))
	{
		*completed = 2;
		checkTimer();
		if ((currentGame.area == 9) && (type == M_DISABLE_TARGET))
			setRadioMessage(FACE_SID, "All vessels disabled!", 1);
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
				*completed = -2;
				switch(id)
				{
					case P_CARGO:
						sprintf(message, "Cargo pod destroyed!");
						if (currentGame.area == 2) // Get lectured by Sid
							setRadioMessage(FACE_SID, "Chris, we needed that pod!! I warned you that we couldn't afford to lose a single one!!", 1);
						break;
					case P_ESCAPEPOD:
						sprintf(message, "Escape Pod lost!");
						if (currentGame.area == 13) // Get lectured by Phoebe
							setRadioMessage(FACE_PHOEBE, "No... Ursula...", 1);
						break;
				}
				break;
			case M_PROTECT_TARGET:
				if (*targetValue <= 0)
				{
					*completed = -2;
					switch (currentGame.area)
					{
						case 7:
							setRadioMessage(FACE_SID, "Dammit, Chris! We just lost her!", 1);
							break;
						case 8:
							setRadioMessage(FACE_CREW, "Noooo!! Hull bre...", 1);
							break;
						case 9:
							setRadioMessage(FACE_SID, "Chris, we've got to disable them, not destroy them!!", 1);
							break;
					}
				}
				break;
			case M_DESTROY_TARGET_TYPE:
				if ((*targetValue <= 10) || (*targetValue % 10 == 0))
				{
					if ((rand() % 2) == 0)
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
		setRadioMessage(FACE_CHRIS, "Sid... I... I'm sorry...", 1);
		currentMission.completed1[0] = -2;
	}

	for (int i = 0 ; i < 3 ; i++)
	{
		if ((currentMission.completed1[i] == OB_INCOMPLETE) || (currentMission.completed1[i] == OB_CONDITION))
		{
			if ((currentMission.primaryType[i] == type) && ((currentMission.target1[i] == id) || (currentMission.target1[i] == CD_ANY)))
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
			if ((currentMission.secondaryType[i] == type) && ((currentMission.target2[i] == id) || (currentMission.target2[i] == CD_ANY)))
			{
				currentMission.targetValue2[i] -= value;
				evaluateRequirement(type, id, &currentMission.completed2[i], &currentMission.targetValue2[i], FONT_YELLOW);
				return;
			}
		}
	}

	// Special Case - Interceptions
	if (currentGame.area == MAX_MISSIONS - 1)
	{
		if ((type == M_COLLECT) && (id == P_SLAVES))
		{
			if (systemPlanet[9].missionCompleted == 0)
			{
				if (currentGame.slavesRescued >= 250)
				{
					setInfoLine("*** Slaves Rescued - Mission Completed ***", FONT_GREEN);
					systemPlanet[9].missionCompleted = 1;
				}
				else
				{
					sprintf(message, "Rescue %d more...", 250 - currentGame.slavesRescued);
					setInfoLine(message, FONT_CYAN);
				}
			}
		}

		if ((type == M_DESTROY_TARGET_TYPE) && (id == CD_CLOAKFIGHTER))
		{
			setInfoLine("*** Experimental Fighter Destroyed - Mission Completed ***", FONT_GREEN);
			systemPlanet[9].missionCompleted = 1;
			setRadioMessage(FACE_CHRIS, "That's one less suprise that WEAPCO can spring on us!", 1);
			currentGame.experimentalShield = 0;
		}
	}
}

/*
This is only used as few times in the game.
Missions 11 and 23 to be exact!
*/
static char revealHiddenObjectives()
{
	char allDone = 1;
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
		if (currentGame.area == 11)
		{
			killAllAliens();
			syncScriptEvents();
			enemy[WC_KLINE].active = true;
			enemy[WC_KLINE].x = player.x + 1000;
			enemy[WC_KLINE].y = player.y;
			enemy[WC_KLINE].flags |= FL_IMMORTAL | FL_NOFIRE;
			setTarget(WC_KLINE);
			loadMusic("music/last_cyber_dance.ogg");
			if ((engine.useAudio) && (engine.useMusic))
				Mix_PlayMusic(engine.music, -1);
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
				currentMission.completed1[i] = 2;
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
				currentMission.completed2[i] = 2;
				checkTimer();
			}
		}
	}

	for (int i = 0 ; i < 3 ; i++)
	{
		if (currentMission.completed1[i] == 2)
		{
			if (currentMission.remainingObjectives1 > 1)
			{
				if ((currentGame.area != 17) || ((currentGame.area == 17) && (i != 1)))
					setInfoLine("*** Primary Objective Completed ***", FONT_GREEN);
				else
					setInfoLine(">>> Primary Objective Failed <<<", FONT_RED);
				currentMission.completed1[i] = OB_COMPLETED;
			}
			else
			{
				if (currentGame.area != MAX_MISSIONS - 1)
					setInfoLine("*** All Primary Objectives Completed ***", FONT_GREEN);
				else
					setInfoLine("*** Interception Destroyed ***", FONT_GREEN);
				currentMission.completed1[i] = OB_COMPLETED;

				// do some area specific things
				if ((currentGame.area == 5) || (currentGame.area == 10) || (currentGame.area == 18) || (currentGame.area == 24))
				{
					if (currentMission.remainingObjectives2 == 0)
					{
						killAllAliens();
						engine.addAliens = -1;
					}
				}

				if (currentGame.area == 25)
					setRadioMessage(FACE_CHRIS, "You guys stay here and keep things under control. I'm going after Kethlan!", 1);
			}
		}

		if (currentMission.completed2[i] == 2)
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
				if ((currentGame.area == 10) && (currentMission.remainingObjectives1 == 0))
				{
					killAllAliens();
					engine.addAliens = -1;
				}
			}
		}

		if (currentMission.completed1[i] == -2)
		{
			setInfoLine(">>> MISSION FAILED <<<", FONT_RED);
			currentMission.completed1[i] = OB_FAILED;
		}

		if (currentMission.completed2[i] == -2)
		{
			setInfoLine(">>> Secondary Objective Failed <<<", FONT_RED);
			currentMission.completed2[i] = OB_FAILED;
		}
	}

	signed char remaining;
	bool add = false;
	bool allDone = true;

	// Zero objective list for a recount
	currentMission.remainingObjectives1 = currentMission.remainingObjectives2 = 0;

	for (int i = 0 ; i < 3 ; i++)
	{
		if (currentMission.primaryType[i] != NONE)
		{
			if (currentMission.completed1[i] == 0)
			{
				currentMission.remainingObjectives1++;
				if (currentMission.primaryType[i] == M_DESTROY_ALL_TARGETS)
					add = true;
				allDone = false;
			}

			if (currentMission.completed1[i] < 0)
				return false;
		}
		if (currentMission.secondaryType[i] != NONE)
		{
			if (currentMission.completed2[i] == 0)
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
		if (currentMission.completed1[i] < 0)
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

	blevelRect(140, 70, 500, 20, 0x00, 0x77, 0x00);
	blevelRect(140, 90, 500, 130, 0x00, 0x33, 0x00);
	drawString("Primary Objectives", 150, 74, FONT_WHITE);

	for (int i = 0 ; i < 3 ; i++)
	{
		if ((currentMission.primaryType[i] != NONE) && (currentMission.completed1[i] != OB_HIDDEN))
		{
			drawString(currentMission.primaryObjective[i], 160, 114 + (i * 30), FONT_WHITE);
		}
	}

	if (currentMission.secondaryType[0] != NONE)
	{
		blevelRect(140, 230, 500, 20, 0x00, 0x77, 0x77);
		blevelRect(140, 250, 500, 130, 0x00, 0x33, 0x33);
		drawString("Secondary Objectives", 150, 234, FONT_WHITE);

		for (int i = 0 ; i < 3 ; i++)
		{
			if (currentMission.secondaryType[i] != NONE)
			{
				drawString(currentMission.secondaryObjective[i], 160, 274 + (i * 30), FONT_WHITE);
				currentGame.secondaryMissions++;
			}
		}
	}

	blevelRect(140, 390, 500, 20, 0x77, 0x77, 0x00);
	blevelRect(140, 410, 500, 130, 0x33, 0x33, 0x00);
	drawString("Additional Information", 150, 394, FONT_WHITE);
}

/*
Simply displays a screen with all the mission information on it, pulled
back from the data stored in the currentMission object. The music for the
mission begins playing here.
*/
void missionBriefScreen()
{
	clearScreen(black);
	updateScreen();

	if (currentGame.area != MAX_MISSIONS - 1)
	{
		clearScreen(black);
		drawBriefScreen();

		if (currentMission.timeLimit1[0] > 0)
		{
			char temp[50];
			if (currentGame.area != 24)
				sprintf(temp, "TIME LIMIT: %d minutes", currentMission.timeLimit1[0]);
			else
				sprintf(temp, "SURVIVAL FOR %d minutes", currentMission.timeLimit1[0]);
			drawString(temp, -1, 500, FONT_RED);
		}

		switch (currentGame.area)
		{
			case 9:
			case 10:
			case 15:
			case 16:
			case 18:
			case 24:
			case 26:
				drawString("Phoebe Lexx will not be present", 160, 420, FONT_WHITE);
				if (currentGame.hasWingMate2)
					drawString("Ursula Lexx will not be present", 160, 450, FONT_WHITE);
				break;
		}

		if ((currentGame.area == 9) || (currentGame.area == 17) || (currentGame.area == 25))
			drawString("Sid Wilson will join you on this mission", 160, 480, FONT_WHITE);

		updateScreen();
	}

	loadGameGraphics();
	textSurface(4, "Shield", 25, 550, FONT_WHITE);
	textSurface(5, "Plasma:", 250, 550, FONT_WHITE);

	if (player.weaponType[1] == W_CHARGER)
		textSurface(6, "Charge", 385, 550, FONT_WHITE);
	else if (player.weaponType[1] == W_LASER)
		textSurface(6, "Heat", 405, 550, FONT_WHITE);
	else
		textSurface(6, "Rockets:", 385, 550, FONT_WHITE);

	textSurface(7, "Target", 550, 550, FONT_WHITE);
	textSurface(8, "Cash: $", 25, 20, FONT_WHITE);
	textSurface(9, "Objectives Remaining:", 550, 20, FONT_WHITE);
	textSurface(10, "Time Remaining - ", 260, 20, FONT_WHITE);
	textSurface(11, "Power", 25, 570, FONT_WHITE);
	textSurface(12, "Output", 250, 570, FONT_WHITE);
	textSurface(13, "Cooler", 485, 570, FONT_WHITE);
	playRandomTrack();

	if (currentGame.area != MAX_MISSIONS - 1)
	{
		drawString("PRESS CTRL TO CONTINUE...", -1, 550, FONT_WHITE);

		updateScreen();

		flushInput();
		engine.done = 0;
		engine.keyState[KEY_FIRE] = 0;

		while (true)
		{
			delayFrame();
			getPlayerInput();
			if ((engine.keyState[KEY_FIRE]))
				break;
		}

		clearScreen(black);
		updateScreen();
		clearScreen(black);
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
	if (currentGame.area != MAX_MISSIONS - 1)
	{
		clearScreen(black);
		updateScreen();

		if (currentGame.shots > 0)
			currentGame.accuracy = (currentGame.hits * 100) / currentGame.shots;

		clearScreen(black);
		drawBriefScreen();

		char temp[100];

		for (int i = 0 ; i < 3 ; i++)
		{
			if (currentMission.primaryType[i] != NONE)
			{
				if ((currentGame.area != 17) || ((currentGame.area == 17) && (i != 1)))
					drawString("COMPLETED", 550, 114 + (i * 30), FONT_GREEN);
				else
					drawString("FAILED", 550, 114 + (i * 30), FONT_RED);
			}
		}

		if (currentMission.secondaryType[0] != NONE)
		{
			for (int i = 0 ; i < 3 ; i++)
			{
				if (currentMission.secondaryType[i] != NONE)
				{
					strcpy(temp, currentMission.secondaryObjective[i]);
					if (currentMission.completed2[i] >= 1)
					{
						drawString("COMPLETED", 550, 274 + (i * 30), FONT_GREEN);
						currentGame.secondaryMissionsCompleted++;
					}
					else
					{
						drawString("FAILED", 550, 274 + (i * 30), FONT_RED);
					}
				}
			}
		}

		if (currentMission.remainingObjectives1 + currentMission.remainingObjectives2 == 0)
		{
			sprintf(temp, "Shield Bonus: $%.3d", (player.shield * 10));
			drawString(temp, -1, 430, FONT_WHITE);
			currentGame.cash += (player.shield * 10);
			currentGame.cashEarned += (player.shield * 10);
		}

		currentGame.timeTaken += engine.timeTaken;

		snprintf(temp, sizeof temp, "Mission Time: %2d:%02d:%02d", engine.timeTaken / 3600, (engine.timeTaken / 60) % 60, engine.timeTaken % 60);

		drawString(temp, -1, 500, FONT_WHITE);

		// Do some mission specific stuff here...
		if (currentGame.area == 1)
			currentGame.cash -= 500;
		else if (currentGame.area == 13)
			currentGame.hasWingMate2 = 1;
		else if (currentGame.area == 16)
			currentGame.cash -= 2000;

		checkForBossMission();

		updateScreen();

		flushInput();
		engine.done = 0;
		engine.keyState[KEY_FIRE] = 0;

		while (true)
		{
			delayFrame();
			getPlayerInput();
			if ((engine.keyState[KEY_FIRE]))
				break;
		}
	}

	// Stop people from "selling" Laser ammo as rockets.
	if (player.weaponType[1] == W_LASER)
		player.ammo[1] = 1;

	Mix_HaltMusic();
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

	// Mission 0
	sprintf(missions[0].primaryObjective[0], "Escape from WEAPCO Persuit");
	missions[0].primaryType[0] = M_DESTROY_ALL_TARGETS;
	missions[0].completed1[0] = OB_INCOMPLETE;

	// Mission 1
	sprintf(missions[1].primaryObjective[0], "Collect $500 to pay Mercenary for FIREFLY");
	missions[1].primaryType[0] = M_COLLECT;
	missions[1].target1[0] = P_CASH;
	missions[1].targetValue1[0] = 500;
	missions[1].completed1[0] = OB_INCOMPLETE;

	sprintf(missions[1].primaryObjective[1], "Destroy all remaining WEAPCO fighters");
	missions[1].primaryType[1] = M_DESTROY_ALL_TARGETS;
	missions[1].completed1[1] = OB_INCOMPLETE;

	missions[1].addAliens = FREQUENT;

	// Mission 2
	sprintf(missions[2].primaryObjective[0], "Collect 6 Cargo Pods");
	missions[2].primaryType[0] = M_COLLECT;
	missions[2].target1[0] = P_CARGO;
	missions[2].targetValue1[0] = 6;
	missions[2].completed1[0] = OB_INCOMPLETE;

	sprintf(missions[2].primaryObjective[1], "Do not destroy *ANY* Cargo Pods");
	missions[2].primaryType[1] = M_PROTECT_PICKUP;
	missions[2].target1[1] = P_CARGO;
	missions[2].targetValue1[1] = 0;
	missions[2].completed1[1] = OB_CONDITION;

	sprintf(missions[2].secondaryObjective[0], "Destroy all remaining WEAPCO fighters");
	missions[2].secondaryType[0] = M_DESTROY_ALL_TARGETS;
	missions[2].completed2[0] = OB_INCOMPLETE;

	missions[2].addAliens = FREQUENT;

	// Mission 3
	sprintf(missions[3].primaryObjective[0], "Destroy 5 WEAPCO Missile Boats");
	missions[3].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[3].target1[0] = CD_MISSILEBOAT;
	missions[3].targetValue1[0] = 5;
	missions[3].completed1[0] = OB_INCOMPLETE;

	sprintf(missions[3].secondaryObjective[0], "Destroy all remaining WEAPCO fighters");
	missions[3].secondaryType[0] = M_DESTROY_ALL_TARGETS;
	missions[3].completed2[0] = OB_INCOMPLETE;

	missions[3].addAliens = NORMAL;

	// Mission 4
	sprintf(missions[4].primaryObjective[0], "Destroy 9 WEAPCO Miners");
	missions[4].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[4].target1[0] = CD_MINER;
	missions[4].targetValue1[0] = 9;
	missions[4].completed1[0] = OB_INCOMPLETE;

	sprintf(missions[4].secondaryObjective[0], "Destroy all remaining WEAPCO fighters");
	missions[4].secondaryType[0] = M_DESTROY_ALL_TARGETS;
	missions[4].completed2[0] = OB_INCOMPLETE;

	missions[4].addAliens = NORMAL;

	// Mission 5
	sprintf(missions[5].primaryObjective[0], "Destroy WEAPCO Frigate");
	missions[5].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[5].target1[0] = CD_BOSS;
	missions[5].targetValue1[0] = 1;
	missions[5].completed1[0] = OB_INCOMPLETE;
	
	missions[5].timeLimit1[0] = 3;

	missions[5].addAliens = SOMETIMES;

	/*
		Mission 6 is the rescue 250 slaves mission. It doesn't actually
		count in this list. This is just here as a place holder
	*/

	// Mission 7
	sprintf(missions[7].primaryObjective[0], "Rescue Phoebe Lexx");
	missions[7].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[7].target1[0] = CD_CARGOSHIP;
	missions[7].targetValue1[0] = 1;
	missions[7].completed1[0] = OB_INCOMPLETE;

	sprintf(missions[7].primaryObjective[1], "Do not allow Phoebe to be killed");
	missions[7].primaryType[1] = M_PROTECT_TARGET;
	missions[7].target1[1] = CD_PHOEBE;
	missions[7].targetValue1[1] = 0;
	missions[7].completed1[1] = OB_CONDITION;

	sprintf(missions[7].primaryObjective[2], "Destroy all WEAPCO forces");
	missions[7].primaryType[2] = M_DESTROY_TARGET_TYPE;
	missions[7].target1[2] = CD_ANY;
	missions[7].targetValue1[2] = 35;
	missions[7].completed1[2] = OB_INCOMPLETE;

	missions[7].addAliens = ALWAYS;

	// Mission 8
	sprintf(missions[8].primaryObjective[0], "Assist medical supply craft");
	missions[8].primaryType[0] = M_ESCAPE_TARGET;
	missions[8].target1[0] = CD_GOODTRANSPORT;
	missions[8].targetValue1[0] = 0;
	missions[8].completed1[0] = OB_INCOMPLETE;

	sprintf(missions[8].primaryObjective[1], "Do not allow supply craft to be destroyed");
	missions[8].primaryType[1] = M_PROTECT_TARGET;
	missions[8].target1[1] = CD_GOODTRANSPORT;
	missions[8].targetValue1[1] = 0;
	missions[8].completed1[1] = OB_CONDITION;
	
	sprintf(missions[8].secondaryObjective[0], "Destroy all remaining WEAPCO fighters");
	missions[8].secondaryType[0] = M_DESTROY_ALL_TARGETS;
	missions[8].completed2[0] = OB_INCOMPLETE;

	missions[8].addAliens = FREQUENT;

	// Mission 9
	sprintf(missions[9].primaryObjective[0], "Disable five WEAPCO supply craft");
	missions[9].primaryType[0] = M_DISABLE_TARGET;
	missions[9].target1[0] = CD_CARGOSHIP;
	missions[9].targetValue1[0] = 5;
	missions[9].completed1[0] = OB_INCOMPLETE;

	sprintf(missions[9].primaryObjective[1], "Destroy all remaining WEAPCO fighters");
	missions[9].primaryType[1] = M_DESTROY_ALL_TARGETS;
	missions[9].completed1[1] = OB_INCOMPLETE;

	sprintf(missions[9].primaryObjective[2], "Protect supply craft AND Sid Wilson");
	missions[9].primaryType[2] = M_PROTECT_TARGET;
	missions[9].target1[2] = CD_CARGOSHIP;
	missions[9].targetValue1[2] = 0;
	missions[9].completed1[2] = OB_CONDITION;

	missions[9].addAliens = FREQUENT;

	// Mission 10
	sprintf(missions[10].primaryObjective[0], "Locate doctor's escape pod");
	missions[10].primaryType[0] = M_COLLECT;
	missions[10].target1[0] = P_ESCAPEPOD;
	missions[10].targetValue1[0] = 1;
	missions[10].completed1[0] = OB_INCOMPLETE;

	sprintf(missions[10].primaryObjective[1], "Do not destroy doctor's escape pod");
	missions[10].primaryType[1] = M_PROTECT_PICKUP;
	missions[10].target1[1] = P_ESCAPEPOD;
	missions[10].targetValue1[1] = 1; // DONE ON PURPOSE!! DO NOT CHANGE THIS!!!!
	missions[10].completed1[1] = OB_CONDITION;

	sprintf(missions[10].secondaryObjective[0], "Collect 10 pieces of Ore");
	missions[10].secondaryType[0] = M_COLLECT;
	missions[10].target2[0] = P_ORE;
	missions[10].targetValue2[0] = 10;
	missions[10].completed2[0] = OB_INCOMPLETE;

	missions[10].addAliens = ALWAYS;

	missions[10].timeLimit1[0] = 3;
	missions[10].timeLimit2[0] = 3;

	// Mission 11 (hmmm, that was too easy...)
	sprintf(missions[11].primaryObjective[0], "Destroy WEAPCO ore mining craft");
	missions[11].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[11].target1[0] = CD_BOSS;
	missions[11].targetValue1[0] = 1;
	missions[11].completed1[0] = OB_INCOMPLETE;

	sprintf(missions[11].secondaryObjective[0], "Save present slaves");
	missions[11].secondaryType[0] = M_PROTECT_PICKUP;
	missions[11].target2[0] = P_SLAVES;
	missions[11].completed2[0] = OB_CONDITION;

	sprintf(missions[11].primaryObjective[1], "Battle Kline");
	missions[11].primaryType[1] = M_ESCAPE_TARGET;
	missions[11].target1[1] = CD_KLINE;
	missions[11].targetValue1[1] = 1;
	missions[11].completed1[1] = OB_HIDDEN;

	missions[11].addAliens = NEVER;

	/*
		Mission 12 is the Destroy Experimental Fighter mission
		this is just a place holder
	*/

	// Mission 13
	sprintf(missions[13].primaryObjective[0], "Destroy Ursula's ship");
	missions[13].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[13].target1[0] = CD_EVILURSULA;
	missions[13].targetValue1[0] = 0;
	missions[13].completed1[0] = OB_INCOMPLETE;

	sprintf(missions[13].primaryObjective[1], "Capture Ursula's escape pod");
	missions[13].primaryType[1] = M_COLLECT;
	missions[13].target1[1] = P_ESCAPEPOD;
	missions[13].targetValue1[1] = 1;
	missions[13].completed1[1] = OB_INCOMPLETE;

	sprintf(missions[13].primaryObjective[2], "Do not kill Ursula");
	missions[13].primaryType[2] = M_PROTECT_PICKUP;
	missions[13].target1[2] = P_ESCAPEPOD;
	missions[13].targetValue1[2] = 0;
	missions[13].completed1[2] = OB_CONDITION;

	sprintf(missions[13].secondaryObjective[0], "Destroy all remaining WEAPCO fighters");
	missions[13].secondaryType[0] = M_DESTROY_ALL_TARGETS;
	missions[13].completed2[0] = OB_INCOMPLETE;

	missions[13].addAliens = FREQUENT;

	// Mission 14
	sprintf(missions[14].primaryObjective[0], "Assist attack on WEAPCO ore mining craft");
	missions[14].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[14].target1[0] = CD_BOSS;
	missions[14].targetValue1[0] = 1;
	missions[14].completed1[0] = OB_INCOMPLETE;

	sprintf(missions[14].primaryObjective[1], "At least 1 rebel craft must survive");
	missions[14].primaryType[1] = M_PROTECT_TARGET;
	missions[14].target1[1] = CD_REBELCARRIER;
	missions[14].targetValue1[1] = 2;
	missions[14].completed1[1] = OB_CONDITION;

	sprintf(missions[14].primaryObjective[2], "Destroy all present WEAPCO forces");
	missions[14].primaryType[2] = M_DESTROY_ALL_TARGETS;
	missions[14].completed1[2] = OB_INCOMPLETE;

	missions[14].addAliens = ALWAYS;

	// Mission 15
	sprintf(missions[15].primaryObjective[0], "Collect 25 pieces of Ore");
	missions[15].primaryType[0] = M_COLLECT;
	missions[15].target1[0] = P_ORE;
	missions[15].targetValue1[0] = 25;
	missions[15].completed1[0] = OB_INCOMPLETE;

	sprintf(missions[15].secondaryObjective[0], "Collect 25 pieces of Ore");
	missions[15].secondaryType[0] = M_COLLECT;
	missions[15].target2[0] = P_ORE;
	missions[15].targetValue2[0] = 25;
	missions[15].completed2[0] = OB_INCOMPLETE;

	missions[15].addAliens = ALWAYS;

	// Mission 16
	sprintf(missions[16].primaryObjective[0], "Collect $2000 to pay mercenary");
	missions[16].primaryType[0] = M_COLLECT;
	missions[16].target1[0] = P_CASH;
	missions[16].targetValue1[0] = 2000;
	missions[16].completed1[0] = OB_INCOMPLETE;

	sprintf(missions[16].primaryObjective[1], "Destroy all remaining WEAPCO fighters");
	missions[16].primaryType[1] = M_DESTROY_ALL_TARGETS;
	missions[16].completed1[1] = OB_INCOMPLETE;

	missions[16].addAliens = ALWAYS;

	// Mission 17
	sprintf(missions[17].primaryObjective[0], "Destroy escorts");
	missions[17].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[17].target1[0] = CD_ESCORT;
	missions[17].targetValue1[0] = 5;
	missions[17].completed1[0] = OB_INCOMPLETE;

	sprintf(missions[17].primaryObjective[1], "Disable executive transport");
	missions[17].primaryType[1] = M_ESCAPE_TARGET;
	missions[17].target1[1] = CD_BOSS;
	missions[17].targetValue1[1] = 1;
	missions[17].completed1[1] = OB_INCOMPLETE;

	sprintf(missions[17].primaryObjective[2], "Destroy all remaining WEAPCO fighters");
	missions[17].primaryType[2] = M_DESTROY_ALL_TARGETS;
	missions[17].completed1[2] = OB_INCOMPLETE;

	missions[17].addAliens = NORMAL;

	// Mission 18
	sprintf(missions[18].primaryObjective[0], "Destroy executive transport");
	missions[18].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[18].target1[0] = CD_BOSS;
	missions[18].targetValue1[0] = 1;
	missions[18].completed1[0] = OB_INCOMPLETE;

	missions[18].addAliens = ALWAYS;

	// Mission 19
	sprintf(missions[19].primaryObjective[0], "Destroy planetary guardian");
	missions[19].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[19].target1[0] = CD_PLUTOBOSS;
	missions[19].targetValue1[0] = 1;
	missions[19].completed1[0] = OB_INCOMPLETE;

	sprintf(missions[19].primaryObjective[1], "Destroy all remaining WEAPCO fighters");
	missions[19].primaryType[1] = M_DESTROY_ALL_TARGETS;
	missions[19].completed1[1] = OB_INCOMPLETE;

	missions[19].timeLimit1[0] = 5;
	missions[19].timeLimit1[1] = 5;

	missions[19].addAliens = ALWAYS;

	// Mission 20
	sprintf(missions[20].primaryObjective[0], "Destroy planetary guardian");
	missions[20].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[20].target1[0] = CD_NEPTUNEBOSS;
	missions[20].targetValue1[0] = 1;
	missions[20].completed1[0] = OB_INCOMPLETE;

	sprintf(missions[20].primaryObjective[1], "Destroy all remaining WEAPCO fighters");
	missions[20].primaryType[1] = M_DESTROY_ALL_TARGETS;
	missions[20].completed1[1] = OB_INCOMPLETE;

	missions[20].timeLimit1[0] = 5;
	missions[20].timeLimit1[1] = 5;

	missions[20].addAliens = ALWAYS;

	// Mission 21
	sprintf(missions[21].primaryObjective[0], "Destroy all present WEAPCO forces");
	missions[21].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[21].target1[0] = CD_URANUSBOSS;
	missions[21].targetValue1[0] = 1;
	missions[21].completed1[0] = OB_INCOMPLETE;

	sprintf(missions[21].primaryObjective[1], "Destroy all remaining WEAPCO fighters");
	missions[21].primaryType[1] = M_DESTROY_ALL_TARGETS;
	missions[21].completed1[1] = OB_INCOMPLETE;

	missions[21].timeLimit1[0] = 5;
	missions[21].timeLimit1[1] = 5;

	missions[21].addAliens = ALWAYS;

	// Mission 22
	sprintf(missions[22].primaryObjective[0], "Destroy outer defence systems");
	missions[22].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[22].target1[0] = CD_MOBILE_RAY;
	missions[22].targetValue1[0] = 6;
	missions[22].completed1[0] = OB_INCOMPLETE;

	sprintf(missions[22].primaryObjective[1], "Destroy all remaining WEAPCO craft");
	missions[22].primaryType[1] = M_DESTROY_ALL_TARGETS;
	missions[22].completed1[1] = OB_INCOMPLETE;

	missions[22].addAliens = NORMAL;

	// Mission 23
	sprintf(missions[23].primaryObjective[0], "Investigate distress call");
	missions[23].primaryType[0] = M_DESTROY_ALL_TARGETS;
	missions[23].completed1[0] = OB_CONDITION;

	sprintf(missions[23].primaryObjective[1], "Defeat Krass Tyler");
	missions[23].primaryType[1] = M_DESTROY_TARGET_TYPE;
	missions[23].target1[1] = CD_KRASS;
	missions[23].targetValue1[1] = 1;
	missions[23].completed1[1] = OB_HIDDEN;

	sprintf(missions[23].primaryObjective[2], "Destroy Krass' support group");
	missions[23].primaryType[2] = M_DESTROY_ALL_TARGETS;
	missions[23].target1[1] = CD_FIREFLY;
	missions[23].targetValue1[1] = 4;
	missions[23].completed1[2] = OB_HIDDEN;

	missions[23].addAliens = ALWAYS;

	// Mission 24
	sprintf(missions[24].primaryObjective[0], "Navigate asteroid belt");
	missions[24].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[24].target1[0] = CD_BOSS;
	missions[24].targetValue1[0] = 1;
	missions[24].completed1[0] = OB_INCOMPLETE;

	missions[24].timeLimit1[0] = 2;

	missions[24].addAliens = ALWAYS;

	// Mission 25
	sprintf(missions[25].primaryObjective[0], "Destroy WEAPCO frontline forces");
	missions[25].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[25].target1[0] = CD_ANY;
	missions[25].targetValue1[0] = 100;
	missions[25].completed1[0] = OB_INCOMPLETE;

	missions[25].addAliens = ALWAYS;

	// Mission 26
	sprintf(missions[26].primaryObjective[0], "Defeat Kline");
	missions[26].primaryType[0] = M_DESTROY_ALL_TARGETS;
	missions[26].completed1[0] = OB_INCOMPLETE;

	// Interception Mission
	strcpy(missions[MAX_MISSIONS - 1].primaryObjective[0], "");
	missions[MAX_MISSIONS - 1].primaryType[0] = M_DESTROY_ALL_TARGETS;
	missions[MAX_MISSIONS - 1].completed1[0] = OB_INCOMPLETE;
}
