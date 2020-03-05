/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012, 2013 Guus Sliepen
Copyright (C) 2015-2019 Layla Marchant <diligentcircle@riseup.net>

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

#include <stdlib.h>
#include <string.h>

#include "SDL.h"

#include "defs.h"
#include "structs.h"

#include "engine.h"
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

/*
Get a random message from those listed in `messages` (separated by '\n')
and assign it to `choice`.  Used for things like taunts and brags.
*/
void radio_getRandomMessage(char *dest, const char *messages)
{
	char *msgs;
	char *patch;
	int nMsg;
	int choice;
	int i;

	msgs = malloc(sizeof(msgs) * strlen(messages));

	nMsg = 0;
	strcpy(msgs, messages);
	patch = strtok(msgs, "\n");
	while (patch != NULL)
	{
		if (strcmp(patch, "") != 0)
			nMsg++;

		patch = strtok(NULL, "\n");
	}

	// Now we know how many choices we have, let's make that choice...
	choice = rand() % nMsg;

	// And go through the search again...
	i = 0;
	strcpy(msgs, messages);
	patch = strtok(msgs, "\n");
	while ((i < choice) && (patch != NULL))
	{
		if (strcmp(patch, "") != 0)
			i++;

		patch = strtok(NULL, "\n");
	}

	if (patch != NULL)
	{
		strcpy(dest, patch);
	}
	else
	{
		engine_warn("Failed to grab a message! Is the list empty?");
		strcpy(dest, "");
	}

	free(msgs);
}
