/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012, 2013 Guus Sliepen
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

#ifndef STRUCTS_H
#define STRUCTS_H

#include "SDL.h"

typedef struct Object_ {

	int active;
	int classDef; // Used by aliens to determine what they are
	int AIType; // Type of articifial intelligence
	int AITypeOriginal; // AIType in Classic difficulty

	int id; // The "job" of the Object
	struct Object_ *target; //  index target in aliens array

	int reload[2];

	int systemPower; // computer systems for craft
	int shield;   // current shield
	int maxShield; // max shield (for recharging)
	int deathCounter; // how long to explode for

	int speed;
	int damage; // Contact damage for bullets
	int ammo[2]; // Ammo for 2nd weapon.

	int face; // Either 0 or 1

	struct Object_ *owner; // Who owns this Object

	int chance[2]; // Chance of using the weapons (out of 1000)

	SDL_Surface *image[2]; // For facing left and right
	int imageIndex[2]; // used for loading
	int hit; // used to make a craft "flash" if it is struck by a shot

	int engineX; // The place for the engine on the other side of the craft
	int engineY; // The middle of the engine on the craft

	int thinktime; // When the Object will next react
	int weaponType[2]; // Weapon types

	int collectChance; // Chance of dropping the Object
	int collectType; // What the Object is carrying
	int collectValue; // What it is worth

	int collectTypeOriginal; // collectType in Classic difficulty
	int collectValueOriginal; // collectValue in Classic difficulty
	int score; // Classic difficulty only; money earned by killing

	unsigned long int flags; // Various flags for an Object

	float x, y, dx, dy;

	struct Object_ *next;

} Object;

typedef struct TextObject_ {

	SDL_Surface *image;
	int life;
	float x, y;
	int fontColor;
	char text[STRMAX];

} TextObject;

typedef struct LinkedRect_ {

	int x, y, w, h;
	struct LinkedRect_ *next;

} LinkedRect;

typedef struct Planet_ {

	int dist;
	char name[50];
	SDL_Surface *image;

	int missionNumber; // associated mission number
	int missionCompleted; // whether it has been completed

	int messageMission;
	int messageSlot;
	int faceImage;
	char from[50];
	char subject[100];

} Planet;

#endif
