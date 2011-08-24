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

extern void playSound(int sid);
extern void updateMissionRequirements(int type, int id, int value);
extern void addCollectable(float x, float y, int type, int value, int life);
extern void addExplosion(float x, float y, int type);
extern void generateShockWave(object *bullet);
extern void setInfoLine(const char *in, int color);
extern void getKillMessage(object *ally);
extern void getMissFireMessage(object *ally);
extern void getPlayerHitMessage(object *ally);
extern void checkMineBulletCollisions(object *bullet);
extern void setKlineAttackMethod(object *theEnemy);
extern void setRadioMessage(signed char face, const char *in, int priority);

extern globalEngineVariables engine;
extern devVariables dev;
extern object weapon[MAX_WEAPONS];
extern object player;
extern mission currentMission;
extern object enemy[MAX_ALIENS];
extern object cargo[20];
extern Game currentGame;
extern Graphics graphics;
