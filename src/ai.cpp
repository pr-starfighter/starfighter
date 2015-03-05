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

void alien_setAI(object *theEnemy)
{
	// Make friendly craft generally concentrate on smaller fighters
	if ((theEnemy->flags & FL_FRIEND) && (theEnemy->target == &enemy[WC_BOSS]))
	{
		if ((rand() % 5) == 0)
		{
			theEnemy->target = theEnemy;
			theEnemy->thinktime = 0;
			return;
		}
	}

	int i = rand() % 10;
	float tx = theEnemy->target->x;
	float ty = theEnemy->target->y;

	int chase = 0; // Chance in 10 of chasing player
	int area = 0; // Chance in 10 of moving to an area around the player
	int stop = 0; // Chance in 10 of hanging back
	int point = 0; // Size of area alien will move into

	switch (theEnemy->AIType)
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
		theEnemy->dx = ((theEnemy->x - tx) / ((300 / theEnemy->speed)  + rand() % 100));
		theEnemy->dy = ((theEnemy->y - ty) / ((300 / theEnemy->speed)  + rand() % 100));
		return;
	}
	else if ((i >= point) && (i <= stop))
	{
		// Fly to a random point around the target
		tx += (rand() % area - (rand() % area * 2));
		ty += (rand() % area - (rand() % area * 2));
		theEnemy->dx = ((theEnemy->x - tx) / ((300 / theEnemy->speed)  + rand() % 100));
		theEnemy->dy = ((theEnemy->y - ty) / ((300 / theEnemy->speed)  + rand() % 100));
		return;
	}
	else
	{
		// Hang back
		theEnemy->dx = 0;
		theEnemy->dy = 0;
		return;
	}
}

void alien_setKlineAttackMethod(object *theEnemy)
{
	theEnemy->maxShield -= 500;
	if (theEnemy->maxShield == 0)
		theEnemy->flags &= ~FL_CANNOTDIE;

	if (theEnemy->maxShield == 1000)
	{
		setRadioMessage(FACE_KLINE, "Very good, Bainfield. Now let's get a little more serious...", 1);
		theEnemy->weaponType[0] = W_SPREADSHOT;
		theEnemy->chance[1] = 40;
	}
	else if (theEnemy->maxShield == 500)
	{
		setRadioMessage(FACE_KLINE, "Your ability to stay alive irritates me!! Try dodging some of these!!", 1);
		theEnemy->weaponType[0] = W_DIRSHOCKMISSILE;
		theEnemy->weaponType[1] = W_DIRSHOCKMISSILE;
		theEnemy->chance[0] = 2;
		theEnemy->chance[1] = 2;
		theEnemy->flags |= FL_AIMS;
	}
	else if (theEnemy->maxShield == 0)
	{
		setRadioMessage(FACE_KLINE, "ENOUGH!! THIS ENDS NOW!!!", 1);
		theEnemy->weaponType[0] = W_AIMED_SHOT;
		theEnemy->weaponType[1] = W_MICRO_HOMING_MISSILES;
		theEnemy->flags |= FL_CANCLOAK;
		theEnemy->chance[0] = 100;
		theEnemy->chance[1] = 2;
	}

	theEnemy->shield = 500;
}

/*
This AI is exclusively for Kline.
*/
void alien_setKlineAI(object *theEnemy)
{
	// Weapon type change
	if ((rand() % 3) == 0)
	{
		if (currentGame.area != 26)
		{
			theEnemy->flags &= ~FL_AIMS;

			switch(rand() % 2)
			{
				case 0:
					theEnemy->weaponType[0] = W_TRIPLE_SHOT;
					break;
				case 1:
					theEnemy->weaponType[0] = W_AIMED_SHOT;
					theEnemy->flags |= FL_AIMS;
					break;
			}
		}
	}

	theEnemy->flags &= ~(FL_CIRCLES | FL_CONTINUOUS_FIRE | FL_DROPMINES);

	switch(rand() % 10)
	{
		case 0:
			if ((theEnemy->weaponType[0] != W_DIRSHOCKMISSILE) && (theEnemy->weaponType[1] != W_MICRO_HOMING_MISSILES))
				theEnemy->flags |= FL_CONTINUOUS_FIRE;
			theEnemy->dx = ((theEnemy->x - theEnemy->target->x) / ((300 / theEnemy->speed)  + rand() % 100));
			theEnemy->dy = ((theEnemy->y - theEnemy->target->y) / ((300 / theEnemy->speed)  + rand() % 100));
			break;
		case 1:
		case 2:
			// Kline only attacks then he is ready!
			if ((!(theEnemy->flags & FL_NOFIRE)) && (currentGame.area == 11))
				theEnemy->flags |= FL_DROPMINES;
			break;
		case 3:
		case 4:
			theEnemy->flags |= FL_CIRCLES;
			break;
		default:
			alien_setAI(theEnemy);
			break;
	}
}
