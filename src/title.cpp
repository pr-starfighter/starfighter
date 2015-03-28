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

static signed char showGameMenu(signed char continueSaveSlot)
{
	blitText(TS_START_NEW_GAME);
	if (continueSaveSlot != -1)
	{
		blitText(TS_LOAD_GAME);
		blitText(TS_CONTINUE_CURRENT_GAME);
	}
	blitText(TS_OPTIONS);
	if (engine.cheat)
	{
		textShape[TS_QUIT].y = 450;
		blitText(TS_CHEAT_OPTIONS);
	}
	else
	{
		textShape[TS_QUIT].y = 430;
	}
	blitText(TS_QUIT);

	if (engine.cheat)
		return 6;

	return 5;
}

static signed char showLoadMenu()
{
	signed char rtn = 1;

	for (int i = TS_SAVESLOT_0 ; i <= TS_SAVESLOT_5 ; i++)
	{
		rtn++;
		if (textShape[i].image != NULL)
		{
			blitText(i);
			textShape[TS_BACK_TO_MAIN_MENU].y = textShape[i].y + 40;
		}
	}
	blitText(TS_BACK_TO_MAIN_MENU);

	return rtn;
}

static void createDifficultyMenu()
{
	textSurface(TS_START_GAME, "START GAME", -1, 350, FONT_WHITE);

	if (currentGame.difficulty == DIFFICULTY_EASY)
		textSurface(TS_DIFFICULTY, "DIFFICULTY - EASY", -1, 370, FONT_WHITE);
	else if (currentGame.difficulty == DIFFICULTY_HARD)
		textSurface(TS_DIFFICULTY, "DIFFICULTY - HARD", -1, 370, FONT_WHITE);
	else if (currentGame.difficulty == DIFFICULTY_NIGHTMARE)
		textSurface(TS_DIFFICULTY, "DIFFICULTY - NIGHTMARE!", -1, 370, FONT_WHITE);
	else if (currentGame.difficulty == DIFFICULTY_ORIGINAL)
		textSurface(TS_DIFFICULTY, "DIFFICULTY - ORIGINAL", -1, 370, FONT_WHITE);
	else
		textSurface(TS_DIFFICULTY, "DIFFICULTY - NORMAL", -1, 370, FONT_WHITE);
}

static signed char showDifficultyMenu()
{
	textShape[TS_BACK_TO_MAIN_MENU].y = 410;

	blitText(TS_START_GAME);
	blitText(TS_DIFFICULTY);
	blitText(TS_BACK_TO_MAIN_MENU);

	return 3;
}

static void createOptionsMenu()
{
	if (engine.useSound)
		textSurface(TS_SOUND, "SOUND - ON", -1, 350, FONT_WHITE);
	else
		textSurface(TS_SOUND, "SOUND - OFF", -1, 350, FONT_WHITE);

	if (engine.useMusic)
		textSurface(TS_MUSIC, "MUSIC - ON", -1, 370, FONT_WHITE);
	else
		textSurface(TS_MUSIC, "MUSIC - OFF", -1, 370, FONT_WHITE);

	if (engine.fullScreen)
		textSurface(TS_FULLSCREEN, "FULLSCREEN - ON", -1, 390, FONT_WHITE);
	else
		textSurface(TS_FULLSCREEN, "FULLSCREEN - OFF", -1, 390, FONT_WHITE);
}

static signed char showOptionsMenu()
{
	textShape[TS_BACK_TO_MAIN_MENU].y = 430;

	blitText(TS_SOUND);
	blitText(TS_MUSIC);
	blitText(TS_FULLSCREEN);
	blitText(TS_BACK_TO_MAIN_MENU);

	return 4;
}

static void createCheatMenu()
{
	if (engine.cheatShield)
		textSurface(TS_UNLIMITED_SHIELD, "UNLIMITED SHIELD - ON", -1, 350,
			FONT_WHITE);
	else
		textSurface(TS_UNLIMITED_SHIELD, "UNLIMITED SHIELD - OFF", -1, 350,
			FONT_WHITE);

	if (engine.cheatAmmo)
		textSurface(TS_UNLIMITED_AMMO, "UNLIMITED AMMO - ON", -1, 370,
			FONT_WHITE);
	else
		textSurface(TS_UNLIMITED_AMMO, "UNLIMITED AMMO - OFF", -1, 370,
			FONT_WHITE);

	if (engine.cheatCash)
		textSurface(TS_UNLIMITED_CASH, "UNLIMITED CASH - ON", -1, 390,
			FONT_WHITE);
	else
		textSurface(TS_UNLIMITED_CASH, "UNLIMITED CASH - OFF", -1, 390,
			FONT_WHITE);

	if (engine.cheatTime)
		textSurface(TS_UNLIMITED_TIME, "UNLIMITED TIME - ON", -1, 410,
			FONT_WHITE);
	else
		textSurface(TS_UNLIMITED_TIME, "UNLIMITED TIME - OFF", -1, 410,
			FONT_WHITE);
}

static signed char showCheatMenu()
{
	textShape[TS_BACK_TO_MAIN_MENU].y = 450;

	blitText(TS_UNLIMITED_SHIELD);
	blitText(TS_UNLIMITED_AMMO);
	blitText(TS_UNLIMITED_CASH);
	blitText(TS_UNLIMITED_TIME);
	blitText(TS_BACK_TO_MAIN_MENU);

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

	loadBackground("gfx/spirit.jpg");

	SDL_Surface *prlogo, *sflogo;
	prlogo = loadImage("gfx/prlogo.png");
	sflogo = loadImage("gfx/sflogo.png");

	int prx = ((screen->w - prlogo->w) / 2);
	int pry = ((screen->h - prlogo->h) / 2);

	int sfx = ((screen->w - sflogo->w) / 2);
	int sfy = ((screen->h - sflogo->h) / 2);

	textSurface(TS_PRESENTS, "PRESENTS", -1, 300, FONT_WHITE);
	textSurface(TS_AN_SDL_GAME, "AN SDL GAME", -1, 300, FONT_WHITE);
	textSurface(TS_START_NEW_GAME, "START NEW GAME", -1, 350, FONT_WHITE);
	textSurface(TS_LOAD_GAME, "LOAD GAME", -1, 370, FONT_WHITE);
	textSurface(TS_CONTINUE_CURRENT_GAME, "CONTINUE CURRENT GAME", -1, 390,
		FONT_WHITE);
	textSurface(TS_OPTIONS, "OPTIONS", -1, 410, FONT_WHITE);
	textSurface(TS_CHEAT_OPTIONS, "CHEAT OPTIONS", -1, 430, FONT_WHITE);
	textSurface(TS_QUIT, "QUIT", -1, 430, FONT_WHITE);

	createOptionsMenu();
	createDifficultyMenu();
	textSurface(TS_BACK_TO_MAIN_MENU, "BACK TO MAIN MENU", -1, 0, FONT_WHITE);

	createCheatMenu();

	// Set the star motion
	engine.ssx = -0.5;
	engine.ssy = 0;
	engine.smx = 0;
	engine.smy = 0;

	int then = SDL_GetTicks();
	int now;

	for (int i = 0 ; i < 15 ; i++)
	{
		aliens[i] = alien_defs[rand() % 3];
		if ((rand() % 5) == 0)
			aliens[i] = alien_defs[CD_TRANSPORTSHIP];
		if ((rand() % 5) == 0)
			aliens[i] = alien_defs[CD_MINER];
		aliens[i].x = rand() % screen->w;
		aliens[i].y = rand() % (screen->h - 40);
		aliens[i].dx = 1 + rand() % 3;
		aliens[i].face = 0;
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
	if (continueSaveSlot > -1)
		{selectedOption = 3; optionRec.y += 40;}

	bool skip = false;
	signed char listLength = 5; // menu list length
	signed char menuType = MENU_MAIN;

	drawBackGround();

	engine.done = 0;
	flushInput();
	engine.keyState[KEY_FIRE] = engine.keyState[KEY_ALTFIRE] = 0;

	audio_playMusic("music/walking_among_androids.ogg", 1);

	while (!engine.done)
	{
		updateScreen();
		unBuffer();

		now = SDL_GetTicks();

		doStarfield();
		doExplosions();

		for (int i = 0 ; i < 15 ; i++)
		{
			addEngine(&aliens[i]);
			aliens[i].x += aliens[i].dx;
			blit(aliens[i].image[0], (int)aliens[i].x, (int)aliens[i].y);
			if (aliens[i].x > 830)
			{
				aliens[i].x = -40;
				aliens[i].y = rand() % (screen->h - 20);
				aliens[i].dx = 1 + rand() % 3;
			}
		}

		if ((now - then > 2000) && (now - then < 8000) && (!skip))
		{
			blit(prlogo, prx, pry);
		}
		else if ((now - then > 9000) && (now - then < 15000) && (!skip))
		{
			blitText(TS_PRESENTS);
		}
		else if ((now - then > 16000) && (now - then < 21000) && (!skip))
		{
			blitText(TS_AN_SDL_GAME);
		}
		else if ((now - then > 25500) || (skip))
		{
			blit(sflogo, sfx, sfy);

			if ((now - then >= 27500) || (skip))
			{
				addBuffer(0, 0, screen->w, screen->h);

				blevelRect(optionRec.x, optionRec.y, optionRec.w, optionRec.h, redGlow, 0x00, 0x00);

				switch(menuType)
				{
					case MENU_MAIN:
						listLength = showGameMenu(continueSaveSlot);
						break;
					case MENU_DIFFICULTY:
						listLength = showDifficultyMenu();
						break;
					case MENU_LOAD:
						listLength = showLoadMenu();
						break;
					case MENU_OPTIONS:
						listLength = showOptionsMenu();
						break;
					case MENU_CHEAT:
						listLength = showCheatMenu();
						break;
				}

				redGlow += redDir;
				if (redGlow <= 0) {redDir = 2; redGlow = 0;}
				if (redGlow >= 255) {redDir = -2; redGlow = 255;}

				if (engine.keyState[KEY_UP])
				{
					engine.keyState[KEY_UP] = 0;
					WRAP_ADD(selectedOption, -1, 1, listLength + 1);
					if (menuType == MENU_MAIN)
						if ((selectedOption == 2) || (selectedOption == 3))
							if (continueSaveSlot == -1)
								selectedOption = 1;
				}
				if (engine.keyState[KEY_DOWN])
				{
					engine.keyState[KEY_DOWN] = 0;
					WRAP_ADD(selectedOption, 1, 0, listLength);
					if (menuType == MENU_MAIN)
						if ((selectedOption == 2) || (selectedOption == 3))
							if (continueSaveSlot == -1)
								selectedOption = 4;
				}

				optionRec.y = 326 + (20 * selectedOption);
				if (menuType > MENU_MAIN)
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

		if ((engine.keyState[KEY_FIRE] || engine.keyState[KEY_ALTFIRE]))
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
				switch (menuType)
				{
					case MENU_MAIN:
						if (selectedOption == 1)
						{
							menuType = MENU_DIFFICULTY;
							selectedOption = 1;
						}
						else if (selectedOption == 2)
						{
							menuType = MENU_LOAD;
							selectedOption = 1;
						}
						else if (selectedOption == 3)
							engine.done = 1;
						else if (selectedOption == 4)
						{
							menuType = MENU_OPTIONS;
							selectedOption = 1;
						}
						else if (selectedOption == 5)
						{
							if (engine.cheat)
							{
								menuType = MENU_CHEAT;
								selectedOption = 1;
							}
							else
								engine.done = 1;
						}
						else if (selectedOption == 6)
							engine.done = 1;
						break;

					case MENU_DIFFICULTY:
						if (selectedOption == 1)
							engine.done = 1;
						else if (selectedOption == 2)
						{
							currentGame.difficulty++;
							currentGame.difficulty %= DIFFICULTY_MAX;
						}
						else if (selectedOption == listLength)
						{
							menuType = MENU_MAIN;
							selectedOption = 1;
						}
						createDifficultyMenu();
						break;

					case MENU_LOAD:
						if (selectedOption != listLength)
						{
							engine.done = 1;
							continueSaveSlot = selectedOption - 1;
							selectedOption = 3;
						}
						else
						{
							menuType = MENU_MAIN;
							selectedOption = 1;
						}
						break;

					case MENU_OPTIONS:
						if ((selectedOption == 1) && (engine.useAudio))
							engine.useSound = !engine.useSound;
						else if ((selectedOption == 2) && (engine.useAudio))
						{
							engine.useMusic = !engine.useMusic;

							if (engine.useMusic)
							{
								audio_playMusic(
									"music/walking_among_androids.ogg", 1);
							}
							else
							{
								audio_haltMusic();
							}
						}
						else if (selectedOption == 3)
						{
							engine.fullScreen = !engine.fullScreen;
							SDL_SetWindowFullscreen(window,
								(engine.fullScreen ?
									SDL_WINDOW_FULLSCREEN_DESKTOP : 0));
						}
						else if (selectedOption == listLength)
						{
							menuType = MENU_MAIN;
							selectedOption = 1;
						}
						createOptionsMenu();
						break;

					case MENU_CHEAT:
						if (selectedOption == 1)
							engine.cheatShield = !engine.cheatShield;
						else if (selectedOption == 2)
							engine.cheatAmmo = !engine.cheatAmmo;
						else if (selectedOption == 3)
							engine.cheatCash = !engine.cheatCash;
						else if (selectedOption == 4)
							engine.cheatTime = !engine.cheatTime;
						else if (selectedOption == listLength)
						{
							menuType = MENU_MAIN;
							selectedOption = 1;
						}
						createCheatMenu();
						break;

					default:
						menuType = MENU_MAIN;
						selectedOption = 1;
						break;
				}
			}
			engine.keyState[KEY_FIRE] = engine.keyState[KEY_ALTFIRE] = 0;
		}

		delayFrame();
	}

	audio_haltMusic();

	SDL_FreeSurface(prlogo);
	SDL_FreeSurface(sflogo);

	engine.keyState[KEY_FIRE] = engine.keyState[KEY_ALTFIRE] = 0;

	resetLists();

	if (selectedOption == 1)
	{
		newGame();
		selectedOption = 2; // go straight to mission 0
	}

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
Scrolls the intro text up the screen and nothing else.
*/
void showStory()
{
	freeGraphics();

	int y = screen->h + 20;

	FILE *fp;

	fp = fopen("data/intro.txt", "rb");

	int i = 0;
	int nextPos = -1;
	char string[255];

	while (fscanf(fp, "%d %[^\n]%*c", &nextPos, string) == 2)
	{
		y += nextPos;
		textSurface(i, string, -1, y, FONT_WHITE);

		i++;
	}

	fclose(fp);

	loadBackground("gfx/startUp.jpg");
	blit(background, 0, 0);
	flushBuffer();

	flushInput();
	engine.keyState[KEY_FIRE] = engine.keyState[KEY_ALTFIRE] = 0;

	while (true)
	{
		updateScreen();
		unBuffer();

		getPlayerInput();

		if ((engine.keyState[KEY_FIRE] || engine.keyState[KEY_ALTFIRE]))
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

	engine.keyState[KEY_FIRE] = engine.keyState[KEY_ALTFIRE] = 0;
	engine.gameSection = SECTION_INTERMISSION;

	SDL_Surface *gameover = loadImage("gfx/gameover.png");

	clearScreen(black);
	updateScreen();
	clearScreen(black);
	SDL_Delay(1000);

	audio_playMusic("music/death.ogg", -1);

	int x = (screen->w - gameover->w) / 2;
	int y = (screen->h - gameover->h) / 2;

	updateScreen();

	flushInput();
	engine.keyState[KEY_FIRE] = engine.keyState[KEY_ALTFIRE] = 0;

	while (true)
	{
		getPlayerInput();

		if ((engine.keyState[KEY_FIRE] || engine.keyState[KEY_ALTFIRE]))
			break;

		updateScreen();

		unBuffer();
		x = ((screen->w - gameover->w) / 2) - rrand(-2, 2);
		y = ((screen->h - gameover->h) / 2)  - rrand(-2, 2);
		blit(gameover, x,  y);

		delayFrame();
	}

	SDL_FreeSurface(gameover);
	audio_haltMusic();
	flushBuffer();
}

void doCredits()
{
	loadBackground("gfx/credits.jpg");
	flushBuffer();
	freeGraphics();

	FILE *fp;
	int lastCredit = -1;

	int yPos = 0;
	int yPos2 = screen->h;
	char text[255];
	int i;

	textObject *credit;

	clearScreen(black);
	updateScreen();
	clearScreen(black);

	drawBackGround();

	audio_playMusic("music/rise_of_spirit.ogg", 1);

	fp = fopen("data/credits.txt", "rb");
	// FIXME: It would be nice for the size of this array to be determined
	// by the number of lines in the text file. I'm not sure how to do
	// that at the moment, so just giving it a very large number for now.
	credit = (textObject*) malloc(sizeof(textObject) * 300);

	while (fscanf(fp, "%d %[^\n]%*c", &yPos, text) == 2)
	{
		lastCredit++;
		credit[lastCredit].image = textSurface(text, FONT_WHITE);
		credit[lastCredit].x = (screen->w - credit[lastCredit].image->w) / 2;
		yPos2 += yPos;
		credit[lastCredit].y = yPos2;
	}

	fclose(fp);

	engine.done = 0;

	engine.keyState[KEY_ESCAPE] = 0;
	flushInput();

	while (true)
	{
		updateScreen();
		unBuffer();

		getPlayerInput();
		if (engine.keyState[KEY_ESCAPE])
			break;

		float speed = 0.5;
		if(engine.keyState[KEY_DOWN])
			speed = 2;
		else if(engine.keyState[KEY_UP])
			speed = -2;

		for (i = 0 ; i <= lastCredit ; i++)
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

	for (i = 0 ; i <= lastCredit ; i++)
	{
		SDL_FreeSurface(credit[i].image);
	}

	free(credit);
}

