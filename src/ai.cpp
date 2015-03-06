/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011 Guus Sliepen

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

/*
Some very simple artificial intelligence routines for the aliens.
Shouldn't really be called AI since they just do things at random.
Aliens are assigned various AI types and this routine makes use of them.
Levels of aggression, defence and evasion are all here.
*/

void alien_setAI(object *alien)
{
	// Make friendly craft generally concentrate on smaller fighters
	if ((alien->flags & FL_FRIEND) && (alien->target == &enemy[WC_BOSS]))
	{
		if ((rand() % 5) == 0)
		{
			alien->target = alien;
			alien->thinktime = 0;
			return;
		}
	}

	int i = rand() % 10;
	float tx = alien->target->x;
	float ty = alien->target->y;

	int chase = 0; // Chance in 10 of chasing player
	int area = 0; // Chance in 10 of moving to an area around the player
	int stop = 0; // Chance in 10 of hanging back
	int point = 0; // Size of area alien will move into

	switch (alien->AIType)
	{
		case AI_NORMAL:
			chase = 3;
			point = 6;
			stop = 9;
			area = 250;
			break;
		case AI_OFFENSIVE:
			chase = 7;
			point = 8;
			stop = 9;
			area = 50;
			break;
		case AI_DEFENSIVE:
			chase = 2;
			point = 6;
			stop = 8;
			area = 300;
			break;
		case AI_EVASIVE:
			chase = 1;
			point = 8;
			stop = 9;
			area = 600;
			break;
		case AI_WANDER:
			chase = -1;
			point = 0;
			stop = 10;
			area = 1200;
			break;
	}

	if (i <= chase)
	{
		// Chase the target
		alien->dx = ((alien->x - tx) / ((300 / alien->speed)  + rand() % 100));
		alien->dy = ((alien->y - ty) / ((300 / alien->speed)  + rand() % 100));
		return;
	}
	else if ((i >= point) && (i <= stop))
	{
		// Fly to a random point around the target
		tx += (rand() % area - (rand() % area * 2));
		ty += (rand() % area - (rand() % area * 2));
		alien->dx = ((alien->x - tx) / ((300 / alien->speed)  + rand() % 100));
		alien->dy = ((alien->y - ty) / ((300 / alien->speed)  + rand() % 100));
		return;
	}
	else
	{
		// Hang back
		alien->dx = 0;
		alien->dy = 0;
		return;
	}
}

void alien_setKlineAttackMethod(object *alien)
{
	alien->maxShield -= 500;
	if (alien->maxShield == 0)
		alien->flags &= ~FL_CANNOTDIE;

	if (alien->maxShield == 1000)
	{
		setRadioMessage(FACE_KLINE, "Very good, Bainfield. Now let's get a little more serious...", 1);
		alien->weaponType[0] = W_SPREADSHOT;
		alien->chance[1] = 40;
	}
	else if (alien->maxShield == 500)
	{
		setRadioMessage(FACE_KLINE, "Your ability to stay alive irritates me!! Try dodging some of these!!", 1);
		alien->weaponType[0] = W_DIRSHOCKMISSILE;
		alien->weaponType[1] = W_DIRSHOCKMISSILE;
		alien->chance[0] = 2;
		alien->chance[1] = 2;
		alien->flags |= FL_AIMS;
	}
	else if (alien->maxShield == 0)
	{
		setRadioMessage(FACE_KLINE, "ENOUGH!! THIS ENDS NOW!!!", 1);
		alien->weaponType[0] = W_AIMED_SHOT;
		alien->weaponType[1] = W_MICRO_HOMING_MISSILES;
		alien->flags |= FL_CANCLOAK;
		alien->chance[0] = 100;
		alien->chance[1] = 2;
	}

	alien->shield = 500;
}

/*
This AI is exclusively for Kline.
*/
void alien_setKlineAI(object *alien)
{
	// Weapon type change
	if ((rand() % 3) == 0)
	{
		if (currentGame.area != 26)
		{
			alien->flags &= ~FL_AIMS;

			switch(rand() % 2)
			{
				case 0:
					alien->weaponType[0] = W_TRIPLE_SHOT;
					break;
				case 1:
					alien->weaponType[0] = W_AIMED_SHOT;
					alien->flags |= FL_AIMS;
					break;
			}
		}
	}

	alien->flags &= ~(FL_CIRCLES | FL_CONTINUOUS_FIRE | FL_DROPMINES);

	switch(rand() % 10)
	{
		case 0:
			if ((alien->weaponType[0] != W_DIRSHOCKMISSILE) && (alien->weaponType[1] != W_MICRO_HOMING_MISSILES))
				alien->flags |= FL_CONTINUOUS_FIRE;
			alien->dx = ((alien->x - alien->target->x) / ((300 / alien->speed)  + rand() % 100));
			alien->dy = ((alien->y - alien->target->y) / ((300 / alien->speed)  + rand() % 100));
			break;
		case 1:
		case 2:
			// Kline only attacks then he is ready!
			if ((!(alien->flags & FL_NOFIRE)) && (currentGame.area == 11))
				alien->flags |= FL_DROPMINES;
			break;
		case 3:
		case 4:
			alien->flags |= FL_CIRCLES;
			break;
		default:
			alien_setAI(alien);
			break;
	}
}
