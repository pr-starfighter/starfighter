/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011 Guus Sliepen
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

#ifndef BULLETS_H
#define BULLETS_H

#include "defs.h"
#include "structs.h"

void bullet_add(Object *theWeapon, Object *attacker, int y, int dy);
Object *bullet_getTarget(Object *bullet);
int bullet_collision(Object *bullet, Object *ship);

#endif
