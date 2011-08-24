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

#include "version.h"
#include "Starfighter.h"

int main(int argc, char *argv[])
{
	defineGlobals(); // Must do this first!

	signed char cheatAttempt = 0;

	if (argc > 1)
	{
		if (strcmp("--help", argv[1]) == 0)
		{
			printf("\nProject: Starfighter %s\n", VERSION);
			printf("Copyright Parallel Realities 2003\n\n");
			printf("Additional Commands\n");
			printf("\t-noaudio     Disables sound and music\n");
			printf("\t-mono        Mono sound output (best for headphones)\n\n");
			printf("www.parallelrealities.co.uk\n");
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
			cheatAttempt = 1;
		if (strcmp(argv[i], "-noaudio") == 0)
			{printf("No Audio\n"); engine.useAudio = 0;}
		if (strcmp(argv[i], "-mono") == 0)
			{printf("Mono sound output\n"); engine.useAudio = 1;}
	}

	atexit(cleanUp);

	initSystem(); // Opens video mode and sound
	loadFont();

	if (cheatAttempt)
	{
		graphics.clearScreen(graphics.black);
		graphics.drawString("That doesn't work anymore", -1, 285, FONT_WHITE);
		graphics.drawString("Try harder...", -1, 315, FONT_WHITE);
		graphics.updateScreen();
		SDL_Delay(2000);
		graphics.clearScreen(graphics.black);
		graphics.updateScreen();
		SDL_Delay(500);
	}

	graphics.freeGraphics();
	loadSound();

	initWeapons();

	initVars();
	defineAliens();

	graphics.setColorIndexes();

	setAllyMessages();

	showStory();

	// Determine which part of the game we will go to...
	int section = 0;

	newGame();

	/*
	currentGame.system = 3;
	currentGame.area = 24;
	currentGame.shieldUnits = 4;
	currentGame.hasWingMate1 = 1;
	currentGame.hasWingMate2 = 1;

	player.shield = (currentGame.shieldUnits * 25);

	player.weaponType[1] = W_LASER;

	player.ammo[0] = 1;
	weapon[1].ammo[0] = 3;
	weapon[1].damage = 5;
	weapon[1].reload[0] = 7;

	engine.cheatAmmo = 1;
	engine.cheatShield = 1;
	engine.cheatCash = 1;
	updateSystemStatus();
	*/

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

	//doTitle();
	//galaxyMap();
	//mainGameLoop();
	//doCutscene(2);
	//doCredits();

	return(0);
}
