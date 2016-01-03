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

static signed char showGameMenu(signed char continueSaveSlot)
{
	screen_blitText(TS_START_NEW_GAME);
	if (continueSaveSlot != -1)
	{
		screen_blitText(TS_LOAD_GAME);
		screen_blitText(TS_CONTINUE_CURRENT_GAME);
	}
	screen_blitText(TS_OPTIONS);
	if (engine.cheat)
	{
		gfx_text[TS_QUIT].y = 450;
		screen_blitText(TS_CHEAT_OPTIONS);
	}
	else
	{
		gfx_text[TS_QUIT].y = 430;
	}
	screen_blitText(TS_QUIT);

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
		if (gfx_text[i].image != NULL)
		{
			screen_blitText(i);
			gfx_text[TS_BACK_TO_MAIN_MENU].y = gfx_text[i].y + 40;
		}
	}
	screen_blitText(TS_BACK_TO_MAIN_MENU);

	return rtn;
}

static void createDifficultyMenu()
{
	gfx_createTextObject(TS_START_GAME, "START GAME", -1, 350, FONT_WHITE);

	if (game.difficulty == DIFFICULTY_EASY)
		gfx_createTextObject(TS_DIFFICULTY, "DIFFICULTY - EASY", -1, 370, FONT_WHITE);
	else if (game.difficulty == DIFFICULTY_HARD)
		gfx_createTextObject(TS_DIFFICULTY, "DIFFICULTY - HARD", -1, 370, FONT_WHITE);
	else if (game.difficulty == DIFFICULTY_NIGHTMARE)
		gfx_createTextObject(TS_DIFFICULTY, "DIFFICULTY - NIGHTMARE!", -1, 370, FONT_WHITE);
	else if (game.difficulty == DIFFICULTY_ORIGINAL)
		gfx_createTextObject(TS_DIFFICULTY, "DIFFICULTY - CLASSIC", -1, 370, FONT_WHITE);
	else
		gfx_createTextObject(TS_DIFFICULTY, "DIFFICULTY - NORMAL", -1, 370, FONT_WHITE);
}

static signed char showDifficultyMenu()
{
	gfx_text[TS_BACK_TO_MAIN_MENU].y = 410;

	screen_blitText(TS_START_GAME);
	screen_blitText(TS_DIFFICULTY);
	screen_blitText(TS_BACK_TO_MAIN_MENU);

	return 3;
}

static void createOptionsMenu()
{
	if (engine.useSound)
		gfx_createTextObject(TS_SOUND, "SOUND - ON", -1, 350, FONT_WHITE);
	else
		gfx_createTextObject(TS_SOUND, "SOUND - OFF", -1, 350, FONT_WHITE);

	if (engine.useMusic)
		gfx_createTextObject(TS_MUSIC, "MUSIC - ON", -1, 370, FONT_WHITE);
	else
		gfx_createTextObject(TS_MUSIC, "MUSIC - OFF", -1, 370, FONT_WHITE);

	if (engine.fullScreen)
		gfx_createTextObject(TS_FULLSCREEN, "FULLSCREEN - ON", -1, 390, FONT_WHITE);
	else
		gfx_createTextObject(TS_FULLSCREEN, "FULLSCREEN - OFF", -1, 390, FONT_WHITE);

	if (engine.autoPause)
		gfx_createTextObject(TS_AUTOPAUSE, "AUTOPAUSE - ON", -1, 410, FONT_WHITE);
	else
		gfx_createTextObject(TS_AUTOPAUSE, "AUTOPAUSE - OFF", -1, 410, FONT_WHITE);
}

static signed char showOptionsMenu()
{
	gfx_text[TS_BACK_TO_MAIN_MENU].y = 450;

	screen_blitText(TS_SOUND);
	screen_blitText(TS_MUSIC);
	screen_blitText(TS_FULLSCREEN);
	screen_blitText(TS_AUTOPAUSE);
	screen_blitText(TS_BACK_TO_MAIN_MENU);

	return 5;
}

static void createCheatMenu()
{
	if (engine.cheatShield)
		gfx_createTextObject(TS_UNLIMITED_SHIELD, "UNLIMITED SHIELD - ON", -1, 350,
			FONT_WHITE);
	else
		gfx_createTextObject(TS_UNLIMITED_SHIELD, "UNLIMITED SHIELD - OFF", -1, 350,
			FONT_WHITE);

	if (engine.cheatAmmo)
		gfx_createTextObject(TS_UNLIMITED_AMMO, "UNLIMITED AMMO - ON", -1, 370,
			FONT_WHITE);
	else
		gfx_createTextObject(TS_UNLIMITED_AMMO, "UNLIMITED AMMO - OFF", -1, 370,
			FONT_WHITE);

	if (engine.cheatCash)
		gfx_createTextObject(TS_UNLIMITED_CASH, "UNLIMITED CASH - ON", -1, 390,
			FONT_WHITE);
	else
		gfx_createTextObject(TS_UNLIMITED_CASH, "UNLIMITED CASH - OFF", -1, 390,
			FONT_WHITE);

	if (engine.cheatTime)
		gfx_createTextObject(TS_UNLIMITED_TIME, "UNLIMITED TIME - ON", -1, 410,
			FONT_WHITE);
	else
		gfx_createTextObject(TS_UNLIMITED_TIME, "UNLIMITED TIME - OFF", -1, 410,
			FONT_WHITE);
}

static signed char showCheatMenu()
{
	gfx_text[TS_BACK_TO_MAIN_MENU].y = 450;

	screen_blitText(TS_UNLIMITED_SHIELD);
	screen_blitText(TS_UNLIMITED_AMMO);
	screen_blitText(TS_UNLIMITED_CASH);
	screen_blitText(TS_UNLIMITED_TIME);
	screen_blitText(TS_BACK_TO_MAIN_MENU);

	return 5;
}

/*
This is the main title screen, with the stars whirling past and the
"Parallel Realities, Present..." text. Nothing too special.
*/
int doTitle()
{
	game_init();

	engine.gameSection = SECTION_TITLE;

	screen_flushBuffer();
	gfx_free();
	resetLists();
	
	// required to stop the title screen crashing
	game.system = 0;
	game.area = MISN_START;

	loadGameGraphics();

	screen_clear(black);
	renderer_update();
	screen_clear(black);

	signed char continueSaveSlot = initSaveSlots();

	loadBackground("gfx/spirit.jpg");

	SDL_Surface *prlogo, *sflogo;
	prlogo = gfx_loadImage("gfx/prlogo.png");
	sflogo = gfx_loadImage("gfx/sflogo.png");

	int prx = ((screen->w - prlogo->w) / 2);
	int pry = ((screen->h - prlogo->h) / 2);

	int sfx = ((screen->w - sflogo->w) / 2);
	int sfy = ((screen->h - sflogo->h) / 2);

	gfx_createTextObject(TS_PRESENTS, "PRESENTS", -1, 300, FONT_WHITE);
	gfx_createTextObject(TS_AN_SDL_GAME, "AN SDL GAME", -1, 300, FONT_WHITE);
	gfx_createTextObject(TS_START_NEW_GAME, "START NEW GAME", -1, 350, FONT_WHITE);
	gfx_createTextObject(TS_LOAD_GAME, "LOAD GAME", -1, 370, FONT_WHITE);
	gfx_createTextObject(TS_CONTINUE_CURRENT_GAME, "CONTINUE CURRENT GAME", -1, 390,
		FONT_WHITE);
	gfx_createTextObject(TS_OPTIONS, "OPTIONS", -1, 410, FONT_WHITE);
	gfx_createTextObject(TS_CHEAT_OPTIONS, "CHEAT OPTIONS", -1, 430, FONT_WHITE);
	gfx_createTextObject(TS_QUIT, "QUIT", -1, 430, FONT_WHITE);

	createOptionsMenu();
	createDifficultyMenu();
	gfx_createTextObject(TS_BACK_TO_MAIN_MENU, "BACK TO MAIN MENU", -1, 0, FONT_WHITE);

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

	screen_drawBackground();

	engine.done = 0;
	flushInput();
	engine.keyState[KEY_FIRE] = engine.keyState[KEY_ALTFIRE] = 0;

	audio_playMusic("music/walking_among_androids.ogg", 1);

	while (!engine.done)
	{
		renderer_update();
		screen_unBuffer();

		now = SDL_GetTicks();

		game_doStars();
		game_doExplosions();

		for (int i = 0 ; i < 15 ; i++)
		{
			explosion_addEngine(&aliens[i]);
			aliens[i].x += aliens[i].dx;
			screen_blit(aliens[i].image[0], (int)aliens[i].x, (int)aliens[i].y);
			if (aliens[i].x > 830)
			{
				aliens[i].x = -40;
				aliens[i].y = rand() % (screen->h - 20);
				aliens[i].dx = 1 + rand() % 3;
			}
		}

		if ((now - then > 2000) && (now - then < 8000) && (!skip))
		{
			screen_blit(prlogo, prx, pry);
		}
		else if ((now - then > 9000) && (now - then < 15000) && (!skip))
		{
			screen_blitText(TS_PRESENTS);
		}
		else if ((now - then > 16000) && (now - then < 21000) && (!skip))
		{
			screen_blitText(TS_AN_SDL_GAME);
		}
		else if ((now - then > 25500) || (skip))
		{
			screen_blit(sflogo, sfx, sfy);

			if ((now - then >= 27500) || (skip))
			{
				screen_addBuffer(0, 0, screen->w, screen->h);

				screen_drawRect(optionRec.x, optionRec.y, optionRec.w, optionRec.h, redGlow, 0x00, 0x00);

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
					gfx_renderString("Copyright Parallel Realities 2003", 5,
						560, FONT_WHITE, 0, gfx_background);
					gfx_renderString("Copyright Guus Sliepen, Astrid S. de Wijn and others 2012",
						5, 580, FONT_WHITE, 0, gfx_background);
					gfx_renderString(buildVersion, 794 - strlen(buildVersion) * 9,
						580, FONT_WHITE, 0, gfx_background);
					screen_addBuffer(0, 560, 800, 40);
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
				gfx_renderString("Copyright Parallel Realities 2003", 5, 560,
					FONT_WHITE, 0, gfx_background);
				gfx_renderString("Copyright Guus Sliepen, Astrid S. de Wijn and others 2012",
					5, 580, FONT_WHITE, 0, gfx_background);
				gfx_renderString(buildVersion, 794 - strlen(buildVersion) * 9,
					580, FONT_WHITE, 0, gfx_background);
				screen_addBuffer(0, 560, 800, 40);
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
							game.difficulty++;
							game.difficulty %= DIFFICULTY_MAX;
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
						else if (selectedOption == 4)
						{
							engine.autoPause = !engine.autoPause;
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

		game_delayFrame();
	}

	audio_haltMusic();

	SDL_FreeSurface(prlogo);
	SDL_FreeSurface(sflogo);

	engine.keyState[KEY_FIRE] = engine.keyState[KEY_ALTFIRE] = 0;

	resetLists();

	if (selectedOption == 1)
	{
		game_init();
		selectedOption = 2; // go straight to mission 0
	}

	if (selectedOption == 3)
	{
		game_init();
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
	gfx_free();

	int y = screen->h + 20;

	FILE *fp;

	fp = fopen("data/intro.txt", "rb");

	int index = 0;
	int nextPos = -1;
	char string[255];

	while (fscanf(fp, "%d %[^\n]%*c", &nextPos, string) == 2)
	{
		y += nextPos;
		gfx_createTextObject(index, string, -1, y, FONT_WHITE);

		index++;
	}

	fclose(fp);

	loadBackground("gfx/startUp.jpg");
	screen_drawBackground();
	screen_flushBuffer();

	flushInput();
	engine.keyState[KEY_FIRE] = engine.keyState[KEY_ALTFIRE] = 0;

	while (true)
	{
		renderer_update();
		screen_unBuffer();

		getPlayerInput();

		if ((engine.keyState[KEY_FIRE] || engine.keyState[KEY_ALTFIRE]))
			break;

		// XXX: The fact that it's line 8 that's watched is completely
		// arbitrary. It might be prudent to replace this with something else.
		if (gfx_text[8].y > (screen->h / 2) + 150)
		{
			for (int i = 0 ; i < 9 ; i++)
			{
				gfx_text[i].y -= 0.33333;
				screen_blitText(i);
			}
		}
		else
		{
			SDL_Delay(3000);
			break;
		}

		game_delayFrame();
	}
}

/*
The game over screen :(
*/
void gameover()
{
	screen_flushBuffer();
	gfx_free();
	SDL_FillRect(gfx_background, NULL, black);

	engine.keyState[KEY_FIRE] = engine.keyState[KEY_ALTFIRE] = 0;
	engine.gameSection = SECTION_INTERMISSION;

	SDL_Surface *gameover = gfx_loadImage("gfx/gameover.png");

	screen_clear(black);
	renderer_update();
	screen_clear(black);
	SDL_Delay(1000);

	audio_playMusic("music/death.ogg", -1);

	int x = (screen->w - gameover->w) / 2;
	int y = (screen->h - gameover->h) / 2;

	renderer_update();

	flushInput();
	engine.keyState[KEY_FIRE] = engine.keyState[KEY_ALTFIRE] = 0;

	while (true)
	{
		getPlayerInput();

		if (engine.keyState[KEY_FIRE] || engine.keyState[KEY_ALTFIRE])
			break;

		renderer_update();

		screen_unBuffer();
		x = ((screen->w - gameover->w) / 2) - RANDRANGE(-2, 2);
		y = ((screen->h - gameover->h) / 2)  - RANDRANGE(-2, 2);
		screen_blit(gameover, x,  y);

		game_delayFrame();
	}

	SDL_FreeSurface(gameover);
	audio_haltMusic();
	screen_flushBuffer();
}

void doCredits()
{
	loadBackground("gfx/credits.jpg");
	screen_flushBuffer();
	gfx_free();

	FILE *fp;
	int lastCredit = -1;

	int yPos = 0;
	int yPos2 = screen->h;
	char text[255];
	int i;

	textObject *credit;

	screen_clear(black);
	renderer_update();
	screen_clear(black);

	screen_drawBackground();

	audio_playMusic("music/rise_of_spirit.ogg", 1);

	fp = fopen("data/credits.txt", "rb");
	// FIXME: It would be nice for the size of this array to be determined
	// by the number of lines in the text file. I'm not sure how to do
	// that at the moment, so just giving it a very large number for now.
	credit = (textObject*) malloc(sizeof(textObject) * 300);

	while (fscanf(fp, "%d %[^\n]%*c", &yPos, text) == 2)
	{
		lastCredit++;
		credit[lastCredit].image = gfx_createTextSurface(text, FONT_WHITE);
		credit[lastCredit].x = (screen->w - credit[lastCredit].image->w) / 2;
		yPos2 += yPos;
		credit[lastCredit].y = yPos2;
	}

	fclose(fp);

	engine.done = 0;

	engine.keyState[KEY_ESCAPE] = 0;
	engine.keyState[KEY_FIRE] = 0;
	engine.keyState[KEY_ALTFIRE] = 0;
	flushInput();

	while (true)
	{
		renderer_update();
		screen_unBuffer();

		getPlayerInput();
		if (engine.keyState[KEY_ESCAPE] || engine.keyState[KEY_FIRE] ||
				engine.keyState[KEY_ALTFIRE])
			break;

		float speed = 0.5;
		if(engine.keyState[KEY_DOWN])
			speed = 2;
		else if(engine.keyState[KEY_UP])
			speed = -2;

		for (i = 0 ; i <= lastCredit ; i++)
		{
			if ((credit[i].y > -10) && (credit[i].y < (screen->h + 10)))
				screen_blit(credit[i].image, (int)credit[i].x, (int)credit[i].y);
			if (speed > 0 && credit[lastCredit].y > ((screen->h / 2) + 100))
				credit[i].y -= speed;
			else if(speed < 0 && credit[0].y < screen->h)
				credit[i].y -= speed;
		}

		game_delayFrame();
	}

	for (i = 0 ; i <= lastCredit ; i++)
	{
		SDL_FreeSurface(credit[i].image);
	}

	free(credit);
}

