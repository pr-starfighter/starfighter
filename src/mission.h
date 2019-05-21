/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011 Guus Sliepen
Copyright (C) 2015-2019 Julie Marchant <onpon4@riseup.net>

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

#ifndef MISSION_H
#define MISSION_H

#include "defs.h"
#include "structs.h"

typedef struct Mission_ {

	char primaryObjective[3][50]; // Description
	int primaryType[3]; // The type of mission this is
	int target1[3]; // index of target in aliens array
	int targetValue1[3]; // Number of things to collect (slaves, cash, etc)
	int timeLimit1[3]; // In minutes
	int completed1[3];

	char secondaryObjective[3][50]; // Description
	int secondaryType[3]; // The type of mission this is
	int target2[3]; // index of target in aliens array
	int targetValue2[3]; // Number of things to collect (slaves, cash, etc)
	int timeLimit2[3]; // In minutes
	int completed2[3];

	int remainingObjectives1;
	int remainingObjectives2;
	int addAliens; // How often to add new enemies

} Mission;

extern Mission mission;

void mission_set(int mission);
void mission_checkTimer();
void mission_updateRequirements(int type, int id, int value);
int mission_checkCompleted();
int mission_checkFailed();
void mission_showStartScreen();
void mission_showFinishedScreen();
void mission_init();

#endif
