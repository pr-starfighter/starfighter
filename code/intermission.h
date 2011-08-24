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

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>

#include "defs.h"
#include "structs.h"
#include "classes.h"

extern SDL_Surface *loadImage(const char *filename);
extern void doStarfield();
extern void getPlayerInput();
extern void showShop();
extern void initShop();
extern int initSaveSlots();
extern int showSaveSlots(SDL_Surface *savesSurface, signed char saveSlot);
extern void saveGame(int slot);
extern void loadMusic(const char *filename);
extern void loadBackground(const char *filename);
extern void createCommsSurface(SDL_Surface *comms);
extern void updateCommsSurface(SDL_Surface *comms);
extern void createSavesSurface(SDL_Surface *savesSurface, signed char clickedSlot);
extern void checkForBossMission();
extern void doComms(SDL_Surface *comms);
extern int locateDataInPak(const char *file, signed char required);
extern int getFace(const char *face);
extern void flushInput();

extern globalEngineVariables engine;
extern object player;
extern Game currentGame;
extern Graphics graphics;
extern Planet systemPlanet[10];
