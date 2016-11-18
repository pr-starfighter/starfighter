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

static const char *deathMessage[6] = {
	"Oh my God... No!",
	"NOOOOOOOOOOOOOOOOOOOOOOOOOOO!!!!",
	"Please tell me that didn't just happen...",
	"Chris, answer me!!",
	"What the hell happened?!",
	"Chriiiiiiiiiiiiiiiiiiiiiiiiiiis!!!!",
};

static const char *missFireMessage[5] = {
	"I am NOT your enemy!",
	"Hey! Watch it!",
	"What are you doing?! Shoot THEM!",
	"OW!!! I hope that was an accident!",
	"Open your eyes!!",
};

static const char *playerHitMessage[3] = {
	"Oops! Sorry!",
	"Get out of the way!",
	"Don't fly into my missiles!",
};

const char *getKlineInsult()
{
	static const char *insult[] = {
		"Pathetic.", "How very disappointing...", "Heroic. And stupid.", "Fool.",
		"And now you're nothing but a DEAD hero."
	};

	if (game.area != MISN_VENUS)
		return (insult[rand() % 3]);
	else
		return (insult[3 + (rand() % 2)]);
}

void getPlayerDeathMessage()
{
	if (aliens[ALIEN_KLINE].active)
	{
		setRadioMessage(FS_KLINE, getKlineInsult(), 1);
		return;
	}
	
	if ((aliens[ALIEN_BOSS].active) && (aliens[ALIEN_BOSS].classDef == CD_KRASS))
	{
		setRadioMessage(FS_KRASS, "That was the easiest $90,000,000 I've ever earned! Bwwah!! Ha!! Ha!! Ha!!", 1);
		return;
	}

	int faceToUse = FS_PHOEBE;
	

	if ((game.area == MISN_URUSOR) || (game.area == MISN_POSWIC) ||
		(game.area == MISN_EARTH))
	{
		faceToUse = FS_SID;
	}
	else if (game.hasWingMate2)
	{
		if ((rand() % 2) == 0)
			faceToUse = FS_URSULA;
	}

	setRadioMessage(faceToUse, deathMessage[rand() % 6], 1);
}

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
