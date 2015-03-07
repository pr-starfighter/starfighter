/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012, 2013 Guus Sliepen
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

void clearInfoLines()
{
	for (int i = 0 ; i < 4 ; i++)
	{
		textShape[i].life = 0;
	}
}

// from a to b
static void copyInfoLine(int a, int b)
{
	textSurface(b, textShape[a].text, -1, 0, textShape[a].fontColor);
	textShape[b].life = textShape[a].life;
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

	for (int i = 0 ; i < 3 ; i++)
	{
		if ((textShape[i].life == 0) && (index == -1))
		{
			index = i;
		}
	}

	// Bump down
	if (index == -1)
	{
		index = 2;
		copyInfoLine(1, 0);
		copyInfoLine(2, 1);
	}

	textSurface(index, in, -1, 0, color);
	textShape[index].life = 240;
}

/*
Sets a radio message that appears at the top of the screen. Used for
script events, etc. We send a message priority too, since we don't want
Phoebe or Ursula's banter to interrupt an important message
*/
void setRadioMessage(signed char face, const char *in, int priority)
{
	if ((textShape[3].life > 0) && (priority == 0))
		return;

	textSurface(3, in, -1, 50, FONT_WHITE);
	textShape[3].life = 240;

	SDL_Surface *faceShape = NULL;
	if (face > -1)
		faceShape = shape[face];

	createMessageBox(faceShape, in, 1);
}

/*
Draw an arrow at the edge of the screen for each enemy ship that is not visible.
*/
static void doArrow(int i)
{
	if (i < 0 || !aliens[i].active || aliens[i].shield <= 0 || aliens[i].flags & FL_ISCLOAKED)
		return;

	int x = aliens[i].x + aliens[i].image[0]->w / 2;
	int y = aliens[i].y + aliens[i].image[0]->h / 2;

	float sx = fabsf((x - (screen->w / 2)) / (screen->w / 2.0));
	float sy = fabsf((y - (screen->h / 2)) / (screen->h / 2.0));
	float sxy = max(sx, sy);

	if (sxy < 1) 
		return;

	x = screen->w / 2 + (x - screen->w / 2) / sxy;
	y = screen->h / 2 + (y - screen->h / 2) / sxy;

	int arrow;

	if (sxy == sx) {
		arrow = x < screen->w / 2 ? 42 : 38;
		x -= x > screen->w / 2 ? shape[arrow]->w : 0;
		y -= shape[arrow]->h / 2;
	} else {
		arrow = y < screen->h / 2 ? 36 : 40;
		x -= shape[arrow]->w / 2;
		y -= y > screen->h / 2 ? shape[arrow]->h : 0;
	}

	blit(shape[arrow], x, y);

	if (i != engine.targetIndex)
		return;

	if (textShape[3].life > 0)
		return;

	if (sxy == sx) {
		x -= x > screen->w / 2 ? 5 + shape[44]->w : -5 - shape[arrow]->w;
		y -= (shape[44]->h - shape[arrow]->h) / 2;
	} else {
		x -= (shape[44]->w - shape[arrow]->w) / 2;
		y -= y > screen->h / 2 ? 5 + shape[44]->h : -5 - shape[arrow]->h;
	}

	blit(shape[44], x, y);
}

/*
Fill in later...
*/
void doInfo()
{
	int shieldColor = 0;
	SDL_Rect bar;
	signed char fontColor;
	char text[25];

	addBuffer(0, 20, 800, 25);
	addBuffer(0, 550, 800, 34);

	if (engine.minutes > -1)
	{
		if ((engine.minutes == 0) && (engine.seconds <= 29))
			fontColor = FONT_RED;
		else if ((engine.minutes == 0) && (engine.seconds > 29))
			fontColor = FONT_YELLOW;
		else
			fontColor = FONT_WHITE;
		blitText(10); // time remaining
		sprintf(text, "%.2d:%.2d", engine.minutes, engine.seconds);
		textSurface(30, text, 410, 21, fontColor);
		blitText(30);
	}

	if (currentGame.area != MAX_MISSIONS - 1)
	{
		blitText(9); // mission objectives
		sprintf(text, "%d", (currentMission.remainingObjectives1 + currentMission.remainingObjectives2));
		textSurface(39, text, 745, 21, FONT_WHITE);
		blitText(39);
	}

	blitText(8); // cash
	sprintf(text, "%.6d", currentGame.cash);
	textSurface(38, text, 90, 21, FONT_WHITE);
	blitText(38);

	for (int i = 0; i < MAX_ALIENS; i++)
		doArrow(i);

	fontColor = FONT_WHITE;
	if (player.ammo[0] > 0)
	{
		if (player.ammo[0] <= 25) fontColor = FONT_YELLOW;
		if (player.ammo[0] <= 10) fontColor = FONT_RED;
	}
	blitText(5); // plasma ammo
	sprintf(text, "%.3d", player.ammo[0]);
	textSurface(35, text, 320, 551, fontColor);
	blitText(35);

	blitText(6);

	if ((player.weaponType[1] != W_CHARGER) && (player.weaponType[1] != W_LASER))
	{
		if (player.ammo[1] == 1)
			fontColor = FONT_RED;
		else
			fontColor = FONT_WHITE;
		sprintf(text, "%.3d", player.ammo[1]); // rocket ammo
		textSurface(36, text, 465, 551, fontColor);
		blitText(36);
	}

	if (((player.weaponType[1] == W_CHARGER) || (player.weaponType[1] == W_LASER)) && (player.ammo[1] > 0))
	{
		int c = white;
		if (player.ammo[1] > 100)
			c = red;

		bar.x = 450;
		bar.y = 550;
		bar.h = 12;

		for (int i = 0 ; i < (player.ammo[1] / 5) ; i++)
		{
			bar.w = 1;
			SDL_FillRect(screen, &bar, c);
			bar.x += 2;
		}
	}

	if ((!allMissionsCompleted()) && (SDL_GetTicks() >= engine.counter2))
	{
		engine.timeTaken++;
		engine.counter2 = SDL_GetTicks() + 1000;
		if (engine.missionCompleteTimer == 0)
			checkScriptEvents();
	}

	if ((engine.timeMission) && (!engine.cheatTime) && (player.shield > 0))
	{
		if (SDL_GetTicks() >= engine.counter)
		{
			if ((engine.seconds > 1) && (engine.seconds <= 11) && (engine.minutes == 0))
			{
				playSound(SFX_CLOCK);
			}

			if (engine.seconds > 0)
			{
				engine.seconds--;
				engine.counter = (SDL_GetTicks() + 1000);
			}
			else if ((engine.seconds == 0) && (engine.minutes > 0))
			{
				engine.minutes--;
				engine.seconds = 59;
				engine.counter = (SDL_GetTicks() + 1000);
				for (int i = 0 ; i < 3 ; i++)
				{
					if (currentMission.timeLimit1[i] > -1)
						currentMission.timeLimit1[i]--;
					if (currentMission.timeLimit2[i] > -1)
						currentMission.timeLimit2[i]--;
				}
				checkTimer();
				checkScriptEvents();
			}

			if ((engine.seconds == 0) && (engine.minutes == 0))
			{
				for (int i = 0 ; i < 3 ; i++)
				{
					if (currentMission.timeLimit1[i] > -1)
						currentMission.timeLimit1[i]--;
					if (currentMission.timeLimit2[i] > -1)
						currentMission.timeLimit2[i]--;
				}
				checkTimer();
				checkScriptEvents();
				engine.counter = (SDL_GetTicks() + 1000);
			}
		}
	}

	for (int i = 0 ; i < 3 ; i++)
	{
		if (textShape[i].life > 0)
		{
			textShape[i].y = (525 - (i * 20));
			blitText(i);
			textShape[i].life--;

			if (textShape[i].life == 0)
			{
				copyInfoLine(i + 1, i);
				copyInfoLine(i + 2, i + 1);
				textShape[2].life = 0;
			}
		}
	}

	// Show the radio message if there is one
	if (textShape[3].life > 0)
	{
		blit(messageBox, (800 - messageBox->w) / 2, 50);
		textShape[3].life--;
	}

	// Do the target's remaining shield (if required)
	if (currentGame.area != 10)
	{
		if ((engine.targetIndex > -1) && (aliens[engine.targetIndex].shield > 0) && (engine.targetIndex > 9))
		{
			blitText(7);
			bar.w = 1;
			bar.h = 12;
			bar.x = 620;
			bar.y = 550;

			for (float i = 0 ; i < (engine.targetShield * aliens[engine.targetIndex].shield) ; i++)
			{
				if (i > 50)
					shieldColor = green;
				else if ((i >= 25) && (i <= 50))
					shieldColor = yellow;
				else
					shieldColor = red;
				SDL_FillRect(screen, &bar, shieldColor);
				bar.x += 2;
			}
		}
	}

	blitText(11);

	bar.w = 25;
	bar.h = 12;
	bar.x = 80;
	bar.y = 571;

	for (int i = 1 ; i <= 5 ; i++)
	{
		if (weapon[W_PLAYER_WEAPON].damage >= i) {
			if(i <= currentGame.maxPlasmaDamage || (SDL_GetTicks() % 1000 > (unsigned)i * 100))
			{
				SDL_FillRect(screen, &bar, green);
			}
		} else if (i <= currentGame.maxPlasmaDamage)
			SDL_FillRect(screen, &bar, darkGreen);
		bar.x += 30;
	}

	blitText(12);

	bar.w = 25;
	bar.h = 12;
	bar.x = 315;
	bar.y = 571;
	SDL_FillRect(screen, &bar, yellow);

	for (int i = 1 ; i <= 5 ; i++)
	{
		if (weapon[W_PLAYER_WEAPON].ammo[0] >= i) {
			if(i <= currentGame.maxPlasmaOutput || (SDL_GetTicks() % 1000 > (unsigned)i * 100))
			{
				SDL_FillRect(screen, &bar, yellow);
			}
		}
		else if (i <= currentGame.maxPlasmaOutput)
			SDL_FillRect(screen, &bar, darkYellow);
		bar.x += 30;
	}

	blitText(13);

	bar.w = 25;
	bar.h = 12;
	bar.x = 550;
	bar.y = 571;

	for (int i = 1 ; i <= 5 ; i++)
	{
		if (weapon[W_PLAYER_WEAPON].reload[0] <= rate2reload[i]) {
			if(i <= currentGame.maxPlasmaRate || (SDL_GetTicks() % 1000 > (unsigned)i * 100))
			{
				SDL_FillRect(screen, &bar, blue);
			}
		}
		else if (i <= currentGame.maxPlasmaRate)
			SDL_FillRect(screen, &bar, darkerBlue);
		bar.x += 30;
	}

	blitText(4);
	if (player.shield < 1)
		return;

	if ((!engine.keyState[KEY_ALTFIRE]) && (player.weaponType[1] == W_LASER) && (engine.eventTimer % 8 == 1))
		limitCharAdd(&player.ammo[1], -1, 1, 255);

	if ((engine.eventTimer < 30) && (player.shield <= engine.lowShield))
		return;

	signed char blockSize = 1;

	bar.w = blockSize;
	bar.h = 12;
	bar.x = 95;
	bar.y = 550;

	for (int i = 0 ; i < player.shield ; i += blockSize)
	{
		if (i >= engine.averageShield)
			shieldColor = green;
		else if ((i >= engine.lowShield) && (i < engine.averageShield))
			shieldColor = yellow;
		else
			shieldColor = red;
		SDL_FillRect(screen, &bar, shieldColor);
		bar.x += blockSize;
		if (player.maxShield < 75)
			bar.x++;
	}
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
			return 90 + i;
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
	
	r1 = bufferHead->next;
	while (r1 != NULL)
	{
		r2 = r1;
		r1 = r1->next;
		delete r2;
	}
	
	bufferHead->next = NULL;
	bufferTail = bufferHead;

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


