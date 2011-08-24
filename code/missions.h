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

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>

#include "defs.h"
#include "structs.h"
#include "classes.h"

extern SDL_Surface *loadImage(const char *file_name);
extern void playRandomTrack();
extern void getPlayerInput();
extern void setInfoLine(const char *in, int color);
extern void loadGameGraphics();
extern void killAllAliens();
extern int locateDataInPak(const char *file, signed char required);
extern void setRadioMessage(signed char face, const char *in, int priority);
extern void syncScriptEvents();
extern void loadMusic(const char *filename);
extern void setTarget(int index);
extern void flushInput();

extern globalEngineVariables engine;
extern object player;
extern object enemy[MAX_ALIENS];
extern mission currentMission;
extern Game currentGame;
extern Graphics graphics;
extern Planet systemPlanet[10];
extern mission missions[MAX_MISSIONS];
