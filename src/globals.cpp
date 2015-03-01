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

devVariables dev;
globalEngineVariables engine;

void defineGlobals()
{
	engine.musicVolume = 100;
	engine.useAudio = true;

	engine.maxAliens = 9;

	engine.ssx = 0;
	engine.ssy = 0;
	engine.smx = 0;
	engine.smy = 0;

	engine.bulletHead = new object;
	engine.bulletHead->next = NULL;
	engine.bulletTail = engine.bulletHead;

	engine.explosionHead = new object;
	engine.explosionHead->next = NULL;
	engine.explosionTail = engine.explosionHead;

	engine.collectableHead = new collectables;
	engine.collectableHead->next = NULL;
	engine.collectableTail = engine.collectableHead;

	engine.debrisHead = new object;
	engine.debrisHead->next = NULL;
	engine.debrisTail = engine.debrisHead;

	engine.commsSection = 0;

	for (int i = 0; i < KEY_LAST; i++)
		engine.keyState[i] = 0;

	engine.eventTimer = 0;
	engine.counter2 = 0;
	engine.timeTaken = 0;
	engine.timeMission = 0;
	engine.counter = 0;
	engine.seconds = 0;
	engine.minutes = 0;
	engine.paused = false;
	engine.gameSection = SECTION_TITLE;

	engine.cheat = false;
	engine.cheatShield = false;
	engine.cheatAmmo = false;
	engine.cheatCash = false;

	// All Development Stuff...
	dev.moveAliens = 1;
	dev.fireAliens = 1;
}
