/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011 Guus Sliepen
Copyright (C) 2015, 2016 Julie Marchant <onpon4@riseup.net>

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

#ifndef ENGINE_H
#define ENGINE_H

#include "SDL.h"

#include "defs.h"
#include "structs.h"

#include "collectable.h"


typedef struct Engine_ {

	SDL_Event event;
	int done;

	SDL_RWops *sdlrw;

	float musicVolume;

	int maxAliens;

	float ssx;
	float ssy;
	float smx;
	float smy;

	Object *bulletHead;
	Object *bulletTail;
	Object *explosionHead;
	Object *explosionTail;
	Collectable *collectableHead;
	Collectable *collectableTail;
	Object *debrisHead;
	Object *debrisTail;

	int cursor_x, cursor_y;

	int commsSection;

	int eventTimer;

	int lowShield;
	int averageShield;

	float targetShield;
	int targetIndex;

	// Mission completion timer (allows for 4 seconds before leaving sector)
	Uint32 missionCompleteTimer;

	// Times the mission normally
	Uint32 counter2;
	long timeTaken; // In seconds

	// For missions with a time limit
	int timeMission;
	Uint32 counter;
	int seconds;
	int minutes;

	// Mission Related stuff
	int allAliensDead;
	int addAliens;

	int paused;
	int gameSection;

	int useAudio;
	int useSound;
	int useMusic;
	int fullScreen;
	int autoPause;

	char configDirectory[1024];
	
	char keyState[KEY_LAST];

	int cheat; // overall cheat
	int cheatShield;
	int cheatCash;
	int cheatAmmo;
	int cheatTime;

} Engine;


extern Engine engine;

void engine_init();
void engine_showError(int errorId, const char *name);
void engine_setupConfigDirectory();
void engine_setMode();
void engine_resetLists();
void engine_cleanup();

#endif
