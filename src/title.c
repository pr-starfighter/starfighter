/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012, 2013 Guus Sliepen
Copyright (C) 2012, 2015-2020 Layla Marchant <diligentcircle@riseup.net>

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
	screen_blitText(TS_START_NEW_GAME, -1, MENU_Y);
	screen_blitText(TS_LOAD_GAME, -1, MENU_Y + MENU_SPACING);
	if (continueSaveSlot != -1)
	{
		screen_blitText(TS_CONTINUE_CURRENT_GAME, -1, MENU_Y + 2 * MENU_SPACING);
	}
	screen_blitText(TS_OPTIONS, -1, MENU_Y + 3 * MENU_SPACING);
	screen_blitText(TS_CREDITS, -1, MENU_Y + 4 * MENU_SPACING);
	if (engine.cheat)
	{
		gfx_textSprites[TS_QUIT].y = MENU_SPACING;
		screen_blitText(TS_CHEAT_OPTIONS, -1, MENU_Y + 5 * MENU_SPACING);
	}
	else
	{
		gfx_textSprites[TS_QUIT].y = 0;
	}
	screen_blitText(TS_QUIT, -1, MENU_Y + 5 * MENU_SPACING);

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
			screen_blitText(i, -1, MENU_Y);
			gfx_textSprites[TS_BACK_TO_MAIN_MENU].y = gfx_textSprites[i].y + 2 * MENU_SPACING;
		}
	}
	screen_blitText(TS_BACK_TO_MAIN_MENU, -1, MENU_Y);

	return rtn;
}

static void createDifficultyMenu()
{
	char difficulty[STRMAX_SHORT];
	char menutext[STRMAX_SHORT];

	gfx_createTextObject(TS_START_GAME, _("START GAME"), 0, 0, FONT_WHITE);

	game_getDifficultyText(difficulty, game.difficulty);

	/// Difficulty menu option.
	/// Retain "%s" as-is.  It is replaced with the difficulty chosen.
	snprintf(menutext, STRMAX_SHORT, _("DIFFICULTY - %s"), difficulty);
	gfx_createTextObject(TS_DIFFICULTY, menutext, 0, 0, FONT_WHITE);
}

static int showDifficultyMenu()
{
	screen_blitText(TS_START_GAME, -1, MENU_Y);
	screen_blitText(TS_DIFFICULTY, -1, MENU_Y + MENU_SPACING);
	gfx_textSprites[TS_BACK_TO_MAIN_MENU].y = 0;
	screen_blitText(TS_BACK_TO_MAIN_MENU, -1, MENU_Y + 3 * MENU_SPACING);

	return 3;
}

static void createOptionsMenu()
{
	if (engine.useSound)
		gfx_createTextObject(TS_SOUND, _("SOUND - ON"), 0, 0, FONT_WHITE);
	else
		gfx_createTextObject(TS_SOUND, _("SOUND - OFF"), 0, 0, FONT_WHITE);

	if (engine.useMusic)
		gfx_createTextObject(TS_MUSIC, _("MUSIC - ON"), 0, 0, FONT_WHITE);
	else
		gfx_createTextObject(TS_MUSIC, _("MUSIC - OFF"), 0, 0, FONT_WHITE);

	if (engine.fullScreen)
		gfx_createTextObject(TS_FULLSCREEN, _("FULLSCREEN - ON"), 0, 0, FONT_WHITE);
	else
		gfx_createTextObject(TS_FULLSCREEN, _("FULLSCREEN - OFF"), 0, 0, FONT_WHITE);

	if (engine.autoPause)
		gfx_createTextObject(TS_AUTOPAUSE, _("AUTOPAUSE - ON"), 0, 0, FONT_WHITE);
	else
		gfx_createTextObject(TS_AUTOPAUSE, _("AUTOPAUSE - OFF"), 0, 0, FONT_WHITE);

	if (engine.radioLife == RADIO_LIFE_INSTANT)
		gfx_createTextObject(TS_RADIO_SPEED, _("MESSAGE SPEED - INSTANT"), 0, 0, FONT_WHITE);
	else if (engine.radioLife == RADIO_LIFE_FAST)
		gfx_createTextObject(TS_RADIO_SPEED, _("MESSAGE SPEED - FAST"), 0, 0, FONT_WHITE);
	else if (engine.radioLife == RADIO_LIFE_NORMAL)
		gfx_createTextObject(TS_RADIO_SPEED, _("MESSAGE SPEED - NORMAL"), 0, 0, FONT_WHITE);
	else if (engine.radioLife == RADIO_LIFE_SLOW)
		gfx_createTextObject(TS_RADIO_SPEED, _("MESSAGE SPEED - SLOW"), 0, 0, FONT_WHITE);
	else if (engine.radioLife == RADIO_LIFE_SLOTH)
		gfx_createTextObject(TS_RADIO_SPEED, _("MESSAGE SPEED - SLOTH"), 0, 0, FONT_WHITE);
	else // Shouldn't happen, but adding this just in case
		gfx_createTextObject(TS_RADIO_SPEED, _("MESSAGE SPEED - ERROR"), 0, 0, FONT_WHITE);
}

static int showOptionsMenu()
{
	screen_blitText(TS_SOUND, -1, MENU_Y);
	screen_blitText(TS_MUSIC, -1, MENU_Y + MENU_SPACING);
	screen_blitText(TS_FULLSCREEN, -1, MENU_Y + 2 * MENU_SPACING);
	screen_blitText(TS_AUTOPAUSE, -1, MENU_Y + 3 * MENU_SPACING);
	screen_blitText(TS_RADIO_SPEED, -1, MENU_Y + 4 * MENU_SPACING);
	gfx_textSprites[TS_BACK_TO_MAIN_MENU].y = 0;
	screen_blitText(TS_BACK_TO_MAIN_MENU, -1, MENU_Y + 6 * MENU_SPACING);

	return 6;
}

static void createCheatMenu()
{
	if (engine.cheatShield)
		gfx_createTextObject(TS_UNLIMITED_SHIELD, _("UNLIMITED SHIELD - ON"), 0, 0, FONT_WHITE);
	else
		gfx_createTextObject(TS_UNLIMITED_SHIELD, _("UNLIMITED SHIELD - OFF"), 0, 0, FONT_WHITE);

	if (engine.cheatAmmo)
		gfx_createTextObject(TS_UNLIMITED_AMMO, _("UNLIMITED AMMO - ON"), 0, 0, FONT_WHITE);
	else
		gfx_createTextObject(TS_UNLIMITED_AMMO, _("UNLIMITED AMMO - OFF"), 0, 0, FONT_WHITE);

	if (engine.cheatCash)
		gfx_createTextObject(TS_UNLIMITED_CASH, _("UNLIMITED CASH - ON"), 0, 0, FONT_WHITE);
	else
		gfx_createTextObject(TS_UNLIMITED_CASH, _("UNLIMITED CASH - OFF"), 0, 0, FONT_WHITE);

	if (engine.cheatTime)
		gfx_createTextObject(TS_UNLIMITED_TIME, _("UNLIMITED TIME - ON"), 0, 0, FONT_WHITE);
	else
		gfx_createTextObject(TS_UNLIMITED_TIME, _("UNLIMITED TIME - OFF"), 0, 0, FONT_WHITE);
}

static int showCheatMenu()
{
	screen_blitText(TS_UNLIMITED_SHIELD, -1, MENU_Y);
	screen_blitText(TS_UNLIMITED_AMMO, -1, MENU_Y + MENU_SPACING);
	screen_blitText(TS_UNLIMITED_CASH, -1, MENU_Y + 2 * MENU_SPACING);
	screen_blitText(TS_UNLIMITED_TIME, -1, MENU_Y + 3 * MENU_SPACING);
	gfx_textSprites[TS_BACK_TO_MAIN_MENU].y = 0;
	screen_blitText(TS_BACK_TO_MAIN_MENU, -1, MENU_Y + 5 * MENU_SPACING);

	return 5;
}

/*
This is the main title screen, with the stars whirling past and the
"Parallel Realities, Present..." text. Nothing too special.
*/
int title_show()
{
	int continueSaveSlot;

	int then;
	int now;
	int x;
	int y;

	int redGlow = 255;
	int redDir = -2;
	char buildVersion[STRMAX_SHORT];

	int selectedOption = 1;
	int skip = 0;
	int listLength = 6; // menu list length
	int menuType = MENU_MAIN;

	SDL_Surface *copyrightText;
	SDL_Surface *infoText;

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

	gfx_createTextObject(TS_PRESENTS, "PRESENTS", 0, 0, FONT_WHITE);
	gfx_createTextObject(TS_AN_SDL_GAME, "AN SDL GAME", 0, 0, FONT_WHITE);
	gfx_createTextObject(TS_START_NEW_GAME, _("START NEW GAME"), 0, 0, FONT_WHITE);
	gfx_createTextObject(TS_LOAD_GAME, _("LOAD GAME"), 0, 0, FONT_WHITE);
	gfx_createTextObject(TS_CONTINUE_CURRENT_GAME, _("CONTINUE CURRENT GAME"), 0, 0, FONT_WHITE);
	gfx_createTextObject(TS_OPTIONS, _("OPTIONS"), 0, 0, FONT_WHITE);
	gfx_createTextObject(TS_CREDITS, _("CREDITS"), 0, 0, FONT_WHITE);
	gfx_createTextObject(TS_CHEAT_OPTIONS, _("CHEAT OPTIONS"), 0, 0, FONT_WHITE);
	gfx_createTextObject(TS_QUIT, _("QUIT"), 0, 0, FONT_WHITE);

	createOptionsMenu();
	createDifficultyMenu();
	gfx_createTextObject(TS_BACK_TO_MAIN_MENU, _("BACK TO MAIN MENU"), 0, 0, FONT_WHITE);

	createCheatMenu();

	copyrightText = gfx_createSurface(screen->w, 3 * MENU_SPACING);
	gfx_renderString("Copyright Parallel Realities 2003",
		5, 0, FONT_WHITE, 0, copyrightText);
	gfx_renderString("Copyright Guus Sliepen, Astrid S. de Wijn and others 2012",
		5, MENU_SPACING, FONT_WHITE, 0, copyrightText);
	gfx_renderString("Copyright 2015-2020 Layla Marchant",
		5, 2 * MENU_SPACING, FONT_WHITE, 0, copyrightText);

	snprintf(buildVersion, STRMAX_SHORT, "Version %s", VERSION );
	infoText = gfx_createSurface(strlen(buildVersion) * (PIXFONT_W + 1) + 6, MENU_SPACING);
	gfx_renderString(buildVersion, 0, 0, FONT_WHITE, 0, infoText);

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

	SDL_Rect optionRec;

	optionRec.x = screen->w / 2 - MENU_W / 2;
	optionRec.y = MENU_Y - 5;
	optionRec.h = MENU_SPACING + 2;
	optionRec.w = MENU_W;

	if (continueSaveSlot != -1)
	{
		selectedOption = 3;
		optionRec.y += 40;
	}

	screen_drawBackground();

	engine.done = ENGINE_RUNNING;
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
			screen_blit(prlogo, ((screen->w - prlogo->w) / 2), ((screen->h - prlogo->h) / 2));
		}
		else if ((now - then > 9000) && (now - then < 15000) && (!skip))
		{
			screen_blitText(TS_PRESENTS, -1, screen->h / 2);
		}
		else if ((now - then > 16000) && (now - then < 21000) && (!skip))
		{
			screen_blitText(TS_AN_SDL_GAME, -1, screen->h / 2);
		}
		else if ((now - then > 25500) || (skip))
		{
			screen_blit(sflogo, ((screen->w - sflogo->w) / 2), ((screen->h - sflogo->h) / 3));

			if ((now - then >= 27500) || (skip))
			{
				optionRec.x = screen->w / 2 - optionRec.w / 2;
				optionRec.y = (MENU_Y - 4 - MENU_SPACING) + MENU_SPACING * selectedOption;
				if (menuType > MENU_MAIN)
					if (selectedOption == listLength)
						optionRec.y += MENU_SPACING;

				screen_drawRect(optionRec.x, optionRec.y, optionRec.w, optionRec.h, redGlow, 0x00, 0x00);
				screen_addBuffer(optionRec.x, optionRec.y, optionRec.w + 1, optionRec.h + 1);

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

				screen_addBuffer(optionRec.x - 1, MENU_Y, optionRec.w + 2, listLength * MENU_SPACING);

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

				x = 0;
				y = screen->h - copyrightText->h;
				screen_blit(copyrightText, x, y);
				screen_addBuffer(x, y, copyrightText->w, copyrightText->h);

				x = screen->w - infoText->w;
				y = screen->h - infoText->h;
				screen_blit(infoText, x, y);
				screen_addBuffer(x, y, infoText->w, infoText->h);
			}
		}

		player_getInput();

		if ((engine.keyState[KEY_FIRE] || engine.keyState[KEY_ALTFIRE]))
		{
			if ((now - then <= 27500) && (!skip))
			{
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
							engine.done = ENGINE_CLOSING;
						else if (selectedOption == 4)
						{
							menuType = MENU_OPTIONS;
							selectedOption = 6;
						}
						else if (selectedOption == 5)
						{
							engine.done = ENGINE_CLOSING;
						}
						else if (selectedOption == 6)
						{
							if (engine.cheat)
							{
								menuType = MENU_CHEAT;
								selectedOption = 1;
							}
							else
								engine.done = ENGINE_CLOSING;
						}
						else if (selectedOption == 7)
							engine.done = ENGINE_CLOSING;
						break;

					case MENU_DIFFICULTY:
						if (selectedOption == 1)
							engine.done = ENGINE_CLOSING;
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
							engine.done = ENGINE_CLOSING;
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
						}
						else if ((selectedOption == 2) && (engine.useAudio))
						{
							engine.useMusic = !engine.useMusic;

							if (engine.useMusic)
							{
								audio_playMusic("music/walking_among_androids.ogg", 1);
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
								(engine.fullScreen ? FULLSCREEN : 0));
						}
						else if (selectedOption == 4)
						{
							engine.autoPause = !engine.autoPause;
						}
						else if (selectedOption == 5)
						{
							if (engine.radioLife == RADIO_LIFE_INSTANT)
								engine.radioLife = RADIO_LIFE_FAST;
							else if (engine.radioLife == RADIO_LIFE_FAST)
								engine.radioLife = RADIO_LIFE_NORMAL;
							else if (engine.radioLife == RADIO_LIFE_NORMAL)
								engine.radioLife = RADIO_LIFE_SLOW;
							else if (engine.radioLife == RADIO_LIFE_SLOW)
								engine.radioLife = RADIO_LIFE_SLOTH;
							else if (engine.radioLife == RADIO_LIFE_SLOTH)
								engine.radioLife = RADIO_LIFE_INSTANT;
							else // Just in case
								engine.radioLife = RADIO_LIFE_NORMAL;
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
	SDL_FreeSurface(copyrightText);
	SDL_FreeSurface(infoText);

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
	int lastCredit;

	int yPos = 0;
	int yPos2 = screen->h;
	char text[STRMAX];
	int i;

	TextObject *credit;
	int nCredit;

	screen_clear(black);
	renderer_update();
	screen_clear(black);

	screen_drawBackground();

	audio_playMusic("music/rise_of_spirit.ogg", 1);

	fp = fopen("data/credits.txt", "rb");

	nCredit = 0;
	while (fscanf(fp, "%d %[^\n]%*c", &yPos, text) == 2)
	{
		nCredit++;
	}
	fseek(fp, 0, SEEK_SET);

	credit = malloc(nCredit * sizeof(*credit));
	if (credit == NULL)
	{
		engine_warn("Failed to allocate memory for credits");
		return;
	}

	lastCredit = -1;
	while (fscanf(fp, "%d %[^\n]%*c", &yPos, text) == 2)
	{
		lastCredit++;
		if (lastCredit < nCredit)
		{
			credit[lastCredit].image = gfx_createTextSurface(text, FONT_WHITE);
			credit[lastCredit].x = (screen->w - credit[lastCredit].image->w) / 2;
			yPos2 += yPos;
			credit[lastCredit].y = yPos2;
		}
		else
		{
			engine_warn("Credit size reached, but still scanning the file!");
			lastCredit--;
			break;
		}
	}

	fclose(fp);

	engine.done = ENGINE_RUNNING;

	engine.keyState[KEY_ESCAPE] = 0;
	engine.keyState[KEY_FIRE] = 0;
	engine.keyState[KEY_ALTFIRE] = 0;
	player_flushInput();

	while (1)
	{
		renderer_update();
		screen_unBuffer();

		player_getInput();
		if (engine.keyState[KEY_ESCAPE] || engine.keyState[KEY_FIRE]
				|| engine.keyState[KEY_ALTFIRE])
			break;
		
		// Check to see if we need to reposition credits
		if (credit[0].x != (screen->w - credit[0].image->w) / 2) {
			for (i=0; i< nCredit; i++) {
				credit[i].x = (screen->w - credit[i].image->w) / 2;
			}
		}

		float speed = 0.5;
		if (engine.keyState[KEY_DOWN])
			speed = 2;
		else if (engine.keyState[KEY_UP])
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

