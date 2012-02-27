/*
Project: Starfighter
Copyright (C) 2003 Parallel Realities
All Rights Reserved

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

int main(int argc, char *argv[])
{
#if !USEPACK
	char datadir[] = PACKLOCATION;
	char *slash = strrchr(datadir, '/');
	if(slash)
		*slash = 0;
	chdir(datadir);
#endif

	defineGlobals(); // Must do this first!

	bool cheatAttempt = false;

	if (argc > 1)
	{
		if (strcmp("--help", argv[1]) == 0)
		{
			printf("\nProject: Starfighter %s\n", VERSION);
			printf("Copyright Parallel Realities 2003\n");
			printf("Copyright Guus Sliepen, Astrid S. de Wijn and others 2012\n");
			printf("Additional Commands\n");
			printf("\t-noaudio     Disables sound and music\n");
			printf("\t-mono        Mono sound output (best for headphones)\n\n");
			printf("http://sourceforge.net/projects/pr-starfighter/\n");
			printf("\n");
			exit(0);
		}
	}

	for (int i = 1 ; i < argc ; i++)
	{
		#if USEPACK
		#else
		if (strcmp(argv[i], "-nomove") == 0)
			{printf("Enemy movement disabled\n"); dev.moveAliens = 0;}
		if (strcmp(argv[i], "-nofire") == 0)
			{printf("Enemy firing disabled\n"); dev.fireAliens = 0;}
		#endif
		if (strcmp(argv[i], "-cheat") == 0)
			cheatAttempt = true;
		if (strcmp(argv[i], "-noaudio") == 0)
			{printf("No Audio\n"); engine.useAudio = false;}
		if (strcmp(argv[i], "-mono") == 0)
			{printf("Mono sound output\n"); engine.useAudio = true;}
	}

	atexit(cleanUp);

	initGraphics();
	initSystem(); // Opens video mode and sound
	loadFont();

	if (cheatAttempt)
	{
		clearScreen(black);
		drawString("That doesn't work anymore", -1, 285, FONT_WHITE);
		drawString("Try harder...", -1, 315, FONT_WHITE);
		updateScreen();
		SDL_Delay(2000);
		clearScreen(black);
		updateScreen();
		SDL_Delay(500);
	}

	freeGraphics();
	loadSound();

	initWeapons();

	initVars();
	defineAliens();

	setColorIndexes();

	setAllyMessages();

	showStory();

	// Determine which part of the game we will go to...
	int section = 0;

	newGame();

	while (true)
	{
		switch(section)
		{
			case 0:
				section = doTitle();
				break;

			case 1:
				section = galaxyMap();
				break;

			case 2:
				if (currentGame.stationedPlanet == -1) {doCutscene(0);}
				section = mainGameLoop();
				break;
		}
	}

	return(0);
}
