/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011 Guus Sliepen
Copyright (C) 2015-2023 Diligent Circle <diligentcircle@riseup.net>

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

#ifndef PLAYER_H
#define PLAYER_H

#include "defs.h"
#include "structs.h"

extern Object player;
extern int player_chargerFired;
extern int player_damageDelay;
extern int player_resetDamageDelay;

void player_init();
void player_setTarget(int index);
void player_damage(int amount, int delay);
void player_checkShockDamage(float x, float y);
void player_exit();
void player_flushInput();
void player_getInput();
void player_leaveSector();

#endif
