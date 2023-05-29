/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012, 2013 Guus Sliepen
Copyright (C) 2015-2020 The Diligent Circle <diligentcircle@riseup.net>

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
#include <libintl.h>
#include <stdio.h>
#include <string.h>

#include "SDL.h"
#include "SDL_image.h"


#ifndef NOFONT

// Undef MIN and MAX since pango-break.h includes these macros
// (avoids compiler warnings)
#ifdef MIN
#undef MIN
#endif

#ifdef MAX
#undef MAX
#endif

#include "SDL_ttf.h"
#include <pango/pango-break.h>

#endif


#include "defs.h"
#include "structs.h"

#include "alien.h"
#include "engine.h"
#include "game.h"
#include "screen.h"
#include "weapons.h"

SDL_Surface *gfx_unscaledBackground;
SDL_Surface *gfx_background;
SDL_Surface *gfx_sprites[SP_MAX];
SDL_Surface *gfx_faceSprites[FS_MAX];
SDL_Surface *gfx_shipSprites[SS_MAX];
SDL_Surface *gfx_fontSprites[FONT_MAX];
SDL_Surface *gfx_shopSprites[SHOP_S_MAX];
TextObject gfx_textSprites[TS_MAX];
SDL_Surface *gfx_messageBox;

#ifndef NOFONT
TTF_Font *gfx_unicodeFont;
#endif

void gfx_init()
{
	screen_bufferHead = malloc(sizeof(*screen_bufferHead));
	if (screen_bufferHead == NULL)
	{
		engine_error("Failed to allocate memory for buffer head.");
	}
	screen_bufferHead->next = NULL;
	screen_bufferTail = screen_bufferHead;

	for (int i = 0 ; i < SP_MAX ; i++)
		gfx_sprites[i] = NULL;

	for (int i = 0 ; i < SS_MAX ; i++)
		gfx_shipSprites[i] = NULL;

	for (int i = 0 ; i < TS_MAX ; i++)
		gfx_textSprites[i].image = NULL;

	for (int i = 0 ; i < SHOP_S_MAX ; i++)
		gfx_shopSprites[i] = NULL;

	for (int i = 0 ; i < FONT_MAX ; i++)
		gfx_fontSprites[i] = NULL;

	gfx_background = NULL;
	gfx_messageBox = NULL;

	screen = NULL;

#ifndef NOFONT
	if (TTF_Init() < 0)
	{
		printf("ERROR: Could not initialize TTF: %s\n", TTF_GetError());
		exit(1);
	}

	/// If the TakaoGothic font is able to display the text of the language
	/// being translated to, DO NOT CHANGE THIS!  If, however, the language
	/// requires characters not available in the default font, you can
	/// place that font in the "data" directory and indicate the name of the
	/// alternate font as a translation to "TakaoPGothic.ttf" (leaving the
	/// rest of the string unchanged).  Please ensure  that the font displays
	/// correctly with ALL text (space is limited and some fonts take up
	/// more space than others), and also check the license of the font
	/// before distributing to make sure you are allowed to do so.
	gfx_unicodeFont = TTF_OpenFont(_("data/TakaoPGothic.ttf"), 13);
	if (gfx_unicodeFont == NULL)
	{
		printf("ERROR: TTF_OpenFont: %s\n", TTF_GetError());
		exit(1);
	}
	TTF_SetFontStyle(gfx_unicodeFont, TTF_STYLE_BOLD);
#endif
}

SDL_Surface *gfx_setTransparent(SDL_Surface *sprite)
{
	SDL_SetColorKey(sprite, SDL_TRUE, SDL_MapRGB(sprite->format, 0, 0, 0));
	return sprite;
}

void gfx_blit(SDL_Surface *image, int x, int y, SDL_Surface *dest)
{
	SDL_Rect blitRect;

	// Exit early if image is not on dest at all
	if (x + image->w < 0 || x >= dest->w || y + image->h < 0 || y >= dest->h)
		return;

	// Set up a rectangle to draw to
	blitRect.x = x;
	blitRect.y = y;
	blitRect.w = image->w;
	blitRect.h = image->h;

	/* Blit onto the destination surface */
	if (SDL_BlitSurface(image, NULL, dest, &blitRect) < 0)
	{
		printf("BlitSurface error: %s\n", SDL_GetError());
		engine_showError(2, "");
	}

	// Only if it is to the screen, mark the region as damaged
	if (dest == screen)
		screen_addBuffer(blitRect.x, blitRect.y, blitRect.w, blitRect.h);
}

/*
In 16 bit mode this is slow. VERY slow. Don't write directly to a surface
that constantly needs updating (eg - the main game screen)
*/
static int gfx_renderStringBase(const char *in, int x, int y, int real_x, int fontColor, int wrap, SDL_Surface *dest)
{
	int i;
	int splitword;
	int creal_x;
	SDL_Rect area;
	SDL_Rect letter;

	area.x = x;
	area.y = y;
	area.w = PIXFONT_W;
	area.h = PIXFONT_H;
	creal_x = real_x;

	letter.y = 0;
	letter.w = PIXFONT_W;
	letter.h = PIXFONT_H;

	while (*in != '\0')
	{
		if (*in != ' ')
		{
			letter.x = (*in - 33);
			letter.x *= PIXFONT_W;

			/* Blit onto the screen surface */
			if (SDL_BlitSurface(gfx_fontSprites[fontColor], &letter, dest, &area) < 0)
			{
				printf("BlitSurface error: %s\n", SDL_GetError());
				engine_showError(2, "");
			}
		}

		area.x += PIXFONT_W + 1;
		creal_x += PIXFONT_W + 1;

		if (wrap)
		{
			if ((creal_x > (dest->w - 70)) && (*in == ' '))
			{
				area.y += PIXFONT_LINE_HEIGHT;
				area.x = x;
				creal_x = real_x;
			}
			else if (creal_x > (dest->w - 31))
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
					letter.x *= PIXFONT_W;
					if (SDL_BlitSurface(gfx_fontSprites[fontColor], &letter, dest, &area) < 0)
					{
						printf("BlitSurface error: %s\n", SDL_GetError());
						engine_showError(2, "");
					}
					area.y += PIXFONT_LINE_HEIGHT;
					area.x = x;
					creal_x = real_x;
				}
			}
		}

		in++;
	}

	return area.y;
}

/*
Legacy text rendering function, the original one which only supports
ASCII. Generally not used anymore with the exception of some title
screen bits that remain untranslated, but also used as a fallback if the
game is compiled without SDL_ttf and Pango. Works OK on the English
text, but not likely to work well with translations (and won't work at
all for languages like Chinese or Japanese based on non-Latin
alphabets).
*/
int gfx_renderString(const char *in, int x, int y, int fontColor, int wrap, SDL_Surface *dest)
{
	if (x == -1)
		x = (dest->w - (strlen(in) * (PIXFONT_W + 1))) / 2;

	gfx_renderStringBase(in, x, y - 1, x, FONT_OUTLINE, wrap, dest);
	gfx_renderStringBase(in, x, y + 1, x, FONT_OUTLINE, wrap, dest);
	gfx_renderStringBase(in, x, y + 2, x, FONT_OUTLINE, wrap, dest);
	gfx_renderStringBase(in, x - 1, y, x, FONT_OUTLINE, wrap, dest);
	gfx_renderStringBase(in, x - 2, y, x, FONT_OUTLINE, wrap, dest);
	gfx_renderStringBase(in, x + 1, y, x, FONT_OUTLINE, wrap, dest);
	return gfx_renderStringBase(in, x, y, x, fontColor, wrap, dest);
}

#ifdef NOFONT
int gfx_unicodeWidth(const char *in)
{
	return (PIXFONT_W + 1) * strlen(in);
}

int gfx_renderUnicode(const char *in, int x, int y, int fontColor, int wrap, SDL_Surface *dest)
{
	return gfx_renderString(in, x, y, fontColor, wrap, dest);
}

#else
static int gfx_charIsUTF8Start(unsigned char c)
{
	// Top bit not set (single byte ASCII character)
	if ((c & 0x80) == 0)
		return 1;
	
	// Top two bits set (start of multi-byte character)
	if ((c & 0x80) && (c & 0x40))
		return 1;
	
	// Top bit set, but second bit not set (somewhere in the middle)
	return 0;
}

static int gfx_unicodeNumCharacters(const char *in)
{
	int i;
	int n = 0;
	char c;

	for (i = 0; i < strlen(in) + 1; i++)
	{
		c = in[i];
		if (gfx_charIsUTF8Start(c))
			n++;
	}

	return n;
}

static int gfx_unicodeCharacterCodepoint(const char *in, int u_char_index)
{
	int i;
	int n = -1;
	char c;

	for (i = 0; i < strlen(in) + 1; i++)
	{
		c = in[i];
		if (gfx_charIsUTF8Start(c) && (++n == u_char_index))
			return i;
	}

	return -1;
}

int gfx_unicodeWidth(const char *in)
{
	int w;

	if (TTF_SizeUTF8(gfx_unicodeFont, in, &w, NULL) < 0)
	{
		engine_error(TTF_GetError());
	}

	return w;
}

int gfx_renderUnicodeBase(const char *in, int x, int y, int real_x, int fontColor, int wrap, SDL_Surface *dest)
{
	SDL_Surface *textSurf;
	SDL_Color color;
	int w, h;
	int avail_w;
	int changed;
	int breakPoints[STRMAX];
	int nBreakPoints;
	char testStr[STRMAX];
	char remainingStr[STRMAX];
	PangoLogAttr logAttrs[STRMAX];
	int nLogAttrs;
	int i;
	SDL_Rect area;

	if (strcmp(in, "") == 0)
		return y;

	avail_w = dest->w - real_x;

	switch (fontColor)
	{
		case FONT_WHITE:
			color.r = 255;
			color.g = 255;
			color.b = 255;
			break;
		case FONT_RED:
			color.r = 255;
			color.g = 0;
			color.b = 0;
			break;
		case FONT_YELLOW:
			color.r = 255;
			color.g = 255;
			color.b = 0;
			break;
		case FONT_GREEN:
			color.r = 0;
			color.g = 255;
			color.b = 0;
			break;
		case FONT_CYAN:
			color.r = 0;
			color.g = 255;
			color.b = 255;
			break;
		case FONT_OUTLINE:
			color.r = 0;
			color.g = 0;
			color.b = 10;
			break;
		default:
			color.r = 255;
			color.g = 255;
			color.b = 255;
	}

	if (gfx_unicodeFont != NULL)
	{
		strcpy(remainingStr, in);
		if (TTF_SizeUTF8(gfx_unicodeFont, remainingStr, &w, &h) < 0)
		{
			engine_error(TTF_GetError());
		}

		changed = wrap;
		while (changed && (w > avail_w))
		{
			nLogAttrs = gfx_unicodeNumCharacters(remainingStr) + 1;
			pango_get_log_attrs(remainingStr, strlen(remainingStr), -1, NULL, logAttrs, nLogAttrs);

			nBreakPoints = 0;
			for (i = 0; i < nLogAttrs; i++)
			{
				if (logAttrs[i].is_line_break
						&& gfx_charIsUTF8Start(remainingStr[i]))
				{
					breakPoints[nBreakPoints] = gfx_unicodeCharacterCodepoint(
							remainingStr, i);
					nBreakPoints++;
				}
			}

			changed = 0;
			for (i = nBreakPoints - 1; i >= 0; i--)
			{
				strncpy(testStr, remainingStr, breakPoints[i]);
				testStr[breakPoints[i]] = '\0';
				if (TTF_SizeUTF8(gfx_unicodeFont, testStr, &w, &h) < 0)
				{
					engine_error(TTF_GetError());
				}
				if (w <= avail_w)
				{
					textSurf = TTF_RenderUTF8_Blended(gfx_unicodeFont, testStr, color);
					if (textSurf == NULL)
					{
						printf("While rendering testStr \"%s\" as unicode...\n", testStr);
						engine_error("Attempted to render UTF8, got null surface!");
					}

					area.x = x;
					area.y = y;
					area.w = textSurf->w;
					area.h = textSurf->h;
					if (SDL_BlitSurface(textSurf, NULL, dest, &area) < 0)
					{
						printf("BlitSurface error: %s\n", SDL_GetError());
						engine_showError(2, "");
					}
					SDL_FreeSurface(textSurf);
					textSurf = NULL;
					y += TTF_FontHeight(gfx_unicodeFont) + 1;

					memmove(remainingStr, remainingStr + breakPoints[i],
						(strlen(remainingStr) - breakPoints[i]) + 1);
					changed = 1;
					break;
				}
			}

			if (TTF_SizeUTF8(gfx_unicodeFont, remainingStr, &w, &h) < 0)
			{
				engine_error(TTF_GetError());
			}
		}
		textSurf = TTF_RenderUTF8_Blended(gfx_unicodeFont, remainingStr, color);
		if (textSurf == NULL)
		{
			printf("While rendering remainingStr \"%s\" as unicode...\n", remainingStr);
			engine_error("Attempted to render UTF8, got null surface!");
		}

		area.x = x;
		area.y = y;
		area.w = textSurf->w;
		area.h = textSurf->h;
		if (SDL_BlitSurface(textSurf, NULL, dest, &area) < 0)
		{
			printf("BlitSurface error: %s\n", SDL_GetError());
			engine_showError(2, "");
		}
		SDL_FreeSurface(textSurf);
		textSurf = NULL;
		y += TTF_FontHeight(gfx_unicodeFont) + 1;
	}
	else
	{
		engine_warn("gfx_unicodeFont is NULL!");
	}

	return y;
}

int gfx_renderUnicode(const char *in, int x, int y, int fontColor, int wrap, SDL_Surface *dest)
{
	int w;

	if (x == -1)
	{
		TTF_SizeUTF8(gfx_unicodeFont, in, &w, NULL);
		x = (dest->w - MIN(w, dest->w)) / 2;
	}

	gfx_renderUnicodeBase(in, x, y - 1, x, FONT_OUTLINE, wrap, dest);
	gfx_renderUnicodeBase(in, x, y + 1, x, FONT_OUTLINE, wrap, dest);
	gfx_renderUnicodeBase(in, x, y + 2, x, FONT_OUTLINE, wrap, dest);
	gfx_renderUnicodeBase(in, x - 1, y, x, FONT_OUTLINE, wrap, dest);
	gfx_renderUnicodeBase(in, x - 2, y, x, FONT_OUTLINE, wrap, dest);
	gfx_renderUnicodeBase(in, x + 1, y, x, FONT_OUTLINE, wrap, dest);
	return gfx_renderUnicodeBase(in, x, y, x, fontColor, wrap, dest);
}
#endif

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

/*
A quick(?) circle draw function. This code was posted to the SDL
mailing list... I didn't write it myself.
*/
void gfx_drawCircle(int xc, int yc, int R, SDL_Surface *PIX, int col)
{
	int x = 0;
	int xx = 0;
	int y = R;
	int yy = 2 * R;
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
	SDL_Rect r = {x, y, w, h};
	SDL_FillRect(dest, &r, SDL_MapRGB(screen->format, red, green, blue));

	r.h = 1;
	SDL_FillRect(dest, &r, SDL_MapRGB(screen->format, 255, 255, 255));

	r.w = 1;
	r.h = h;
	SDL_FillRect(dest, &r, SDL_MapRGB(screen->format, 255, 255, 255));

	r.y = y + h;
	r.w = w;
	r.h = 1;
	SDL_FillRect(dest, &r, SDL_MapRGB(screen->format, 128, 128, 128));

	r.x = x + w;
	r.y = y + 1;
	r.w = 1;
	r.h = h - 1;
	SDL_FillRect(dest, &r, SDL_MapRGB(screen->format, 128, 128, 128));
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
		engine_showError(2, "");
	}

	return surface;
}

SDL_Surface *gfx_createTextSurface(const char *inString, int color)
{
	int w, h, th;

#ifndef NOFONT
	if (TTF_SizeUTF8(gfx_unicodeFont, inString, &w, &th) < 0)
	{
		engine_error(TTF_GetError());
	}
	w += 2;
	th += 2;
	h = MAX(th, PIXFONT_LINE_HEIGHT);
#else
	w = strlen(inString) * (PIXFONT_W + 1) + 1;
	th = PIXFONT_H;
	h = MAX(PIXFONT_LINE_HEIGHT, PIXFONT_H + 2);
#endif

	SDL_Surface *surface = gfx_createSurface(w, h);

	gfx_renderUnicode(inString, 1, (h - th) / 2, color, 0, surface);

	return gfx_setTransparent(surface);
}

void gfx_createTextObject(int index, const char *inString, int x, int y, int fontColor)
{
	// Reset the life of the text object
	gfx_textSprites[index].life = 0;

	/* Shortcut: if we already rendered the same string in the same color, don't render it again. */
	// TODO: Double-check this, I think it's trying to test if gfx_textSprites[index].image is NULL.
	// Also, check what `text` will be when "empty".
	if (gfx_textSprites[index].text && gfx_textSprites[index].image
			&& (gfx_textSprites[index].fontColor == fontColor)
			&& (strcmp(gfx_textSprites[index].text, inString) == 0))
	{
		gfx_textSprites[index].x = x;
		gfx_textSprites[index].y = y;
		return;
	}

	strcpy(gfx_textSprites[index].text, inString);
	gfx_textSprites[index].x = x;
	gfx_textSprites[index].y = y;
	gfx_textSprites[index].fontColor = fontColor;
	if (gfx_textSprites[index].image != NULL)
	{
		SDL_FreeSurface(gfx_textSprites[index].image);
	}
	gfx_textSprites[index].image = gfx_createTextSurface(inString, fontColor);
}

SDL_Surface *gfx_createAlphaRect(int width, int height, Uint8 red, Uint8 green, Uint8 blue)
{
	SDL_Surface *surface = gfx_createSurface(width, height);

	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, red, green, blue));

	SDL_SetSurfaceAlphaMod(surface, 128);

	return surface;
}

void gfx_createMessageBox(SDL_Surface *face, const char *message, int transparent)
{
	int border = 5;
	int x = border;
	int y = border;

	if (gfx_messageBox != NULL)
	{
		SDL_FreeSurface(gfx_messageBox);
		gfx_messageBox = NULL;
	}

	if (transparent)
		gfx_messageBox = gfx_createAlphaRect(550, 60, 0x00, 0x00, 0x00);
	else
		gfx_messageBox = gfx_createSurface(550, 60);

	if (face != NULL)
	{
		gfx_drawRect(gfx_messageBox, 0, 0, gfx_messageBox->w - 1, gfx_messageBox->h - 1, 0x00, 0x00, 0xaa);
		gfx_blit(face, border, border, gfx_messageBox);
		x = (2 * border) + face->w;
	}
	else
	{
		gfx_drawRect(gfx_messageBox, 0, 0, gfx_messageBox->w - 1, gfx_messageBox->h - 1, 0x00, 0x00, 0x00);
		x = border;
	}

	gfx_renderUnicode(message, x, y, FONT_WHITE, 1, gfx_messageBox);
}

SDL_Surface *gfx_loadImage(const char *filename)
{
	SDL_Surface *image, *newImage;

	image = IMG_Load(filename);

	if (image == NULL) {
		printf("Couldn't load %s: %s\n", filename, SDL_GetError());
		engine_showError(0, filename);
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

void gfx_free()
{
	for (int i = 0 ; i < SP_MAX ; i++)
	{
		if (gfx_sprites[i] != NULL)
		{
			SDL_FreeSurface(gfx_sprites[i]);
			gfx_sprites[i] = NULL;
		}
	}

	for (int i = 0 ; i < FS_MAX ; i++)
	{
		if (gfx_faceSprites[i] != NULL)
		{
			SDL_FreeSurface(gfx_faceSprites[i]);
			gfx_faceSprites[i] = NULL;
		}
	}

	for (int i = 0 ; i < SS_MAX ; i++)
	{
		if (gfx_shipSprites[i] != NULL)
		{
			SDL_FreeSurface(gfx_shipSprites[i]);
			gfx_shipSprites[i] = NULL;
		}
	}

	for (int i = 0 ; i < TS_MAX ; i++)
	{
		if (gfx_textSprites[i].image != NULL)
		{
			SDL_FreeSurface(gfx_textSprites[i].image);
			gfx_textSprites[i].image = NULL;
		}
	}

	for (int i = 0 ; i < SHOP_S_MAX ; i++)
	{
		if (gfx_shopSprites[i] != NULL)
		{
			SDL_FreeSurface(gfx_shopSprites[i]);
				gfx_shopSprites[i] = NULL;
		}
	}

	if (gfx_messageBox != NULL)
	{
		SDL_FreeSurface(gfx_messageBox);
		gfx_messageBox = NULL;
	}
}

void gfx_scaleBackground()
{
	if (gfx_background != NULL)
	{
		SDL_FreeSurface(gfx_background);
		gfx_background = NULL;
	}
	gfx_background = gfx_createSurface(screen->w, screen->h);
	if (gfx_background == NULL)
		engine_error("Failed to create surface for scaled background");

	SDL_SetColorKey(gfx_background, 0, 0);
	SDL_BlitScaled(gfx_unscaledBackground, NULL, gfx_background, NULL);
}

void gfx_loadBackground(const char *filename)
{
	if (gfx_unscaledBackground != NULL)
	{
		SDL_FreeSurface(gfx_unscaledBackground);
		gfx_unscaledBackground = NULL;
	}
	gfx_unscaledBackground = gfx_loadImage(filename);
	if (gfx_unscaledBackground == NULL)
		engine_error("Failed to load unscaled background image");

	SDL_SetColorKey(gfx_unscaledBackground, 0, 0);
	gfx_scaleBackground();
}

void gfx_loadSprites()
{
	Uint32 *p32;
	Uint16 *p16;
	Uint8 *p8;

	gfx_free();

	// Faces
	gfx_faceSprites[FS_CHRIS] = gfx_loadImage("gfx/face_chris.png");
	gfx_faceSprites[FS_SID] = gfx_loadImage("gfx/face_sid.png");
	gfx_faceSprites[FS_KRASS] = gfx_loadImage("gfx/face_krass.png");
	gfx_faceSprites[FS_PHOEBE] = gfx_loadImage("gfx/face_phoebe.png");
	gfx_faceSprites[FS_URSULA] = gfx_loadImage("gfx/face_ursula.png");
	gfx_faceSprites[FS_KLINE] = gfx_loadImage("gfx/face_kline.png");
	gfx_faceSprites[FS_CREW] = gfx_loadImage("gfx/face_crew.png");

	// Ships
	gfx_shipSprites[SS_FIREFLY] = gfx_loadImage("gfx/firefly1.png");
	gfx_shipSprites[SS_FIREFLY_L] = gfx_loadImage("gfx/firefly2.png");
	gfx_shipSprites[SS_SID] = gfx_loadImage("gfx/sid1.png");
	gfx_shipSprites[SS_SID_L] = gfx_loadImage("gfx/sid2.png");
	gfx_shipSprites[SS_FRIEND] = gfx_loadImage("gfx/wingmate1.png");
	gfx_shipSprites[SS_FRIEND_L] = gfx_loadImage("gfx/wingmate2.png");
	gfx_shipSprites[SS_GOODTRANSPORT] = gfx_loadImage("gfx/goodTrans1.png");
	gfx_shipSprites[SS_GOODTRANSPORT_L] = gfx_loadImage("gfx/goodTrans2.png");
	gfx_shipSprites[SS_REBELCARRIER] = gfx_loadImage("gfx/rebelCarrier1.png");
	gfx_shipSprites[SS_REBELCARRIER_L] = gfx_loadImage("gfx/rebelCarrier2.png");
	gfx_shipSprites[SS_DUALFIGHTER] = gfx_loadImage("gfx/dualFighter1.png");
	gfx_shipSprites[SS_DUALFIGHTER_L] = gfx_loadImage("gfx/dualFighter2.png");
	gfx_shipSprites[SS_MISSILEBOAT] = gfx_loadImage("gfx/missileBoat1.png");
	gfx_shipSprites[SS_MISSILEBOAT_L] = gfx_loadImage("gfx/missileBoat2.png");
	gfx_shipSprites[SS_PROTOFIGHTER] = gfx_loadImage("gfx/eliteFighter1.png");
	gfx_shipSprites[SS_PROTOFIGHTER_L] = gfx_loadImage("gfx/eliteFighter2.png");
	gfx_shipSprites[SS_AIMFIGHTER] = gfx_loadImage("gfx/aimFighter1.png");
	gfx_shipSprites[SS_AIMFIGHTER_L] = gfx_loadImage("gfx/aimFighter2.png");
	gfx_shipSprites[SS_DRONE] = gfx_loadImage("gfx/drone1.png");
	gfx_shipSprites[SS_DRONE_L] = gfx_loadImage("gfx/drone2.png");
	gfx_shipSprites[SS_MINER] = gfx_loadImage("gfx/miner1.png");
	gfx_shipSprites[SS_MINER_L] = gfx_loadImage("gfx/miner2.png");
	gfx_shipSprites[SS_ESCORT] = gfx_loadImage("gfx/escort1.png");
	gfx_shipSprites[SS_ESCORT_L] = gfx_loadImage("gfx/escort2.png");
	gfx_shipSprites[SS_MOBILE_RAY] = gfx_loadImage("gfx/mobileCannon1.png");
	gfx_shipSprites[SS_MOBILE_RAY_L] = gfx_loadImage("gfx/mobileCannon2.png");
	gfx_shipSprites[SS_TRANSPORTSHIP] = gfx_loadImage("gfx/transport1.png");
	gfx_shipSprites[SS_TRANSPORTSHIP_L] = gfx_loadImage("gfx/transport2.png");
	gfx_shipSprites[SS_CARGOSHIP] = gfx_loadImage("gfx/tug1.png");
	gfx_shipSprites[SS_CARGOSHIP_L] = gfx_loadImage("gfx/tug2.png");
	gfx_shipSprites[SS_SLAVETRANSPORT] = gfx_loadImage("gfx/slaveTrans1.png");
	gfx_shipSprites[SS_SLAVETRANSPORT_L] = gfx_loadImage("gfx/slaveTrans2.png");
	gfx_shipSprites[SS_BARRIER] = gfx_loadImage("gfx/barrier.png");
	gfx_shipSprites[SS_MOBILESHIELD] = gfx_loadImage("gfx/mobileShield1.png");
	gfx_shipSprites[SS_MOBILESHIELD_L] = gfx_loadImage("gfx/mobileShield2.png");
	gfx_shipSprites[SS_ASTEROID] = gfx_loadImage("gfx/asteroid1.png");
	gfx_shipSprites[SS_ASTEROID_SMALL] = gfx_loadImage("gfx/asteroid2.png");
	gfx_shipSprites[SS_ASTEROID_SMALL_L] = gfx_loadImage("gfx/asteroid3.png");
	gfx_shipSprites[SS_CLOAKFIGHTER] = gfx_loadImage("gfx/cloakShip1.png");
	gfx_shipSprites[SS_CLOAKFIGHTER_L] = gfx_loadImage("gfx/cloakShip2.png");
	gfx_shipSprites[SS_EVILURSULA] = gfx_loadImage("gfx/evilUrsula1.png");
	gfx_shipSprites[SS_EVILURSULA_L] = gfx_loadImage("gfx/evilUrsula2.png");
	gfx_shipSprites[SS_KRASS] = gfx_loadImage("gfx/merc1.png");
	gfx_shipSprites[SS_KRASS_L] = gfx_loadImage("gfx/merc2.png");
	gfx_shipSprites[SS_FRIGATE] = gfx_loadImage("gfx/frigateBody1.png");
	gfx_shipSprites[SS_FRIGATE_L] = gfx_loadImage("gfx/frigateBody2.png");
	gfx_shipSprites[SS_FRIGATE_WING1] = gfx_loadImage("gfx/frigateGun11.png");
	gfx_shipSprites[SS_FRIGATE_WING1_L] = gfx_loadImage("gfx/frigateGun12.png");
	gfx_shipSprites[SS_FRIGATE_WING2] = gfx_loadImage("gfx/frigateGun21.png");
	gfx_shipSprites[SS_FRIGATE_WING2_L] = gfx_loadImage("gfx/frigateGun22.png");
	gfx_shipSprites[SS_MINERBOSS] = gfx_loadImage("gfx/mineBoss1.png");
	gfx_shipSprites[SS_MINERBOSS_L] = gfx_loadImage("gfx/mineBoss2.png");
	gfx_shipSprites[SS_MINERBOSS_WING1] = gfx_loadImage("gfx/mineBossWing11.png");
	gfx_shipSprites[SS_MINERBOSS_WING1_L] = gfx_loadImage("gfx/mineBossWing12.png");
	gfx_shipSprites[SS_MINERBOSS_WING2] = gfx_loadImage("gfx/mineBossWing21.png");
	gfx_shipSprites[SS_MINERBOSS_WING2_L] = gfx_loadImage("gfx/mineBossWing22.png");
	gfx_shipSprites[SS_MINERBOSS_WING3] = gfx_loadImage("gfx/mineBossWing31.png");
	gfx_shipSprites[SS_MINERBOSS_WING3_L] = gfx_loadImage("gfx/mineBossWing32.png");
	gfx_shipSprites[SS_MINERBOSS_WING4] = gfx_loadImage("gfx/mineBossWing41.png");
	gfx_shipSprites[SS_MINERBOSS_WING4_L] = gfx_loadImage("gfx/mineBossWing42.png");
	gfx_shipSprites[SS_EXEC] = gfx_loadImage("gfx/execTrans1.png");
	gfx_shipSprites[SS_EXEC_L] = gfx_loadImage("gfx/execTrans2.png");
	gfx_shipSprites[SS_PLUTOBOSS] = gfx_loadImage("gfx/plutoBoss1.png");
	gfx_shipSprites[SS_PLUTOBOSS_L] = gfx_loadImage("gfx/plutoBoss2.png");
	gfx_shipSprites[SS_URANUSBOSS] = gfx_loadImage("gfx/splitBoss11.png");
	gfx_shipSprites[SS_URANUSBOSS_L] = gfx_loadImage("gfx/splitBoss12.png");
	gfx_shipSprites[SS_URANUSBOSS_WING1] = gfx_loadImage("gfx/splitBoss21.png");
	gfx_shipSprites[SS_URANUSBOSS_WING1_L] = gfx_loadImage("gfx/splitBoss22.png");
	gfx_shipSprites[SS_URANUSBOSS_WING2] = gfx_loadImage("gfx/splitBoss31.png");
	gfx_shipSprites[SS_URANUSBOSS_WING2_L] = gfx_loadImage("gfx/splitBoss32.png");
	gfx_shipSprites[SS_KLINE] = gfx_loadImage("gfx/kline11.png");
	gfx_shipSprites[SS_KLINE_L] = gfx_loadImage("gfx/kline12.png");

	/*
	Create images of ships being hit that show a lot of red
	*/
	for (int i = SS_HIT_INDEX ; i < SS_MAX ; i++)
	{
		if (gfx_shipSprites[i - SS_HIT_INDEX] == NULL)
			continue;
		gfx_shipSprites[i] = gfx_createSurface(gfx_shipSprites[i - SS_HIT_INDEX]->w,
			gfx_shipSprites[i - SS_HIT_INDEX]->h);
		SDL_SetSurfaceBlendMode(gfx_shipSprites[i - SS_HIT_INDEX], SDL_BLENDMODE_NONE);
		gfx_blit(gfx_shipSprites[i - SS_HIT_INDEX], 0, 0, gfx_shipSprites[i]);
		SDL_SetSurfaceBlendMode(gfx_shipSprites[i - SS_HIT_INDEX], SDL_BLENDMODE_BLEND);

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

	// Other sprites
	gfx_sprites[SP_PLASMA_GREEN] = gfx_loadImage("gfx/plasmaGreen.png");
	gfx_sprites[SP_PLASMA_RED] = gfx_loadImage("gfx/plasmaRed.png");
	gfx_sprites[SP_DIR_PLASMA_GREEN] = gfx_loadImage("gfx/greenDir.png");
	gfx_sprites[SP_DIR_PLASMA_RED] = gfx_loadImage("gfx/redDir.png");
	gfx_sprites[SP_ROCKET] = gfx_loadImage("gfx/rocket1.png");
	gfx_sprites[SP_ROCKET_L] = gfx_loadImage("gfx/rocket2.png");
	gfx_sprites[SP_SMALL_EXPLOSION] = gfx_loadImage("gfx/explode1.png");
	gfx_sprites[SP_SMALL_EXPLOSION_2] = gfx_loadImage("gfx/explode2.png");
	gfx_sprites[SP_SMALL_EXPLOSION_3] = gfx_loadImage("gfx/explode3.png");
	gfx_sprites[SP_SMALL_EXPLOSION_L] = gfx_loadImage("gfx/explode4.png");
	gfx_sprites[SP_BIG_EXPLOSION] = gfx_loadImage("gfx/explode05.png");
	gfx_sprites[SP_BIG_EXPLOSION_2] = gfx_loadImage("gfx/explode06.png");
	gfx_sprites[SP_BIG_EXPLOSION_3] = gfx_loadImage("gfx/explode07.png");
	gfx_sprites[SP_BIG_EXPLOSION_L] = gfx_loadImage("gfx/explode08.png");
	gfx_sprites[SP_SMOKE] = gfx_loadImage("gfx/explode9.png");
	gfx_sprites[SP_SMOKE_2] = gfx_loadImage("gfx/explode10.png");
	gfx_sprites[SP_SMOKE_3] = gfx_loadImage("gfx/explode11.png");
	gfx_sprites[SP_SMOKE_L] = gfx_loadImage("gfx/explode12.png");
	gfx_sprites[SP_TINY_EXPLOSION] = gfx_loadImage("gfx/explode13.png");
	gfx_sprites[SP_TINY_EXPLOSION_2] = gfx_loadImage("gfx/explode14.png");
	gfx_sprites[SP_TINY_EXPLOSION_3] = gfx_loadImage("gfx/explode15.png");
	gfx_sprites[SP_TINY_EXPLOSION_L] = gfx_loadImage("gfx/explode16.png");
	gfx_sprites[SP_ELECTRICAL] = gfx_loadImage("gfx/elec1.png");
	gfx_sprites[SP_ELECTRICAL_2] = gfx_loadImage("gfx/elec2.png");
	gfx_sprites[SP_ELECTRICAL_3] = gfx_loadImage("gfx/elec3.png");
	gfx_sprites[SP_ELECTRICAL_L] = gfx_loadImage("gfx/elec4.png");
	gfx_sprites[SP_PICKUP_MONEY] = gfx_loadImage("gfx/dollar.png");
	gfx_sprites[SP_PICKUP_PLASMA] = gfx_loadImage("gfx/rocket.png");
	gfx_sprites[SP_PICKUP_SHIELD] = gfx_loadImage("gfx/heart.png");
	gfx_sprites[SP_PICKUP_PLASMA_OUTPUT] = gfx_loadImage("gfx/plasmaAmmo.png");
	gfx_sprites[SP_PICKUP_PLASMA_RATE] = gfx_loadImage("gfx/plasmaRate.png");
	gfx_sprites[SP_PICKUP_PLASMA_POWER] = gfx_loadImage("gfx/plasmaDamage.png");
	gfx_sprites[SP_CHAIN_LINK] = gfx_loadImage("gfx/chainLink.png");
	gfx_sprites[SP_MINE] = gfx_loadImage("gfx/mine.png");
	gfx_sprites[SP_CARGO] = gfx_loadImage("gfx/cargo1.png");
	gfx_sprites[SP_ION] = gfx_loadImage("gfx/stunBolt.png");
	gfx_sprites[SP_ARROW_NORTH] = gfx_loadImage("gfx/arrowNorth.png");
	gfx_sprites[SP_ARROW_NORTHEAST] = gfx_loadImage("gfx/arrowNorthEast.png");
	gfx_sprites[SP_ARROW_EAST] = gfx_loadImage("gfx/arrowEast.png");
	gfx_sprites[SP_ARROW_SOUTHEAST] = gfx_loadImage("gfx/arrowSouthEast.png");
	gfx_sprites[SP_ARROW_SOUTH] = gfx_loadImage("gfx/arrowSouth.png");
	gfx_sprites[SP_ARROW_SOUTHWEST] = gfx_loadImage("gfx/arrowSouthWest.png");
	gfx_sprites[SP_ARROW_WEST] = gfx_loadImage("gfx/arrowWest.png");
	gfx_sprites[SP_ARROW_NORTHWEST] = gfx_loadImage("gfx/arrowNorthWest.png");
	gfx_sprites[SP_ARROW_FRIEND_NORTH] = gfx_loadImage("gfx/friendArrowNorth.png");
	gfx_sprites[SP_ARROW_FRIEND_NORTHEAST] = gfx_loadImage("gfx/friendArrowNorthEast.png");
	gfx_sprites[SP_ARROW_FRIEND_EAST] = gfx_loadImage("gfx/friendArrowEast.png");
	gfx_sprites[SP_ARROW_FRIEND_SOUTHEAST] = gfx_loadImage("gfx/friendArrowSouthEast.png");
	gfx_sprites[SP_ARROW_FRIEND_SOUTH] = gfx_loadImage("gfx/friendArrowSouth.png");
	gfx_sprites[SP_ARROW_FRIEND_SOUTHWEST] = gfx_loadImage("gfx/friendArrowSouthWest.png");
	gfx_sprites[SP_ARROW_FRIEND_WEST] = gfx_loadImage("gfx/friendArrowWest.png");
	gfx_sprites[SP_ARROW_FRIEND_NORTHWEST] = gfx_loadImage("gfx/friendArrowNorthWest.png");
	gfx_sprites[SP_INDICATOR_TARGET] = gfx_loadImage("gfx/targetText.png");
	gfx_sprites[SP_INDICATOR_SID] = gfx_loadImage("gfx/sidText.png");
	gfx_sprites[SP_INDICATOR_PHOEBE] = gfx_loadImage("gfx/phoebeText.png");
	gfx_sprites[SP_INDICATOR_URSULA] = gfx_loadImage("gfx/ursulaText.png");
	gfx_sprites[SP_INDICATOR_KLINE] = gfx_loadImage("gfx/klineText.png");
	gfx_sprites[SP_ESCAPE_POD] = gfx_loadImage("gfx/pod.png");
	gfx_sprites[SP_ORE] = gfx_loadImage("gfx/ore1.png");
	gfx_sprites[SP_ORE_2] = gfx_loadImage("gfx/ore2.png");
	gfx_sprites[SP_ORE_L] = gfx_loadImage("gfx/ore3.png");
	gfx_sprites[SP_PICKUP_ROCKETS] = gfx_loadImage("gfx/rocketAmmo.png");
	gfx_sprites[SP_SUPERCHARGE] = gfx_loadImage("gfx/superCharge.png");

	gfx_loadBackground(systemBackground[game.system]);

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

	for (int i = 0 ; i < W_MAX ; i++)
	{
		weapons[i].image[0] = gfx_sprites[weapons[i].imageIndex[0]];
		weapons[i].image[1] = gfx_sprites[weapons[i].imageIndex[1]];
	}
}

/*
Custom loading to alter the font color before doing
all other things
*/
void gfx_loadFont()
{
	SDL_Surface *image, *newImage;

	for (int i = 0 ; i < FONT_MAX ; i++)
	{
		image = IMG_Load("gfx/smallFont.png");

		if (image == NULL) {
			printf("Couldn't load game font! (%s) Exitting.\n", SDL_GetError());
		   exit(1);
		}

		switch(i)
		{
			case FONT_RED:
				SDL_SetSurfaceColorMod(image, 255, 0, 0);
				break;
			case FONT_YELLOW:
				SDL_SetSurfaceColorMod(image, 255, 255, 0);
				break;
			case FONT_GREEN:
				SDL_SetSurfaceColorMod(image, 0, 255, 0);
				break;
			case FONT_CYAN:
				SDL_SetSurfaceColorMod(image, 0, 255, 255);
				break;
			case FONT_OUTLINE:
				SDL_SetSurfaceColorMod(image, 0, 0, 10);
				break;
		}

		newImage = SDL_ConvertSurface(image, screen->format, 0);

		gfx_fontSprites[i] = gfx_setTransparent(newImage);

		SDL_FreeSurface(image);
 	}
}
