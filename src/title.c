/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012, 2013 Guus Sliepen
Copyright (C) 2012, 2015-2019 Julie Marchant <onpon4@riseup.net>

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

#include <stdlib.h>
#include <stdio.h>

#include "SDL.h"

#include "colors.h"
#include "defs.h"
#include "structs.h"

#include "alien.h"
#include "audio.h"
#include "engine.h"
#include "explosion.h"
#include "game.h"
#include "gfx.h"
#include "player.h"
#include "renderer.h"
#include "save.h"
#include "screen.h"
#include "title.h"
#include "window.h"

static int showGameMenu(int continueSaveSlot)
{
	screen_blitText(TS_START_NEW_GAME);
	screen_blitText(TS_LOAD_GAME);
	if (continueSaveSlot != -1)
	{
		screen_blitText(TS_CONTINUE_CURRENT_GAME);
	}
	screen_blitText(TS_OPTIONS);
	screen_blitText(TS_CREDITS);
	if (engine.cheat)
	{
		gfx_textSprites[TS_QUIT].y = screen->h / 3 + 170;
		screen_blitText(TS_CHEAT_OPTIONS);
	}
	else
	{
		gfx_textSprites[TS_QUIT].y = screen->h / 3 + 150;
	}
	screen_blitText(TS_QUIT);

	if (engine.cheat)
		return 7;

	return 6;
}

static int showLoadMenu()
{
	int rtn = 1;

	for (int i = TS_SAVESLOT_0 ; i <= TS_SAVESLOT_5 ; i++)
	{
		rtn++;
		if (gfx_textSprites[i].image != NULL)
		{
			screen_blitText(i);
			gfx_textSprites[TS_BACK_TO_MAIN_MENU].y = gfx_textSprites[i].y + 40;
		}
	}
	screen_blitText(TS_BACK_TO_MAIN_MENU);

	return rtn;
}

static void createDifficultyMenu()
{
	gfx_createTextObject(TS_START_GAME, "START GAME",
		-1, screen->h / 3 + 50, FONT_WHITE);

	if (game.difficulty == DIFFICULTY_EASY)
		gfx_createTextObject(TS_DIFFICULTY, "DIFFICULTY - EASY",
			-1, screen->h / 3 + 70, FONT_WHITE);
	else if (game.difficulty == DIFFICULTY_HARD)
		gfx_createTextObject(TS_DIFFICULTY, "DIFFICULTY - HARD",
			-1, screen->h / 3 + 70, FONT_WHITE);
	else if (game.difficulty == DIFFICULTY_NIGHTMARE)
		gfx_createTextObject(TS_DIFFICULTY, "DIFFICULTY - NIGHTMARE!",
			-1, screen->h / 3 + 70, FONT_WHITE);
	else if (game.difficulty == DIFFICULTY_ORIGINAL)
		gfx_createTextObject(TS_DIFFICULTY, "DIFFICULTY - CLASSIC",
			-1, screen->h / 3 + 70, FONT_WHITE);
	else
		gfx_createTextObject(TS_DIFFICULTY, "DIFFICULTY - NORMAL",
			-1, screen->h / 3 + 70, FONT_WHITE);
}

static int showDifficultyMenu()
{
	gfx_textSprites[TS_BACK_TO_MAIN_MENU].y = screen->h / 3 + 110;

	screen_blitText(TS_START_GAME);
	screen_blitText(TS_DIFFICULTY);
	screen_blitText(TS_BACK_TO_MAIN_MENU);

	return 3;
}

static void createOptionsMenu()
{
	if (engine.useSound)
		gfx_createTextObject(TS_SOUND, "SOUND - ON",
			-1, screen->h / 3 + 50, FONT_WHITE);
	else
		gfx_createTextObject(TS_SOUND, "SOUND - OFF",
			-1, screen->h / 3 + 50, FONT_WHITE);

	if (engine.useMusic)
		gfx_createTextObject(TS_MUSIC, "MUSIC - ON",
			-1, screen->h / 3 + 70, FONT_WHITE);
	else
		gfx_createTextObject(TS_MUSIC, "MUSIC - OFF",
			-1, screen->h / 3 + 70, FONT_WHITE);

	if (engine.fullScreen)
		gfx_createTextObject(TS_FULLSCREEN, "FULLSCREEN - ON",
			-1, screen->h / 3 + 90, FONT_WHITE);
	else
		gfx_createTextObject(TS_FULLSCREEN, "FULLSCREEN - OFF",
			-1, screen->h / 3 + 90, FONT_WHITE);

	if (engine.autoPause)
		gfx_createTextObject(TS_AUTOPAUSE, "AUTOPAUSE - ON",
			-1, screen->h / 3 + 110, FONT_WHITE);
	else
		gfx_createTextObject(TS_AUTOPAUSE, "AUTOPAUSE - OFF",
			-1, screen->h / 3 + 110, FONT_WHITE);
}

static int showOptionsMenu()
{
	gfx_textSprites[TS_BACK_TO_MAIN_MENU].y = screen->h / 3 + 150;

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
		gfx_createTextObject(TS_UNLIMITED_SHIELD, "UNLIMITED SHIELD - ON",
			-1, screen->h / 3 + 50, FONT_WHITE);
	else
		gfx_createTextObject(TS_UNLIMITED_SHIELD, "UNLIMITED SHIELD - OFF",
			-1, screen->h / 3 + 50, FONT_WHITE);

	if (engine.cheatAmmo)
		gfx_createTextObject(TS_UNLIMITED_AMMO, "UNLIMITED AMMO - ON",
			-1, screen->h / 3 + 70, FONT_WHITE);
	else
		gfx_createTextObject(TS_UNLIMITED_AMMO, "UNLIMITED AMMO - OFF",
			-1, screen->h / 3 + 70, FONT_WHITE);

	if (engine.cheatCash)
		gfx_createTextObject(TS_UNLIMITED_CASH, "UNLIMITED CASH - ON",
			-1, screen->h / 3 + 90, FONT_WHITE);
	else
		gfx_createTextObject(TS_UNLIMITED_CASH, "UNLIMITED CASH - OFF",
			-1, screen->h / 3 + 90, FONT_WHITE);

	if (engine.cheatTime)
		gfx_createTextObject(TS_UNLIMITED_TIME, "UNLIMITED TIME - ON",
			-1, screen->h / 3 + 110, FONT_WHITE);
	else
		gfx_createTextObject(TS_UNLIMITED_TIME, "UNLIMITED TIME - OFF",
			-1, screen->h / 3 + 110, FONT_WHITE);
}

static int showCheatMenu()
{
	gfx_textSprites[TS_BACK_TO_MAIN_MENU].y = screen->h / 3 + 150;

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
int title_show()
{
	int continueSaveSlot;

	int prx;
	int pry;
	int sfx;
	int sfy;

	int then;
	int now;

	int redGlow = 255;
	int redDir = -2;
	char buildVersion[25];

	int selectedOption = 1;
	int skip = 0;
	int listLength = 6; // menu list length
	int menuType = MENU_MAIN;

	game_init();

	engine.gameSection = SECTION_TITLE;

	screen_flushBuffer();
	gfx_free();
	engine_resetLists();
	
	// required to stop the title screen crashing
	game.system = 0;
	game.area = MISN_START;

	gfx_loadSprites();

	screen_clear(black);
	renderer_update();
	screen_clear(black);

	continueSaveSlot = save_initSlots();

	gfx_loadBackground("gfx/spirit.jpg");

	SDL_Surface *prlogo, *sflogo;
	prlogo = gfx_loadImage("gfx/prlogo.png");
	sflogo = gfx_loadImage("gfx/sflogo.png");

	prx = ((screen->w - prlogo->w) / 2);
	pry = ((screen->h - prlogo->h) / 2);

	sfx = ((screen->w - sflogo->w) / 2);
	sfy = ((screen->h - sflogo->h) / 3);

	gfx_createTextObject(TS_PRESENTS, "PRESENTS",
		-1, screen->h / 2, FONT_WHITE);
	gfx_createTextObject(TS_AN_SDL_GAME, "AN SDL GAME",
		-1, screen->h / 2, FONT_WHITE);
	gfx_createTextObject(TS_START_NEW_GAME, "START NEW GAME",
		-1, screen->h / 3 + 50, FONT_WHITE);
	gfx_createTextObject(TS_LOAD_GAME, "LOAD GAME",
		-1, screen->h / 3 + 70, FONT_WHITE);
	gfx_createTextObject(TS_CONTINUE_CURRENT_GAME, "CONTINUE CURRENT GAME",
		-1, screen->h / 3 + 90, FONT_WHITE);
	gfx_createTextObject(TS_OPTIONS, "OPTIONS",
		-1, screen->h / 3 + 110, FONT_WHITE);
	gfx_createTextObject(TS_CREDITS, "CREDITS",
		-1, screen->h / 3 + 130, FONT_WHITE);
	gfx_createTextObject(TS_CHEAT_OPTIONS, "CHEAT OPTIONS",
		-1, screen->h / 3 + 150, FONT_WHITE);
	gfx_createTextObject(TS_QUIT, "QUIT",
		-1, screen->h / 3 + 150, FONT_WHITE);

	createOptionsMenu();
	createDifficultyMenu();
	gfx_createTextObject(TS_BACK_TO_MAIN_MENU, "BACK TO MAIN MENU", -1, 0, FONT_WHITE);

	createCheatMenu();

	// Set the star motion
	engine.ssx = -0.5;
	engine.ssy = 0;
	engine.smx = 0;
	engine.smy = 0;

	then = SDL_GetTicks();

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

    sprintf(buildVersion, "Version %s", VERSION );

	SDL_Rect optionRec;

	optionRec.x = screen->w / 2 - 110;
	optionRec.y = screen->h / 3 + 45;
	optionRec.h = 22;
	optionRec.w = 215;

	if (continueSaveSlot != -1)
	{
		selectedOption = 3;
		optionRec.y += 40;
	}

	screen_drawBackground();

	engine.done = 0;
	player_flushInput();
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
			if (aliens[i].x > screen->w + 30)
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
						if (selectedOption == 3)
							if (continueSaveSlot == -1)
								selectedOption = 2;
				}
				if (engine.keyState[KEY_DOWN])
				{
					engine.keyState[KEY_DOWN] = 0;
					WRAP_ADD(selectedOption, 1, 0, listLength);
					if (menuType == MENU_MAIN)
						if (selectedOption == 3)
							if (continueSaveSlot == -1)
								selectedOption = 4;
				}

				optionRec.y = screen->h / 3 + 26 + (20 * selectedOption);
				if (menuType > MENU_MAIN)
					if (selectedOption == listLength)
						optionRec.y += 20;

				if (!skip)
				{
					gfx_renderString("Copyright Parallel Realities 2003",
						5, gfx_background->h - 60, FONT_WHITE, 0, gfx_background);
					gfx_renderString("Copyright Guus Sliepen, Astrid S. de Wijn and others 2012",
						5, gfx_background->h - 40, FONT_WHITE, 0, gfx_background);
					gfx_renderString("Copyright 2015-2017 Julie Marchant",
						5, gfx_background->h - 20, FONT_WHITE, 0, gfx_background);
					gfx_renderString(buildVersion, gfx_background->w - 6 - strlen(buildVersion) * 9,
						gfx_background->h - 20, FONT_WHITE, 0, gfx_background);
					screen_addBuffer(0, 0, screen->w, screen->h);
					skip = 1;
				}
			}
		}

		player_getInput();

		if ((engine.keyState[KEY_FIRE] || engine.keyState[KEY_ALTFIRE]))
		{
			if ((now - then <= 27500) && (!skip))
			{
				gfx_renderString("Copyright Parallel Realities 2003",
					5, screen->h - 60, FONT_WHITE, 0, gfx_background);
				gfx_renderString("Copyright Guus Sliepen, Astrid S. de Wijn and others 2012",
					5, screen->h - 40, FONT_WHITE, 0, gfx_background);
				gfx_renderString("Copyright 2015-2019 Julie Marchant",
					5, screen->h - 20, FONT_WHITE, 0, gfx_background);
				gfx_renderString(buildVersion, screen->w - 6 - strlen(buildVersion) * 9,
					screen->h - 20, FONT_WHITE, 0, gfx_background);
				screen_addBuffer(0, screen->h - 40, screen->w, 40);
				skip = 1;
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
							selectedOption = 5;
						}
						else if (selectedOption == 5)
						{
							engine.done = 1;
						}
						else if (selectedOption == 6)
						{
							if (engine.cheat)
							{
								menuType = MENU_CHEAT;
								selectedOption = 1;
							}
							else
								engine.done = 1;
						}
						else if (selectedOption == 7)
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
						{
							engine.useSound = !engine.useSound;
							selectedOption = listLength;
						}
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
							selectedOption = listLength;
						}
						else if (selectedOption == 3)
						{
							engine.fullScreen = !engine.fullScreen;
							SDL_SetWindowFullscreen(window,
								(engine.fullScreen ? FULLSCREEN : 0));
							selectedOption = listLength;
						}
						else if (selectedOption == 4)
						{
							engine.autoPause = !engine.autoPause;
							selectedOption = listLength;
						}
						else if (selectedOption == listLength)
						{
							menuType = MENU_MAIN;
							selectedOption = 4;
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
			engine.keyState[KEY_FIRE] = 0;
			engine.keyState[KEY_ALTFIRE] = 0;
		}

		game_delayFrame();
	}

	audio_haltMusic();

	SDL_FreeSurface(prlogo);
	SDL_FreeSurface(sflogo);

	engine.keyState[KEY_FIRE] = 0;
	engine.keyState[KEY_ALTFIRE] = 0;

	engine_resetLists();

	if (selectedOption == 1)
	{
		game_init();
		selectedOption = 2; // go straight to mission 0
	}

	if (selectedOption == 3)
	{
		game_init();
		selectedOption = save_load(continueSaveSlot);
	}

	if (selectedOption == 5)
	{
		title_showCredits();
		selectedOption = 0;
	}

	// Send back a negative number...
	if (selectedOption > 5)
	{
		selectedOption = -1;
		exit(0);
	}

	return selectedOption;
}

void title_showCredits()
{
	gfx_loadBackground("gfx/credits.jpg");
	screen_flushBuffer();
	gfx_free();

	FILE *fp;
	int lastCredit = -1;

	int yPos = 0;
	int yPos2 = screen->h;
	char text[255];
	int i;

	TextObject *credit;

	screen_clear(black);
	renderer_update();
	screen_clear(black);

	screen_drawBackground();

	audio_playMusic("music/rise_of_spirit.ogg", 1);

	fp = fopen("data/credits.txt", "rb");
	// FIXME: It would be nice for the size of this array to be determined
	// by the number of lines in the text file. I'm not sure how to do
	// that at the moment, so just giving it a very large number for now.
	credit = malloc(300 * sizeof(*credit));
	if (credit == NULL)
	{
		engine_warn("Failed to allocate memory for credits");
		return;
	}

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
	player_flushInput();

	while (1)
	{
		renderer_update();
		screen_unBuffer();

		player_getInput();
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

