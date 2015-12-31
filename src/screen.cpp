/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012, 2013 Guus Sliepen
Copyright (C) 2015 Julian Marchant

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

#include "SDL.h"

#include "gfx.h"
#include "init.h"
#include "structs.h"

SDL_Surface *screen;

void screen_blit(SDL_Surface *image, int x, int y)
{
	gfx_blit(image, x, y, screen);
}

void screen_blitText(int i)
{
	screen_blit(gfx_text[i].image, (int)gfx_text[i].x, (int)gfx_text[i].y);
}

int screen_renderString(const char *in, int x, int y, int fontColor)
{
	return gfx_renderString(in, x, y, fontColor, 0, screen);
}

/*
Draws the background surface that has been loaded
*/
void screen_drawBackground()
{
	screen_blit(gfx_background, 0, 0);
}

void screen_addBuffer(int x, int y, int w, int h)
{
	bRect *rect = new bRect;

	rect->next = NULL;
	rect->x = x;
	rect->y = y;
	rect->w = w;
	rect->h = h;

	screen_bufferTail->next = rect;
	screen_bufferTail = rect;
}

void screen_flushBuffer()
{
	bRect *prevRect = screen_bufferHead;
	bRect *rect = screen_bufferHead;
	screen_bufferTail = screen_bufferHead;

	while (rect->next != NULL)
	{
		rect = rect->next;

		prevRect->next = rect->next;
		delete rect;
		rect = prevRect;
	}

	screen_bufferHead->next = NULL;
}

void screen_unBuffer()
{
	bRect *prevRect = screen_bufferHead;
	bRect *rect = screen_bufferHead;
	screen_bufferTail = screen_bufferHead;

	while (rect->next != NULL)
	{
		rect = rect->next;

		SDL_Rect blitRect;

		blitRect.x = rect->x;
		blitRect.y = rect->y;
		blitRect.w = rect->w;
		blitRect.h = rect->h;

		if (SDL_BlitSurface(gfx_background, &blitRect, screen, &blitRect) < 0)
		{
			printf("BlitSurface error: %s\n", SDL_GetError());
			showErrorAndExit(2, "");
		}

		prevRect->next = rect->next;
		delete rect;
		rect = prevRect;
	}

	screen_bufferHead->next = NULL;
}

void screen_clear(Uint32 color)
{
	SDL_FillRect(screen, NULL, color);
}

void screen_drawRect(int x, int y, int w, int h, Uint8 red, Uint8 green, Uint8 blue)
{
	gfx_drawRect(screen, x, y, w, h, red, green, blue);
}
