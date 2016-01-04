/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012, 2013 Guus Sliepen
Copyright (C) 2015, 2016 onpon4 <onpon4@riseup.net>

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

void clearInfoLines()
{
	for (int i = 0 ; i < 4 ; i++)
	{
		gfx_textSprites[i].life = 0;
	}
}

// from a to b
void copyInfoLine(int a, int b)
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
void setInfoLine(const char *in, int color)
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
			copyInfoLine(i, i - 1);
		}
	}

	gfx_createTextObject(index, in, -1, 0, color);
	gfx_textSprites[index].life = 240;
}

/*
Sets a radio message that appears at the top of the screen. Used for
script events, etc. We send a message priority too, since we don't want
Phoebe or Ursula's banter to interrupt an important message
*/
void setRadioMessage(int face, const char *in, int priority)
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

static const char *faces[] = {
			"FACE_CHRIS", "FACE_SID", "FACE_KRASS",
			"FACE_KLINE", "FACE_PHOEBE", "FACE_URSULA",
			"FACE_CREW"
};

int getFace(const char *face)
{
	for (int i = 0 ; i < 7 ; i++)
	{
		if (strcmp(faces[i], face) == 0)
			return FS_CHRIS + i;
	}

	return -1;
}

void resetLists()
{
	object *ob, *ob2;
	collectables *c1, *c2;
	bRect *r1, *r2;

	ob = engine.bulletHead->next;
	while(ob != NULL)
	{
		ob2 = ob;
		ob = ob->next;
		delete ob2;
	}
	engine.bulletHead->next = NULL;
	engine.bulletTail = engine.bulletHead;

	ob = engine.explosionHead->next;
	while(ob != NULL)
	{
		ob2 = ob;
		ob = ob->next;
		delete ob2;
	}
	engine.explosionHead->next = NULL;
	engine.explosionTail = engine.explosionHead;

	c1 = engine.collectableHead->next;
	while (c1 != NULL)
	{
		c2 = c1;
		c1 = c1->next;
		delete c2;
	}

	engine.collectableHead->next = NULL;
	engine.collectableTail = engine.collectableHead;
	
	r1 = screen_bufferHead->next;
	while (r1 != NULL)
	{
		r2 = r1;
		r1 = r1->next;
		delete r2;
	}
	
	screen_bufferHead->next = NULL;
	screen_bufferTail = screen_bufferHead;

	ob = engine.debrisHead->next;
	while(ob != NULL)
	{
		ob2 = ob;
		ob = ob->next;
		delete ob2;
	}
	engine.debrisHead->next = NULL;
	engine.debrisTail = engine.debrisHead;
}


