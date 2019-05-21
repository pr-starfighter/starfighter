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

#ifndef INTERMISSION_H
#define INTERMISSION_H

#include "defs.h"
#include "structs.h"

extern Planet intermission_planets[MAX_PLANETS];

#define INTERMISSION_YCENTER (50 + (screen->h - 150) / 2)

void intermission_initPlanets(int system);
void intermission_unlockPlanets();
void intermission_updateSystemStatus();
int intermission();

#endif
