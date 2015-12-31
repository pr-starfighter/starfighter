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

#include <ctype.h>

#include "Starfighter.h"

SDL_Surface *gfx_background;
SDL_Surface *shape[MAX_SHAPES];
SDL_Surface *shipShape[MAX_SHIPSHAPES];
SDL_Surface *fontShape[MAX_FONTSHAPES];
SDL_Surface *shopSurface[MAX_SHOPSHAPES];
bRect *screen_bufferHead;
bRect *screen_bufferTail;
textObject gfx_text[MAX_TEXTSHAPES];
SDL_Surface *messageBox;

void gfx_init()
{
	screen_bufferHead = new bRect;
	screen_bufferHead->next = NULL;
	screen_bufferTail = screen_bufferHead;

	for (int i = 0 ; i < MAX_SHAPES ; i++)
		shape[i] = NULL;

	for (int i = 0 ; i < MAX_SHIPSHAPES ; i++)
		shipShape[i] = NULL;

	for (int i = 0 ; i < MAX_TEXTSHAPES ; i++)
		gfx_text[i].image = NULL;

	for (int i = 0 ; i < MAX_SHOPSHAPES ; i++)
		shopSurface[i] = NULL;

	for (int i = 0 ; i < MAX_FONTSHAPES ; i++)
		fontShape[i] = NULL;

	gfx_background = NULL;
	messageBox = NULL;

	screen = NULL;
}

SDL_Surface *gfx_setTransparent(SDL_Surface *sprite)
{
	SDL_SetColorKey(sprite, SDL_TRUE, SDL_MapRGB(sprite->format, 0, 0, 0));
	return sprite;
}

void gfx_blit(SDL_Surface *image, int x, int y, SDL_Surface *dest)
{
	// Exit early if image is not on dest at all
	if (x + image->w < 0 || x >= dest->w || y + image->h < 0 || y >= dest->h)
		return;

	// Set up a rectangle to draw to
	SDL_Rect blitRect;

	blitRect.x = x;
	blitRect.y = y;
	blitRect.w = image->w;
	blitRect.h = image->h;

	/* Blit onto the destination surface */
	if (SDL_BlitSurface(image, NULL, dest, &blitRect) < 0)
	{
		printf("BlitSurface error: %s\n", SDL_GetError());
		showErrorAndExit(2, "");
	}

	// Only if it is to the screen, mark the region as damaged
	if (dest == screen)
		screen_addBuffer(blitRect.x, blitRect.y, blitRect.w, blitRect.h);
}

/*
In 16 bit mode this is slow. VERY slow. Don't write directly to a surface
that constantly needs updating (eg - the main game screen)
*/
static int gfx_renderStringBase(const char *in, int x, int y, int fontColor, signed char wrap, SDL_Surface *dest)
{
	int i;
	int splitword;
	SDL_Rect area;
	SDL_Rect letter;

	area.x = x;
	area.y = y;
	area.w = 8;
	area.h = 14;

	letter.y = 0;
	letter.w = 8;
	letter.h = 14;

	while (*in != '\0')
	{
		if (*in != ' ')
		{
			letter.x = (*in - 33);
			letter.x *= 8;

			/* Blit onto the screen surface */
			if (SDL_BlitSurface(fontShape[fontColor], &letter, dest, &area) < 0)
			{
				printf("BlitSurface error: %s\n", SDL_GetError());
				showErrorAndExit(2, "");
			}
		}

		area.x += 9;

		if (wrap)
		{
			if ((area.x > (dest->w - 70)) && (*in == ' '))
			{
				area.y += 16;
				area.x = x;
			}
			else if (area.x > (dest->w - 31))
			{
				splitword = 1;
				for (i = 0 ; i < 4 ; i++)
				{
					if (!isalpha(*(in + i)))
					{
						splitword = 0;
						break;
					}
				}

				if (splitword)
				{
					letter.x = (int)('-') - 33;
					letter.x *= 8;
					if (SDL_BlitSurface(fontShape[fontColor], &letter, dest, &area) < 0)
					{
						printf("BlitSurface error: %s\n", SDL_GetError());
						showErrorAndExit(2, "");
					}
					area.y += 16;
					area.x = x;
				}
			}
		}

		in++;
	}

	return area.y;
}

int gfx_renderString(const char *in, int x, int y, int fontColor, int wrap, SDL_Surface *dest)
{
	if (x == -1)
		x = (dest->w - (strlen(in) * 9)) / 2;

	gfx_renderStringBase(in, x, y - 1, FONT_OUTLINE, wrap, dest);
	gfx_renderStringBase(in, x, y + 1, FONT_OUTLINE, wrap, dest);
	gfx_renderStringBase(in, x, y + 2, FONT_OUTLINE, wrap, dest);
	gfx_renderStringBase(in, x - 1, y, FONT_OUTLINE, wrap, dest);
	gfx_renderStringBase(in, x - 2, y, FONT_OUTLINE, wrap, dest);
	gfx_renderStringBase(in, x + 1, y, FONT_OUTLINE, wrap, dest);
	return gfx_renderStringBase(in, x, y, fontColor, wrap, dest);
}

/*
 * Set the pixel at (x, y) to the given value
 * NOTE: The surface must be locked before calling this!
 */
void gfx_putPixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to set */
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	switch(bpp)
	{
		case 1:
			*p = pixel;
			break;

		case 2:
			*(Uint16 *)p = pixel;
			break;

		case 3:
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			{
				p[0] = (pixel >> 16) & 0xff;
				p[1] = (pixel >> 8) & 0xff;
				p[2] = pixel & 0xff;
			}
			else
			{
				p[0] = pixel & 0xff;
				p[1] = (pixel >> 8) & 0xff;
				p[2] = (pixel >> 16) & 0xff;
			}
			break;

		 case 4:
			*(Uint32 *)p = pixel;
			break;
	}
}

void gfx_drawLine(SDL_Surface *dest, int x1, int y1, int x2, int y2, int col)
{
	int counter = 0;

	if ( SDL_MUSTLOCK(dest) )
	{
		if ( SDL_LockSurface(dest) < 0 )
		{
			printf("Can't lock screen: %s\n", SDL_GetError());
			showErrorAndExit(2, "");
		}
	}

	while(1)
	{
		gfx_putPixel(dest, x1, y1, col);

		if (x1 > x2) x1--;
		if (x1 < x2) x1++;
		if (y1 > y2) y1--;
		if (y1 < y2) y1++;

		if ((x1 == x2) && (y1 == y2))
			{break;}
		if (counter == 1000)
			{printf("Loop Error!\n"); break;}
		counter++;
	}

	if (SDL_MUSTLOCK(dest))
	{
		SDL_UnlockSurface(dest);
	}
}

/*
A quick(?) circle draw function. This code was posted to the SDL
mailing list... I didn't write it myself.
*/
void gfx_drawCircle(int xc, int yc, int R, SDL_Surface *PIX, int col)
{
	int x = 0, xx = 0;
	int y = R, yy = 2 * R;
	int p = 1 - R;

	gfx_putPixel(PIX, xc, yc - y, col);
	gfx_putPixel(PIX, xc, yc + y, col);
	gfx_putPixel(PIX, xc - y, yc, col);
	gfx_putPixel(PIX, xc + y, yc, col);

	while (x < y)
	{
		xx += 2;
		x++;
		if (p >= 0)
		{
			yy -= 2;
			y--;
			p -= yy;
		}
		p += xx + 1;

		gfx_putPixel(PIX, xc - x, yc - y, col);
		gfx_putPixel(PIX, xc + x, yc - y, col);
		gfx_putPixel(PIX, xc - x, yc + y, col);
		gfx_putPixel(PIX, xc + x, yc + y, col);
		gfx_putPixel(PIX, xc - y, yc - x, col);
		gfx_putPixel(PIX, xc + y, yc - x, col);
		gfx_putPixel(PIX, xc - y, yc + x, col);
		gfx_putPixel(PIX, xc + y, yc + x, col);
	}

	if ((x = y))
	{
		gfx_putPixel(PIX, xc - x, yc - y, col);
		gfx_putPixel(PIX, xc + x, yc - y, col);
		gfx_putPixel(PIX, xc - x, yc + y, col);
		gfx_putPixel(PIX, xc + x, yc + y, col);
	}
}

void gfx_drawRect(SDL_Surface *dest, int x, int y, int w, int h, Uint8 red, Uint8 green, Uint8 blue)
{
	SDL_Rect r = {(int16_t)x, (int16_t)y, (uint16_t)w, (uint16_t)h};
	SDL_FillRect(dest, &r, SDL_MapRGB(screen->format, red, green, blue));

	gfx_drawLine(dest, x, y, x + w, y, SDL_MapRGB(screen->format, 255, 255, 255));
	gfx_drawLine(dest, x, y, x, y + h, SDL_MapRGB(screen->format, 255, 255, 255));
	gfx_drawLine(dest, x, y + h, x + w, y + h, SDL_MapRGB(screen->format, 128, 128, 128));
	gfx_drawLine(dest, x + w, y + 1, x + w, y + h, SDL_MapRGB(screen->format, 128, 128, 128));
}

SDL_Surface *gfx_createSurface(int width, int height)
{
	SDL_Surface *surface;
	Uint32 rmask, gmask, bmask, amask;

	/* SDL interprets each pixel as a 32-bit number, so our masks must depend
	on the endianness (byte order) of the machine */
	#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
		rmask = 0xff000000;
		gmask = 0x00ff0000;
		bmask = 0x0000ff00;
		amask = 0x000000ff;
	#else
		rmask = 0x000000ff;
		gmask = 0x0000ff00;
		bmask = 0x00ff0000;
		amask = 0xff000000;
	#endif

	surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, rmask, gmask, bmask, amask);

	if (surface == NULL) {
		printf("CreateRGBSurface failed: %s\n", SDL_GetError());
		showErrorAndExit(2, "");
	}

	return surface;
}

SDL_Surface *gfx_createTextSurface(const char *inString, int color)
{
	// XXX: Magic numbers
	SDL_Surface *surface = gfx_createSurface(strlen(inString) * 9, 16);

	gfx_renderString(inString, 1, 1, color, 0, surface);

	return gfx_setTransparent(surface);
}

void gfx_createTextObject(int index, const char *inString, int x, int y, int fontColor)
{
	/* Shortcut: if we already rendered the same string in the same color, don't render it again. */
	if(gfx_text[index].text && gfx_text[index].image && gfx_text[index].fontColor == fontColor && !strcmp(gfx_text[index].text, inString)) {
		gfx_text[index].x = x;
		gfx_text[index].y = y;
		if (x == -1)
			gfx_text[index].x = (screen->w - gfx_text[index].image->w) / 2;
		return;
	}

	strcpy(gfx_text[index].text, inString);
	gfx_text[index].x = x;
	gfx_text[index].y = y;
	gfx_text[index].fontColor = fontColor;
	if (gfx_text[index].image != NULL)
	{
		SDL_FreeSurface(gfx_text[index].image);
	}
	gfx_text[index].image = gfx_createTextSurface(inString, fontColor);
	if (x == -1)
		gfx_text[index].x = (screen->w - gfx_text[index].image->w) / 2;
}

SDL_Surface *alphaRect(int width, int height, Uint8 red, Uint8 green, Uint8 blue)
{
	SDL_Surface *surface = gfx_createSurface(width, height);

	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, red, green, blue));

	SDL_SetSurfaceAlphaMod(surface, 128);

	return surface;
}

void createMessageBox(SDL_Surface *face, const char *message, signed char transparent)
{
	if (messageBox != NULL)
	{
		SDL_FreeSurface(messageBox);
		messageBox = NULL;
	}

	if (transparent)
		messageBox = alphaRect(550, 60, 0x00, 0x00, 0x00);
	else
		messageBox = gfx_createSurface(550, 60);

	signed char x = 60;

	if (face != NULL)
	{
		gfx_drawRect(messageBox, 0, 0, messageBox->w - 1, messageBox->h - 1, 0x00, 0x00, 0xaa);
		gfx_blit(face, 5, 5, messageBox);
	}
	else
	{
		gfx_drawRect(messageBox, 0, 0, messageBox->w - 1, messageBox->h - 1, 0x00, 0x00, 0x00);
		x = 10;
	}

	gfx_renderString(message, x, 5, FONT_WHITE, 1, messageBox);
}

void freeGraphics()
{
	for (int i = 0 ; i < MAX_SHAPES ; i++)
	{
		if (shape[i] != NULL)
		{
			SDL_FreeSurface(shape[i]);
			shape[i] = NULL;
		}
	}

	for (int i = 0 ; i < MAX_SHIPSHAPES ; i++)
	{
		if (shipShape[i] != NULL)
		{
			SDL_FreeSurface(shipShape[i]);
			shipShape[i] = NULL;
		}
	}

	for (int i = 0 ; i < MAX_TEXTSHAPES ; i++)
	{
		if (gfx_text[i].image != NULL)
		{
			SDL_FreeSurface(gfx_text[i].image);
			gfx_text[i].image = NULL;
		}
	}

	for (int i = 0 ; i < MAX_SHOPSHAPES ; i++)
	{
		if (shopSurface[i] != NULL)
		{
			SDL_FreeSurface(shopSurface[i]);
				shopSurface[i] = NULL;
		}
	}

	if (messageBox != NULL)
	{
		SDL_FreeSurface(messageBox);
		messageBox = NULL;
	}
}


SDL_Surface *loadImage(const char *filename)
{
	SDL_Surface *image, *newImage;

	image = IMG_Load(filename);

	if (image == NULL) {
		printf("Couldn't load %s: %s\n", filename, SDL_GetError());
		showErrorAndExit(0, filename);
	}

	newImage = SDL_ConvertSurface(image, screen->format, 0);
	if ( newImage ) {
		SDL_FreeSurface(image);
	}
	else
	{
		// This happens when we are loading the window icon image
		newImage = image;
	}

	return gfx_setTransparent(newImage);
}
