/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011 Guus Sliepen
Copyright (C) 2015, 2016 Julie Marchant <onpon4@riseup.net>

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

#include "Starfighter.h"

/*
Create a new explosion based on supplied parameters.
The "type" will actually be used as an explosion frame check.
All explosion types have 4 images. The "thinktime" will be used
to change frames on a 21, 14, 7 basis.
*/
void explosion_add(float x, float y, int type)
{
	Object *explosion = new Object;

	explosion->next = NULL;
	explosion->active = 1;
	explosion->x = x;
	explosion->y = y;
	explosion->thinktime = 28;
	explosion->face = type;
	explosion->image[0] = gfx_sprites[type];

	engine.explosionTail->next = explosion;
	engine.explosionTail = explosion;
}

/*
* This very simply just adds a tiny explosion at the coordinate specified.
* It creates a small engine like effect.
*/
void explosion_addEngine(Object *craft)
{
	if (CHANCE(0.5))
		return;

	float x = craft->x + (craft->engineX * craft->face);
	float y = craft->y + craft->engineY;

	y += RANDRANGE(-3, 3);
	explosion_add(x, y, SP_TINY_EXPLOSION);
}
