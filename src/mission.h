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

#ifndef MISSION_H
#define MISSION_H

extern Mission currentMission;

void mission_set(int mission);
void mission_checkTimer();
void mission_updateRequirements(int type, int id, int value);
int mission_checkCompleted();
int mission_checkFailed();
void mission_showStartScreen();
void mission_showFinishedScreen();
void mission_init();

#endif
