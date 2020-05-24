/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011 Guus Sliepen
Copyright (C) 2015-2020 Layla Marchant <diligentcircle@riseup.net>

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

#ifndef ALIEN_H
#define ALIEN_H

#include "defs.h"
#include "structs.h"

extern Object alien_defs[CD_MAX];
extern Object aliens[ALIEN_MAX];

void alien_defs_init();
void aliens_init();
int alien_add();
void alien_addDrone(Object *hostAlien);
void alien_addSmallAsteroid(Object *hostAlien);
void alien_addFriendly(int type);
int alien_place(Object *alien);
void alien_setAI(Object *alien);
void alien_setKlineAttackMethod(Object *alien);
void alien_setKlineAI(Object *alien);
void alien_searchForTarget(Object *alien);
int alien_checkTarget(Object *alien);
int alien_enemiesInFront(Object *alien);
void alien_move(Object *alien);
void alien_destroy(Object *alien, Object *attacker);
void alien_hurt(Object *alien, Object *attacker, int damage, int ion);

#endif
