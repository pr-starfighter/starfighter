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

#include "intermission.h"

/*
Drives the cursor. Is used by some other screens too
*/
void doCursor()
{
	getPlayerInput();

	Math::limitInt(&engine.cursor_x, 10, 790);
	Math::limitInt(&engine.cursor_y, 10, 590);
	graphics.blit(graphics.shape[0], engine.cursor_x, engine.cursor_y);
}

/*
Sets the player's current status information lines. These are the lines
that are scrolled up the screen when the player clicks on Current Status
These are set only once.
*/
void setStatusLines()
{
	char string[50];

	sprintf(string, "System : %s", systemNames[currentGame.system]);

	graphics.textSurface(0, string, 0, 0, FONT_WHITE);

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
		graphics.textSurface(i, "", 0, 0, FONT_WHITE);

	sprintf(string, "Missions Completed : %d/%d", completed, total);
	graphics.textSurface(1, string, 0, 0, FONT_WHITE);

	sprintf(string, "Shots Fired : %d", currentGame.shots);
	graphics.textSurface(2, string, 0, 0, FONT_WHITE);

	sprintf(string, "Hits Scored : %d", currentGame.hits);
	graphics.textSurface(3, string, 0, 0, FONT_WHITE);

	sprintf(string, "Accuracy : %d%%", currentGame.accuracy);
	graphics.textSurface(4, string, 0, 0, FONT_WHITE);

	sprintf(string, "Enemies Killed by Others : %d", currentGame.totalOtherKills);
	graphics.textSurface(5, string, 0, 0, FONT_WHITE);

	sprintf(string, "Total Cash Earned : %d", currentGame.cashEarned);
	graphics.textSurface(6, string, 0, 0, FONT_WHITE);

	graphics.textSurface(7, "*** Chris ***", 0, 0, FONT_WHITE);

	sprintf(string, "Enemies Killed : %d", currentGame.totalKills);
	graphics.textSurface(8, string, 0, 0, FONT_WHITE);

	sprintf(string, "Shield Restores Picked Up : %d", currentGame.shieldPickups);
	graphics.textSurface(9, string, 0, 0, FONT_WHITE);

	sprintf(string, "Plasma Cells Picked Up : %d", currentGame.cellPickups);
	graphics.textSurface(10, string, 0, 0, FONT_WHITE);

	sprintf(string, "Rockets Picked Up : %d", currentGame.rocketPickups);
	graphics.textSurface(11, string, 0, 0, FONT_WHITE);

	sprintf(string, "Powerups Picked Up : %d", currentGame.rocketPickups);
	graphics.textSurface(12, string, 0, 0, FONT_WHITE);

	sprintf(string, "Mines Destroyed : %d", currentGame.minesKilled);
	graphics.textSurface(13, string, 0, 0, FONT_WHITE);

	sprintf(string, "Slaves Rescued : %d", currentGame.slavesRescued);
	graphics.textSurface(14, string, 0, 0, FONT_WHITE);

	sprintf(string, "Cargo Picked Up : %d", currentGame.cargoPickups);
	graphics.textSurface(15, string, 0, 0, FONT_WHITE);

	if (currentGame.hasWingMate1)
	{
		graphics.textSurface(16, "*** Phoebe ***", 0, 0, FONT_WHITE);

		sprintf(string, "Enemies Killed : %d", currentGame.wingMate1Kills);
		graphics.textSurface(17, string, 0, 0, FONT_WHITE);

		sprintf(string, "Ejections : %d", currentGame.wingMate1Ejects);
		graphics.textSurface(18, string, 0, 0, FONT_WHITE);
	}

	if (currentGame.hasWingMate2)
	{
		graphics.textSurface(19, "*** Ursula ***", 0, 0, FONT_WHITE);

		sprintf(string, "Enemies Killed : %d", currentGame.wingMate2Kills);
		graphics.textSurface(20, string, 0, 0, FONT_WHITE);

		sprintf(string, "Ejections : %d", currentGame.wingMate2Ejects);
		graphics.textSurface(21, string, 0, 0, FONT_WHITE);
	}

	signed char percentage = 0;
	if ((currentGame.secondaryMissions > 0) && (currentGame.secondaryMissionsCompleted > 0))
		percentage = (currentGame.secondaryMissionsCompleted / currentGame.secondaryMissions) * 100;
	sprintf(string, "Seconday Missions Completed : %d / %d (%d%%)", currentGame.secondaryMissionsCompleted, currentGame.secondaryMissions, percentage);
	graphics.textSurface(24, string, 0, 0, FONT_WHITE);

	int timeTaken = currentGame.timeTaken;

	snprintf(string, sizeof string, "Total Time : %2d:%02d:%02d", timeTaken / 3600, (timeTaken / 60) % 60, timeTaken % 60);

	graphics.textSurface(26, string, -1, 0, FONT_WHITE);
	graphics.textSurface(27, "Current Status", -1, 0, FONT_WHITE);

	graphics.textShape[0].y = 400;
	graphics.textShape[0].x = 150;

	for (int i = 1 ; i < 25 ; i++)
	{
		graphics.textShape[i].y = graphics.textShape[i - 1].y + 20;
		if ((i == 7) || (i == 16) || (i == 19))
			graphics.textShape[i].y += 25;

		graphics.textShape[i].x = 150;
	}

	graphics.textShape[26].y = 404;
	graphics.textShape[27].y = 83;
}

/*
Sets the names and stats of the planets within the current system.
This will later be placed into a data file.
*/
void setSystemPlanets()
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

	#if USEPACK
	int dataLocation = locateDataInPak(string, 1);
	fp = fopen(PACKLOCATION, "rb");
	fseek(fp, dataLocation, SEEK_SET);
	#else
	fp = fopen(string, "rb");
	#endif

	int distance;
	char name[50];
	int image;

	for (int i = 0 ; i < 10 ; i++)
	{
		fscanf(fp, "%d %s %d", &distance, name, &image);

		systemPlanet[i].y = distance;
		strcpy(systemPlanet[i].name, name);
		systemPlanet[i].image = graphics.shape[image];
	}

 	int messageMission;
	int messageSlot;
	char face[50];
	char from[100];
	char subject[100];

	for (int i = 0 ; i < 10 ; i++)
	{
		fscanf(fp, "%d %d %s%*c", &messageMission, &messageSlot, face);
		fscanf(fp, "%[^\n]%*c", from);
		fscanf(fp, "%[^\n]%*c", subject);

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
signed char showSystem(float x, float y)
{
	SDL_Rect r;
	signed char planet = 0;
	int planetSpace = systemPlanet[planet].y;
	signed char rtn = 0;

	// Blit the sun
	graphics.blit(graphics.shape[30], 370, 220);

	for (int i = 50 ; i < 300 ; i+= planetSpace)
	{
		x *= 0.75;
		y *= 0.75;

		graphics.circle(400, 250, i, graphics.screen, graphics.darkGrey);

		r.x = int(400 + (sinf(x) * i));
		r.y = int(250 + (cosf(y) * i));
		r.w = 10;
		r.h = 10;

		r.x -= (systemPlanet[planet].image->w / 2);
		r.y -= (systemPlanet[planet].image->h / 2);
		graphics.blit(systemPlanet[planet].image, r.x, r.y);

		if (Collision::collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, r.x, r.y, systemPlanet[planet].image->w, systemPlanet[planet].image->h))
		{
			graphics.drawString(systemPlanet[planet].name, -1, 545, FONT_WHITE);
			if ((engine.keyState[SDLK_LCTRL]) || (engine.keyState[SDLK_RCTRL]))
			{
				if (currentGame.system == 0)
				{
					currentGame.stationedPlanet = planet;
					currentGame.destinationPlanet = planet;
					currentGame.area = systemPlanet[currentGame.stationedPlanet].missionNumber;
					strcpy(currentGame.stationedName, systemPlanet[currentGame.stationedPlanet].name);
				}
				else
				{
					currentGame.destinationPlanet = planet;
					strcpy(currentGame.destinationName, systemPlanet[currentGame.destinationPlanet].name);
				}

				rtn = 1;
				engine.keyState[SDLK_LCTRL] = engine.keyState[SDLK_RCTRL] = 0;
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
void showStatus(SDL_Surface *infoSurface)
{
	graphics.blit(infoSurface, 100, 80);

	for (int i = 0 ; i < 22 ; i++)
	{
		graphics.textShape[i].y -= 0.25;
		if ((graphics.textShape[i].y > 80) && (graphics.textShape[i].y < 400))
			graphics.blitText(i);
	}

	if (graphics.textShape[21].y < 65)
	{
		graphics.textShape[0].y = 400;

		for (int i = 1 ; i < 25 ; i++)
		{
			graphics.textShape[i].y = graphics.textShape[i - 1].y + 20;
			if ((i == 7) || (i == 16) || (i == 19))
				graphics.textShape[i].y += 25;
		}
	}

	graphics.blevelRect(100, 80, 600, 20, 0x00, 0x00, 0x99);

	graphics.blevelRect(100, 400, 600, 20, 0x00, 0x00, 0x99);

	graphics.blitText(26);
	graphics.blitText(27);
}

void createOptions(SDL_Surface *optionsSurface)
{
	SDL_FillRect(optionsSurface, NULL, graphics.black);

	graphics.blevelRect(optionsSurface, 0, 0, optionsSurface->w - 2, optionsSurface->h - 2, 0x00, 0x00, 0x44);

	graphics.drawString("++ OPTIONS ++", 105, 8, FONT_WHITE, optionsSurface);

	graphics.blevelRect(optionsSurface, 190, 45, 50, 22, 0x00, 0x00, 0x00);
	graphics.blevelRect(optionsSurface, 250, 45, 50, 22, 0x00, 0x00, 0x00);
	graphics.blevelRect(optionsSurface, 20, 45, 150, 22, 0x00, 0x00, 0x00);
	if (currentGame.useSound)
		graphics.blevelRect(optionsSurface, 190, 45, 50, 22, 0xff, 0x00, 0x00);
	else
		graphics.blevelRect(optionsSurface, 250, 45, 50, 22, 0xff, 0x00, 0x00);
	graphics.drawString("ON", 207, 50, FONT_WHITE, optionsSurface);
	graphics.drawString("OFF", 263, 50, FONT_WHITE, optionsSurface);
	graphics.drawString("SOUND", 30, 50, FONT_WHITE, optionsSurface);

	graphics.blevelRect(optionsSurface, 190, 95, 50, 22, 0x00, 0x00, 0x00);
	graphics.blevelRect(optionsSurface, 250, 95, 50, 22, 0x00, 0x00, 0x00);
	graphics.blevelRect(optionsSurface, 20, 95, 150, 22, 0x00, 0x00, 0x00);
	if (currentGame.useMusic)
		graphics.blevelRect(optionsSurface, 190, 95, 50, 22, 0xff, 0x00, 0x00);
	else
		graphics.blevelRect(optionsSurface, 250, 95, 50, 22, 0xff, 0x00, 0x00);
	graphics.drawString("ON", 207, 100, FONT_WHITE, optionsSurface);
	graphics.drawString("OFF", 263, 100, FONT_WHITE, optionsSurface);
	graphics.drawString("MUSIC", 30, 100, FONT_WHITE, optionsSurface);

 	graphics.blevelRect(optionsSurface, 190, 145, 50, 22, 0x00, 0x00, 0x00);
	graphics.blevelRect(optionsSurface, 250, 145, 50, 22, 0x00, 0x00, 0x00);
	graphics.blevelRect(optionsSurface, 20, 145, 150, 22, 0x00, 0x00, 0x00);
	if (currentGame.fullScreen)
		graphics.blevelRect(optionsSurface, 190, 145, 50, 22, 0xff, 0x00, 0x00);
	else
		graphics.blevelRect(optionsSurface, 250, 145, 50, 22, 0xff, 0x00, 0x00);
	graphics.drawString("ON", 207, 150, FONT_WHITE, optionsSurface);
	graphics.drawString("OFF", 263, 150, FONT_WHITE, optionsSurface);
	graphics.drawString("FULLSCREEN", 30, 150, FONT_WHITE, optionsSurface);

	graphics.blevelRect(optionsSurface, 20, 195, 150, 22, 0x00, 0x00, 0x00);
	graphics.blevelRect(optionsSurface, 190, 195, 110, 22, 0x00, 0x00, 0x00);
	if (currentGame.autoSaveSlot == -1)
	{
		graphics.drawString("NONE", 225, 200, FONT_WHITE, optionsSurface);
	}
	else
	{
		char string[] = "Slot %d";
		sprintf(string, "Slot %d", currentGame.autoSaveSlot + 1);
		graphics.blevelRect(optionsSurface, 190, 195, 110, 22, 0xff, 0x00, 0x00);
		graphics.drawString(string, 225, 200, FONT_WHITE, optionsSurface);
	}
	graphics.drawString("AUTOSAVE SLOT", 30, 200, FONT_WHITE, optionsSurface);
}

void showOptions(SDL_Surface *optionsSurface)
{
	if ((engine.keyState[SDLK_LCTRL]) || (engine.keyState[SDLK_RCTRL]))
	{
		if (Collision::collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, 417, 172, 45, 22))
			currentGame.useSound = true;

		if (Collision::collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, 478, 172, 45, 22))
			currentGame.useSound = false;


		if (Collision::collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, 417, 222, 45, 22))
		{
			currentGame.useMusic = true;
			if (engine.useAudio)
			{
				if (Mix_PausedMusic() == 1)
					Mix_ResumeMusic();
				else
					Mix_PlayMusic(engine.music, -1);
			}
		}

		if (Collision::collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, 478, 222, 45, 22))
		{
			currentGame.useMusic = false;
			if (engine.useAudio)
				Mix_PauseMusic();
		}

		if (Collision::collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, 417, 272, 45, 22))
		{
			if (!currentGame.fullScreen)
			{
				#if LINUX
				SDL_WM_ToggleFullScreen(graphics.screen);
				#else
				graphics.screen = SDL_SetVideoMode(800, 600, 0, SDL_DOUBLEBUF|SDL_HWPALETTE|SDL_FULLSCREEN);
				graphics.drawBackground();
				flushBuffer();
				#endif
				currentGame.fullScreen = true;
			}
		}

		if (Collision::collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, 478, 272, 45, 22))
		{
			if (currentGame.fullScreen)
			{
				#if LINUX
				SDL_WM_ToggleFullScreen(graphics.screen);
				#else
				graphics.screen = SDL_SetVideoMode(800, 600, 0, SDL_DOUBLEBUF|SDL_HWPALETTE);
				graphics.drawBackground();
				flushBuffer();
				#endif
				currentGame.fullScreen = false;
			}
		}
		
		if (Collision::collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, 417, 322, 100, 22))
		{
			Math::wrapChar(&(++currentGame.autoSaveSlot), -1, 4);
			engine.keyState[SDLK_LCTRL] = engine.keyState[SDLK_RCTRL] = 0;
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
	graphics.freeGraphics();

	checkForBossMission(); // double check just to make sure!

	// Tell the game we are not in a mission so
	// do not perform certain keyboard actions
	engine.gameSection = SECTION_INTERMISSION;

	graphics.clearScreen(graphics.black);
	graphics.updateScreen();
	graphics.clearScreen(graphics.black);

	initSaveSlots();

	loadMusic("music/3DParadise.mod");

	loadBackground((char *)systemBackground[currentGame.system]);

	char string[25];

	engine.cursor_x = engine.cursor_y = 500;
	graphics.shape[0] = loadImage("gfx/cursor.bmp");

	// Icons 1 - 29
	for (int i = 0 ; i < 26 ; i++)
	{
		sprintf(string, "gfx/icon%d.bmp", (i + 1));
		graphics.shape[i + 1] = loadImage(string);
	}

	graphics.shape[27] = loadImage("gfx/buyIcon.bmp");
	graphics.shape[28] = loadImage("gfx/sellIcon.bmp");
	graphics.shape[29] = loadImage("gfx/firefly1.png");

	// Planets 30 - 39
	graphics.shape[30] = loadImage("gfx/planet_sun.gif");
	graphics.shape[31] = loadImage("gfx/planet_green.gif");
	graphics.shape[32] = loadImage("gfx/planet_blue.gif");
	graphics.shape[33] = loadImage("gfx/planet_red.gif");
	graphics.shape[34] = loadImage("gfx/planet_orange.gif");

	// Faces (as defines)
	graphics.shape[FACE_CHRIS] = loadImage("gfx/face_chris.png");
	graphics.shape[FACE_SID] = loadImage("gfx/face_sid.png");
	graphics.shape[FACE_KRASS] = loadImage("gfx/face_krass.png");
	graphics.shape[FACE_PHOEBE] = loadImage("gfx/face_phoebe.png");
	graphics.shape[FACE_URSULA] = loadImage("gfx/face_ursula.png");
	graphics.shape[FACE_KLINE] = loadImage("gfx/face_kline.png");

	engine.done = 0;
	engine.keyState[SDLK_LCTRL] = engine.keyState[SDLK_RCTRL] = 0;
	engine.ssx = engine.ssy = 0;

	SDL_Rect r;
	SDL_Rect destRect;
	int distance = 0;
	int interceptionChance = 0;

	setStatusLines();
	initShop();
	setSystemPlanets();

	SDL_Surface *statsSurface = graphics.alphaRect(600, 330, 0x00, 0x00, 0x99);
	SDL_Surface *savesSurface = graphics.createSurface(350, 300);
	SDL_Surface *optionsSurface = graphics.createSurface(320, 240);
	SDL_Surface *commsSurface = graphics.createSurface(450, 400);

	createSavesSurface(savesSurface, -1);
	createOptions(optionsSurface);
	createCommsSurface(commsSurface);

	signed char section = 1;

	float sinX = 300;
	float cosY = 300;
	signed char movePlanets = 1;
	signed char saveSlot = -1;

	if (currentGame.system > 0)
		interceptionChance = (300 / currentGame.system);

	// There is no chance of being interceptted after the final attack on Earth
	if ((currentGame.system == 3) && (systemPlanet[2].missionCompleted))
		interceptionChance = 0;

	int rtn = 0;

	if ((engine.useAudio) && (currentGame.useMusic))
		Mix_PlayMusic(engine.music, -1);

	textObject iconInfo[12];

	iconInfo[0].image = graphics.textSurface("Start Next Mission", FONT_WHITE);
	iconInfo[1].image = graphics.textSurface("View System Map", FONT_WHITE);
	iconInfo[2].image = graphics.textSurface("Current Status", FONT_WHITE);
	iconInfo[3].image = graphics.textSurface("Save Game", FONT_WHITE);
	iconInfo[4].image = graphics.textSurface("Upgrade FIREFLY", FONT_WHITE);
	iconInfo[5].image = graphics.textSurface("Comms", FONT_WHITE);
	iconInfo[6].image = graphics.textSurface("Options", FONT_WHITE);
	iconInfo[7].image = graphics.textSurface("Exit to Title Screen", FONT_WHITE);

	sprintf(string, "System : %s", systemNames[currentGame.system]);
	iconInfo[8].image = graphics.textSurface(string, FONT_WHITE);

	sprintf(string, "Stationed At: %s", systemPlanet[currentGame.stationedPlanet].name);
	iconInfo[9].image = graphics.textSurface(string, FONT_WHITE);

	strcpy(string, "Destination: None");
	if (currentGame.destinationPlanet > -1)
		sprintf(string, "Destination: %s", systemPlanet[currentGame.destinationPlanet].name);
	iconInfo[10].image = graphics.textSurface(string, FONT_WHITE);
	for (int i = 0 ; i < 9 ; i++)
		iconInfo[i].x = (800 - iconInfo[i].image->w) / 2;

	iconInfo[11].image = graphics.textSurface("Go to Destination Planet", FONT_WHITE);

	signed char redrawBackGround = 1;

	player.maxShield = (25 * currentGame.shieldUnits);

	if (currentGame.distanceCovered > 0)
		section = 0;
	else
		player.shield = player.maxShield;

	flushInput();
	engine.keyState[SDLK_LCTRL] = engine.keyState[SDLK_RCTRL] = engine.keyState[SDLK_SPACE] = 0;
	engine.done = 0;

	while (!engine.done)
	{
		graphics.updateScreen();

		if (redrawBackGround)
		{
			graphics.drawBackGround();
			redrawBackGround = 0;
		}
		else
		{
			graphics.unBuffer();
		}

  		doStarfield();

		r.x = 0;
		r.y = 0;
		r.h = 600;
		r.w = 1;
		for (int i = 40 ; i < 800 ; i+= 40)
		{
			r.x = i;
			SDL_FillRect(graphics.screen, &r, graphics.darkerBlue);
		}

		r.x = 0;
		r.y = 0;
		r.h = 1;
		r.w = 800;
		for (int i = 40 ; i < 600 ; i+= 40)
		{
			r.y = i;
			SDL_FillRect(graphics.screen, &r, graphics.darkerBlue);
		}


		if (rand() % 1000 < 2)
		{
			engine.ssx = Math::rrand(100, 100);
			engine.ssy = Math::rrand(100, 100);
			engine.ssx /= 100;
			engine.ssy /= 100;
		}

		graphics.blit(iconInfo[8].image, (int)iconInfo[8].x, 15);

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
					iconInfo[9].image = graphics.textSurface(systemPlanet[currentGame.stationedPlanet].name, FONT_WHITE);

					SDL_FreeSurface(iconInfo[10].image);
					iconInfo[10].image = graphics.textSurface(systemPlanet[currentGame.destinationPlanet].name, FONT_WHITE);

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
				if (engine.keyState[SDLK_SPACE])
				{
					movePlanets = !movePlanets;
					engine.keyState[SDLK_SPACE] = 0;
				}

				if (movePlanets)
				{
					sinX += 0.01;
					cosY += 0.01;
				}

				if (showSystem(sinX, cosY))
				{
					if (currentGame.system == 0)
					{
						sprintf(string, "Stationed At: %s", systemPlanet[currentGame.stationedPlanet].name);
						SDL_FreeSurface(iconInfo[9].image);
						iconInfo[9].image = graphics.textSurface(string, FONT_WHITE);
						updateCommsSurface(commsSurface);
					}
					else
					{
						sprintf(string, "Destination: %s", systemPlanet[currentGame.destinationPlanet].name);
						SDL_FreeSurface(iconInfo[10].image);
						iconInfo[10].image = graphics.textSurface(string, FONT_WHITE);
					}
				}

				graphics.blit(iconInfo[9].image, 90, 450);
				if ((currentGame.system > 0) && (currentGame.stationedPlanet != currentGame.destinationPlanet))
					graphics.blit(iconInfo[10].image, 550, 450);
				break;

			case 2:
				showStatus(statsSurface);
				break;

			case 3:
				graphics.blit(savesSurface, 200, 100);
				saveSlot = showSaveSlots(savesSurface, saveSlot);
				break;

         case 4:
         	showShop();
         	break;

			case 5:
				graphics.blit(commsSurface, 170, 70);
				doComms(commsSurface);
				break;

			case 6:
				graphics.blit(optionsSurface, 230, 130);
				showOptions(optionsSurface);
				break;

         case 7:
         	rtn = 0;
				engine.done = 1;
				break;

			case 8:
				showSystem(sinX, cosY);

				graphics.blit(systemPlanet[currentGame.stationedPlanet].image, 150, 450);
				graphics.blit(iconInfo[9].image, 135, 480);
				graphics.blit(systemPlanet[currentGame.destinationPlanet].image, 650, 450);
				graphics.blit(iconInfo[10].image, 635, 480);

				destRect.w += distance;
				SDL_FillRect(graphics.screen, &destRect, graphics.red);

				if (destRect.w >= 450)
				{
					currentGame.stationedPlanet = currentGame.destinationPlanet;
					currentGame.distanceCovered = 0;
					player.shield = player.maxShield;
					sprintf(string, "Stationed At: %s", systemPlanet[currentGame.stationedPlanet].name);
					strcpy(currentGame.stationedName, systemPlanet[currentGame.stationedPlanet].name);
					SDL_FreeSurface(iconInfo[9].image);
					iconInfo[9].image = graphics.textSurface(string, FONT_WHITE);
					updateCommsSurface(commsSurface);
					section = 1;
					redrawBackGround = 1;
				}

				if (interceptionChance > 0)
				{
					if ((rand() % interceptionChance) == 0)
					{
						currentGame.area = MAX_MISSIONS - 1;
						rtn = 2;
						engine.done = 1;
						currentGame.distanceCovered = destRect.w;
					}
				}

				break;
		}

		graphics.addBuffer(300, 545, 200, 15);

		if (section != 8)
		{
			for (int i = 0 ; i < 8 ; i++)
			{
				// if the mission has been completed, there is no "Start Next Mission" icon
				if (i == 0)
				{
					if ((currentGame.stationedPlanet == currentGame.destinationPlanet) && (systemPlanet[currentGame.stationedPlanet].missionCompleted != 0))
						continue;
					else if (currentGame.stationedPlanet == currentGame.destinationPlanet)
						graphics.blit(graphics.shape[1], 80 + (i * 90), 500);
					else if (currentGame.stationedPlanet != currentGame.destinationPlanet)
						graphics.blit(graphics.shape[26], 80 + (i * 90), 500);
				}
				else
				{
					graphics.blit(graphics.shape[i + 1], 80 + (i * 90), 500);
				}

				if (Collision::collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, 80 + (i * 90), 500, 32, 32))
				{
					if (i != 0)
					{
						graphics.blit(iconInfo[i].image, (int)iconInfo[i].x, 545);
					}
					else
					{
						if (currentGame.stationedPlanet == currentGame.destinationPlanet)
							graphics.blit(iconInfo[0].image, (int)iconInfo[i].x, 545);
						else
							graphics.blit(iconInfo[11].image, (int)iconInfo[i].x, 545);
					}

					if ((engine.keyState[SDLK_LCTRL]) || (engine.keyState[SDLK_RCTRL]))
					{
						redrawBackGround = 1;
						section = i;
						engine.keyState[SDLK_LCTRL] = engine.keyState[SDLK_RCTRL] = 0;
					}
				}
			}
		}

		engine.keyState[SDLK_LCTRL] = engine.keyState[SDLK_RCTRL] = engine.keyState[SDLK_SPACE] = 0;
		doCursor();

		graphics.delayFrame();
	}

	Mix_HaltMusic();
	SDL_FreeSurface(statsSurface);
	SDL_FreeSurface(savesSurface);
	SDL_FreeSurface(optionsSurface);
	SDL_FreeSurface(commsSurface);
	for (int i = 0 ; i < 12 ; i++)
		SDL_FreeSurface(iconInfo[i].image);

	player.maxShield = (25 * currentGame.shieldUnits);
	if (currentGame.distanceCovered == 0)
		player.shield = player.maxShield;

	return rtn;
}

