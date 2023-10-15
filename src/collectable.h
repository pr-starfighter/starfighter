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

#ifndef COLLECTABLE_H
#define COLLECTABLE_H

#include "SDL.h"

#include "defs.h"
#include "structs.h"

typedef struct Collectable_ {

	int active;
	float x, y, dx, dy;
	SDL_Surface *image;
	int type; // What kind of collectable is it?
	int value; // How much is it worth?
	int life; // How long it will stay around for

	struct Collectable_ *next;

} Collectable;

void collectable_add(float x, float y, int type, int value, int life);
int collectable_collision(Collectable *collectable, Object *ship);
void collectable_explode(Collectable *collectable);
int collectable_numGood();

#endif
