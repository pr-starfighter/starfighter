/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012, 2013 Guus Sliepen
Copyright (C) 2015-2017 Julie Marchant <onpon4@riseup.net>

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

#include "SDL.h"

#include "defs.h"
#include "structs.h"

#include "gfx.h"

/*
Sets a radio message that appears at the top of the screen. Used for
script events, etc. We send a message priority too, since we don't want
Phoebe or Ursula's banter to interrupt an important message
*/
void radio_setMessage(int face, const char *in, int priority)
{
	SDL_Surface *faceShape = NULL;

	if ((gfx_textSprites[TS_RADIO].life > 0) && (priority == 0))
		return;

	gfx_createTextObject(TS_RADIO, in, -1, 50, FONT_WHITE);
	gfx_textSprites[TS_RADIO].life = 240;

	if (face > -1)
		faceShape = gfx_faceSprites[face];

	gfx_createMessageBox(faceShape, in, 1);
}
