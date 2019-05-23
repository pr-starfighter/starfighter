/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012, 2013 Guus Sliepen
Copyright (C) 2015-2019 Julie Marchant <onpon4@riseup.net>

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

#include "defs.h"
#include "structs.h"

#include "gfx.h"

void info_clearLines()
{
	for (int i = 0 ; i < MAX_INFOLINES ; i++)
	{
		gfx_textSprites[i].life = 0;
	}
}

// from a to b
void info_copyLine(int a, int b)
{
	gfx_createTextObject(b, gfx_textSprites[a].text, -1, 0, gfx_textSprites[a].fontColor);
	gfx_textSprites[b].life = gfx_textSprites[a].life;
}

/*
Sets one of the three information lines on the screen. The accepts the
string and colors. It will set the information to the first free infoline
it finds (top to bottom). If it doesn't find any free ones, it will push
all the other info lines down one and add itself to the top.
*/
void info_setLine(const char *in, int color)
{
	int index = -1;

	for (int i = 0 ; i < MAX_INFOLINES ; i++)
	{
		if ((gfx_textSprites[i].life == 0) && (index == -1))
		{
			index = i;
		}
	}

	// Bump down
	if (index == -1)
	{
		index = MAX_INFOLINES - 1;
		for (int i = 1 ; i < MAX_INFOLINES ; i++)
		{
			info_copyLine(i, i - 1);
		}
	}

	gfx_createTextObject(index, in, -1, 0, color);
	gfx_textSprites[index].life = 240;
}
