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

#include <stdlib.h>
#include <string.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>

#include "defs.h"
#include "structs.h"
#include "classes.h"

extern void resetLists();
extern void setMission(int mission);
extern void missionBriefScreen();
extern void initPlayer();
extern void initAliens();
extern signed char allMissionsCompleted();
extern void getPlayerInput();
extern void leaveSector();
extern void doStarfield();
extern void doCollectables();
extern void doCargo();
extern void doBullets();
extern void doAliens();
extern void doPlayer();
extern void doExplosions();
extern void doDebris();
extern void doInfo();
extern signed char allMissionsCompleted();
extern signed char checkPauseRequest();
extern signed char addAlien();
extern void missionFinishedScreen();
extern void gameover();
extern void clearInfoLines();
extern signed char missionFailed();
extern void playSound(int sid);
extern void loadScriptEvents();
extern void doCutscene(int scene);
extern void addCollectable(float x, float y, int type, int value, int life);
extern void doCredits();
extern void updateSystemStatus();
extern void saveGame(int slot);
extern void flushInput();

extern void initWeapons();
extern void initMissions();
extern void initCargo();
extern void initPlanetMissions(signed char system);

extern globalEngineVariables engine;
extern object player;
extern object enemy[MAX_ALIENS];
extern mission currentMission;
extern Game currentGame;
extern Graphics graphics;
