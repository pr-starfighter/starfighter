/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012 Guus Sliepen

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
	"Chris, Answer Me!!",
	"What the hell happened?!",
	"Chriiiiiiiiiiiiiiiiiiiiiiiiiiis!!!!",
};

static const char *killMessage[10] = {
	"Chalk another one up for me!",
	"That'll teach you!",
	"One more for me!",
	"Target destroyed!",
	"You aint so tough!",
	"Kicked your ass!",

	"That takes me up to %d",

	"Hey %s, you asleep over there?!",
	"I'm catching up with you, %s!",

	"Number One, Baby!",
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

void getKillMessage(object *ally)
{
	char in[50], otherName[30];
	int kills, difference;
	bool firstPlace = false;
	int faceToUse = FACE_PHOEBE;

	if (ally == &aliens[ALIEN_PHOEBE])
	{
		strcpy(otherName, "Ursula");
		kills = currentGame.wingMate1Kills;
		difference = currentGame.wingMate1Kills - currentGame.wingMate2Kills;
		if ((currentGame.wingMate1Kills > currentGame.wingMate2Kills) &&
				(currentGame.wingMate1Kills > currentGame.totalKills))
			firstPlace = true;
		faceToUse = FACE_PHOEBE;
	}
	else
	{
		strcpy(otherName, "Phoebe");
		kills = currentGame.wingMate2Kills;
		difference = currentGame.wingMate2Kills - currentGame.wingMate1Kills;
		if ((currentGame.wingMate2Kills > currentGame.wingMate1Kills) &&
				(currentGame.wingMate2Kills > currentGame.totalKills))
			firstPlace = true;
		faceToUse = FACE_URSULA;
	}

	int r = rand() % 10;
	
	if (currentGame.hasWingMate2 == 0)
		r = rand() % 7;

	switch(r)
	{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			strcpy(in, killMessage[rand() % 6]);
			break;

		case 6:
		case 7:
			sprintf(in, killMessage[6], kills);
			break;

		case 8:
			if (difference > 0)
			{
				sprintf(in, killMessage[7], otherName);
			}
			else
			{
				sprintf(in, killMessage[8], otherName);
			}
			break;

		case 9:
			if (firstPlace)
			{
				strcpy(in, killMessage[9]);
			}
			else
			{
				strcpy(in, killMessage[rand() % 6]);
			}
			break;
	}

	setRadioMessage(faceToUse, in, 0);
}

const char *getKlineInsult()
{
	static const char *insult[] = {
		"Pathetic.", "How very disappointing...", "Heroic. And stupid.", "Fool.",
		"And now you're nothing but a DEAD hero."
	};

	if (currentGame.area != MISN_VENUS)
		return (insult[rand() % 3]);
	else
		return (insult[3 + (rand() % 2)]);
}

void getPlayerDeathMessage()
{
	if (aliens[ALIEN_KLINE].active)
	{
		setRadioMessage(FACE_KLINE, getKlineInsult(), 1);
		return;
	}
	
	if ((aliens[ALIEN_BOSS].active) && (aliens[ALIEN_BOSS].classDef == CD_KRASS))
	{
		setRadioMessage(FACE_KRASS, "That was the easiest $90,000,000 I've ever earned! Bwwah!! Ha!! Ha!! Ha!!", 1);
		return;
	}

	int faceToUse = FACE_PHOEBE;
	

	if ((currentGame.area == MISN_URUSOR) || (currentGame.area == MISN_POSWIC) ||
		(currentGame.area == MISN_EARTH))
	{
		faceToUse = FACE_SID;
	}
	else if (currentGame.hasWingMate2)
	{
		if ((rand() % 2) == 0)
			faceToUse = FACE_URSULA;
	}

	setRadioMessage(faceToUse, deathMessage[rand() % 6], 1);
}

void getMissFireMessage(object *ally)
{
	int faceToUse = FACE_PHOEBE;

	if (ally == &aliens[ALIEN_PHOEBE])
		faceToUse = FACE_PHOEBE;
	else
		faceToUse = FACE_URSULA;

	setRadioMessage(faceToUse, missFireMessage[rand() % 5], 0);
}

void getPlayerHitMessage(object *ally)
{
	int faceToUse = FACE_PHOEBE;

	if (ally == &aliens[ALIEN_PHOEBE])
		faceToUse = FACE_PHOEBE;
	else
		faceToUse = FACE_URSULA;

	setRadioMessage(faceToUse, playerHitMessage[rand() % 3], 0);
}
