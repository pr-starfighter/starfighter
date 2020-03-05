/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011 Guus Sliepen
Copyright (C) 2015-2019 Layla Marchant <diligentcircle@riseup.net>

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

#ifndef COLORS_H
#define COLORS_H

#include "SDL.h"

#include "defs.h"
#include "structs.h"

extern Uint32 red;
extern Uint32 darkRed;
extern Uint32 yellow;
extern Uint32 darkYellow;
extern Uint32 green;
extern Uint32 darkGreen;
extern Uint32 blue;
extern Uint32 darkBlue;
extern Uint32 darkerBlue;
extern Uint32 black;
extern Uint32 white;
extern Uint32 lightGrey;
extern Uint32 darkGrey;

void colors_init();

#endif
