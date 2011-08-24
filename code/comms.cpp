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

#include "comms.h"

void updateCommsSurface(SDL_Surface *comms)
{
	if (engine.commsSection == 1)
		return;

	char string[255];

	graphics.blevelRect(comms, 0, 10, comms->w - 1, 55, 0x00, 0x22, 0x00);
	graphics.blit(graphics.shape[FACE_CHRIS], 20, 15, comms);
	graphics.drawString("Chris Bainfield", 80, 15, FONT_WHITE, comms);
	sprintf(string, "Current Location: %s", systemPlanet[currentGame.stationedPlanet].name);
	graphics.drawString(string, 80, 35, FONT_WHITE, comms);
}

void createCommsSurface(SDL_Surface *comms)
{
	engine.commsSection = 0;

	graphics.blevelRect(comms, 0, 0, comms->w - 1, comms->h - 1, 0x00, 0x00, 0x25);

	graphics.drawString("+++ RECIEVED MESSAGES +++", 115, 80, FONT_GREEN, comms);

	int yOffset;

	for (int i = 0 ; i < 10 ; i++)
	{
		if ((systemPlanet[i].messageSlot != -1) && (systemPlanet[i].missionCompleted == 0))
		{
			yOffset = systemPlanet[i].messageSlot * 60;
			graphics.blevelRect(comms, 0, 105 + yOffset, comms->w - 1, 55, 0x00, 0x00, 0x77);
			graphics.blit(graphics.shape[systemPlanet[i].faceImage], 20, 110 + yOffset, comms);
			graphics.drawString(systemPlanet[i].from, 80, 110 + yOffset, FONT_WHITE, comms);
			graphics.drawString(systemPlanet[i].subject, 80, 130 + yOffset, FONT_CYAN, comms);
			graphics.drawString("INCOMPLETE", 350, 110 + yOffset, FONT_RED, comms);
		}
	}

	updateCommsSurface(comms);
}

void createMissionDetailSurface(SDL_Surface *comms, int missionSlot)
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
			//printf("Slot %d - Mission %d - Completed %d\n", missionSlot, systemPlanet[i].messageMission, systemPlanet[i].missionCompleted);
			mission = systemPlanet[i].messageMission;
		}
	}

	if (mission == -1)
		return;

	graphics.blevelRect(comms, 0, 0, comms->w - 1, comms->h - 1, 0x00, 0x00, 0x25);

	strcpy(string, "");
	sprintf(string, "data/brief%d.txt", mission);

	#if USEPACK
	int dataLocation = locateDataInPak(string, 1);
	fp = fopen(PACKLOCATION, "rb");
	fseek(fp, dataLocation, SEEK_SET);
	#else
	fp = fopen(string, "rb");
	#endif

	fscanf(fp, "%[^\n]%*c", name);
	sprintf(string, "+++ Communication with %s +++", name);
	graphics.drawString(string, -1, 20, FONT_GREEN, comms);

	fscanf(fp, "%d%*c", &lines);

	for (int i = 0 ; i < lines ; i++)
	{
		fscanf(fp, "%[^\n]%*c", string);
		faceNumber = getFace(string);
		if (faceNumber > -1)
		{
			graphics.blit(graphics.shape[faceNumber], 10, y, comms);
			col = FONT_WHITE;
		}
		else
		{
			newY = graphics.drawString(string, 80, y, col, 1, comms) + 25;
			if (newY < y + 60)
				newY += (60 - (newY - y));
			y = newY;
		}
	}

	fclose(fp);

	graphics.blevelRect(comms, 5, comms->h - 28, 180, 20, 0x25, 0x00, 0x00);
	graphics.drawString("RETURN TO MESSAGES", 15, comms->h - 25, FONT_WHITE, 1, comms);

	engine.commsSection = 1;
}

void doComms(SDL_Surface *comms)
{
	if ((engine.keyState[SDLK_LCTRL]) || (engine.keyState[SDLK_RCTRL]))
	{
		if (engine.commsSection == 0)
		{
			for (int i = 0 ; i < 4 ; i++)
			{
				if (Collision::collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, 170, 180 + (i * 60), 430, 50))
				{
					createMissionDetailSurface(comms, i);
					engine.keyState[SDLK_LCTRL] = engine.keyState[SDLK_RCTRL] = 0;
				}
			}
		}
		else
		{
			if (Collision::collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, 170, 440, 160, 20))
			{
				createCommsSurface(comms);
				engine.keyState[SDLK_LCTRL] = engine.keyState[SDLK_RCTRL] = 0;
			}
		}
	}
}
