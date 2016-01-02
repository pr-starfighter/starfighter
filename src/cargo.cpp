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

#include "Starfighter.h"

object cargo[MAX_CARGO];

void cargo_init()
{
	for (int i = 0 ; i < MAX_CARGO ; i++)
	{
		cargo[i].active = false;
		cargo[i].owner = NULL;
	}
}

/*
* I think you all know what this does by now! ;)
*/
static int cargo_get()
{
	for (int i = 0 ; i < MAX_CARGO ; i++)
	{
		if (!cargo[i].active)
			return i;
	}

	return -1;
}

object *cargo_add(object *owner, int cargoType)
{
	int index = cargo_get();

	if (index == -1)
		return NULL;

	cargo[index].active = true;
	cargo[index].owner = owner;
	cargo[index].x = owner->x;
	cargo[index].y = owner->y;
	cargo[index].dx = 0;
	cargo[index].dy = 0;
	cargo[index].collectType = cargoType;
	cargo[index].image[0] = shape[32];
	if (cargoType == P_PHOEBE)
		cargo[index].image[0] = shipShape[20];

	return &cargo[index];
}

void cargo_becomeCollectable(int i)
{
	if (cargo[i].collectType != P_PHOEBE)
	{
		collectable_add(cargo[i].x, cargo[i].y, cargo[i].collectType, 1, 600);
	}
	else
	{
		aliens[ALIEN_PHOEBE].active = true;
		aliens[ALIEN_PHOEBE].x = cargo[i].x;
		aliens[ALIEN_PHOEBE].y = cargo[i].y;
		setRadioMessage(FACE_PHOEBE, "Thanks!! Watch out, WEAPCO! Phoebe's loose and she's ANGRY!!!", 1);
	}

	cargo[i].active = false;
}
