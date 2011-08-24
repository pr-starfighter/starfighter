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

#include "version.h"
#include "title.h"

signed char showGameMenu(signed char continueSaveSlot)
{
	graphics.blitText(2);
	if (continueSaveSlot != 0)
	{
		graphics.blitText(3);
		graphics.blitText(4);
	}
	graphics.blitText(5);
	if (engine.cheat)
	{
		graphics.textShape[7].y = 450;
		graphics.blitText(6);
	}
	else
	{
		graphics.textShape[7].y = 430;
	}
	graphics.blitText(7);

	if (engine.cheat)
		return 6;

	return 5;
}

signed char showLoadMenu()
{
	signed char rtn = 1;

	for (int i = 13 ; i < 18 ; i++)
	{
		if (graphics.textShape[i].image != NULL)
		{
			graphics.blitText(i);
			rtn++;
			graphics.textShape[12].y = graphics.textShape[i].y + 40;
		}
	}
	graphics.blitText(12);

	return rtn;
}

void createOptionsMenu()
{
	if (currentGame.useSound)
		graphics.textSurface(8, "SOUND - ON", -1, 350, FONT_WHITE);
	else
		graphics.textSurface(8, "SOUND - OFF", -1, 350, FONT_WHITE);

	if (currentGame.useMusic)
		graphics.textSurface(9, "MUSIC - ON", -1, 370, FONT_WHITE);
	else
		graphics.textSurface(9, "MUSIC - OFF", -1, 370, FONT_WHITE);

	if (currentGame.fullScreen)
		graphics.textSurface(10, "FULLSCREEN - ON", -1, 390, FONT_WHITE);
	else
		graphics.textSurface(10, "FULLSCREEN - OFF", -1, 390, FONT_WHITE);

	char string[50];
	strcpy(string, "AUTO SAVE SLOT - NONE");
	if (currentGame.autoSaveSlot > -1)
		sprintf(string, "AUTO SAVE SLOT - #%d", currentGame.autoSaveSlot + 1);
	graphics.textSurface(11, string, -1, 410, FONT_WHITE);
}

signed char showOptionsMenu()
{
	graphics.textShape[12].y = 450;

	graphics.blitText(8);
	graphics.blitText(9);
	graphics.blitText(10);
	graphics.blitText(11);
	graphics.blitText(12);

	return 5;
}

void createCheatMenu()
{
	if (engine.cheatShield)
		graphics.textSurface(18, "UNLIMITED SHIELD - ON", -1, 350, FONT_WHITE);
	else
		graphics.textSurface(18, "UNLIMITED SHIELD - OFF", -1, 350, FONT_WHITE);

	if (engine.cheatAmmo)
		graphics.textSurface(19, "UNLIMITED AMMO - ON", -1, 370, FONT_WHITE);
	else
		graphics.textSurface(19, "UNLIMITED AMMO - OFF", -1, 370, FONT_WHITE);

	if (engine.cheatCash)
		graphics.textSurface(20, "UNLIMITED CASH - ON", -1, 390, FONT_WHITE);
	else
		graphics.textSurface(20, "UNLIMITED CASH - OFF", -1, 390, FONT_WHITE);

	if (engine.cheatTime)
		graphics.textSurface(21, "UNLIMITED TIME - ON", -1, 410, FONT_WHITE);
	else
		graphics.textSurface(21, "UNLIMITED TIME - OFF", -1, 410, FONT_WHITE);
}

signed char showCheatMenu()
{
	graphics.textShape[12].y = 450;

	graphics.blitText(18);
	graphics.blitText(19);
	graphics.blitText(20);
	graphics.blitText(21);
	graphics.blitText(12);

	return 5;
}

/*
This is the main title screen, with the stars whirling past and the
"Parallel Realities, Present..." text. Nothing too special.
*/
int doTitle()
{
	newGame();

	engine.gameSection = SECTION_TITLE;

	graphics.flushBuffer();
	graphics.freeGraphics();
	resetLists();
	
	// required to stop the title screen crashing
	currentGame.system = 0;
	currentGame.area = 0;

	loadGameGraphics();

	graphics.clearScreen(graphics.black);
	graphics.updateScreen();
	graphics.clearScreen(graphics.black);
	SDL_Delay(1000);

	signed char continueSaveSlot = initSaveSlots();

	loadMusic("music/Platinum.mod");

	loadBackground("gfx/spirit.jpg");

	SDL_Surface *prlogo, *sflogo;
	prlogo = loadImage("gfx/prlogo.gif");
	sflogo = loadImage("gfx/sflogo.gif");

	int prx = ((800 - prlogo->w) / 2);
	int pry = ((600 - prlogo->h) / 2);

	int sfx = ((800 - sflogo->w) / 2);
	int sfy = ((600 - sflogo->h) / 2);

	graphics.textSurface(0, "PRESENTS", -1, 300, FONT_WHITE);
	graphics.textSurface(1, "AN SDL GAME", -1, 300, FONT_WHITE);
	graphics.textSurface(2, "START NEW GAME", -1, 350, FONT_WHITE);
	graphics.textSurface(3, "LOAD GAME", -1, 370, FONT_WHITE);
	graphics.textSurface(4, "CONTINUE CURRENT GAME", -1, 390, FONT_WHITE);
	graphics.textSurface(5, "OPTIONS", -1, 410, FONT_WHITE);
	graphics.textSurface(6, "CHEAT OPTIONS", -1, 430, FONT_WHITE);
	graphics.textSurface(7, "QUIT", -1, 430, FONT_WHITE);

	createOptionsMenu();
	graphics.textSurface(12, "BACK TO MAIN MENU", -1, 0, FONT_WHITE);

	createCheatMenu();

	// Set the star motion
	engine.ssx = -0.5;
	engine.ssy = 0;

	int then = SDL_GetTicks();
	int now;

	for (int i = 0 ; i < 15 ; i++)
	{
		enemy[i] = defEnemy[rand() % 3];
		if ((rand() % 5) == 0)
			enemy[i] = defEnemy[CD_TRANSPORTSHIP];
		if ((rand() % 5) == 0)
			enemy[i] = defEnemy[CD_MINER];
		enemy[i].x = rand() % 800;
		enemy[i].y = rand() % 560;
		enemy[i].dx = 1 + rand() % 3;
		enemy[i].face = 0;
	}

	int redGlow = 255;
	signed char redDir = -2;
	char buildVersion[25];
	sprintf(buildVersion, "Version "VERSION);

	SDL_Rect optionRec;

	optionRec.x = 290;
	optionRec.y = 345;
	optionRec.h = 22;
	optionRec.w = 215;

	signed char selectedOption = 1;
	if (continueSaveSlot > 0)
		{selectedOption = 3; optionRec.y += 40;}

	signed char skip = 0;
	signed char listLength = 5; // menu list length
	signed char menuType = 0;

	graphics.drawBackGround();
	unsigned long frameLimit = SDL_GetTicks();

	engine.done = 0;
	flushInput();
	engine.keyState[SDLK_LCTRL] = engine.keyState[SDLK_RCTRL] = 0;

	if ((currentGame.useMusic) && (engine.useAudio))
		Mix_PlayMusic(engine.music, 1);

	while (!engine.done)
	{
		graphics.updateScreen();
		graphics.unBuffer();

		now = SDL_GetTicks();

		doStarfield();
		doExplosions();

		for (int i = 0 ; i < 15 ; i++)
		{
			addEngine(&enemy[i]);
			enemy[i].x += enemy[i].dx;
			graphics.blit(enemy[i].image[0], (int)enemy[i].x, (int)enemy[i].y);
			if (enemy[i].x > 830)
			{
				enemy[i].x = -10;
				enemy[i].y = rand() % 580;
				enemy[i].dx = 1 + rand() % 3;
			}
		}

		if ((now - then > 2000) && (now - then < 8000) && (!skip))
		{
			graphics.blit(prlogo, prx, pry);
		}
		else if ((now - then > 9000) && (now - then < 15000) && (!skip))
		{
   		graphics.blitText(0);
		}
		else if ((now - then > 16000) && (now - then < 21000) && (!skip))
		{
			graphics.blitText(1);
		}
		else if ((now - then > 25500) || (skip))
		{
			graphics.blit(sflogo, sfx, sfy);

			if ((now - then >= 27500) || (skip))
			{
				graphics.addBuffer(280, 345, 235, 145);

				graphics.blevelRect(optionRec.x, optionRec.y, optionRec.w, optionRec.h, redGlow, 0x00, 0x00);

				switch(menuType)
				{
					case 0:
						listLength = showGameMenu(continueSaveSlot);
						break;
					case 1:
						listLength = showLoadMenu();
						break;
					case 2:
						listLength = showOptionsMenu();
						break;
					case 3:
						listLength = showCheatMenu();
						break;
				}

				redGlow += redDir;
				if (redGlow <= 0) {redDir = 2; redGlow = 0;}
				if (redGlow >= 255) {redDir = -2; redGlow = 255;}

				if (engine.keyState[SDLK_UP])
				{
					engine.keyState[SDLK_UP] = 0;
					Math::wrapChar(&(--selectedOption), 1, listLength);
					if (menuType == 0)
						if ((selectedOption == 2) || (selectedOption == 3))
							if (continueSaveSlot == 0)
								selectedOption = 1;
				}
				if (engine.keyState[SDLK_DOWN])
				{
					engine.keyState[SDLK_DOWN] = 0;
					Math::wrapChar(&(++selectedOption), 1, listLength);
					if (menuType == 0)
						if ((selectedOption == 2) || (selectedOption == 3))
							if (continueSaveSlot == 0)
								selectedOption = 4;
				}

				optionRec.y = 326 + (20 * selectedOption);
				if (menuType > 0)
					if (selectedOption == listLength)
						optionRec.y += 20;

				if (!skip)
				{
					graphics.drawString("Copyright Parallel Realities 2003", 5, 580, FONT_WHITE, graphics.background);
					graphics.drawString(buildVersion, 695, 580, FONT_WHITE, graphics.background);
					graphics.addBuffer(0, 580, 800, 20);
					skip = 1;
				}
			}
		}

		getPlayerInput();

		// if someone has invoked the credits cheat
		if (engine.cheatCredits)
		{
			doCredits();
			engine.cheatCredits = 0;
		}

		if ((engine.keyState[SDLK_LCTRL]) || (engine.keyState[SDLK_RCTRL]) || (engine.keyState[SDLK_SPACE]))
		{
			if ((now - then <= 27500) && (!skip))
			{
				graphics.drawString("Copyright Parallel Realities 2003", 5, 580, FONT_WHITE, graphics.background);
				graphics.drawString(buildVersion, 695, 580, FONT_WHITE, graphics.background);
				graphics.addBuffer(0, 580, 800, 20);
				skip = 1;
			}
			else
			{
				switch(menuType)
				{
					case 0:
						if ((selectedOption == 1) || (selectedOption == 3))
							engine.done = 1;
						else if (selectedOption == 2)
							{menuType = 1; selectedOption = 1;}
						else if (selectedOption == 4)
							{menuType = 2; selectedOption = 1;}
						else if (selectedOption == 5)
						{
							if (engine.cheat)
								{menuType = 3; selectedOption = 1;}
							else
								engine.done = 1;
						}
						else if (selectedOption == 6)
							engine.done = 1;
						break;

					case 1:
						if (selectedOption != listLength)
							{engine.done = 1; continueSaveSlot = selectedOption; selectedOption = 3;}
						else
							{menuType = 0; selectedOption = 1;}
						break;

					case 2:
						if ((selectedOption == 1) && (engine.useAudio))
							currentGame.useSound = 1 - currentGame.useSound;
						else if ((selectedOption == 2) && (engine.useAudio))
						{
							currentGame.useMusic = 1 - currentGame.useMusic;

							if (currentGame.useMusic)
							{
								if (Mix_PausedMusic() == 1)
									Mix_ResumeMusic();
								else
									Mix_PlayMusic(engine.music, 1);
							}
							else
							{
								Mix_PauseMusic();
							}
						}
						else if (selectedOption == 3)
						{
							currentGame.fullScreen = 1 - currentGame.fullScreen;
							#if LINUX
							SDL_WM_ToggleFullScreen(graphics.screen);
							#else
							if (currentGame.fullScreen)
								graphics.screen = SDL_SetVideoMode(800, 600, 16, SDL_HWSURFACE|SDL_HWPALETTE|SDL_FULLSCREEN);
							else
								graphics.screen = SDL_SetVideoMode(800, 600, 0, SDL_HWSURFACE|SDL_HWPALETTE);

							graphics.drawBackground();
							flushBuffer();
							#endif
						}
						else if (selectedOption == 4)
							Math::wrapChar(&(++currentGame.autoSaveSlot), -1, 4);
						else if (selectedOption == listLength)
							{menuType = 0; selectedOption = 1;}
						createOptionsMenu();
						break;

					case 3:
						if (selectedOption == 1)
							engine.cheatShield = 1 - engine.cheatShield;
						else if (selectedOption == 2)
							engine.cheatAmmo = 1 - engine.cheatAmmo;
						else if (selectedOption == 3)
							engine.cheatCash = 1 - engine.cheatCash;
						else if (selectedOption == 4)
							engine.cheatTime = 1 - engine.cheatTime;
						else if (selectedOption == listLength)
							{menuType = 0; selectedOption = 1;}
						createCheatMenu();
						break;

					case 4:
						if (selectedOption == listLength)
							{menuType = 0; selectedOption = 1;}
						break;
				}
			}
			engine.keyState[SDLK_LCTRL] = engine.keyState[SDLK_RCTRL] = engine.keyState[SDLK_SPACE] = 0;
		}

		while (SDL_GetTicks() < (frameLimit + 16)){}
		frameLimit = SDL_GetTicks();
	}

	Mix_HaltMusic();

	SDL_FreeSurface(prlogo);
	SDL_FreeSurface(sflogo);

	engine.keyState[SDLK_LCTRL] = engine.keyState[SDLK_RCTRL] = engine.keyState[SDLK_SPACE] = 0;

	resetLists();

	if (selectedOption == 1)
		selectedOption = 2; // go straight to mission 0

	if (selectedOption == 3)
	{
		newGame();
		selectedOption = loadGame(continueSaveSlot);
	}

	// Send back a negative number...
	if (selectedOption > 4)
	{
		selectedOption = -1;
		exit(0);
	}

	return selectedOption;
}

/*
Scrolls the intro text up the screen and nothing else. The text will be placed
into a data file when the game is finished.
*/
void showStory()
{
	graphics.freeGraphics();

	int y = 620;

	FILE *fp;

	#if USEPACK
	int dataLocation = locateDataInPak("data/intro.txt", 1);
	fp = fopen(PACKLOCATION, "rb");
	fseek(fp, dataLocation, SEEK_SET);
	#else
	fp = fopen("data/intro.txt", "rb");
	#endif

	int i = 0;
	int nextPos = -1;
	char string[255];

	fscanf(fp, "%d ", &nextPos);

	while (nextPos != -1)
	{
		fscanf(fp, "%[^\n]%*c", string);

		y += nextPos;
		graphics.textSurface(i, string, -1, y, FONT_WHITE);

		i++;

		fscanf(fp, "%d ", &nextPos);
	}

	fclose(fp);

	loadBackground("gfx/startUp.jpg");
	graphics.blit(graphics.background, 0, 0);
	graphics.flushBuffer();

	unsigned long frameLimit = SDL_GetTicks();

	flushInput();
	engine.keyState[SDLK_LCTRL] = engine.keyState[SDLK_RCTRL] = engine.keyState[SDLK_SPACE] = 0;

	while (true)
	{
		graphics.updateScreen();
		graphics.unBuffer();

		getPlayerInput();

		if ((engine.keyState[SDLK_LCTRL]) || (engine.keyState[SDLK_RCTRL]) || (engine.keyState[SDLK_SPACE]))
			break;

		if (graphics.textShape[8].y > 450)
		{
			for (int i = 0 ; i < 9 ; i++)
			{
				graphics.textShape[i].y -= 0.25;
				graphics.blitText(i);
			}
		}
		else
		{
			SDL_Delay(3000);
			break;
		}

		while (SDL_GetTicks() < (frameLimit + 16)){}
		frameLimit = SDL_GetTicks();
	}
}

/*
The game over screen :(
*/
void gameover()
{
	graphics.flushBuffer();
	graphics.freeGraphics();
	SDL_FillRect(graphics.background, NULL, graphics.black);

	engine.keyState[SDLK_LCTRL] = engine.keyState[SDLK_RCTRL] = engine.keyState[SDLK_SPACE] = 0;
	engine.gameSection = SECTION_INTERMISSION;

	loadMusic("music/Wybierak.mod");

	SDL_Surface *gameover = loadImage("gfx/gameover.png");

	graphics.clearScreen(graphics.black);
	graphics.updateScreen();
	graphics.clearScreen(graphics.black);
	SDL_Delay(1000);

	if ((currentGame.useMusic) && (engine.useAudio))
	{
		Mix_VolumeMusic(100);
		Mix_PlayMusic(engine.music, 1);
	}

	int x = (800 - gameover->w) / 2;
	int y = (600 - gameover->h) / 2;

	unsigned long frameLimit = SDL_GetTicks();
	graphics.updateScreen();

	flushInput();
	engine.keyState[SDLK_LCTRL] = engine.keyState[SDLK_RCTRL] = engine.keyState[SDLK_SPACE] = 0;

	while (true)
	{
		getPlayerInput();

		if ((engine.keyState[SDLK_LCTRL]) || (engine.keyState[SDLK_RCTRL]) || (engine.keyState[SDLK_SPACE]))
			break;

		graphics.updateScreen();

		graphics.unBuffer();
		x = ((800 - gameover->w) / 2) - Math::rrand(-2, 2);
		y = ((600 - gameover->h) / 2)  - Math::rrand(-2, 2);
		graphics.blit(gameover, x,  y);

		// Limit us to 60 frame a second
		while (SDL_GetTicks() < (frameLimit + 16)){}
		frameLimit = SDL_GetTicks();
	}

	SDL_FreeSurface(gameover);

	if ((currentGame.useMusic) && (engine.useAudio))
		Mix_HaltMusic();

	graphics.flushBuffer();
}

void doCredits()
{
	loadBackground("gfx/credits.jpg");
	graphics.flushBuffer();
	graphics.freeGraphics();

	if ((currentGame.useMusic) && (engine.useAudio))
		loadMusic("music/Solace.s3m");

	FILE *fp;
	int numberOfCredits = 0;
	int lastCredit = 0;

	int yPos = 0;
	int yPos2 = 510;
	char text[255];

	textObject *credit;

	graphics.clearScreen(graphics.black);
	graphics.updateScreen();
	graphics.clearScreen(graphics.black);
	SDL_Delay(1000);

	graphics.drawBackGround();

	#if USEPACK
	int dataLocation = locateDataInPak("data/credits.txt", 1);
	fp = fopen(PACKLOCATION, "rb");
	fseek(fp, dataLocation, SEEK_SET);
	#else
	fp = fopen("data/credits.txt", "rb");
	#endif

	for (int i = 0 ; i < 6 ; i++)
	{
		fscanf(fp, "%[^\n]%*c", text);
		graphics.drawString(text, -1, 240 + (i * 20), FONT_WHITE);
	}

	fscanf(fp, "%d%*c", &numberOfCredits);

	credit = (textObject*) malloc(sizeof(textObject) * numberOfCredits);

	for (int i = 0 ; i < numberOfCredits ; i++)
	{
		fscanf(fp, "%d %[^\n]%*c", &yPos, text);
		credit[i].image = graphics.textSurface(text, FONT_WHITE);
		credit[i].x = (800 - credit[i].image->w) / 2;
		yPos2 += yPos;
		credit[i].y = yPos2;
	}

	fclose(fp);

	if ((currentGame.useMusic) && (engine.useAudio))
	{
		Mix_VolumeMusic(100);
		Mix_PlayMusic(engine.music, 1);
	}

	SDL_Delay(3000);

	graphics.updateScreen();
 	SDL_Delay(10000);
	graphics.drawBackGround();

	unsigned long frameLimit = SDL_GetTicks();
	engine.done = 0;

	lastCredit = numberOfCredits - 1;

	SDL_Rect r1 = {0, 80, 800, 20};
	SDL_Rect r2 = {0, 500, 800, 20};

	engine.keyState[SDLK_ESCAPE] = 0;
	flushInput();

	while (true)
	{
		graphics.updateScreen();
		graphics.unBuffer();

		getPlayerInput();
		if (engine.keyState[SDLK_ESCAPE])
			break;

		for (int i = 0 ; i < numberOfCredits ; i++)
		{
			if ((credit[i].y > 80) && (credit[i].y < 500))
				graphics.blit(credit[i].image, (int)credit[i].x, (int)credit[i].y);
			if (credit[lastCredit].y > 400)
				credit[i].y -= 0.3;
		}

		SDL_FillRect(graphics.screen, &r1, graphics.black);
		SDL_FillRect(graphics.screen, &r2, graphics.black);

		while (SDL_GetTicks() < (frameLimit + 16)){}
		frameLimit = SDL_GetTicks();
	}

	for (int i = 0 ; i < numberOfCredits ; i++)
	{
		SDL_FreeSurface(credit[i].image);
	}

	free(credit);
}

