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

#ifndef MISSIONS_H
#define MISSIONS_H

extern Planet systemPlanet[10];
extern mission currentMission;

extern void initPlanetMissions(int system);
extern void checkForBossMission();
extern void updateSystemStatus();
extern void setMission(int mission);
extern void checkTimer();
extern void updateMissionRequirements(int type, int id, int value);
extern int allMissionsCompleted();
extern int missionFailed();
extern void missionBriefScreen();
extern void missionFinishedScreen();
extern void initMissions();

#endif
