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

Star star[200];

static unsigned long frameLimit;
static int thirds;

Uint32 red;
Uint32 darkRed;
Uint32 yellow;
Uint32 darkYellow;
Uint32 green;
Uint32 darkGreen;
Uint32 blue;
Uint32 darkBlue;
Uint32 darkerBlue;
Uint32 black;
Uint32 white;
Uint32 lightGrey;
Uint32 darkGrey;
SDL_Surface *screen, *background;
SDL_Surface *shape[MAX_SHAPES];
SDL_Surface *shipShape[MAX_SHIPSHAPES];
SDL_Surface *fontShape[MAX_FONTSHAPES];
SDL_Surface *shopSurface[MAX_SHOPSHAPES];
bRect *bufferHead;
bRect *bufferTail;
textObject textShape[MAX_TEXTSHAPES];
SDL_Surface *messageBox;

bool collision(float x0, float y0, int w0, int h0, float x2, float y2, int w1, int h1)
{
	float x1 = x0 + w0;
	float y1 = y0 + h0;

	float x3 = x2 + w1;
	float y3 = y2 + h1;

	return !(x1<x2 || x3<x0 || y1<y2 || y3<y0);
}

bool collision(object *object1, object *object2)
{
	float x0 = object1->x;
	float y0 = object1->y;
	float w0 = object1->image[0]->w;
	float h0 = object1->image[0]->h;

	float x2 = object2->x;
	float y2 = object2->y;
	float w1 = object2->image[0]->w;
	float h1 = object2->image[0]->h;

	float x1 = x0 + w0;
	float y1 = y0 + h0;

	float x3 = x2 + w1;
	float y3 = y2 + h1;

	return !(x1<x2 || x3<x0 || y1<y2 || y3<y0);
}

bool collision(collectables *object1, object *object2)
{
	float x0 = object1->x;
	float y0 = object1->y;
	float w0 = object1->image->w;
	float h0 = object1->image->h;

	float x2 = object2->x;
	float y2 = object2->y;
	float w1 = object2->image[0]->w;
	float h1 = object2->image[0]->h;

	float x1 = x0 + w0;
	float y1 = y0 + h0;

	float x3 = x2 + w1;
	float y3 = y2 + h1;

	return !(x1<x2 || x3<x0 || y1<y2 || y3<y0);
}

void initGraphics()
{
	bufferHead = new bRect;
	bufferHead->next = NULL;
	bufferTail = bufferHead;

	for (int i = 0 ; i < MAX_SHAPES ; i++)
		shape[i] = NULL;

	for (int i = 0 ; i < MAX_SHIPSHAPES ; i++)
		shipShape[i] = NULL;

	for (int i = 0 ; i < MAX_TEXTSHAPES ; i++)
		textShape[i].image = NULL;

	for (int i = 0 ; i < MAX_SHOPSHAPES ; i++)
		shopSurface[i] = NULL;

	for (int i = 0 ; i < MAX_FONTSHAPES ; i++)
		fontShape[i] = NULL;

	background = NULL;
	messageBox = NULL;

	frameLimit = 0;
	thirds = 0;
	screen = NULL;
}

SDL_Surface *setTransparent(SDL_Surface *sprite)
{
	SDL_SetColorKey(sprite, (SDL_SRCCOLORKEY|SDL_RLEACCEL), SDL_MapRGB(sprite->format, 0, 0, 0));
	return sprite;
}

void addBuffer(int x, int y, int w, int h)
{
	bRect *rect = new bRect;

	rect->next = NULL;
	rect->x = x;
	rect->y = y;
	rect->w = w;
	rect->h = h;

	bufferTail->next = rect;
	bufferTail = rect;
}

void blit(SDL_Surface *image, int x, int y, SDL_Surface *dest)
{
	// Exit early if image is not on dest at all
	if(x + image->w < 0 || x >= dest->w || y + image->h < 0 || y >= dest->h)
		return;

	// Set up a rectangle to draw to
	SDL_Rect blitRect;

	blitRect.x = x;
	blitRect.y = y;
	blitRect.w = image->w;
	blitRect.h = image->h;

	/* Blit onto the destination surface */
	if(SDL_BlitSurface(image, NULL, dest, &blitRect) < 0)
	{
		printf("BlitSurface error: %s\n", SDL_GetError());
		showErrorAndExit(2, "");
	}

	// Only ff it is to the screen, mark the region as damaged
	if(dest == screen)
		addBuffer(blitRect.x, blitRect.y, blitRect.w, blitRect.h);
}

void blit(SDL_Surface *image, int x, int y)
{
	blit(image, x, y, screen);
}

void blitText(int i)
{
	blit(textShape[i].image, (int)textShape[i].x, (int)textShape[i].y, screen);
}

void flushBuffer()
{
	bRect *prevRect = bufferHead;
	bRect *rect = bufferHead;
	bufferTail = bufferHead;

	while (rect->next != NULL)
	{
		rect = rect->next;

		prevRect->next = rect->next;
		delete rect;
		rect = prevRect;
	}

	bufferHead->next = NULL;
}

void unBuffer()
{
	bRect *prevRect = bufferHead;
	bRect *rect = bufferHead;
	bufferTail = bufferHead;

	while (rect->next != NULL)
	{
		rect = rect->next;

		SDL_Rect blitRect;

		blitRect.x = rect->x;
		blitRect.y = rect->y;
		blitRect.w = rect->w;
		blitRect.h = rect->h;

		if (SDL_BlitSurface(background, &blitRect, screen, &blitRect) < 0)
		{
			printf("BlitSurface error: %s\n", SDL_GetError());
			showErrorAndExit(2, "");
		}

		prevRect->next = rect->next;
		delete rect;
		rect = prevRect;
	}

	bufferHead->next = NULL;
}

/*
In 16 bit mode this is slow. VERY slow. Don't write directly to a surface
that constantly needs updating (eg - the main game screen)
*/
static int renderString(const char *in, int x, int y, int fontColor, signed char wrap, SDL_Surface *dest)
{
	SDL_Rect area;
	area.x = x;
	area.y = y;
	area.w = 8;
	area.h = 14;

	SDL_Rect letter;
	letter.y = 0;
	letter.w = 8;
	letter.h = 14;

	while (*in != '\0')
	{
		if (*in != 32)
		{
			letter.x = (*in - 33);
			letter.x *= 8;
			letter.x--; // Temp fix

			/* Blit onto the screen surface */
			 if(SDL_BlitSurface(fontShape[fontColor], &letter, dest, &area) < 0)
			 {
				printf("BlitSurface error: %s\n", SDL_GetError());
				showErrorAndExit(2, "");
			 }
		}

		area.x += 9;

		if (wrap)
		{
			if ((area.x > (dest->w - 70)) && (*in == 32))
			{
				area.y += 16;
				area.x = x;
			}
		}

		in++;
	}

	return area.y;
}

int drawString(const char *in, int x, int y, int fontColor, signed char wrap, SDL_Surface *dest)
{
	renderString(in, x, y - 1, FONT_OUTLINE, wrap, dest);
	renderString(in, x, y + 1, FONT_OUTLINE, wrap, dest);
	renderString(in, x, y + 2, FONT_OUTLINE, wrap, dest);
	renderString(in, x - 1, y, FONT_OUTLINE, wrap, dest);
	renderString(in, x - 2, y, FONT_OUTLINE, wrap, dest);
	renderString(in, x + 1, y, FONT_OUTLINE, wrap, dest);
	return renderString(in, x, y, fontColor, wrap, dest);
}

int drawString(const char *in, int x, int y, int fontColor, SDL_Surface *dest)
{
	if (x == -1)
		x = (dest->w - (strlen(in) * 9)) / 2;
	return drawString(in, x, y, fontColor, 0, dest);
}

int drawString(const char *in, int x, int y, int fontColor)
{
	if (x == -1)
		x = (800 - (strlen(in) * 9)) / 2;
	return drawString(in, x, y, fontColor, 0, screen);
}

/*
Finds the location of the requested color within the palette and returns
it's number. This colors are used for drawing rectangles, circle, etc in
the correct colors.
*/
void setColorIndexes()
{
	red = SDL_MapRGB(screen->format, 0xff, 0x00, 0x00);
	darkRed = SDL_MapRGB(screen->format, 0x66, 0x00, 0x00);

	yellow = SDL_MapRGB(screen->format, 0xff, 0xff, 0x00);
	darkYellow = SDL_MapRGB(screen->format, 0x66, 0x66, 0x00);

	green = SDL_MapRGB(screen->format, 0x00, 0xff, 0x00);
	darkGreen = SDL_MapRGB(screen->format, 0x00, 0x66, 0x00);

	blue = SDL_MapRGB(screen->format, 0x00, 0x00, 0xff);
	darkBlue = SDL_MapRGB(screen->format, 0x00, 0x00, 0x99);
	darkerBlue = SDL_MapRGB(screen->format, 0x00, 0x00, 0x44);

	black = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	white = SDL_MapRGB(screen->format, 0xff, 0xff, 0xff);
	lightGrey = SDL_MapRGB(screen->format, 0xcc, 0xcc, 0xcc);
	darkGrey = SDL_MapRGB(screen->format, 0x99, 0x99, 0x99);
}

/*
Draws the background surface that has been loaded
*/
void drawBackGround()
{
	blit(background, 0, 0, screen);
}

void clearScreen(Uint32 color)
{
	SDL_FillRect(screen, NULL, color);
}

void updateScreen()
{
	SDL_Flip(screen);
}

/*
 * Delay until the next 60 Hz frame
 */
void delayFrame()
{
	unsigned long now = SDL_GetTicks();

	// Add 16 2/3 to frameLimit
	frameLimit += 16;
	if(thirds >= 2) {
		thirds = 0;
	} else {
		thirds++;
		frameLimit++;
	}

	if(now < frameLimit)
		SDL_Delay(frameLimit - now);
	else
		frameLimit = now;
}

/*
 * Set the pixel at (x, y) to the given value
 * NOTE: The surface must be locked before calling this!
 */
void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
	 int bpp = surface->format->BytesPerPixel;
	 /* Here p is the address to the pixel we want to set */
	 Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	 switch(bpp) {
	 case 1:
		  *p = pixel;
		  break;

	 case 2:
		  *(Uint16 *)p = pixel;
		  break;

	 case 3:
		  if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
				p[0] = (pixel >> 16) & 0xff;
				p[1] = (pixel >> 8) & 0xff;
				p[2] = pixel & 0xff;
		  } else {
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

void drawLine(SDL_Surface *dest, int x1, int y1, int x2, int y2, int col)
{
	int counter = 0;

	if ( SDL_MUSTLOCK(dest) ) {
		if ( SDL_LockSurface(dest) < 0 ) {
			printf("Can't lock screen: %s\n", SDL_GetError());
			showErrorAndExit(2, "");
		}
	}

	while(1)
	{
		putpixel(dest, x1, y1, col);

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

	if ( SDL_MUSTLOCK(dest) ) {
		SDL_UnlockSurface(dest);
	}
}

void drawLine(int x1, int y1, int x2, int y2, int col)
{
	drawLine(screen, x1, y1, x2, y2, col);
}

/*
A quick(?) circle draw function. This code was posted to the SDL
mailing list... I didn't write it myself.
*/
void circle(int xc, int yc, int R, SDL_Surface *PIX, int col)
{
	int x = 0, xx = 0;
	int y = R, yy = R+R;
	int p = 1-R;

	putpixel(PIX, xc, yc - y, col);
	putpixel(PIX, xc, yc + y, col);
	putpixel(PIX, xc - y, yc, col);
	putpixel(PIX, xc + y, yc, col);

	while(x < y)
	{
		xx += 2;
		++x;
		if (p >= 0)
		{
			yy -= 2;
			--y;
			p -= yy;
		}
		p += xx + 1;

		putpixel(PIX, xc - x, yc - y, col);
		putpixel(PIX, xc + x, yc - y, col);
		putpixel(PIX, xc - x, yc + y, col);
		putpixel(PIX, xc + x, yc + y, col);
		putpixel(PIX, xc - y, yc - x, col);
		putpixel(PIX, xc + y, yc - x, col);
		putpixel(PIX, xc - y, yc + x, col);
		putpixel(PIX, xc + y, yc + x, col);
	}

	if ((x = y))
	{
		putpixel(PIX, xc - x, yc - y, col);
		putpixel(PIX, xc + x, yc - y, col);
		putpixel(PIX, xc - x, yc + y, col);
		putpixel(PIX, xc + x, yc + y, col);
	}
}

void blevelRect(SDL_Surface *dest, int x, int y, int w, int h, Uint8 red, Uint8 green, Uint8 blue)
{
	SDL_Rect r = {x, y, w, h};
	SDL_FillRect(dest, &r, SDL_MapRGB(screen->format, red, green, blue));

	drawLine(dest, x, y, x + w, y, SDL_MapRGB(screen->format, 255, 255, 255));
	drawLine(dest, x, y, x, y + h, SDL_MapRGB(screen->format, 255, 255, 255));
	drawLine(dest, x, y + h, x + w, y + h, SDL_MapRGB(screen->format, 128, 128, 128));
	drawLine(dest, x + w, y + 1, x + w, y + h, SDL_MapRGB(screen->format, 128, 128, 128));
}

void blevelRect(int x, int y, int w, int h, Uint8 red, Uint8 green, Uint8 blue)
{
	blevelRect(screen, x, y, w, h, red, green, blue);
}

SDL_Surface *createSurface(int width, int height)
{
	SDL_Surface *surface, *newImage;
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

	newImage = SDL_DisplayFormat(surface);

	SDL_FreeSurface(surface);

	return newImage;
}

SDL_Surface *textSurface(const char *inString, int color)
{
	SDL_Surface *surface = createSurface(strlen(inString) * 9, 16);

	drawString(inString, 1, 1, color, surface);

	return setTransparent(surface);
}

void textSurface(int index, const char *inString, int x, int y, int fontColor)
{
	/* Shortcut: if we already rendered the same string in the same color, don't render it again. */
	if(textShape[index].text && textShape[index].image && textShape[index].fontColor == fontColor && !strcmp(textShape[index].text, inString)) {
		textShape[index].x = x;
		textShape[index].y = y;
		if (x == -1)
			textShape[index].x = (800 - textShape[index].image->w) / 2;
		return;
	}

	strcpy(textShape[index].text, inString);
	textShape[index].x = x;
	textShape[index].y = y;
	textShape[index].fontColor = fontColor;
	if (textShape[index].image != NULL)
	{
		SDL_FreeSurface(textShape[index].image);
	}
	textShape[index].image = textSurface(inString, fontColor);
	if (x == -1)
		textShape[index].x = (800 - textShape[index].image->w) / 2;
}

SDL_Surface *alphaRect(int width, int height, Uint8 red, Uint8 green, Uint8 blue)
{
	SDL_Surface *surface = createSurface(width, height);

	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, red, green, blue));

	SDL_SetAlpha(surface, SDL_SRCALPHA|SDL_RLEACCEL, 128);

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
		messageBox = createSurface(550, 60);

	signed char x = 60;

	if (face != NULL)
	{
		blevelRect(messageBox, 0, 0, messageBox->w - 1, messageBox->h - 1, 0x00, 0x00, 0xaa);
		blit(face, 5, 5, messageBox);
	}
	else
	{
		blevelRect(messageBox, 0, 0, messageBox->w - 1, messageBox->h - 1, 0x00, 0x00, 0x00);
		x = 10;
	}

	drawString(message, x, 5, FONT_WHITE, 1, messageBox);
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
		if (textShape[i].image != NULL)
		{
			SDL_FreeSurface(textShape[i].image);
			textShape[i].image = NULL;
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

	return setTransparent(newImage);
}

/*
Simply draws the stars in their positions on screen and moves
them around. They are wrapped around using the wrapFloat()
function, as defined above, and putpixel as defined in cpp
*/
void doStarfield()
{
	/* Lock the screen for direct access to the pixels */
	if (SDL_MUSTLOCK(screen))
	{
		if (SDL_LockSurface(screen) < 0 )
			showErrorAndExit(2, "");
	}

	int color = 0;

	SDL_Rect r;

	for (int i = 0 ; i < 200 ; i++)
	{
		if (star[i].speed == 3)
			color = white;
		else if (star[i].speed == 2)
			color = lightGrey;
		else if (star[i].speed == 1)
			color = darkGrey;

		wrapFloat(&(star[i].x += (engine.ssx * star[i].speed)), 0, screen->w - 1);
		wrapFloat(&(star[i].y += (engine.ssy * star[i].speed)), 0, screen->h - 1);

		putpixel(screen, (int)star[i].x, (int)star[i].y, color);
		r.x = (int)star[i].x;
		r.y = (int)star[i].y;
		r.w = 1;
		r.h = 1;

		addBuffer(r.x, r.y, r.w, r.h);
	}

	if (SDL_MUSTLOCK(screen))
	{
		SDL_UnlockSurface(screen);
	}
}
