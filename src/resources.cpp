/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012, 2013 Guus Sliepen
Copyright (C) 2015, 2016 onpon4 <onpon4@riseup.net>

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

void loadBackground(const char *filename)
{
	if (gfx_background != NULL)
	{
		SDL_FreeSurface(gfx_background);
		gfx_background = NULL;
	}
	gfx_background = gfx_loadImage(filename);
	SDL_SetColorKey(gfx_background, 0, 0);
}

void loadGameGraphics()
{
	int index;
	char string[75] = "";
	FILE *fp;
	Uint32 *p32;
	Uint16 *p16;
	Uint8 *p8;

	gfx_free();

	gfx_shipSprites[0] = gfx_loadImage("gfx/firefly1.png");
	gfx_shipSprites[1] = gfx_loadImage("gfx/firefly2.png");

	switch(game.system)
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

	fp = fopen(string, "rb");

	if (fp == NULL)
		exit(1);

	while (fscanf(fp, "%d %s", &index, string) == 2)
	{
		gfx_shipSprites[index] = gfx_loadImage(string);
	}

	fclose(fp);

	/*
	Create images of ships being hit that show a lot of red
	*/
	for (int i = SHIP_HIT_INDEX ; i < MAX_SHIPSPRITES ; i++)
	{
		if (gfx_shipSprites[i - SHIP_HIT_INDEX] == NULL)
			continue;
		gfx_shipSprites[i] = gfx_createSurface(gfx_shipSprites[i - SHIP_HIT_INDEX]->w,
			gfx_shipSprites[i - SHIP_HIT_INDEX]->h);
		SDL_SetSurfaceBlendMode(gfx_shipSprites[i - SHIP_HIT_INDEX], SDL_BLENDMODE_NONE);
		gfx_blit(gfx_shipSprites[i - SHIP_HIT_INDEX], 0, 0, gfx_shipSprites[i]);
		SDL_SetSurfaceBlendMode(gfx_shipSprites[i - SHIP_HIT_INDEX], SDL_BLENDMODE_BLEND);

		switch (gfx_shipSprites[i]->format->BitsPerPixel)
		{
			case 32:
				SDL_LockSurface(gfx_shipSprites[i]);
				p32 = (Uint32 *)gfx_shipSprites[i]->pixels;
				for (int j = 0; j < gfx_shipSprites[i]->w * gfx_shipSprites[i]->h; j++)
				{
					if (p32[j])
						p32[j] |= gfx_shipSprites[i]->format->Rmask;
				}
				SDL_UnlockSurface(gfx_shipSprites[i]);
				break;

			case 16:
				SDL_LockSurface(gfx_shipSprites[i]);
				p16 = (Uint16 *)gfx_shipSprites[i]->pixels;
				for (int j = 0; j < gfx_shipSprites[i]->w * gfx_shipSprites[i]->h; j++)
				{
					if (p16[j])
						p16[j] |= gfx_shipSprites[i]->format->Rmask;
				}
				SDL_UnlockSurface(gfx_shipSprites[i]);
				break;

			case 8:
				SDL_LockSurface(gfx_shipSprites[i]);
				p8 = (Uint8 *)gfx_shipSprites[i]->pixels;
				for (int j = 0; j < gfx_shipSprites[i]->w * gfx_shipSprites[i]->h; j++)
				{
					if (p8[j])
						p8[j] = SDL_MapRGB(gfx_shipSprites[i]->format, 255, 0, 0);
				}
				SDL_UnlockSurface(gfx_shipSprites[i]);
				break;
		}

		SDL_SetColorKey(gfx_shipSprites[i], SDL_TRUE,
			SDL_MapRGB(gfx_shipSprites[i]->format, 0, 0, 0));
	}

	strcpy(string, "data/resources_all.dat");

	fp = fopen(string, "rb");

	while (fscanf(fp, "%d %s", &index, string) == 2)
	{
		gfx_sprites[index] = gfx_loadImage(string);
	}

	fclose(fp);

	loadBackground(systemBackground[game.system]);

	for (int i = 0 ; i < CD_MAX ; i++)
	{
		if (gfx_shipSprites[alien_defs[i].imageIndex[0]] != NULL)
		{
			alien_defs[i].image[0] = gfx_shipSprites[alien_defs[i].imageIndex[0]];
			alien_defs[i].image[1] = gfx_shipSprites[alien_defs[i].imageIndex[1]];
			alien_defs[i].engineX = alien_defs[i].image[0]->w;
			alien_defs[i].engineY = (alien_defs[i].image[0]->h / 2);
		}
	}

	setWeaponShapes();
}


/*
Custom loading to alter the font color before doing
all other things
*/
void loadFont()
{
	SDL_Surface *image, *newImage;

	for (int i = 0 ; i < MAX_FONTSPRITES ; i++)
	{
		image = IMG_Load("gfx/smallFont.png");

		if (image == NULL) {
			printf("Couldn't load game font! (%s) Exitting.\n", SDL_GetError());
		   exit(1);
		}

		switch(i)
		{
			case 1:
				SDL_SetSurfaceColorMod(image, 255, 0, 0);
				break;
			case 2:
				SDL_SetSurfaceColorMod(image, 255, 255, 0);
				break;
			case 3:
				SDL_SetSurfaceColorMod(image, 0, 255, 0);
				break;
			case 4:
				SDL_SetSurfaceColorMod(image, 0, 255, 255);
				break;
			case 5:
				SDL_SetSurfaceColorMod(image, 0, 0, 10);
				break;
		}

		newImage = SDL_ConvertSurface(image, screen->format, 0);

		gfx_fontSprites[i] = gfx_setTransparent(newImage);

		SDL_FreeSurface(image);
 	}
}
