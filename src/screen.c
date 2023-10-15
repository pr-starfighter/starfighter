/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012, 2013 Guus Sliepen
Copyright (C) 2015-2023 Diligent Circle <diligentcircle@riseup.net>

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

#include "defs.h"
#include "structs.h"

#include "engine.h"
#include "gfx.h"

SDL_Surface *screen;
LinkedRect *screen_bufferHead;
LinkedRect *screen_bufferTail;

void screen_blit(SDL_Surface *image, int x, int y)
{
	gfx_blit(image, x, y, screen);
}

void screen_blitText(int i, int x, int y)
{
	if (x == -1)
		x = (screen->w - gfx_textSprites[i].image->w) / 2;

	screen_blit(gfx_textSprites[i].image,
		(x + (int)gfx_textSprites[i].x), (y +(int)gfx_textSprites[i].y));
}

int screen_renderString(const char *in, int x, int y, int fontColor)
{
	return gfx_renderString(in, x, y, fontColor, 0, screen);
}

int screen_renderUnicode(const char *in, int x, int y, int fontColor)
{
	return gfx_renderUnicode(in, x, y, fontColor, 0, screen);
}

/*
Draws the background surface that has been loaded
*/
void screen_drawBackground()
{
	if (gfx_background != NULL)
		screen_blit(gfx_background, 0, 0);
}

void screen_addBuffer(int x, int y, int w, int h)
{
	LinkedRect *rect;

	rect = malloc(sizeof(*rect));
	if (rect == NULL)
	{
		engine_warn("Failed to allocate memor for screen buffer");
		return;
	}

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
	LinkedRect *prevRect = screen_bufferHead;
	LinkedRect *rect = screen_bufferHead;
	screen_bufferTail = screen_bufferHead;

	while (rect->next != NULL)
	{
		rect = rect->next;

		prevRect->next = rect->next;
		free(rect);
		rect = prevRect;
	}

	screen_bufferHead->next = NULL;
}

void screen_unBuffer()
{
	LinkedRect *prevRect = screen_bufferHead;
	LinkedRect *rect = screen_bufferHead;
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
			engine_showError(2, "");
		}

		prevRect->next = rect->next;
		free(rect);
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

/*
Adjust the dimensions of the screen to match the aspect ratio of the
window, using DEFAULT_SCREEN_WIDTH and DEFAULT_SCREEN_HEIGHT as a
baseline.
*/
void screen_adjustDimensions(int w, int h)
{
	double default_ratio = (double)DEFAULT_SCREEN_WIDTH / (double)DEFAULT_SCREEN_HEIGHT;
	double new_ratio = (double)w / (double)h;

	// Calculate dimensions
	if (new_ratio > default_ratio)
	{
		// Wide screen
		w = (DEFAULT_SCREEN_HEIGHT * w) / h;
		h = DEFAULT_SCREEN_HEIGHT;
	}
	else
	{
		// Tall screen
		h = (DEFAULT_SCREEN_WIDTH * h) / w;
		w = DEFAULT_SCREEN_WIDTH;
	}

	// Free previous surface (if it exists)
	if (screen != NULL)
	{
		SDL_FreeSurface(screen);
		screen = NULL;
	}

	// Create the surface
	screen = SDL_CreateRGBSurface(0, w, h, 32, 0xff0000, 0xff00, 0xff, 0xff000000);
	if (screen == NULL)
	{
		printf("Couldn't create %ix%ix32 surface: %s\n", w, h, SDL_GetError());
		exit(1);
	}
}
