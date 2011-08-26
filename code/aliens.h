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

extern void setEnemyAI(object *theEnemy);
extern void setKlineAI(object *theEnemy);
extern void fireBullet(object *attacker, int weaponType);
extern void addExplosion(float x, float y, int type);
extern void addEngine(object *craft);
extern void fireRay(object *attacker);
extern void addDebris(int x, int y, int amount);
extern void playSound(int sid);
extern int locateDataInPak(const char *file, bool required);
extern object *addCargo(object *owner, int cargoType);
extern void addCollectable(float x, float y, int type, int value, int life);
extern void updateMissionRequirements(int type, int id, int value);
extern void setInfoLine(const char *in, int color);
extern void addBullet(object *theWeapon, object *attacker, int y, int dy);
extern void showErrorAndExit(int errorId, const char *name);

extern globalEngineVariables engine;
extern devVariables dev;
extern Game currentGame;
extern object player;
extern object enemy[MAX_ALIENS];
extern Graphics graphics;
extern mission currentMission;
extern object weapon[MAX_WEAPONS];

object defEnemy[MAX_DEFALIENS]; // A predefined enemy
