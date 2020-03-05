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

#ifndef _CARGO_H_
#define _CARGO_H_

#include "defs.h"
#include "structs.h"

extern Object cargo[MAX_CARGO];

void cargo_init();
Object *cargo_add(Object *owner, int cargoType);
void cargo_becomeCollectable(int i);

#endif
