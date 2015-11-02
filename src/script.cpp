/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012, 2013 Guus Sliepen
Copyright (C) 2012, 2015 Julian Marchant

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

static cutMsg cutMessage[10];
static event gameEvent[20];

static void setKlineGreeting()
{
	static const char *greet[] = {
		"How nice to see you again, Bainfield!",
		"It all ends here, rebel!",
		"I hope you won't disappoint me this time...",
		"Do you really think you can defeat us?!"
	};

	gameEvent[0].time = 2;
	gameEvent[0].face = FACE_KLINE;
	strcpy(gameEvent[0].message, greet[rand() % 4]);
	gameEvent[0].entity = -1;
	gameEvent[0].flag = 0;
}

void loadScriptEvents()
{
	for (int i = 0 ; i < 20 ; i++)
	{
		gameEvent[i].time = 0;
		strcpy(gameEvent[i].message, "");
		gameEvent[i].entity = -1;
		gameEvent[i].flag = 0;
	}
	
	if ((game.area == MISN_INTERCEPTION) &&
			(aliens[ALIEN_KLINE].classDef == CD_KLINE) &&
			(aliens[ALIEN_KLINE].active))
		setKlineGreeting();

	char filename[255];
	sprintf(filename, "data/script%d.txt", game.area);

	FILE *fp;
	int i = 0;

	int time, entity, flags;
	char face[255], message[255];

	fp = fopen(filename, "rb");
	if (fp == NULL)
		return;

	while (fscanf(fp, "%d %s %d %d ", &time, face, &entity, &flags) == 4)
	{
		if (fscanf(fp, "%[^\n]%*c", message) < 1)
			strcpy(message, "Error: Text missing");

		gameEvent[i].time = time;
		gameEvent[i].face = getFace(face);
		gameEvent[i].entity = entity;
		gameEvent[i].flag = flags;
		strcpy(gameEvent[i].message, message);

		i++;
	}

	fclose(fp);
}

void checkScriptEvents()
{
	for (int i = 0 ; i < 20 ; i++)
	{
		if (engine.timeTaken == gameEvent[i].time)
		{
			if (strcmp(gameEvent[i].message, "@none@") != 0)
			{
				setRadioMessage(gameEvent[i].face, gameEvent[i].message, 1);
			}

			if (gameEvent[i].entity > -1)
			{
				if (gameEvent[i].flag != -FL_ACTIVATE)
				{
					aliens[gameEvent[i].entity].flags += gameEvent[i].flag;
				}
				else
				{
					aliens[gameEvent[i].entity].active = true;
					aliens[gameEvent[i].entity].x = ((int)player.x +
						RANDRANGE(400, 800));
					aliens[gameEvent[i].entity].y = ((int)player.y +
						RANDRANGE(-400, 800));
				}
			}

			gameEvent[i].time = 0;
		}
	}
}

void syncScriptEvents()
{
	for (int i = 0 ; i < 20 ; i++)
	{
		if (gameEvent[i].time < 0)
		{
			gameEvent[i].time = engine.timeTaken + abs(gameEvent[i].time);
		}
	}
}

static void setScene(int scene)
{
	FILE *fp;
	char string[255], face[255];
	float sx, sy, x, y, speed;
	int index, shape;

	sprintf(string, "data/cutscene%d.dat", scene);

	fp = fopen(string, "rb");

	// Load in the specified background
	if (fscanf(fp, "%s", string) < 1)
	{
		printf("Warning: didn't find a background definition for \"%s\"\n", string);
		strcpy(string, "gfx/spirit.jpg");
	}
	loadBackground(string);

	// Set the star speed
	if (fscanf(fp, "%f %f", &sx, &sy) < 2)
		printf("Warning: failed to read star speed data for cutscene");
	engine.ssx = sx;
	engine.ssy = sy;

	// Read in the specs for each ship
	while (fscanf(fp, "%d %d %f %f %f", &index, &shape, &x, &y, &speed) == 5)
	{
		if (x < 0) x = (rand() % abs((int)x));
		if (y < 0) y = (rand() % abs((int)y));
		if (speed <= -1) speed = 1 + (rand() % abs((int)speed));

		if (shape > -1)
		{
			aliens[index].image[0] = shipShape[shape];
			aliens[index].x = x;
			aliens[index].y = y;
			aliens[index].dx = speed;
			aliens[index].active = true;
		}
	}

	// And finally read in the messages
	index = 0;
	while (fscanf(fp, "%s%*c %[^\n]", face, string) == 2)
	{
		if (strcmp(string, "@none@") == 0)
			break;

		cutMessage[index].face = getFace(face);
		strcpy(cutMessage[index].message, string);

		index++;
	}

	fclose(fp);
}

void doCutscene(int scene)
{
	clearScreen(black);
	renderer_update();
	clearScreen(black);

	engine.keyState[KEY_FIRE] = 0; 
	engine.keyState[KEY_ALTFIRE] = 0;

	engine.ssx = -0.5;
	engine.ssy = 0;
	engine.smx = 0;
	engine.smy = 0;

	screen_flushBuffer();
	freeGraphics();
	resetLists();
	loadGameGraphics();

	for (int i = 0 ; i < 15 ; i++)
	{
		aliens[i] = alien_defs[0];
		aliens[i].face = 0;
		aliens[i].active = false;
	}

	for (int i = 0 ; i < 10 ; i++)
	{
		strcpy(cutMessage[i].message, "");
		cutMessage[i].face = -1;
	}

	setScene(scene);

	/*
		Because we can fiddle with the images, we need to set the engines to 
		the correct places on the craft. Otherwise it will look wrong
	*/
	for (int i = 0 ; i < 15 ; i++)
	{
		aliens[i].engineX = aliens[i].image[0]->w;
		aliens[i].engineY = (aliens[i].image[0]->h / 2);
	}

	bool showMessage = false;
	signed char currentMessage = -1;
	int timer = 60 * 4;

	drawBackGround();

	SDL_Surface *face;
	
	flushInput();

	while (true)
	{
		renderer_update();
		screen_unBuffer();
		getPlayerInput();
		game_doStars();
		game_doExplosions();

		for (int i = 0 ; i < 15 ; i++)
		{
			if (aliens[i].active)
			{
				explosion_addEngine(&aliens[i]);
				if (scene == 0 && i > 0 && (timer % 15) == i) {
					aliens[i].dx += (drand48() - 0.5) * 0.1;
					aliens[i].dy += (drand48() - 0.5) * 0.1;
					if (aliens[i].x > 500 - timer)
						aliens[i].dx -= 0.2;
					if (aliens[i].x < 0)
						aliens[i].dx += 0.2;
				}
				aliens[i].x += aliens[i].dx;
				aliens[i].y += aliens[i].dy;
				aliens[i].x += engine.ssx + engine.smx;
				screen_blit(aliens[i].image[0], (int)aliens[i].x, (int)aliens[i].y);
				if (aliens[i].x > (screen->w + 50))
				{
					aliens[i].x = -50;
					aliens[i].y = rand() % (screen->h - 40);
				}
				if (aliens[i].y < -50)
					aliens[i].y = (screen->h + 50);
				if (aliens[i].y > (screen->h + 50))
					aliens[i].y = -50;
			}
		}

		timer--;
		if (timer == 0)
		{
			showMessage = !showMessage;
			timer = 120;
			if (showMessage)
			{
				timer = 60 * 7;
				currentMessage++;

				if (currentMessage == 10)
					break;

				if (strcmp(cutMessage[currentMessage].message, "") == 0)
					break;

				face = NULL;
				if (cutMessage[currentMessage].face != -1)
					face = shape[cutMessage[currentMessage].face];
				createMessageBox(face, cutMessage[currentMessage].message, 0);
			}
		}

		if ((showMessage) && (messageBox != NULL))
			screen_blit(messageBox, (screen->w - messageBox->w) / 2, screen->h - 100);

		drawString("Press [Escape] to skip", -1, 580, FONT_WHITE);

		delayFrame();

		if ((engine.keyState[KEY_ESCAPE]) || (engine.keyState[KEY_FIRE]) ||
				(engine.keyState[KEY_ALTFIRE]))
			break;
	}

	screen_flushBuffer();
	freeGraphics();
	clearScreen(black);
	renderer_update();
}
