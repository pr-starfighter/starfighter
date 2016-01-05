/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011 Guus Sliepen
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

#ifndef GFX_H
#define GFX_H

#include "Starfighter.h"

extern SDL_Surface *gfx_background;
extern SDL_Surface *gfx_sprites[MAX_SPRITES];
extern SDL_Surface *gfx_faceSprites[FS_MAX];
extern SDL_Surface *gfx_shipSprites[SS_MAX];
extern SDL_Surface *gfx_fontSprites[MAX_FONTSPRITES];
extern SDL_Surface *gfx_shopSprites[SHOP_S_MAX];
extern textObject gfx_textSprites[TS_MAX];
extern SDL_Surface *gfx_messageBox;

void gfx_init();
SDL_Surface *gfx_setTransparent(SDL_Surface *sprite);
void gfx_blit(SDL_Surface *image, int x, int y, SDL_Surface *dest);
int gfx_renderString(const char *in, int x, int y, int fontColor, int wrap, SDL_Surface *dest);
void gfx_putPixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
void gfx_drawLine(SDL_Surface *dest, int x1, int y1, int x2, int y2, int col);
void gfx_drawCircle(int xc, int yc, int R, SDL_Surface *PIX, int col);
void gfx_drawRect(SDL_Surface *dest, int x, int y, int w, int h, Uint8 red, Uint8 green, Uint8 blue);
SDL_Surface *gfx_createSurface(int width, int height);
SDL_Surface *gfx_createTextSurface(const char *inString, int color);
void gfx_createTextObject(int index, const char *inString, int x, int y, int fontColor);
SDL_Surface *gfx_createAlphaRect(int width, int height, Uint8 red, Uint8 green, Uint8 blue);
void gfx_createMessageBox(SDL_Surface *face, const char *message, signed char transparent);
SDL_Surface *gfx_loadImage(const char *filename);
void gfx_free();

#endif
