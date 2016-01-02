/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011 Guus Sliepen
Copyright (C) 2015 onpon4 <onpon4@riseup.net>

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

extern object alien_defs[CD_MAX];
extern object aliens[ALIEN_MAX];

void alien_defs_init();
void aliens_init();
bool alien_add();
void alien_addDrone(object *hostAlien);
void alien_addSmallAsteroid(object *hostAlien);
void alien_addFriendly(int type);
bool alien_place(object *alien);
void alien_setAI(object *alien);
void alien_setKlineAttackMethod(object *alien);
void alien_setKlineAI(object *alien);
void alien_searchForTarget(object *alien);
int alien_checkTarget(object *alien);
int alien_enemiesInFront(object *alien);
void alien_move(object *alien);
void alien_destroy(object *alien, object *attacker);
void alien_hurt(object *alien, object *attacker, int damage, bool ion);

#endif
