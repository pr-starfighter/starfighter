/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012, 2013 Guus Sliepen
Copyright (C) 2015-2020 Layla Marchant <diligentcircle@riseup.net>

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

#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef __HAIKU__
#include <FindDirectory.h>
#elif !defined(_WIN32)
#include <pwd.h>
#endif

#include "SDL.h"

#ifndef NOFONT
#include "SDL_ttf.h"
#endif

#ifndef NOSOUND
#include "SDL_mixer.h"
#endif

#include "colors.h"
#include "defs.h"
#include "structs.h"

#include "audio.h"
#include "collectable.h"
#include "engine.h"
#include "game.h"
#include "gfx.h"
#include "player.h"
#include "renderer.h"
#include "screen.h"
#include "window.h"

Engine engine;

void engine_init()
{
	engine.musicVolume = 100;
	engine.useAudio = 1;

	engine.maxAliens = 9;

	engine.ssx = 0;
	engine.ssy = 0;
	engine.smx = 0;
	engine.smy = 0;

	engine.bulletHead = malloc(sizeof(*engine.bulletHead));
	if (engine.bulletHead == NULL)
	{
		engine_error("Failed to allocate memory for bullet head.");
	}
	engine.bulletHead->next = NULL;
	engine.bulletTail = engine.bulletHead;

	engine.explosionHead = malloc(sizeof(*engine.explosionHead));
	if (engine.explosionHead == NULL)
	{
		engine_error("Failed to allocate memory for explosion head.");
	}
	engine.explosionHead->next = NULL;
	engine.explosionTail = engine.explosionHead;

	engine.collectableHead = malloc(sizeof(*engine.collectableHead));
	if (engine.collectableHead == NULL)
	{
		engine_error("Failed to allocate memory for collectable head.");
	}
	engine.collectableHead->next = NULL;
	engine.collectableTail = engine.collectableHead;

	engine.debrisHead = malloc(sizeof(*engine.debrisHead));
	if (engine.debrisHead == NULL)
	{
		engine_error("Failed to allocate memory for debris head.");
	}
	engine.debrisHead->next = NULL;
	engine.debrisTail = engine.debrisHead;

	engine.commsSection = 0;

	for (int i = 0; i < KEY_LAST; i++)
		engine.keyState[i] = 0;

	engine.xaxis = 0;
	engine.yaxis = 0;

	engine.eventTimer = 0;
	engine.counter2 = 0;
	engine.timeTaken = 0;
	engine.timeMission = 0;
	engine.counter = 0;
	engine.seconds = 0;
	engine.minutes = 0;
	engine.paused = 0;
	engine.gameSection = SECTION_TITLE;

	engine.cheat = 0;
	engine.cheatShield = 0;
	engine.cheatAmmo = 0;
	engine.cheatCash = 0;
}

/*
Something went wrong. This stops the game, present the error message and
prompts the user to press space or ctrl to exit the game. This is unlikely to
be seen by people unless something really stoopid happens!
*/
void engine_showError(int errorId, const char *name)
{
	screen_clear(black);

	if (errorId != 2)
	{
		screen_renderString("A file error has occurred", -1, 200, FONT_RED);
	}
	else
	{
		printf("Couldn't create or write to directory '%s'\n", name);
		exit(1);
	}

	char string[STRMAX];

	switch(errorId)
	{
		case 0:
			snprintf(string, STRMAX, "%s was not found in the Starfighter data package", name);
			screen_renderString(string, -1, 250, FONT_WHITE);
			screen_renderString("Please try again. If this error persists, contact the authors", -1, 275, FONT_WHITE);
			screen_renderString("or reinstall the game", -1, 300, FONT_WHITE);
			break;
		case 1:
			screen_renderString("Project: Starfighter encountered an error whilst", -1, 250, FONT_WHITE);
			screen_renderString("attempting to load game data. Please try running", -1, 275, FONT_WHITE);
			screen_renderString("the game again. If the errors persist, reinstall the game", -1, 300, FONT_WHITE);
			break;
		case 2:
			screen_renderString("Project: Starfighter encountered a critical error", -1, 250, FONT_WHITE);
			screen_renderString("while attempting to perform a required program function.", -1, 275, FONT_WHITE);
			screen_renderString("Please contact the authors with details.", -1, 300, FONT_WHITE);
			break;
	}

	screen_renderString("Project: Starfighter will now exit", -1, 450, FONT_WHITE);
	screen_renderString("Press Space to continue", -1, 475, FONT_WHITE);

	renderer_update();

	engine.keyState[KEY_ALTFIRE] = 0;

	while (!engine.keyState[KEY_ALTFIRE])
	{
		player_getInput();
		game_delayFrame();
	}

	exit(1);
}

/*
Show a warning. Used when non-fatal things go wrong.
*/
void engine_warn(const char *msg)
{
	printf("WARNING: %s\n", msg);
}

/*
Show an error and exit. Used for critical errors that should definitely
never happen.
*/
void engine_error(const char *msg)
{
	printf("ERROR: %s\nAborting\n", msg);
	exit(1);
}

/*
This gets the user's home directory, then creates the config directory.
*/
void engine_setupConfigDirectory()
{
	const char *userHome;
	const char *subdir;
	char dir[PATH_MAX];

#ifdef _WIN32
	subdir = "pr-starfighter-config";

	if ((userHome = getenv("APPDATA")) == NULL)
		userHome = ".";

	snprintf(dir, PATH_MAX, "%s/%s", userHome, subdir);
	if ((mkdir(dir) != 0) && (errno != EEXIST))
		engine_showError(2, dir);

	snprintf(engine.configDirectory, PATH_MAX, "%s/", dir);
#elif defined(__HAIKU__)
	subdir = "starfighter";

	char path[PATH_MAX];
	if (find_directory(B_USER_SETTINGS_DIRECTORY, 0, false, path, PATH_MAX) == B_OK)
		snprintf(dir, PATH_MAX, "%s/%s", path, subdir);

	if ((mkdir(dir, S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH) != 0) && (errno != EEXIST))
		engine_showError(2, dir);

	snprintf(engine.configDirectory, PATH_MAX, "%s/", dir);
#else
	subdir = "starfighter";

	if ((userHome = getenv("XDG_CONFIG_HOME")) != NULL)
	{
		snprintf(dir, PATH_MAX, "%s/%s", userHome, subdir);
	}
	{
		if ((userHome = getenv("HOME")) == NULL)
			userHome = getpwuid(getuid())->pw_dir;

		snprintf(dir, PATH_MAX, "%s/.config", userHome);
		if ((mkdir(dir, S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH) != 0) && (errno != EEXIST))
			engine_showError(2, dir);

		snprintf(dir, PATH_MAX, "%s/.config/%s", userHome, subdir);
	}

	if ((mkdir(dir, S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH) != 0) && (errno != EEXIST))
		engine_showError(2, dir);
	
	snprintf(engine.configDirectory, PATH_MAX, "%s/", dir);
#endif
}

/*
This sets up the video and sound system, and creates Starfighter's window.
*/
void engine_setMode()
{
	char filename[PATH_MAX];
	int fullScreen = 0;
	int useSound = 1;
	int useMusic = 1;
	int autoPause = 0;
	int radioLife = DEFAULT_RADIO_LIFE;
	char lang[STRMAX_SHORT];
	int i;

	strcpy(lang, "default");

	strcpy(engine.configDirectory, "");

	engine_setupConfigDirectory();

	/* Initialize the SDL library */
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_JOYSTICK|SDL_INIT_GAMECONTROLLER) < 0)
	{
		printf("Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	FILE *fp;
	snprintf(filename, PATH_MAX, "%sconf", engine.configDirectory);
	fp = fopen(filename, "r");

	if (fp != NULL)
	{
		if (fscanf(fp, "%d %d %d %d %d%*c%[^\n]",
				&fullScreen, &useSound, &useMusic, &autoPause,
				&radioLife, lang) < 6)
			printf("Warning: Config file \"%s\" is not correctly formatted\n", filename);
		fclose(fp);
	}

	engine.fullScreen = fullScreen;
	engine.useSound = useSound;
	engine.useMusic = useMusic;
	engine.autoPause = autoPause;
	engine.radioLife = radioLife;
	strcpy(engine.lang, lang);

	screen_adjustDimensions(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);

	window = SDL_CreateWindow("Project: Starfighter",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		screen->w, screen->h, SDL_WINDOW_RESIZABLE);
	if (window == NULL)
	{
		printf("Could not create window: %s\n", SDL_GetError());
		exit(1);
	}

	SDL_SetWindowIcon(window, gfx_loadImage("gfx/alienDevice.png"));
	SDL_SetWindowFullscreen(window, engine.fullScreen ? FULLSCREEN : 0);

	renderer_reset();

#ifndef NOSOUND
	if (engine.useAudio)
	{
		if (Mix_OpenAudio(44100, AUDIO_S16, 2, 1024) < 0)
		{
			printf("Warning: Couldn't set 44100 Hz 16-bit stereo audio - Reason:\n%s\n", Mix_GetError());
			printf("Sound and Music will be disabled\n");
			engine.useAudio = 0;
		}
	}
#endif

	SDL_ShowCursor(SDL_DISABLE);
	SDL_EventState(SDL_MOUSEMOTION, SDL_DISABLE);

	// Determine if the GameController API can be used
	engine.useController = 1;
	for (i=0; i<SDL_NumJoysticks(); i++) {
		if (!SDL_IsGameController(i)) {
			engine.useController = 0;
			break;
		}
	}
	
	if (engine.useController) {
		SDL_GameControllerEventState(SDL_ENABLE);
	}
	else {
		SDL_JoystickEventState(SDL_ENABLE);
	}
	
	// Open controllers
	for (i=0; i<SDL_NumJoysticks(); i++) {
		if (engine.useController)
			SDL_GameControllerOpen(i);
		else
			SDL_JoystickOpen(i);
	}
}

void engine_setFullscreen(int value)
{
	engine.fullScreen = value;

	// Clear the screen (prevents artifacts)
	screen_clear(black);
	renderer_update();
	screen_clear(black);
	screen_addBuffer(0, 0, screen->w, screen->h);

	SDL_SetWindowFullscreen(window, engine.fullScreen ? FULLSCREEN : 0);
}

void engine_resetLists()
{
	Object *ob, *ob2;
	Collectable *c1, *c2;
	LinkedRect *r1, *r2;

	ob = engine.bulletHead->next;
	while(ob != NULL)
	{
		ob2 = ob;
		ob = ob->next;
		free(ob2);
	}
	engine.bulletHead->next = NULL;
	engine.bulletTail = engine.bulletHead;

	ob = engine.explosionHead->next;
	while(ob != NULL)
	{
		ob2 = ob;
		ob = ob->next;
		free(ob2);
	}
	engine.explosionHead->next = NULL;
	engine.explosionTail = engine.explosionHead;

	c1 = engine.collectableHead->next;
	while (c1 != NULL)
	{
		c2 = c1;
		c1 = c1->next;
		free(c2);
	}

	engine.collectableHead->next = NULL;
	engine.collectableTail = engine.collectableHead;

	r1 = screen_bufferHead->next;
	while (r1 != NULL)
	{
		r2 = r1;
		r1 = r1->next;
		free(r2);
	}

	screen_bufferHead->next = NULL;
	screen_bufferTail = screen_bufferHead;

	ob = engine.debrisHead->next;
	while(ob != NULL)
	{
		ob2 = ob;
		ob = ob->next;
		free(ob2);
	}
	engine.debrisHead->next = NULL;
	engine.debrisTail = engine.debrisHead;
}

/*
Removes [hopefully] all the resources that has been
loaded and created during the game. This is called by
atexit();
*/
void engine_cleanup()
{
	int i;

	gfx_free();
	SDL_FreeSurface(gfx_background);
	SDL_FreeSurface(gfx_unscaledBackground);
	audio_free();
	engine_resetLists();
	free(engine.bulletHead);
	free(engine.explosionHead);
	free(engine.collectableHead);
	free(screen_bufferHead);

	for (i=0 ; i<FONT_MAX ; i++)
	{
		if (gfx_fontSprites[i] != NULL)
			SDL_FreeSurface(gfx_fontSprites[i]);
	}

	char filename[PATH_MAX];
	strcpy(filename, "");

#ifndef NOFONT
	if (gfx_unicodeFont != NULL)
	{
		TTF_CloseFont(gfx_unicodeFont);
		gfx_unicodeFont = NULL;
	}
	if (TTF_WasInit())
	{
		TTF_Quit();
	}
#endif

#ifndef NOSOUND
	if (engine.useAudio)
	{
		Mix_CloseAudio();
	}
#endif

	// Save the config using current settings
	FILE *fp;
	snprintf(filename, PATH_MAX, "%sconf", engine.configDirectory);
	fp = fopen(filename, "w");
	if (fp != NULL)
	{
		fprintf(fp,
			"%d %d %d %d %d\n"
			"%s\n",
			engine.fullScreen, engine.useSound, engine.useMusic,
			engine.autoPause, engine.radioLife, engine.lang);
		fclose(fp);
	}
	else
	{
		printf("Error saving config\n");
	}

	SDL_Quit();
	printf("Thank You for playing Starfighter\n");
}
