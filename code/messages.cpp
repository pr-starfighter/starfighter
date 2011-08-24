/*
Copyright (C) 2003 Parallel Realities

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "messages.h"

void setKillMessages()
{
	strcpy(killMessage[0], "Chalk another one up for me!");
	strcpy(killMessage[1], "That'll teach you!");
	strcpy(killMessage[2], "One more for me!");
	strcpy(killMessage[3], "Target destroyed!");
	strcpy(killMessage[4], "You aint so tough!");
	strcpy(killMessage[5], "Kicked your ass!");

	strcpy(killMessage[6], "That takes me up to %d");

	strcpy(killMessage[7], "Hey %s, you asleep over there?!");
	strcpy(killMessage[8], "I'm catching up with you, %s!");

	strcpy(killMessage[9], "Number One, Baby!");
}

void setPlayerDeadMessages()
{
	strcpy(deathMessage[0], "Oh my God... No!");
	strcpy(deathMessage[1], "NOOOOOOOOOOOOOOOOOOOOOOOOOOO!!!!");
	strcpy(deathMessage[2], "Please tell me that didn't just happen...");
	strcpy(deathMessage[3], "Chris, Answer Me!!");
	strcpy(deathMessage[4], "What the hell happened?!");
	strcpy(deathMessage[5], "Chriiiiiiiiiiiiiiiiiiiiiiiiiiis!!!!");
}

void setMissFireMessages()
{
	strcpy(missFireMessage[0], "I am NOT your enemy!");
	strcpy(missFireMessage[1], "Hey! Watch it!");
	strcpy(missFireMessage[2], "What are you doing?! Shoot THEM!");
	strcpy(missFireMessage[3], "OW!!! I hope that was an accident!");
	strcpy(missFireMessage[4], "Open your eyes!!");
}

void setHitPlayerMessages()
{
	strcpy(playerHitMessage[0], "Oops! Sorry!");
	strcpy(playerHitMessage[1], "Get out of the way!");
	strcpy(playerHitMessage[2], "Don't fly into my missiles!");
}

void setAllyMessages()
{
	setKillMessages();
	setPlayerDeadMessages();
	setMissFireMessages();
	setHitPlayerMessages();
}

void getKillMessage(object *ally)
{
	char in[50], name[30], otherName[30];
	int kills, difference;
	signed char firstPlace = 0;
	int faceToUse = FACE_PHOEBE;

	if (ally == &enemy[FR_PHOEBE])
	{
		strcpy(name, "Phoebe");
		strcpy(otherName, "Ursula");
		kills = currentGame.wingMate1Kills;
		difference = currentGame.wingMate1Kills - currentGame.wingMate2Kills;
		if ((currentGame.wingMate1Kills > currentGame.wingMate2Kills) && (currentGame.wingMate1Kills > currentGame.totalKills))
			firstPlace = 1;
		faceToUse = FACE_PHOEBE;
	}
	else
	{
		strcpy(name, "Ursula");
		strcpy(otherName, "Phoebe");
		kills = currentGame.wingMate2Kills;
		difference = currentGame.wingMate2Kills - currentGame.wingMate1Kills;
		if ((currentGame.wingMate2Kills > currentGame.wingMate1Kills) && (currentGame.wingMate2Kills > currentGame.totalKills))
			firstPlace = 1;
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
			sprintf(in, killMessage[rand() % 6]);
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
			if (firstPlace == 1)
			{
				sprintf(in, killMessage[9]);
			}
			else
			{
				sprintf(in, killMessage[rand() % 6]);
			}
			break;
	}

	setRadioMessage(faceToUse, in, 0);
}

const char *getKlineInsult()
{
	static const char insult[][40] = {
	"Pathetic", "How very disappointing...", "Heroic. And stupid", "Fool", "And now you're nothing but a DEAD hero"
	};

	if (currentGame.area != 26)
		return (insult[rand() % 3]);
	else
		return (insult[3 + (rand() % 2)]);
}

void getPlayerDeathMessage()
{
	if (enemy[WC_KLINE].active)
	{
		setRadioMessage(FACE_KLINE, getKlineInsult(), 1);
		return;
	}
	
	if ((enemy[WC_BOSS].active) && (enemy[WC_BOSS].classDef == CD_KRASS))
	{
		setRadioMessage(FACE_KRASS, "That was the easiest $90,000,000 I've ever earned! Bwwah!! Ha!! Ha!! Ha!!", 1);
		return;
	}

	char name[30], in[50];
	int faceToUse = FACE_PHOEBE;
	
	strcpy(name, "Phoebe");
	faceToUse = FACE_PHOEBE;

	if (currentGame.hasWingMate2)
	{
		if ((rand() % 2) == 0)
		{
			strcpy(name, "Ursula");
			faceToUse = FACE_URSULA;
		}
	}

	sprintf(in, deathMessage[rand() % 6]);
	setRadioMessage(faceToUse, in, 1);
}

void getMissFireMessage(object *ally)
{
	char name[30], in[50];
	int faceToUse = FACE_PHOEBE;

	if (ally == &enemy[FR_PHOEBE])
	{
		strcpy(name, "Phoebe");
		faceToUse = FACE_PHOEBE;
	}
	else
	{
		strcpy(name, "Ursula");
		faceToUse = FACE_URSULA;
	}

	sprintf(in, missFireMessage[rand() % 5]);
	setRadioMessage(faceToUse, in, 0);
}

void getPlayerHitMessage(object *ally)
{
	char name[30], in[50];
	int faceToUse = FACE_PHOEBE;

	if (ally == &enemy[FR_PHOEBE])
	{
		strcpy(name, "Phoebe");
		faceToUse = FACE_PHOEBE;
	}
	else
	{
		strcpy(name, "Ursula");
		faceToUse = FACE_URSULA;
	}

	sprintf(in, playerHitMessage[rand() % 3]);
	setRadioMessage(faceToUse, in, 0);
}
