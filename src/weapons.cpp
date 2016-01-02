/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2013 Guus Sliepen
Copyright (C) 2015 onpon4 <onpon4@riseup.net>

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

object weapon[MAX_WEAPONS];

void setWeaponShapes()
{
	for (int i = 0 ; i < MAX_WEAPONS ; i++)
	{
		weapon[i].image[0] = shape[weapon[i].imageIndex[0]];
		weapon[i].image[1] = shape[weapon[i].imageIndex[1]];
	}
}

/*
A list of predefined weaponary.
*/
void initWeapons()
{
	// Player's weapon (this NEVER allocated to anything else)
	weapon[W_PLAYER_WEAPON].id = WT_PLASMA;
	weapon[W_PLAYER_WEAPON].ammo[0] = 1;
	weapon[W_PLAYER_WEAPON].damage = 1;
	weapon[W_PLAYER_WEAPON].reload[0] = 15;
	weapon[W_PLAYER_WEAPON].speed = 10;
	weapon[W_PLAYER_WEAPON].imageIndex[0] = 0;
	weapon[W_PLAYER_WEAPON].imageIndex[1] = 0;
	weapon[W_PLAYER_WEAPON].flags = WF_SPREAD;

	// Single Shot
	weapon[W_SINGLE_SHOT].id = WT_PLASMA;
	weapon[W_SINGLE_SHOT].ammo[0] = 1;
	weapon[W_SINGLE_SHOT].damage = 1;
	weapon[W_SINGLE_SHOT].reload[0] = 15;
	weapon[W_SINGLE_SHOT].speed = 10;
	weapon[W_SINGLE_SHOT].imageIndex[0] = 0;
	weapon[W_SINGLE_SHOT].imageIndex[1] = 1;
	weapon[W_SINGLE_SHOT].flags = 0;

	// Double Shot
	weapon[W_DOUBLE_SHOT] = weapon[W_SINGLE_SHOT];
	weapon[W_DOUBLE_SHOT].ammo[0] = 2;

	// Triple Shot
	weapon[W_TRIPLE_SHOT] = weapon[W_SINGLE_SHOT];
	weapon[W_TRIPLE_SHOT].ammo[0] = 3;

	// Rockets
	weapon[W_ROCKETS].id = WT_ROCKET;
	weapon[W_ROCKETS].ammo[0] = 1;
	weapon[W_ROCKETS].damage = 15;
	weapon[W_ROCKETS].reload[0] = 45;
	weapon[W_ROCKETS].speed = 20;
	weapon[W_ROCKETS].flags = 0;
	weapon[W_ROCKETS].imageIndex[0] = 2;
	weapon[W_ROCKETS].imageIndex[1] = 3;

	// Double Rockets (uses ROCKETS as base)
	weapon[W_DOUBLE_ROCKETS] = weapon[W_ROCKETS];
	weapon[W_DOUBLE_ROCKETS].ammo[0] = 2;
	weapon[W_DOUBLE_ROCKETS].reload[0] = 80;

	// Micro Rockets
	weapon[W_MICRO_ROCKETS].id = WT_ROCKET;
	weapon[W_MICRO_ROCKETS].ammo[0] = 5;
	weapon[W_MICRO_ROCKETS].damage = 6;
	weapon[W_MICRO_ROCKETS].reload[0] = 30;
	weapon[W_MICRO_ROCKETS].speed = 15;
	weapon[W_MICRO_ROCKETS].flags = WF_VARIABLE_SPEED;
	weapon[W_MICRO_ROCKETS].imageIndex[0] = 2;
	weapon[W_MICRO_ROCKETS].imageIndex[1] = 3;

	// Energy Ray
	weapon[W_ENERGYRAY].id = WT_ENERGYRAY;
	weapon[W_ENERGYRAY].ammo[0] = 255;
	weapon[W_ENERGYRAY].damage = 1;
	weapon[W_ENERGYRAY].reload[0] = 25; // reload for energy ray is never used
	weapon[W_ENERGYRAY].speed = 15;
	weapon[W_ENERGYRAY].flags = 0;

	// Laser
	weapon[W_LASER].id = WT_LASER;
	weapon[W_LASER].ammo[0] = 1;
	weapon[W_LASER].damage = 3;
	weapon[W_LASER].reload[0] = 1;
	weapon[W_LASER].speed = 10;
	weapon[W_LASER].imageIndex[0] = 1;
	weapon[W_LASER].imageIndex[1] = 1;
	weapon[W_LASER].flags = 0;

	// Beam up weapon
	weapon[W_CHARGER].id = WT_CHARGER;
	weapon[W_CHARGER].ammo[0] = 1;
	weapon[W_CHARGER].damage = 1;
	weapon[W_CHARGER].reload[0] = 0;
	weapon[W_CHARGER].speed = 12;
	weapon[W_CHARGER].flags = 0;
	weapon[W_CHARGER].imageIndex[0] = 33;
	weapon[W_CHARGER].imageIndex[1] = 34;

	// Homing missile
	weapon[W_HOMING_MISSILE].id = WT_ROCKET;
	weapon[W_HOMING_MISSILE].ammo[0] = 1;
	weapon[W_HOMING_MISSILE].damage = 15;
	weapon[W_HOMING_MISSILE].reload[0] = 35;
	weapon[W_HOMING_MISSILE].speed = 10;
	weapon[W_HOMING_MISSILE].flags = WF_HOMING;
	weapon[W_HOMING_MISSILE].imageIndex[0] = 4;
	weapon[W_HOMING_MISSILE].imageIndex[1] = 4;

	// Double homing missile
	weapon[W_DOUBLE_HOMING_MISSILES] = weapon[W_HOMING_MISSILE];
	weapon[W_DOUBLE_HOMING_MISSILES].ammo[0] = 2;
	weapon[W_DOUBLE_HOMING_MISSILES].reload[0] = 65;
	weapon[W_DOUBLE_HOMING_MISSILES].imageIndex[0] = 4;
	weapon[W_DOUBLE_HOMING_MISSILES].imageIndex[1] = 4;

	// Micro homing missiles
	weapon[W_MICRO_HOMING_MISSILES].id = WT_ROCKET;
	weapon[W_MICRO_HOMING_MISSILES].ammo[0] = 5;
	weapon[W_MICRO_HOMING_MISSILES].damage = 12;
	weapon[W_MICRO_HOMING_MISSILES].reload[0] = 65;
	weapon[W_MICRO_HOMING_MISSILES].speed = 3;
	weapon[W_MICRO_HOMING_MISSILES].flags = WF_HOMING;
	weapon[W_MICRO_HOMING_MISSILES].imageIndex[0] = 4;
	weapon[W_MICRO_HOMING_MISSILES].imageIndex[1] = 4;

	// Aimed plasma bolt
	weapon[W_AIMED_SHOT].id = WT_DIRECTIONAL;
	weapon[W_AIMED_SHOT].ammo[0] = 1;
	weapon[W_AIMED_SHOT].damage = 2;
	weapon[W_AIMED_SHOT].reload[0] = 15;
	weapon[W_AIMED_SHOT].speed = 0;
	weapon[W_AIMED_SHOT].flags = WF_AIMED;
	weapon[W_AIMED_SHOT].imageIndex[0] = 33;
	weapon[W_AIMED_SHOT].imageIndex[1] = 34;

	// 3 way spread weapon
	weapon[W_SPREADSHOT].id = WT_SPREAD;
	weapon[W_SPREADSHOT].ammo[0] = 3;
	weapon[W_SPREADSHOT].damage = 2;
	weapon[W_SPREADSHOT].reload[0] = 10;
	weapon[W_SPREADSHOT].speed = 10;
	weapon[W_SPREADSHOT].flags = WF_SPREAD;
	weapon[W_SPREADSHOT].imageIndex[0] = 0;
	weapon[W_SPREADSHOT].imageIndex[1] = 1;

	// Sid's ion cannon like weapon
	weapon[W_IONCANNON].id = WT_PLASMA;
	weapon[W_IONCANNON].ammo[0] = 1;
	weapon[W_IONCANNON].damage = 1;
	weapon[W_IONCANNON].reload[0] = 2;
	weapon[W_IONCANNON].speed = 10;
	weapon[W_IONCANNON].flags = WF_DISABLE | WF_AIMED;
	weapon[W_IONCANNON].imageIndex[0] = 35;
	weapon[W_IONCANNON].imageIndex[1] = 35;

	// Directional Shock Missile - Used by Kline in final battle
	weapon[W_DIRSHOCKMISSILE].id = WT_ROCKET;
	weapon[W_DIRSHOCKMISSILE].ammo[0] = 5;
	weapon[W_DIRSHOCKMISSILE].damage = 20;
	weapon[W_DIRSHOCKMISSILE].reload[0] = 60;
	weapon[W_DIRSHOCKMISSILE].speed = 0;
	weapon[W_DIRSHOCKMISSILE].flags = WF_AIMED | WF_TIMEDEXPLOSION;
	weapon[W_DIRSHOCKMISSILE].imageIndex[0] = 4;
	weapon[W_DIRSHOCKMISSILE].imageIndex[1] = 4;
}
