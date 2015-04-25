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

const char *systemNames[] = {"Spirit", "Eyananth", "Mordor", "Sol"};

const char *systemBackground[] = {
			"gfx/spirit.jpg", "gfx/eyananth.jpg",
			"gfx/mordor.jpg", "gfx/sol.jpg"
};

/*
Drives the cursor. Is used by some other screens too
*/
static void doCursor()
{
	getPlayerInput();

	LIMIT(engine.cursor_x, 10, screen->w - 10 - shape[0]->w);
	LIMIT(engine.cursor_y, 10, screen->h - 10 - shape[0]->h);
	blit(shape[0], engine.cursor_x, engine.cursor_y);
}

/*
Sets the player's current status information lines. These are the lines
that are scrolled up the screen when the player clicks on Current Status
These are set only once.
*/
static void setStatusLines()
{
	char string[50];

	sprintf(string, "System : %s", systemNames[currentGame.system]);

	textSurface(0, string, 0, 0, FONT_WHITE);

	signed char total = 0;
	signed char completed = 0;
	
	for (int i = 0 ; i < 10 ; i++)
	{
		if (systemPlanet[i].missionNumber > -1)
		{
			switch(systemPlanet[i].missionCompleted)
   		{
				case 0:
					total++;
					break;
				case 1:
					total++;
					completed++;
					break;
			}
		}
	}

	for (int i = 0 ; i < 30 ; i++)
		textSurface(i, "", 0, 0, FONT_WHITE);

	sprintf(string, "Missions Completed : %d/%d", completed, total);
	textSurface(1, string, 0, 0, FONT_WHITE);

	sprintf(string, "Shots Fired : %d", currentGame.shots);
	textSurface(2, string, 0, 0, FONT_WHITE);

	sprintf(string, "Hits Scored : %d", currentGame.hits);
	textSurface(3, string, 0, 0, FONT_WHITE);

	sprintf(string, "Accuracy : %d%%", currentGame.accuracy);
	textSurface(4, string, 0, 0, FONT_WHITE);

	sprintf(string, "Enemies Killed by Others : %d", currentGame.totalOtherKills);
	textSurface(5, string, 0, 0, FONT_WHITE);

	sprintf(string, "Total Cash Earned : %d", currentGame.cashEarned);
	textSurface(6, string, 0, 0, FONT_WHITE);

	textSurface(7, "*** Chris ***", 0, 0, FONT_WHITE);

	sprintf(string, "Enemies Killed : %d", currentGame.totalKills);
	textSurface(8, string, 0, 0, FONT_WHITE);

	sprintf(string, "Shield Restores Picked Up : %d", currentGame.shieldPickups);
	textSurface(9, string, 0, 0, FONT_WHITE);

	sprintf(string, "Plasma Cells Picked Up : %d", currentGame.cellPickups);
	textSurface(10, string, 0, 0, FONT_WHITE);

	sprintf(string, "Rockets Picked Up : %d", currentGame.rocketPickups);
	textSurface(11, string, 0, 0, FONT_WHITE);

	sprintf(string, "Powerups Picked Up : %d", currentGame.rocketPickups);
	textSurface(12, string, 0, 0, FONT_WHITE);

	sprintf(string, "Mines Destroyed : %d", currentGame.minesKilled);
	textSurface(13, string, 0, 0, FONT_WHITE);

	sprintf(string, "Slaves Rescued : %d", currentGame.slavesRescued);
	textSurface(14, string, 0, 0, FONT_WHITE);

	sprintf(string, "Cargo Picked Up : %d", currentGame.cargoPickups);
	textSurface(15, string, 0, 0, FONT_WHITE);

	if (currentGame.hasWingMate1)
	{
		textSurface(16, "*** Phoebe ***", 0, 0, FONT_WHITE);

		sprintf(string, "Enemies Killed : %d", currentGame.wingMate1Kills);
		textSurface(17, string, 0, 0, FONT_WHITE);

		sprintf(string, "Ejections : %d", currentGame.wingMate1Ejects);
		textSurface(18, string, 0, 0, FONT_WHITE);
	}

	if (currentGame.hasWingMate2)
	{
		textSurface(19, "*** Ursula ***", 0, 0, FONT_WHITE);

		sprintf(string, "Enemies Killed : %d", currentGame.wingMate2Kills);
		textSurface(20, string, 0, 0, FONT_WHITE);

		sprintf(string, "Ejections : %d", currentGame.wingMate2Ejects);
		textSurface(21, string, 0, 0, FONT_WHITE);
	}

	signed char percentage = 0;
	if ((currentGame.secondaryMissions > 0) && (currentGame.secondaryMissionsCompleted > 0))
		percentage = (currentGame.secondaryMissionsCompleted / currentGame.secondaryMissions) * 100;
	sprintf(string, "Seconday Missions Completed : %d / %d (%d%%)", currentGame.secondaryMissionsCompleted, currentGame.secondaryMissions, percentage);
	textSurface(24, string, 0, 0, FONT_WHITE);

	int timeTaken = currentGame.timeTaken;

	snprintf(string, sizeof string, "Total Time : %2d:%02d:%02d", timeTaken / 3600, (timeTaken / 60) % 60, timeTaken % 60);

	textSurface(26, string, -1, 0, FONT_WHITE);
	textSurface(27, "Current Status", -1, 0, FONT_WHITE);

	textShape[0].y = 400;
	textShape[0].x = 150;

	for (int i = 1 ; i < 25 ; i++)
	{
		textShape[i].y = textShape[i - 1].y + 20;
		if ((i == 7) || (i == 16) || (i == 19))
			textShape[i].y += 25;

		textShape[i].x = 150;
	}

	textShape[26].y = 404;
	textShape[27].y = 83;
}

/*
Sets the names and stats of the planets within the current system.
This will later be placed into a data file.
*/
static void setSystemPlanets()
{
	FILE *fp;

	char string[100];
	strcpy(string, "");

	switch (currentGame.system)
	{
		case 0:
			strcpy(string, "data/planets_spirit.dat");
			break;
		case 1:
			strcpy(string, "data/planets_eyananth.dat");
			break;
		case 2:
			strcpy(string, "data/planets_mordor.dat");
			break;
		case 3:
			strcpy(string, "data/planets_sol.dat");
			break;
	}

	fp = fopen(string, "rb");

	int distance;
	char name[50];
	int image;

	for (int i = 0 ; i < 10 ; i++)
	{
		if (fscanf(fp, "%d %s %d", &distance, name, &image) < 3)
		{
			printf("Warning: Data for planet %i in \"%s\" is not correctly formatted\n", i, string);
			break;
		}

		systemPlanet[i].y = distance;
		strcpy(systemPlanet[i].name, name);
		systemPlanet[i].image = shape[image];
	}

 	int messageMission;
	int messageSlot;
	char face[50];
	char from[100];
	char subject[100];

	for (int i = 0 ; i < 10 ; i++)
	{
		if ((fscanf(fp, "%d %d %s%*c", &messageMission, &messageSlot, face) < 3) ||
			(fscanf(fp, "%[^\n]%*c", from) < 1) ||
			(fscanf(fp, "%[^\n]%*c", subject) < 1))
		{
			printf("Warning: Mission data for planet %i in \"%s\" is not correctly formatted\n", i, string);
			break;
		}

		systemPlanet[i].messageMission = messageMission;
		systemPlanet[i].messageSlot = messageSlot;
		systemPlanet[i].faceImage = getFace(face);

		strcpy(systemPlanet[i].from, from);
		strcpy(systemPlanet[i].subject, subject);
	}

	fclose(fp);
}

/*
Spins the planets around the sun, spaced according to their Y value
as defined in setSystemPlanets(). Moving the cursor over the planet
will show their name and their current status
*/
static bool showSystem(float x, float y, bool selectable)
{
	SDL_Rect r;
	signed char planet = 0;
	int planetSpace = systemPlanet[planet].y;
	bool rtn = false;

	// Blit the sun
	blit(shape[30], 370, 220);

	for (int i = 50 ; i < 300 ; i+= planetSpace)
	{
		x *= 0.75;
		y *= 0.75;

		circle(400, 250, i, screen, darkGrey);

		r.x = int(400 + (sinf(x) * i));
		r.y = int(250 + (cosf(y) * i));
		r.w = 10;
		r.h = 10;

		r.x -= (systemPlanet[planet].image->w / 2);
		r.y -= (systemPlanet[planet].image->h / 2);
		blit(systemPlanet[planet].image, r.x, r.y);

		if (selectable &&
			collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6,
				r.x, r.y, systemPlanet[planet].image->w,
				systemPlanet[planet].image->h))
		{
			drawString(systemPlanet[planet].name, -1, 545, FONT_WHITE);
			if ((engine.keyState[KEY_FIRE]))
			{
				if (currentGame.system == 0)
				{
					currentGame.stationedPlanet = planet;
					currentGame.destinationPlanet = planet;
					currentGame.area = systemPlanet[currentGame.stationedPlanet].missionNumber;
					strcpy(currentGame.stationedName, systemPlanet[currentGame.stationedPlanet].name);
					saveGame(0);
				}
				else
				{
					currentGame.destinationPlanet = planet;
					strcpy(currentGame.destinationName, systemPlanet[currentGame.destinationPlanet].name);
				}

				rtn = true;
				engine.keyState[KEY_FIRE] = 0;
			}
		}

		planet++;
		if (systemPlanet[planet].y == -1)
			break;
		planetSpace = systemPlanet[planet].y;
	}

	return rtn;
}

/*
Scrolls the player's current information up the screen. When
the specified status line reaches a certain Y value, the entire
list is reset and the information lines begin again from the bottom
(in other words, they loop around).
*/
static void showStatus(SDL_Surface *infoSurface)
{
	float speed = 0.25;

	if(engine.keyState[KEY_DOWN])
		speed = 1;
	else if(engine.keyState[KEY_UP])
		speed = -1;

	blit(infoSurface, 100, 80);

	for (int i = 0 ; i < 22 ; i++)
	{
		textShape[i].y -= speed;
		if ((textShape[i].y > 80) && (textShape[i].y < 400))
			blitText(i);
	}

	if (textShape[21].y < 65)
	{
		textShape[0].y = 400;

		for (int i = 1 ; i < 25 ; i++)
		{
			textShape[i].y = textShape[i - 1].y + 20;
			if ((i == 7) || (i == 16) || (i == 19))
				textShape[i].y += 25;
		}
	}

	blevelRect(100, 80, 600, 20, 0x00, 0x00, 0x99);

	blevelRect(100, 400, 600, 20, 0x00, 0x00, 0x99);

	blitText(26);
	blitText(27);
}

static void updateCommsSurface(SDL_Surface *comms)
{
	if (engine.commsSection == 1)
		return;

	char string[255];

	blevelRect(comms, 0, 10, comms->w - 1, 55, 0x00, 0x22, 0x00);
	blit(shape[FACE_CHRIS], 20, 15, comms);
	drawString("Chris Bainfield", 80, 15, FONT_WHITE, comms);
	sprintf(string, "Current Location: %s", systemPlanet[currentGame.stationedPlanet].name);
	drawString(string, 80, 35, FONT_WHITE, comms);
}

static void createCommsSurface(SDL_Surface *comms)
{
	engine.commsSection = 0;

	blevelRect(comms, 0, 0, comms->w - 1, comms->h - 1, 0x00, 0x00, 0x25);

	drawString("+++ RECEIVED MESSAGES +++", 115, 80, FONT_GREEN, comms);

	int yOffset;

	for (int i = 0 ; i < 10 ; i++)
	{
		if ((systemPlanet[i].messageSlot != -1) && (systemPlanet[i].missionCompleted == 0))
		{
			yOffset = systemPlanet[i].messageSlot * 60;
			blevelRect(comms, 0, 105 + yOffset, comms->w - 1, 55, 0x00, 0x00, 0x77);
			blit(shape[systemPlanet[i].faceImage], 20, 110 + yOffset, comms);
			drawString(systemPlanet[i].from, 80, 110 + yOffset, FONT_WHITE, comms);
			drawString(systemPlanet[i].subject, 80, 130 + yOffset, FONT_CYAN, comms);
			drawString("INCOMPLETE", 350, 110 + yOffset, FONT_RED, comms);
		}
	}

	updateCommsSurface(comms);
}

static void createMissionDetailSurface(SDL_Surface *comms, int missionSlot)
{
	char name[50];
	char string[2000];
	int y = 50;
	int newY = y;
	int col = FONT_WHITE;
	int mission = -1;
	int faceNumber = -1;
	FILE *fp;

	for (int i = 0 ; i < 10 ; i++)
	{
		if ((systemPlanet[i].messageSlot == missionSlot) && (systemPlanet[i].missionCompleted == 0))
		{
			mission = systemPlanet[i].messageMission;
		}
	}

	if (mission == -1)
		return;

	blevelRect(comms, 0, 0, comms->w - 1, comms->h - 1, 0x00, 0x00, 0x25);

	sprintf(string, "data/brief%d.txt", mission);

	fp = fopen(string, "rb");

	if (fscanf(fp, "%[^\n]%*c", name) < 1)
	{
		printf("Warning: Failed to retrieve name from \"%s\"\n", string);
		strcpy(name, "Error");
	}
	sprintf(string, "+++ Communication with %s +++", name);
	drawString(string, -1, 20, FONT_GREEN, comms);

	while (fscanf(fp, "%[^\n]%*c", string) == 1)
	{
		faceNumber = getFace(string);
		if (faceNumber > -1)
		{
			blit(shape[faceNumber], 10, y, comms);
			col = FONT_WHITE;
		}
		else
		{
			newY = drawString(string, 80, y, col, 1, comms) + 25;
			if (newY < y + 60)
				newY += (60 - (newY - y));
			y = newY;
		}
	}

	fclose(fp);

	blevelRect(comms, 5, comms->h - 28, 180, 20, 0x25, 0x00, 0x00);
	drawString("RETURN TO MESSAGES", 15, comms->h - 25, FONT_WHITE, 1, comms);

	engine.commsSection = 1;
}

static void doComms(SDL_Surface *comms)
{
	if ((engine.keyState[KEY_FIRE]))
	{
		if (engine.commsSection == 0)
		{
			for (int i = 0 ; i < 4 ; i++)
			{
				if (collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, 170, 180 + (i * 60), 430, 50))
				{
					createMissionDetailSurface(comms, i);
					engine.keyState[KEY_FIRE] = 0;
				}
			}
		}
		else
		{
			if (collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, 170, 440, 160, 20))
			{
				createCommsSurface(comms);
				engine.keyState[KEY_FIRE] = 0;
			}
		}
	}
}

static void createOptions(SDL_Surface *optionsSurface)
{
	SDL_FillRect(optionsSurface, NULL, black);

	blevelRect(optionsSurface, 0, 0, optionsSurface->w - 2, optionsSurface->h - 2, 0x00, 0x00, 0x44);

	drawString("++ OPTIONS ++", 105, 8, FONT_WHITE, optionsSurface);

	blevelRect(optionsSurface, 190, 45, 50, 22, 0x00, 0x00, 0x00);
	blevelRect(optionsSurface, 250, 45, 50, 22, 0x00, 0x00, 0x00);
	blevelRect(optionsSurface, 20, 45, 150, 22, 0x00, 0x00, 0x00);
	if (engine.useSound)
		blevelRect(optionsSurface, 190, 45, 50, 22, 0xff, 0x00, 0x00);
	else
		blevelRect(optionsSurface, 250, 45, 50, 22, 0xff, 0x00, 0x00);
	drawString("ON", 207, 50, FONT_WHITE, optionsSurface);
	drawString("OFF", 263, 50, FONT_WHITE, optionsSurface);
	drawString("SOUND", 30, 50, FONT_WHITE, optionsSurface);

	blevelRect(optionsSurface, 190, 95, 50, 22, 0x00, 0x00, 0x00);
	blevelRect(optionsSurface, 250, 95, 50, 22, 0x00, 0x00, 0x00);
	blevelRect(optionsSurface, 20, 95, 150, 22, 0x00, 0x00, 0x00);
	if (engine.useMusic)
		blevelRect(optionsSurface, 190, 95, 50, 22, 0xff, 0x00, 0x00);
	else
		blevelRect(optionsSurface, 250, 95, 50, 22, 0xff, 0x00, 0x00);
	drawString("ON", 207, 100, FONT_WHITE, optionsSurface);
	drawString("OFF", 263, 100, FONT_WHITE, optionsSurface);
	drawString("MUSIC", 30, 100, FONT_WHITE, optionsSurface);

 	blevelRect(optionsSurface, 190, 145, 50, 22, 0x00, 0x00, 0x00);
	blevelRect(optionsSurface, 250, 145, 50, 22, 0x00, 0x00, 0x00);
	blevelRect(optionsSurface, 20, 145, 150, 22, 0x00, 0x00, 0x00);
	if (engine.fullScreen)
		blevelRect(optionsSurface, 190, 145, 50, 22, 0xff, 0x00, 0x00);
	else
		blevelRect(optionsSurface, 250, 145, 50, 22, 0xff, 0x00, 0x00);
	drawString("ON", 207, 150, FONT_WHITE, optionsSurface);
	drawString("OFF", 263, 150, FONT_WHITE, optionsSurface);
	drawString("FULLSCREEN", 30, 150, FONT_WHITE, optionsSurface);
}

static void showOptions(SDL_Surface *optionsSurface)
{
	if ((engine.keyState[KEY_FIRE]))
	{
		if (collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, 417, 172, 45, 22))
			engine.useSound = true;

		if (collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, 478, 172, 45, 22))
			engine.useSound = false;


		if (collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, 417, 222, 45, 22))
		{
			engine.useMusic = true;
			audio_playMusic("music/through_space.ogg", -1);
		}

		if (collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, 478, 222, 45, 22))
		{
			engine.useMusic = false;
			audio_haltMusic();
		}

		if (collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, 417, 272, 45, 22))
		{
			if (!engine.fullScreen)
			{
				SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
				engine.fullScreen = true;
			}
		}

		if (collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, 478, 272, 45, 22))
		{
			if (engine.fullScreen)
			{
				SDL_SetWindowFullscreen(window, 0);
				engine.fullScreen = false;
			}
		}

		createOptions(optionsSurface);
	}
}

/*
Oddly named function that controls the entire intermission
screen. This simply draws a background, stars, gridlines and the icons
at the bottom of the screen. Will call (and continue to call) the specified
functions when the player has selected an icon.
*/
int galaxyMap()
{
	freeGraphics();

	checkForBossMission(); // double check just to make sure!

	// Tell the game we are not in a mission so
	// do not perform certain keyboard actions
	engine.gameSection = SECTION_INTERMISSION;

	clearScreen(black);
	updateScreen();
	clearScreen(black);

	initSaveSlots();

	loadBackground(systemBackground[currentGame.system]);

	char string[25];

	engine.cursor_x = screen->w / 2;
	engine.cursor_y = screen->h / 2;
	shape[0] = loadImage("gfx/cursor.png");

	// Icons 1 - 29
	for (int i = 0 ; i < 26 ; i++)
	{
		sprintf(string, "gfx/icon%d.png", (i + 1));
		shape[i + 1] = loadImage(string);
	}

	shape[27] = loadImage("gfx/buyIcon.png");
	shape[28] = loadImage("gfx/sellIcon.png");
	shape[29] = loadImage("gfx/firefly1.png");

	// Planets 30 - 39
	shape[30] = loadImage("gfx/planet_sun.png");
	shape[31] = loadImage("gfx/planet_green.png");
	shape[32] = loadImage("gfx/planet_blue.png");
	shape[33] = loadImage("gfx/planet_red.png");
	shape[34] = loadImage("gfx/planet_orange.png");

	// Faces (as defines)
	shape[FACE_CHRIS] = loadImage("gfx/face_chris.png");
	shape[FACE_SID] = loadImage("gfx/face_sid.png");
	shape[FACE_KRASS] = loadImage("gfx/face_krass.png");
	shape[FACE_PHOEBE] = loadImage("gfx/face_phoebe.png");
	shape[FACE_URSULA] = loadImage("gfx/face_ursula.png");
	shape[FACE_KLINE] = loadImage("gfx/face_kline.png");

	engine.done = 0;
	engine.keyState[KEY_FIRE] = 0;
	engine.ssx = 0;
	engine.ssy = 0;

	SDL_Rect r;
	SDL_Rect destRect;
	int distance = 0;
	int interceptionChance = 0;

	setStatusLines();
	initShop();
	setSystemPlanets();

	SDL_Surface *statsSurface = alphaRect(600, 330, 0x00, 0x00, 0x99);
	SDL_Surface *savesSurface = createSurface(350, 300);
	SDL_Surface *optionsSurface = createSurface(320, 240);
	SDL_Surface *commsSurface = createSurface(450, 400);

	createSavesSurface(savesSurface, -1);
	createOptions(optionsSurface);
	createCommsSurface(commsSurface);

	signed char section = 1;

	float sinX = 300;
	float cosY = 300;
	bool movePlanets = true;
	signed char saveSlot = -1;

	// Remove the Supercharge, if it is there
	if ((currentGame.difficulty != DIFFICULTY_EASY) &&
		(currentGame.difficulty != DIFFICULTY_ORIGINAL))
	{
		weapon[W_PLAYER_WEAPON].reload[0] = MAX(
			weapon[W_PLAYER_WEAPON].reload[0],
			rate2reload[currentGame.maxPlasmaRate]);
		weapon[W_PLAYER_WEAPON].ammo[0] = MIN(weapon[W_PLAYER_WEAPON].ammo[0],
			currentGame.maxPlasmaOutput);
		weapon[W_PLAYER_WEAPON].damage = MIN(weapon[W_PLAYER_WEAPON].damage,
			currentGame.maxPlasmaDamage);
	}

	if (currentGame.system > 0)
		interceptionChance = (300 / currentGame.system);

	// There is no chance of being interceptted after the final attack on Earth
	if ((currentGame.system == 3) && (systemPlanet[2].missionCompleted))
		interceptionChance = 0;

	int rtn = 0;

	if ((engine.useAudio) && (engine.useMusic))
		audio_playMusic("music/through_space.ogg", -1);

	textObject iconInfo[12];

	iconInfo[0].image = textSurface("Start Next Mission", FONT_WHITE);
	iconInfo[1].image = textSurface("View System Map", FONT_WHITE);
	iconInfo[2].image = textSurface("Current Status", FONT_WHITE);
	iconInfo[3].image = textSurface("Save Game", FONT_WHITE);
	iconInfo[4].image = textSurface("Upgrade FIREFLY", FONT_WHITE);
	iconInfo[5].image = textSurface("Comms", FONT_WHITE);
	iconInfo[6].image = textSurface("Options", FONT_WHITE);
	iconInfo[7].image = textSurface("Exit to Title Screen", FONT_WHITE);

	sprintf(string, "System : %s", systemNames[currentGame.system]);
	iconInfo[8].image = textSurface(string, FONT_WHITE);

	sprintf(string, "Stationed At: %s", systemPlanet[currentGame.stationedPlanet].name);
	iconInfo[9].image = textSurface(string, FONT_WHITE);

	if (currentGame.destinationPlanet > -1)
		sprintf(string, "Destination: %s", systemPlanet[currentGame.destinationPlanet].name);
	else
		strcpy(string, "Destination: None");
	iconInfo[10].image = textSurface(string, FONT_WHITE);
	for (int i = 0 ; i < 9 ; i++)
		iconInfo[i].x = (800 - iconInfo[i].image->w) / 2;

	iconInfo[11].image = textSurface("Go to Destination Planet", FONT_WHITE);

	bool redrawBackGround = true;

	if (currentGame.distanceCovered > 0)
		section = 0;
	else
		player.shield = player.maxShield;

	flushInput();
	engine.keyState[KEY_FIRE] = engine.keyState[KEY_ALTFIRE] = 0;
	engine.done = 0;

	while (!engine.done)
	{
		updateScreen();

		if (redrawBackGround)
		{
			drawBackGround();
			redrawBackGround = false;
		}
		else
		{
			unBuffer();
		}

  		doStarfield();

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


		if (rand() % 1000 < 2)
		{
			// XXX: This code originally had the range set to [100, 100],
			// which effectively always caused the star speed to be set to
			// 1. I don't think this was the intention, so I changed the
			// minimum from 100 to -100, which is what I think was probably
			// intended.
			engine.ssx = RANDRANGE(-100, 100);
			engine.ssy = RANDRANGE(-100, 100);
			engine.ssx /= 100;
			engine.ssy /= 100;
		}

		blit(iconInfo[8].image, (int)iconInfo[8].x, 15);

		switch(section)
		{
			case 0:
				if (currentGame.stationedPlanet == currentGame.destinationPlanet)
				{
					currentGame.area = systemPlanet[currentGame.stationedPlanet].missionNumber;
					rtn = 2;
					engine.done = 1;
				}
				else
				{
					distance = abs(currentGame.stationedPlanet - currentGame.destinationPlanet);
					distance = (5 / distance);
					if (distance < 1)
						distance = 1;

					SDL_FreeSurface(iconInfo[9].image);
					iconInfo[9].image = textSurface(systemPlanet[currentGame.stationedPlanet].name, FONT_WHITE);

					SDL_FreeSurface(iconInfo[10].image);
					iconInfo[10].image = textSurface(systemPlanet[currentGame.destinationPlanet].name, FONT_WHITE);

					section = 8;

					destRect.x = 180;
					destRect.y = 450;
					destRect.w = 1;
					if (currentGame.distanceCovered > 0)
						destRect.w = currentGame.distanceCovered;
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
					sinX += 0.01;
					cosY += 0.01;
				}

				if (showSystem(sinX, cosY, true))
				{
					if (currentGame.system == 0)
					{
						sprintf(string, "Stationed At: %s", systemPlanet[currentGame.stationedPlanet].name);
						SDL_FreeSurface(iconInfo[9].image);
						iconInfo[9].image = textSurface(string, FONT_WHITE);
						updateCommsSurface(commsSurface);
					}
					else
					{
						sprintf(string, "Destination: %s", systemPlanet[currentGame.destinationPlanet].name);
						SDL_FreeSurface(iconInfo[10].image);
						iconInfo[10].image = textSurface(string, FONT_WHITE);
					}
				}

				blit(iconInfo[9].image, 90, 450);
				if ((currentGame.system > 0) && (currentGame.stationedPlanet != currentGame.destinationPlanet))
					blit(iconInfo[10].image, 550, 450);
				break;

			case 2:
				showStatus(statsSurface);
				break;

			case 3:
				blit(savesSurface, 200, 100);
				saveSlot = showSaveSlots(savesSurface, saveSlot);
				break;

			case 4:
				showShop();
				break;

			case 5:
				blit(commsSurface, 170, 70);
				doComms(commsSurface);
				break;

			case 6:
				blit(optionsSurface, 230, 130);
				showOptions(optionsSurface);
				break;

			case 7:
				rtn = 0;
				engine.done = 1;
				break;

			case 8:
				showSystem(sinX, cosY, false);

				blit(systemPlanet[currentGame.stationedPlanet].image, 150, 450);
				blit(iconInfo[9].image, 135, 480);
				blit(systemPlanet[currentGame.destinationPlanet].image, 650, 450);
				blit(iconInfo[10].image, 635, 480);

				destRect.w += distance;
				SDL_FillRect(screen, &destRect, red);

				if (destRect.w >= 450)
				{
					currentGame.stationedPlanet = currentGame.destinationPlanet;
					currentGame.distanceCovered = 0;
					player.shield = player.maxShield;
					sprintf(string, "Stationed At: %s",
						systemPlanet[currentGame.stationedPlanet].name);
					strcpy(currentGame.stationedName,
						systemPlanet[currentGame.stationedPlanet].name);
					SDL_FreeSurface(iconInfo[9].image);
					iconInfo[9].image = textSurface(string, FONT_WHITE);
					updateCommsSurface(commsSurface);
					section = 1;
					redrawBackGround = true;
					saveGame(0);
				}
				else if (interceptionChance > 0)
				{
					if ((rand() % interceptionChance) == 0)
					{
						currentGame.area = MISN_INTERCEPTION;
						rtn = 2;
						engine.done = 1;
						currentGame.distanceCovered = destRect.w;
					}
				}

				break;
		}

		addBuffer(300, 545, 200, 15);

		if (section != 8)
		{
			for (int i = 0 ; i < 8 ; i++)
			{
				// if the mission has been completed, there is no
				// "Start Next Mission" icon
				if (i == 0)
				{
					if ((currentGame.stationedPlanet == currentGame.destinationPlanet) &&
							(systemPlanet[currentGame.stationedPlanet].missionCompleted != 0))
						continue;
					else if (currentGame.stationedPlanet == currentGame.destinationPlanet)
						blit(shape[1], 80 + (i * 90), 500);
					else if (currentGame.stationedPlanet != currentGame.destinationPlanet)
						blit(shape[26], 80 + (i * 90), 500);
				}
				else
				{
					blit(shape[i + 1], 80 + (i * 90), 500);
				}

				if (collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, 80 + (i * 90), 500, 32, 32))
				{
					if (i != 0)
					{
						blit(iconInfo[i].image, (int)iconInfo[i].x, 545);
					}
					else
					{
						if (currentGame.stationedPlanet == currentGame.destinationPlanet)
							blit(iconInfo[0].image, (int)iconInfo[i].x, 545);
						else
							blit(iconInfo[11].image, (int)iconInfo[i].x, 545);
					}

					if ((engine.keyState[KEY_FIRE]))
					{
						redrawBackGround = true;
						section = i;
						engine.keyState[KEY_FIRE] = 0;
					}
				}
			}
		}

		engine.keyState[KEY_FIRE] = engine.keyState[KEY_ALTFIRE] = 0;
		doCursor();

		delayFrame();
	}

	audio_haltMusic();
	SDL_FreeSurface(statsSurface);
	SDL_FreeSurface(savesSurface);
	SDL_FreeSurface(optionsSurface);
	SDL_FreeSurface(commsSurface);
	for (int i = 0 ; i < 12 ; i++)
		SDL_FreeSurface(iconInfo[i].image);

	if (currentGame.distanceCovered == 0)
		player.shield = player.maxShield;

	return rtn;
}

