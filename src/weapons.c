/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2013 Guus Sliepen
Copyright (C) 2015-2023 Diligent Circle <diligentcircle@riseup.net>

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

#include "defs.h"
#include "structs.h"

Object weapons[W_MAX];

/*
A list of predefined weaponry.
*/
void weapons_init()
{
	// Player's weapon (this NEVER allocated to anything else)
	weapons[W_PLAYER_WEAPON].id = WT_PLASMA;
	weapons[W_PLAYER_WEAPON].ammo[0] = 1;
	weapons[W_PLAYER_WEAPON].damage = 1;
	weapons[W_PLAYER_WEAPON].reload[0] = 15;
	weapons[W_PLAYER_WEAPON].speed = 10;
	weapons[W_PLAYER_WEAPON].imageIndex[0] = SP_PLASMA_GREEN;
	weapons[W_PLAYER_WEAPON].imageIndex[1] = SP_PLASMA_GREEN;
	weapons[W_PLAYER_WEAPON].flags = WF_SPREAD;

	// Single Shot
	weapons[W_SINGLE_SHOT].id = WT_PLASMA;
	weapons[W_SINGLE_SHOT].ammo[0] = 1;
	weapons[W_SINGLE_SHOT].damage = 1;
	weapons[W_SINGLE_SHOT].reload[0] = 15;
	weapons[W_SINGLE_SHOT].speed = 10;
	weapons[W_SINGLE_SHOT].imageIndex[0] = SP_PLASMA_GREEN;
	weapons[W_SINGLE_SHOT].imageIndex[1] = SP_PLASMA_RED;
	weapons[W_SINGLE_SHOT].flags = 0;

	// Double Shot
	weapons[W_DOUBLE_SHOT] = weapons[W_SINGLE_SHOT];
	weapons[W_DOUBLE_SHOT].ammo[0] = 2;

	// Triple Shot
	weapons[W_TRIPLE_SHOT] = weapons[W_SINGLE_SHOT];
	weapons[W_TRIPLE_SHOT].ammo[0] = 3;

	// Rockets
	weapons[W_ROCKETS].id = WT_ROCKET;
	weapons[W_ROCKETS].ammo[0] = 1;
	weapons[W_ROCKETS].damage = 15;
	weapons[W_ROCKETS].reload[0] = 45;
	weapons[W_ROCKETS].speed = 20;
	weapons[W_ROCKETS].flags = 0;
	weapons[W_ROCKETS].imageIndex[0] = SP_ROCKET;
	weapons[W_ROCKETS].imageIndex[1] = SP_ROCKET_L;

	// Double Rockets (uses ROCKETS as base)
	weapons[W_DOUBLE_ROCKETS] = weapons[W_ROCKETS];
	weapons[W_DOUBLE_ROCKETS].ammo[0] = 2;
	weapons[W_DOUBLE_ROCKETS].reload[0] = 80;

	// Micro Rockets
	weapons[W_MICRO_ROCKETS].id = WT_ROCKET;
	weapons[W_MICRO_ROCKETS].ammo[0] = 5;
	weapons[W_MICRO_ROCKETS].damage = 6;
	weapons[W_MICRO_ROCKETS].reload[0] = 30;
	weapons[W_MICRO_ROCKETS].speed = 15;
	weapons[W_MICRO_ROCKETS].flags = WF_VARIABLE_SPEED;
	weapons[W_MICRO_ROCKETS].imageIndex[0] = SP_ROCKET;
	weapons[W_MICRO_ROCKETS].imageIndex[1] = SP_ROCKET_L;

	// Energy Ray
	weapons[W_ENERGYRAY].id = WT_ENERGYRAY;
	weapons[W_ENERGYRAY].ammo[0] = 255;
	weapons[W_ENERGYRAY].damage = 1;
	weapons[W_ENERGYRAY].reload[0] = 25; // reload for energy ray is never used
	weapons[W_ENERGYRAY].speed = 15;
	weapons[W_ENERGYRAY].imageIndex[0] = SP_PLASMA_RED;
	weapons[W_ENERGYRAY].imageIndex[1] = SP_PLASMA_RED;
	weapons[W_ENERGYRAY].flags = 0;

	// Laser
	weapons[W_LASER].id = WT_LASER;
	weapons[W_LASER].ammo[0] = 1;
	weapons[W_LASER].damage = 3;
	weapons[W_LASER].reload[0] = 1;
	weapons[W_LASER].speed = 10;
	weapons[W_LASER].imageIndex[0] = SP_PLASMA_RED;
	weapons[W_LASER].imageIndex[1] = SP_PLASMA_RED;
	weapons[W_LASER].flags = 0;

	// Beam up weapon
	weapons[W_CHARGER].id = WT_CHARGER;
	weapons[W_CHARGER].ammo[0] = 1;
	weapons[W_CHARGER].damage = 1;
	weapons[W_CHARGER].reload[0] = 0;
	weapons[W_CHARGER].speed = 12;
	weapons[W_CHARGER].flags = 0;
	weapons[W_CHARGER].imageIndex[0] = SP_DIR_PLASMA_GREEN;
	weapons[W_CHARGER].imageIndex[1] = SP_DIR_PLASMA_RED;

	// Homing missile
	weapons[W_HOMING_MISSILE].id = WT_ROCKET;
	weapons[W_HOMING_MISSILE].ammo[0] = 1;
	weapons[W_HOMING_MISSILE].damage = 15;
	weapons[W_HOMING_MISSILE].reload[0] = 35;
	weapons[W_HOMING_MISSILE].speed = 10;
	weapons[W_HOMING_MISSILE].flags = WF_HOMING;
	weapons[W_HOMING_MISSILE].imageIndex[0] = SP_SMALL_EXPLOSION;
	weapons[W_HOMING_MISSILE].imageIndex[1] = SP_SMALL_EXPLOSION;

	// Double homing missile
	weapons[W_DOUBLE_HOMING_MISSILES] = weapons[W_HOMING_MISSILE];
	weapons[W_DOUBLE_HOMING_MISSILES].ammo[0] = 2;
	weapons[W_DOUBLE_HOMING_MISSILES].reload[0] = 65;
	weapons[W_DOUBLE_HOMING_MISSILES].imageIndex[0] = SP_SMALL_EXPLOSION;
	weapons[W_DOUBLE_HOMING_MISSILES].imageIndex[1] = SP_SMALL_EXPLOSION;

	// Micro homing missiles
	weapons[W_MICRO_HOMING_MISSILES].id = WT_ROCKET;
	weapons[W_MICRO_HOMING_MISSILES].ammo[0] = 5;
	weapons[W_MICRO_HOMING_MISSILES].damage = 12;
	weapons[W_MICRO_HOMING_MISSILES].reload[0] = 65;
	weapons[W_MICRO_HOMING_MISSILES].speed = 3;
	weapons[W_MICRO_HOMING_MISSILES].flags = WF_HOMING;
	weapons[W_MICRO_HOMING_MISSILES].imageIndex[0] = SP_SMALL_EXPLOSION;
	weapons[W_MICRO_HOMING_MISSILES].imageIndex[1] = SP_SMALL_EXPLOSION;

	// Aimed plasma bolt
	weapons[W_AIMED_SHOT].id = WT_DIRECTIONAL;
	weapons[W_AIMED_SHOT].ammo[0] = 1;
	weapons[W_AIMED_SHOT].damage = 2;
	weapons[W_AIMED_SHOT].reload[0] = 15;
	weapons[W_AIMED_SHOT].speed = 0;
	weapons[W_AIMED_SHOT].flags = WF_AIMED;
	weapons[W_AIMED_SHOT].imageIndex[0] = SP_DIR_PLASMA_GREEN;
	weapons[W_AIMED_SHOT].imageIndex[1] = SP_DIR_PLASMA_RED;

	// 3 way spread weapon
	weapons[W_SPREADSHOT].id = WT_SPREAD;
	weapons[W_SPREADSHOT].ammo[0] = 3;
	weapons[W_SPREADSHOT].damage = 2;
	weapons[W_SPREADSHOT].reload[0] = 10;
	weapons[W_SPREADSHOT].speed = 10;
	weapons[W_SPREADSHOT].flags = WF_SPREAD;
	weapons[W_SPREADSHOT].imageIndex[0] = SP_PLASMA_GREEN;
	weapons[W_SPREADSHOT].imageIndex[1] = SP_PLASMA_RED;

	// Sid's ion cannon like weapon
	weapons[W_IONCANNON].id = WT_PLASMA;
	weapons[W_IONCANNON].ammo[0] = 1;
	weapons[W_IONCANNON].damage = 1;
	weapons[W_IONCANNON].reload[0] = 2;
	weapons[W_IONCANNON].speed = 10;
	weapons[W_IONCANNON].flags = WF_DISABLE | WF_AIMED;
	weapons[W_IONCANNON].imageIndex[0] = SP_ION;
	weapons[W_IONCANNON].imageIndex[1] = SP_ION;

	// Directional Shock Missile - Used by Kline in final battle
	weapons[W_DIRSHOCKMISSILE].id = WT_ROCKET;
	weapons[W_DIRSHOCKMISSILE].ammo[0] = 5;
	weapons[W_DIRSHOCKMISSILE].damage = 20;
	weapons[W_DIRSHOCKMISSILE].reload[0] = 60;
	weapons[W_DIRSHOCKMISSILE].speed = 0;
	weapons[W_DIRSHOCKMISSILE].flags = WF_AIMED | WF_TIMEDEXPLOSION;
	weapons[W_DIRSHOCKMISSILE].imageIndex[0] = SP_SMALL_EXPLOSION;
	weapons[W_DIRSHOCKMISSILE].imageIndex[1] = SP_SMALL_EXPLOSION;
}
