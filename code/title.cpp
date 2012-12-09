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

static signed char showGameMenu(signed char continueSaveSlot)
{
	blitText(2);
	if (continueSaveSlot != 0)
	{
		blitText(3);
		blitText(4);
	}
	blitText(5);
	if (engine.cheat)
	{
		textShape[7].y = 450;
		blitText(6);
	}
	else
	{
		textShape[7].y = 430;
	}
	blitText(7);

	if (engine.cheat)
		return 6;

	return 5;
}

static signed char showLoadMenu()
{
	signed char rtn = 1;

	for (int i = 13 ; i < 18 ; i++)
	{
		if (textShape[i].image != NULL)
		{
			blitText(i);
			rtn++;
			textShape[12].y = textShape[i].y + 40;
		}
	}
	blitText(12);

	return rtn;
}

static void createDifficultyMenu()
{
	textSurface(13, "START GAME", -1, 350, FONT_WHITE);

	if (currentGame.difficulty == DIFFICULTY_EASY)
		textSurface(14, "DIFFICULTY - EASY", -1, 370, FONT_WHITE);
	else if(currentGame.difficulty == DIFFICULTY_HARD)
		textSurface(14, "DIFFICULTY - HARD", -1, 370, FONT_WHITE);
	else
		textSurface(14, "DIFFICULTY - NORMAL", -1, 370, FONT_WHITE);

	if (currentGame.gamePlay == GAMEPLAY_ONPON)
		textSurface(15, "GAMEPLAY - ONPON", -1, 390, FONT_WHITE);
	else
		textSurface(15, "GAMEPLAY - NORMAL", -1, 390, FONT_WHITE);
}

static signed char showDifficultyMenu()
{
	textShape[12].y = 430;

	blitText(13);
	blitText(14);
	blitText(15);
	blitText(12);

	return 4;
}

static void createOptionsMenu()
{
	if (engine.useSound)
		textSurface(8, "SOUND - ON", -1, 350, FONT_WHITE);
	else
		textSurface(8, "SOUND - OFF", -1, 350, FONT_WHITE);

	if (engine.useMusic)
		textSurface(9, "MUSIC - ON", -1, 370, FONT_WHITE);
	else
		textSurface(9, "MUSIC - OFF", -1, 370, FONT_WHITE);

	if (engine.fullScreen)
		textSurface(10, "FULLSCREEN - ON", -1, 390, FONT_WHITE);
	else
		textSurface(10, "FULLSCREEN - OFF", -1, 390, FONT_WHITE);

	char string[50];
	strcpy(string, "AUTO SAVE SLOT - NONE");
	if (currentGame.autoSaveSlot > -1)
		sprintf(string, "AUTO SAVE SLOT - #%d", currentGame.autoSaveSlot + 1);
	textSurface(11, string, -1, 410, FONT_WHITE);
}

static signed char showOptionsMenu()
{
	textShape[12].y = 450;

	blitText(8);
	blitText(9);
	blitText(10);
	blitText(11);
	blitText(12);

	return 5;
}

static void createCheatMenu()
{
	if (engine.cheatShield)
		textSurface(18, "UNLIMITED SHIELD - ON", -1, 350, FONT_WHITE);
	else
		textSurface(18, "UNLIMITED SHIELD - OFF", -1, 350, FONT_WHITE);

	if (engine.cheatAmmo)
		textSurface(19, "UNLIMITED AMMO - ON", -1, 370, FONT_WHITE);
	else
		textSurface(19, "UNLIMITED AMMO - OFF", -1, 370, FONT_WHITE);

	if (engine.cheatCash)
		textSurface(20, "UNLIMITED CASH - ON", -1, 390, FONT_WHITE);
	else
		textSurface(20, "UNLIMITED CASH - OFF", -1, 390, FONT_WHITE);

	if (engine.cheatTime)
		textSurface(21, "UNLIMITED TIME - ON", -1, 410, FONT_WHITE);
	else
		textSurface(21, "UNLIMITED TIME - OFF", -1, 410, FONT_WHITE);
}

static signed char showCheatMenu()
{
	textShape[12].y = 450;

	blitText(18);
	blitText(19);
	blitText(20);
	blitText(21);
	blitText(12);

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

	flushBuffer();
	freeGraphics();
	resetLists();
	
	// required to stop the title screen crashing
	currentGame.system = 0;
	currentGame.area = 0;

	loadGameGraphics();

	clearScreen(black);
	updateScreen();
	clearScreen(black);

	signed char continueSaveSlot = initSaveSlots();

	loadMusic("music/Platinum.mod");

	loadBackground("gfx/spirit.jpg");

	SDL_Surface *prlogo, *sflogo;
	prlogo = loadImage("gfx/prlogo.png");
	sflogo = loadImage("gfx/sflogo.png");

	int prx = ((screen->w - prlogo->w) / 2);
	int pry = ((screen->h - prlogo->h) / 2);

	int sfx = ((screen->w - sflogo->w) / 2);
	int sfy = ((screen->h - sflogo->h) / 2);

	textSurface(0, "PRESENTS", -1, 300, FONT_WHITE);
	textSurface(1, "AN SDL GAME", -1, 300, FONT_WHITE);
	textSurface(2, "START NEW GAME", -1, 350, FONT_WHITE);
	textSurface(3, "LOAD GAME", -1, 370, FONT_WHITE);
	textSurface(4, "CONTINUE CURRENT GAME", -1, 390, FONT_WHITE);
	textSurface(5, "OPTIONS", -1, 410, FONT_WHITE);
	textSurface(6, "CHEAT OPTIONS", -1, 430, FONT_WHITE);
	textSurface(7, "QUIT", -1, 430, FONT_WHITE);

	createOptionsMenu();
	createDifficultyMenu();
	textSurface(12, "BACK TO MAIN MENU", -1, 0, FONT_WHITE);

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
		enemy[i].x = rand() % screen->w;
		enemy[i].y = rand() % (screen->h - 40);
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

	bool skip = false;
	signed char listLength = 5; // menu list length
	signed char menuType = 0;

	drawBackGround();

	engine.done = 0;
	flushInput();
	engine.keyState[SDLK_LCTRL] = engine.keyState[SDLK_RCTRL] = 0;

	if ((engine.useMusic) && (engine.useAudio))
		Mix_PlayMusic(engine.music, 1);

	while (!engine.done)
	{
		updateScreen();
		unBuffer();

		now = SDL_GetTicks();

		doStarfield();
		doExplosions();

		for (int i = 0 ; i < 15 ; i++)
		{
			addEngine(&enemy[i]);
			enemy[i].x += enemy[i].dx;
			blit(enemy[i].image[0], (int)enemy[i].x, (int)enemy[i].y);
			if (enemy[i].x > 830)
			{
				enemy[i].x = -40;
				enemy[i].y = rand() % (screen->h - 20);
				enemy[i].dx = 1 + rand() % 3;
			}
		}

		if ((now - then > 2000) && (now - then < 8000) && (!skip))
		{
			blit(prlogo, prx, pry);
		}
		else if ((now - then > 9000) && (now - then < 15000) && (!skip))
		{
   		blitText(0);
		}
		else if ((now - then > 16000) && (now - then < 21000) && (!skip))
		{
			blitText(1);
		}
		else if ((now - then > 25500) || (skip))
		{
			blit(sflogo, sfx, sfy);

			if ((now - then >= 27500) || (skip))
			{
				addBuffer(280, 345, 235, 145);

				blevelRect(optionRec.x, optionRec.y, optionRec.w, optionRec.h, redGlow, 0x00, 0x00);

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
					case 4:
						listLength = showDifficultyMenu();
						break;
				}

				redGlow += redDir;
				if (redGlow <= 0) {redDir = 2; redGlow = 0;}
				if (redGlow >= 255) {redDir = -2; redGlow = 255;}

				if (engine.keyState[SDLK_UP])
				{
					engine.keyState[SDLK_UP] = 0;
					wrapChar(&(--selectedOption), 1, listLength);
					if (menuType == 0)
						if ((selectedOption == 2) || (selectedOption == 3))
							if (continueSaveSlot == 0)
								selectedOption = 1;
				}
				if (engine.keyState[SDLK_DOWN])
				{
					engine.keyState[SDLK_DOWN] = 0;
					wrapChar(&(++selectedOption), 1, listLength);
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
					drawString("Copyright Parallel Realities 2003", 5, 560, FONT_WHITE, background);
					drawString("Copyright Guus Sliepen, Astrid S. de Wijn and others 2012", 5, 580, FONT_WHITE, background);
					drawString(buildVersion, 794 - strlen(buildVersion) * 9, 580, FONT_WHITE, background);
					addBuffer(0, 560, 800, 40);
					skip = true;
				}
			}
		}

		getPlayerInput();

		// if someone has invoked the credits cheat
		if (engine.cheatCredits)
		{
			doCredits();
			engine.cheatCredits = false;
		}

		if ((engine.keyState[SDLK_LCTRL]) || (engine.keyState[SDLK_RCTRL]) || (engine.keyState[SDLK_SPACE]))
		{
			if ((now - then <= 27500) && (!skip))
			{
				drawString("Copyright Parallel Realities 2003", 5, 560, FONT_WHITE, background);
				drawString("Copyright Guus Sliepen, Astrid S. de Wijn and others 2012", 5, 580, FONT_WHITE, background);
				drawString(buildVersion, 794 - strlen(buildVersion) * 9, 580, FONT_WHITE, background);
				addBuffer(0, 560, 800, 40);
				skip = true;
			}
			else
			{
				switch(menuType)
				{
					case 0: // Main menu
						if (selectedOption == 1)
							{menuType = 4; selectedOption = 1;}
						else if (selectedOption == 2)
							{menuType = 1; selectedOption = 1;}
						else if (selectedOption == 3)
							engine.done = 1;
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

					case 1: // Load game menu
						if (selectedOption != listLength)
							{engine.done = 1; continueSaveSlot = selectedOption; selectedOption = 3;}
						else
							{menuType = 0; selectedOption = 1;}
						break;

					case 2: // Options menu
						if ((selectedOption == 1) && (engine.useAudio))
							engine.useSound = !engine.useSound;
						else if ((selectedOption == 2) && (engine.useAudio))
						{
							engine.useMusic = !engine.useMusic;

							if (engine.useMusic)
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
							engine.fullScreen = !engine.fullScreen;
							#if LINUX
							SDL_WM_ToggleFullScreen(screen);
							#else
							if (engine.fullScreen)
								screen = SDL_SetVideoMode(screen->w, screen->h, 0, SDL_DOUBLEBUF|SDL_HWPALETTE|SDL_FULLSCREEN);
							else
								screen = SDL_SetVideoMode(screen->w, screen->h, 0, SDL_DOUBLEBUF|SDL_HWPALETTE);

							drawBackground();
							flushBuffer();
							#endif
						}
						else if (selectedOption == 4)
							wrapChar(&(++currentGame.autoSaveSlot), -1, 4);
						else if (selectedOption == listLength)
							{menuType = 0; selectedOption = 1;}
						createOptionsMenu();
						break;

					case 3: // Cheat menu
						if (selectedOption == 1)
							engine.cheatShield = !engine.cheatShield;
						else if (selectedOption == 2)
							engine.cheatAmmo = !engine.cheatAmmo;
						else if (selectedOption == 3)
							engine.cheatCash = !engine.cheatCash;
						else if (selectedOption == 4)
							engine.cheatTime = !engine.cheatTime;
						else if (selectedOption == listLength)
							{menuType = 0; selectedOption = 1;}
						createCheatMenu();
						break;

					case 4: // Difficulty menu
						if (selectedOption == 1)
							engine.done = 1;
						else if (selectedOption == 2)
							currentGame.difficulty++;
							if(currentGame.difficulty > DIFFICULTY_HARD)
								currentGame.difficulty = DIFFICULTY_EASY;
						else if (selectedOption == 3)
							currentGame.gamePlay++;
							if(currentGame.gamePlay > GAMEPLAY_ONPON)
								currentGame.gamePlay = GAMEPLAY_ORIGINAL;
						else if (selectedOption == listLength)
							{menuType = 0; selectedOption = 1;}
						createDifficultyMenu();
						break;

					default:
						menuType = 0, selectedOption = 1;
						break;
				}
			}
			engine.keyState[SDLK_LCTRL] = engine.keyState[SDLK_RCTRL] = engine.keyState[SDLK_SPACE] = 0;
		}

		delayFrame();
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

	fprintf(stderr, "Difficulty %d, gameplay %d\n", currentGame.difficulty, currentGame.gamePlay);

	return selectedOption;
}

/*
Scrolls the intro text up the screen and nothing else. The text will be placed
into a data file when the game is finished.
*/
void showStory()
{
	freeGraphics();

	int y = screen->h + 20;

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
		textSurface(i, string, -1, y, FONT_WHITE);

		i++;

		fscanf(fp, "%d ", &nextPos);
	}

	fclose(fp);

	loadBackground("gfx/startUp.jpg");
	blit(background, 0, 0);
	flushBuffer();

	flushInput();
	engine.keyState[SDLK_LCTRL] = engine.keyState[SDLK_RCTRL] = engine.keyState[SDLK_SPACE] = 0;

	while (true)
	{
		updateScreen();
		unBuffer();

		getPlayerInput();

		if ((engine.keyState[SDLK_LCTRL]) || (engine.keyState[SDLK_RCTRL]) || (engine.keyState[SDLK_SPACE]))
			break;

		if (textShape[8].y > (screen->h / 2) + 150)
		{
			for (int i = 0 ; i < 9 ; i++)
			{
				textShape[i].y -= 0.33333;
				blitText(i);
			}
		}
		else
		{
			SDL_Delay(3000);
			break;
		}

		delayFrame();
	}
}

/*
The game over screen :(
*/
void gameover()
{
	flushBuffer();
	freeGraphics();
	SDL_FillRect(background, NULL, black);

	engine.keyState[SDLK_LCTRL] = engine.keyState[SDLK_RCTRL] = engine.keyState[SDLK_SPACE] = 0;
	engine.gameSection = SECTION_INTERMISSION;

	loadMusic("music/Wybierak.mod");

	SDL_Surface *gameover = loadImage("gfx/gameover.png");

	clearScreen(black);
	updateScreen();
	clearScreen(black);
	SDL_Delay(1000);

	if ((engine.useMusic) && (engine.useAudio))
	{
		Mix_VolumeMusic(100);
		Mix_PlayMusic(engine.music, 1);
	}

	int x = (screen->w - gameover->w) / 2;
	int y = (screen->h - gameover->h) / 2;

	updateScreen();

	flushInput();
	engine.keyState[SDLK_LCTRL] = engine.keyState[SDLK_RCTRL] = engine.keyState[SDLK_SPACE] = 0;

	while (true)
	{
		getPlayerInput();

		if ((engine.keyState[SDLK_LCTRL]) || (engine.keyState[SDLK_RCTRL]) || (engine.keyState[SDLK_SPACE]))
			break;

		updateScreen();

		unBuffer();
		x = ((screen->w - gameover->w) / 2) - rrand(-2, 2);
		y = ((screen->h - gameover->h) / 2)  - rrand(-2, 2);
		blit(gameover, x,  y);

		delayFrame();
	}

	SDL_FreeSurface(gameover);

	if ((engine.useMusic) && (engine.useAudio))
		Mix_HaltMusic();

	flushBuffer();
}

void doCredits()
{
	loadBackground("gfx/credits.jpg");
	flushBuffer();
	freeGraphics();

	if ((engine.useMusic) && (engine.useAudio))
		loadMusic("music/Solace.s3m");

	FILE *fp;
	int numberOfCredits = 0;
	int lastCredit = 0;

	int yPos = 0;
	int yPos2 = screen->h;
	char text[255];

	textObject *credit;

	clearScreen(black);
	updateScreen();
	clearScreen(black);

	drawBackGround();

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
		drawString(text, -1, 240 + (i * 20), FONT_WHITE);
	}

	fscanf(fp, "%d%*c", &numberOfCredits);

	credit = (textObject*) malloc(sizeof(textObject) * numberOfCredits);

	for (int i = 0 ; i < numberOfCredits ; i++)
	{
		fscanf(fp, "%d %[^\n]%*c", &yPos, text);
		credit[i].image = textSurface(text, FONT_WHITE);
		credit[i].x = (screen->w - credit[i].image->w) / 2;
		yPos2 += yPos;
		credit[i].y = yPos2;
	}

	fclose(fp);

	if ((engine.useMusic) && (engine.useAudio))
	{
		Mix_VolumeMusic(100);
		Mix_PlayMusic(engine.music, 1);
	}

	updateScreen();
 	SDL_Delay(10000);
	drawBackGround();

	engine.done = 0;

	lastCredit = numberOfCredits - 1;

	engine.keyState[SDLK_ESCAPE] = 0;
	flushInput();

	while (true)
	{
		updateScreen();
		unBuffer();

		getPlayerInput();
		if (engine.keyState[SDLK_ESCAPE])
			break;

		float speed = 0.5;
		if(engine.keyState[SDLK_DOWN])
			speed = 2;
		else if(engine.keyState[SDLK_UP])
			speed = -2;

		for (int i = 0 ; i < numberOfCredits ; i++)
		{
			if ((credit[i].y > -10) && (credit[i].y < (screen->h + 10)))
				blit(credit[i].image, (int)credit[i].x, (int)credit[i].y);
			if (speed > 0 && credit[lastCredit].y > ((screen->h / 2) + 100))
				credit[i].y -= speed;
			else if(speed < 0 && credit[0].y < screen->h)
				credit[i].y -= speed;
		}

		delayFrame();
	}

	for (int i = 0 ; i < numberOfCredits ; i++)
	{
		SDL_FreeSurface(credit[i].image);
	}

	free(credit);
}

