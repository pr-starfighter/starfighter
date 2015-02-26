/*
Copyright (C) 2003 Parallel Realities

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

void updateCommsSurface(SDL_Surface *comms)
{
	if (engine.commsSection == 1)
		return;

	char string[255];

	blevelRect(comms, 0, 10, comms->w - 1, 55, 0x00, 0x22, 0x00);
	blit(shape[FACE_CHRIS], 20, 15, comms);
	drawString("Chris Bainfield", 80, 15, FONT_WHITE, comms);
	sprintf(string, "Current Location: %s", systemPlanet[currentGame.stationedPlanet].name);
	drawString(string, 80, 35, FONT_WHITE, comms);
}

void createCommsSurface(SDL_Surface *comms)
{
	engine.commsSection = 0;

	blevelRect(comms, 0, 0, comms->w - 1, comms->h - 1, 0x00, 0x00, 0x25);

	drawString("+++ RECEIVED MESSAGES +++", 115, 80, FONT_GREEN, comms);

	int yOffset;

	for (int i = 0 ; i < 10 ; i++)
	{
		if ((systemPlanet[i].messageSlot != -1) && (systemPlanet[i].missionCompleted == 0))
		{
			yOffset = systemPlanet[i].messageSlot * 60;
			blevelRect(comms, 0, 105 + yOffset, comms->w - 1, 55, 0x00, 0x00, 0x77);
			blit(shape[systemPlanet[i].faceImage], 20, 110 + yOffset, comms);
			drawString(systemPlanet[i].from, 80, 110 + yOffset, FONT_WHITE, comms);
			drawString(systemPlanet[i].subject, 80, 130 + yOffset, FONT_CYAN, comms);
			drawString("INCOMPLETE", 350, 110 + yOffset, FONT_RED, comms);
		}
	}

	updateCommsSurface(comms);
}

static void createMissionDetailSurface(SDL_Surface *comms, int missionSlot)
{
	char name[50];
	char string[2000];
	int lines = 0;
	int y = 50;
	int newY = y;
	int col = FONT_WHITE;
	int mission = -1;
	int faceNumber = -1;
	FILE *fp;

	for (int i = 0 ; i < 10 ; i++)
	{
		if ((systemPlanet[i].messageSlot == missionSlot) && (systemPlanet[i].missionCompleted == 0))
		{
			mission = systemPlanet[i].messageMission;
		}
	}

	if (mission == -1)
		return;

	blevelRect(comms, 0, 0, comms->w - 1, comms->h - 1, 0x00, 0x00, 0x25);

	sprintf(string, "data/brief%d.txt", mission);

	fp = fopen(string, "rb");

	fscanf(fp, "%[^\n]%*c", name);
	sprintf(string, "+++ Communication with %s +++", name);
	drawString(string, -1, 20, FONT_GREEN, comms);

	fscanf(fp, "%d%*c", &lines);

	for (int i = 0 ; i < lines ; i++)
	{
		fscanf(fp, "%[^\n]%*c", string);
		faceNumber = getFace(string);
		if (faceNumber > -1)
		{
			blit(shape[faceNumber], 10, y, comms);
			col = FONT_WHITE;
		}
		else
		{
			newY = drawString(string, 80, y, col, 1, comms) + 25;
			if (newY < y + 60)
				newY += (60 - (newY - y));
			y = newY;
		}
	}

	fclose(fp);

	blevelRect(comms, 5, comms->h - 28, 180, 20, 0x25, 0x00, 0x00);
	drawString("RETURN TO MESSAGES", 15, comms->h - 25, FONT_WHITE, 1, comms);

	engine.commsSection = 1;
}

void doComms(SDL_Surface *comms)
{
	if ((engine.keyState[KEY_FIRE]))
	{
		if (engine.commsSection == 0)
		{
			for (int i = 0 ; i < 4 ; i++)
			{
				if (collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, 170, 180 + (i * 60), 430, 50))
				{
					createMissionDetailSurface(comms, i);
					engine.keyState[KEY_FIRE] = 0;
				}
			}
		}
		else
		{
			if (collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, 170, 440, 160, 20))
			{
				createCommsSurface(comms);
				engine.keyState[KEY_FIRE] = 0;
			}
		}
	}
}
