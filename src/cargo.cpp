/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011 Guus Sliepen
Copyright (C) 2015 Julian Marchant

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

void initCargo()
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
static int getCargo()
{
	for (int i = 0 ; i < MAX_CARGO ; i++)
	{
		if (!cargo[i].active)
			return i;
	}

	return -1;
}

object *addCargo(object *owner, int cargoType)
{
	int index = getCargo();

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

static void becomeCollectable(int i)
{
	if (cargo[i].collectType != P_PHOEBE)
	{
		addCollectable(cargo[i].x, cargo[i].y, cargo[i].collectType, 1, 600);
	}
	else
	{
		enemy[FR_PHOEBE].active = true;
		enemy[FR_PHOEBE].x = cargo[i].x;
		enemy[FR_PHOEBE].y = cargo[i].y;
		setRadioMessage(FACE_PHOEBE, "Thanks!! Watch out, WEAPCO! Phoebe's loose and she's ANGRY!!!", 1);
	}

	cargo[i].active = false;
}

void doCargo()
{
	float dx, dy, chainX, chainY;

	for (int i = 0 ; i < MAX_CARGO ; i++)
	{
		if (cargo[i].active)
		{
			if (!cargo[i].owner->active)
			{
				becomeCollectable(i);
				continue;
			}

			blit(cargo[i].image[0], (int)cargo[i].x, (int)cargo[i].y);

			cargo[i].x += engine.ssx + engine.smx;
			cargo[i].y += engine.ssy + engine.smy;

			limitFloat(&cargo[i].x, cargo[i].owner->x - 50, cargo[i].owner->x + 50);
			limitFloat(&cargo[i].y, cargo[i].owner->y - 50, cargo[i].owner->y + 50);

			dx = (cargo[i].x - cargo[i].owner->x) / 10;
			dy = (cargo[i].y - cargo[i].owner->y) / 10;
			chainX = cargo[i].x - cargo[i].dx;
			chainY = cargo[i].y - cargo[i].dy;

			// draw the chain link line
			for (int j = 0 ; j < 10 ; j++)
			{
				blit(shape[30], (int)chainX, (int)chainY);
				chainX -= dx;
				chainY -= dy;
			}
		}
	}
}
