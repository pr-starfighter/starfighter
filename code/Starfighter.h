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

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_mixer.h"

#include "defs.h"
#include "structs.h"
#include "classes.h"

extern void initSystem();
extern void loadGameGraphics();
extern void loadSound();
extern void initWeapons();
extern void initMissions();
extern void initVars();
extern void showStory();
extern int doTitle();
extern void defineAliens();
extern void loadFont();
extern int galaxyMap();
extern void defineGlobals();
extern void cleanUp();
extern int mainGameLoop();
extern void setAllyMessages();
extern void newGame();
extern void doCredits();
extern void getPlayerInput();
extern void initPlanetMissions(signed char system);
extern void updateSystemStatus();
extern void doCutscene(int scene);

globalEngineVariables engine;
devVariables dev;
Game currentGame;
object player;
mission currentMission;
Graphics graphics;

object enemy[MAX_ALIENS];
Star star[200];
Planet systemPlanet[10];
mission missions[MAX_MISSIONS];
ShopItem shopItems[MAX_SHOPITEMS];
Mix_Chunk *sound[MAX_SOUNDS];
object weapon[MAX_WEAPONS];
object cargo[MAX_CARGO];
event gameEvent[10];
cutMsg cutMessage[10];
