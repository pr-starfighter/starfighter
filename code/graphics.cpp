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

Graphics graphics;
Star star[200];

SDL_Surface *loadImage(const char *filename)
{
	 SDL_Surface *image, *newImage;

	 #if USEPACK
	 unpack(filename, PAK_IMG);
	 image = IMG_Load_RW(engine.sdlrw, 1);
	 #else
	 image = IMG_Load(filename);
	 #endif

	 if (image == NULL) {
		  printf("Couldn't load %s: %s\n", filename, SDL_GetError());
		  showErrorAndExit(0, filename);
	 }

	 newImage = SDL_DisplayFormat(image);
	 if ( newImage ) {
	 	SDL_FreeSurface(image);
	 } else {
		 // This happens when we are loading the window icon image
		 newImage = image;
	 }

	 return graphics.setTransparent(newImage);
}

/*
Simply draws the stars in their positions on screen and moves
them around. They are wrapped around using the wrapFloat()
function, as defined above, and putpixel as defined in graphics.cpp
*/
void doStarfield()
{
	/* Lock the screen for direct access to the pixels */
	if (SDL_MUSTLOCK(graphics.screen))
	{
		if (SDL_LockSurface(graphics.screen) < 0 )
		{
			showErrorAndExit(2, "");
		 }
	}

	int color = 0;

	SDL_Rect r;

	for (int i = 0 ; i < 200 ; i++)
	{
		if (star[i].speed == 3)
			color = graphics.white;
		else if (star[i].speed == 2)
			color = graphics.lightGrey;
		else if (star[i].speed == 1)
			color = graphics.darkGrey;

		Math::wrapFloat(&(star[i].x += (engine.ssx * star[i].speed)), 0, 799);
		Math::wrapFloat(&(star[i].y += (engine.ssy * star[i].speed)), 0, 599);

		graphics.putpixel(graphics.screen, (int)star[i].x, (int)star[i].y, color);
		r.x = (int)star[i].x;
		r.y = (int)star[i].y;
		r.w = 1;
		r.h = 1;

		graphics.addBuffer(r.x, r.y, r.w, r.h);
	}

	if (SDL_MUSTLOCK(graphics.screen))
	{
		SDL_UnlockSurface(graphics.screen);
	}
}

int isOnScreen(int x, int y, int w, int h)
{
	if ((x + w > 0) && (x < 800) && (y + h > 0) && (y < 600))
		return 1;
		
	return 0;
}
