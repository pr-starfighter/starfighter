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

#include "Starfighter.h"

/*
Create a new explosion based on supplied parameters.
The "type" will actually be used as an explosion frame check.
All explosion types have 4 images. The "thinktime" will be used
to change frames on a 21, 14, 7 basis.
*/
void addExplosion(float x, float y, int type)
{
	object *explosion = new object;

	explosion->next = NULL;
	explosion->active = true;
	explosion->x = x;
	explosion->y = y;
	explosion->thinktime = 28;
	explosion->face = type;
	explosion->image[0] = graphics.shape[type];

	engine.explosionTail->next = explosion;
	engine.explosionTail = explosion;
}

/*
* This very simply just adds a tiny explosion at the coordinate specified.
* It creates a small engine like effect.
*/
void addEngine(object *craft)
{
	if (rand() % 2 == 0)
		return;

	float x = craft->x + (craft->engineX * craft->face);
	float y = craft->y + craft->engineY;

	y += Math::rrand(-3, 3);
	addExplosion(x, y, E_TINY_EXPLOSION);
}

/*
Loops through active explosions and decrements their think time.
If their thinktime is divisable by 5, then the frame is changed to
the next one up (for example 0->1->2-3). When their think time is 0,
the explosion is killed off.
*/
void doExplosions()
{
	object *prevExplosion = engine.explosionHead;
	object *explosion = engine.explosionHead;
	engine.explosionTail = engine.explosionHead;
	
	while (explosion->next != NULL)
	{
		explosion = explosion->next;

		if (explosion->active)
		{
			explosion->x += engine.ssx;
			explosion->y += engine.ssy;
	
			if (isOnScreen((int)explosion->x, (int)explosion->y, explosion->image[0]->w, explosion->image[0]->h))
				graphics.blit(explosion->image[0], (int)explosion->x, (int)explosion->y);

			if(rand() % 7 == 0)
			{
				explosion->thinktime -= 7;

				if(explosion->thinktime < 1)
				{
					explosion->active = false;
				}
				else
				{
					explosion->face++;
					explosion->image[0] = graphics.shape[explosion->face];
				}
			}
		}

		if (explosion->active)
		{
			prevExplosion = explosion;
			engine.explosionTail = explosion;
		}
		else
		{
			prevExplosion->next = explosion->next;
			delete explosion;
         explosion = prevExplosion;
		}
	}
}

