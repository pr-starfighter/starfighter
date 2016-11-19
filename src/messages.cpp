/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012 Guus Sliepen
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

#include "Starfighter.h"

static const char *missFireMessage[5] = {
	"I am NOT your enemy!",
	"Hey! Watch it!",
	"What are you doing?! Shoot THEM!",
	"OW! I hope that was an accident!",
	"Open your eyes!",
};

static const char *playerHitMessage[3] = {
	"Oops! Sorry!",
	"Get out of the way!",
	"Don't fly into my missiles!",
};

void getMissFireMessage(object *ally)
{
	int faceToUse = FS_PHOEBE;

	if (ally == &aliens[ALIEN_PHOEBE])
		faceToUse = FS_PHOEBE;
	else
		faceToUse = FS_URSULA;

	setRadioMessage(faceToUse, missFireMessage[rand() % 5], 0);
}

void getPlayerHitMessage(object *ally)
{
	int faceToUse = FS_PHOEBE;

	if (ally == &aliens[ALIEN_PHOEBE])
		faceToUse = FS_PHOEBE;
	else
		faceToUse = FS_URSULA;

	setRadioMessage(faceToUse, playerHitMessage[rand() % 3], 0);
}
