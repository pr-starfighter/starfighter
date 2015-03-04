/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011 Guus Sliepen
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

#ifndef GRAPHICS_H
#define GRAPHICS_H

extern Star star[200];

extern Uint32 red;
extern Uint32 darkRed;
extern Uint32 yellow;
extern Uint32 darkYellow;
extern Uint32 green;
extern Uint32 darkGreen;
extern Uint32 blue;
extern Uint32 darkBlue;
extern Uint32 darkerBlue;
extern Uint32 black;
extern Uint32 white;
extern Uint32 lightGrey;
extern Uint32 darkGrey;
extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Texture *texture;
extern SDL_Surface *screen, *background;
extern SDL_Surface *shape[MAX_SHAPES];
extern SDL_Surface *shipShape[MAX_SHIPSHAPES];
extern SDL_Surface *fontShape[MAX_FONTSHAPES];
extern SDL_Surface *shopSurface[MAX_SHOPSHAPES];
extern bRect *bufferHead;
extern bRect *bufferTail;
extern textObject textShape[MAX_TEXTSHAPES];
extern SDL_Surface *messageBox;


extern bool collision(float x0, float y0, int w0, int h0, float x2, float y2, int w1, int h1);
extern bool collision(object *object1, object *object2);
extern bool collision(collectables *object1, object *object2);

extern void initGraphics();
extern SDL_Surface *setTransparent(SDL_Surface *sprite);
extern void addBuffer(int x, int y, int w, int h);
extern void blit(SDL_Surface *image, int x, int y, SDL_Surface *dest);
extern void blit(SDL_Surface *image, int x, int y);
extern void blitText(int i);
extern void flushBuffer();
extern void unBuffer();
extern int drawString(const char *in, int x, int y, int fontColor, signed char wrap, SDL_Surface *dest);
extern int drawString(const char *in, int x, int y, int fontColor, SDL_Surface *dest);
extern int drawString(const char *in, int x, int y, int fontColor);
extern void setColorIndexes();
extern void drawBackGround();
extern void clearScreen(Uint32 color);
extern void updateScreen();
extern void delayFrame();
extern void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
extern void drawLine(SDL_Surface *dest, int x1, int y1, int x2, int y2, int col);
extern void drawLine(int x1, int y1, int x2, int y2, int col);
extern void circle(int xc, int yc, int R, SDL_Surface *PIX, int col);
extern void blevelRect(SDL_Surface *dest, int x, int y, int w, int h, Uint8 red, Uint8 green, Uint8 blue);
extern void blevelRect(int x, int y, int w, int h, Uint8 red, Uint8 green, Uint8 blue);
extern SDL_Surface *createSurface(int width, int height);
extern SDL_Surface *textSurface(const char *inString, int color);
extern void textSurface(int index, const char *inString, int x, int y, int fontColor);
extern SDL_Surface *alphaRect(int width, int height, Uint8 red, Uint8 green, Uint8 blue);
extern void createMessageBox(SDL_Surface *face, const char *message, signed char transparent);
extern void freeGraphics();

extern SDL_Surface *loadImage(const char *filename);
extern void doStarfield();

#endif
