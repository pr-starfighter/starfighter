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

#include "resources.h"

void loadBackground(char *filename)
{
	if (graphics.background != NULL)
	{
		SDL_FreeSurface(graphics.background);
		graphics.background = NULL;
	}
	graphics.background = loadImage(filename);
	SDL_SetColorKey(graphics.background, 0, 0);
}

void loadGameGraphics()
{
	int index;
	char string[75];

	graphics.freeGraphics();

	graphics.shipShape[0] = loadImage("gfx/firefly1.png");
	graphics.shipShape[1] = loadImage("gfx/firefly2.png");

	strcpy(string, "");
	switch(currentGame.system)
	{
		case 0:
			strcpy(string, "data/resources_spirit.dat");
			break;
		case 1:
			strcpy(string, "data/resources_eyananth.dat");
			break;
		case 2:
			strcpy(string, "data/resources_mordor.dat");
			break;
		case 3:
			strcpy(string, "data/resources_sol.dat");
			break;
	}

	FILE *fp;

	#if USEPACK
	int dataLocation = locateDataInPak(string, 1);
	fp = fopen(PACKLOCATION, "rb");
	fseek(fp, dataLocation, SEEK_SET);
	#else
	fp = fopen(string, "rb");
	#endif

	if (fp == NULL)
		exit(1);

	fscanf(fp, "%d %s", &index, string);
	while (index != -1)
	{
		graphics.shipShape[index] = loadImage(string);
		fscanf(fp, "%d %s", &index, string);
	}

	fclose(fp);

	/*
	Overlay a red alpha surface onto
	*/
	SDL_Surface *hitRect;
	for (int i = SHIP_HIT_INDEX ; i < MAX_SHIPSHAPES ; i++)
	{
		if (graphics.shipShape[i - SHIP_HIT_INDEX] == NULL)
			continue;
		graphics.shipShape[i] = graphics.createSurface(graphics.shipShape[i - SHIP_HIT_INDEX]->w, graphics.shipShape[i- SHIP_HIT_INDEX]->h);
		graphics.blit(graphics.shipShape[i - SHIP_HIT_INDEX], 0, 0, graphics.shipShape[i]);
		hitRect = graphics.alphaRect(graphics.shipShape[i]->w, graphics.shipShape[i]->h, 255, 0, 0);
		graphics.blit(hitRect, 0, 0, graphics.shipShape[i]);
		SDL_SetColorKey(graphics.shipShape[i], (SDL_SRCCOLORKEY|SDL_RLEACCEL), SDL_MapRGB(graphics.shipShape[i]->format, 127, 0, 0));
		SDL_FreeSurface(hitRect);
	}

	strcpy(string, "data/resources_all.dat");

	#if USEPACK
	dataLocation = locateDataInPak(string, 1);
	fp = fopen(PACKLOCATION, "rb");
	fseek(fp, dataLocation, SEEK_SET);
	#else
	fp = fopen(string, "rb");
	#endif

	fscanf(fp, "%d %s", &index, string);
	while (index != -1)
	{
		graphics.shape[index] = loadImage(string);
		fscanf(fp, "%d %s", &index, string);
	}

	fclose(fp);

	loadBackground((char *)systemBackground[currentGame.system]);

	setAlienShapes();

	setWeaponShapes();
}

void loadSound()
{
	sound[SFX_EXPLOSION] = loadSound("sound/explode.wav");
	sound[SFX_HIT] = loadSound("sound/explode2.wav");
	sound[SFX_DEATH] = loadSound("sound/maledeath.wav");
	sound[SFX_MISSILE] = loadSound("sound/missile.wav");
	sound[SFX_PLASMA] = loadSound("sound/plasma.wav");
	sound[SFX_CLOCK] = loadSound("sound/clock.wav");
	sound[SFX_FLY] = loadSound("sound/flyby.wav");
	sound[SFX_ENERGYRAY] = loadSound("sound/beamLaser.wav");
	sound[SFX_PICKUP] = loadSound("sound/item.wav");
	sound[SFX_SHIELDUP] = loadSound("sound/shield.wav");
	sound[SFX_CLOAK] = loadSound("sound/cloak.wav");
	sound[SFX_DEBRIS] = loadSound("sound/explode3.wav");
	sound[SFX_DEBRIS2] = loadSound("sound/explode4.wav");
	sound[SFX_LASER] = loadSound("sound/laser.wav");
	sound[SFX_PLASMA2] = loadSound("sound/plasma2.wav");
	sound[SFX_PLASMA3] = loadSound("sound/plasma3.wav");
}

void freeSound()
{
	for (int i = 0 ; i < MAX_SOUNDS ; i++)
	{
		if (sound[i] != NULL)
			Mix_FreeChunk(sound[i]);
	}

	if (engine.music != NULL)
		Mix_FreeMusic(engine.music);
}


void setFontColor(SDL_Surface *image, int red, int green, int blue)
{
	SDL_Color colors[256];
	colors[0].r = 0;
	colors[0].g = 0;
	colors[0].b = 0;
	for (int i = 1 ; i < 256 ; i++)
	{
		colors[i].r = red;
		colors[i].g = green;
		colors[i].b = blue;
	}

	SDL_SetPalette(image, SDL_LOGPAL|SDL_PHYSPAL, colors, 0, 256);
}

/*
Custom loading to alter the font color before doing
all other things
*/
void loadFont()
{
	SDL_Surface *image, *newImage;

	for (int i = 0 ; i < MAX_FONTSHAPES ; i++)
	{
		#if USEPACK
		unpack("gfx/smallFont.bmp", PAK_FONT);
		image = IMG_Load_RW(engine.sdlrw, 1);
		#else
		image = IMG_Load("gfx/smallFont.bmp");
		#endif

		if (image == NULL) {
			printf("Couldn't load game font! (%s) Exitting.\n", SDL_GetError());
		   exit(1);
		}

		switch(i)
		{
			case 1:
				setFontColor(image, 255, 0, 0);
				break;
			case 2:
				setFontColor(image, 255, 255, 0);
				break;
			case 3:
				setFontColor(image, 0, 255, 0);
				break;
			case 4:
				setFontColor(image, 0, 255, 255);
				break;
			case 5:
				setFontColor(image, 0, 0, 10);
				break;
		}

		newImage = SDL_DisplayFormat(image);

    	graphics.fontShape[i] = graphics.setTransparent(newImage);

		SDL_FreeSurface(image);
 	}
}
