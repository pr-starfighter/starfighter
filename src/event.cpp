/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012, 2013 Guus Sliepen
Copyright (C) 2012, 2015, 2016 onpon4 <onpon4@riseup.net>

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

static Event gameEvent[20];
static const char *klineGreeting[] = {
	"How nice to see you again, Bainfield!",
	"It all ends here, rebel!",
	"I hope you won't disappoint me this time...",
	"Do you really think you can defeat us?!"
};

void event_set()
{
	for (int i = 0 ; i < 20 ; i++)
	{
		gameEvent[i].time = 0;
		gameEvent[i].face = -1;
		strcpy(gameEvent[i].message, "");
		gameEvent[i].entity = -1;
		gameEvent[i].flag = 0;
	}

	switch (game.area)
	{
		case MISN_INTERCEPTION:
			if ((aliens[ALIEN_KLINE].classDef == CD_KLINE) &&
					(aliens[ALIEN_KLINE].active))
				gameEvent[0].time = 2;
				gameEvent[0].face = FACE_KLINE;
				strcpy(gameEvent[0].message, klineGreeting[rand() % 4]);

			break;

		case MISN_MOEBO:
			gameEvent[0].time = 2;
			gameEvent[0].face = FACE_SID;
			strcpy(gameEvent[0].message, "You should try and destroy some of the frigate's weaponary, it'll help!");

			gameEvent[1].time = 120;
			gameEvent[1].face = FACE_SID;
			strcpy(gameEvent[1].message, "Chris, get a move on! The frigate is almost in range!");

			gameEvent[2].time = 150;
			gameEvent[2].face = FACE_SID;
			strcpy(gameEvent[2].message, "The frigate is charging its cannon!");

			gameEvent[3].time = 170;
			gameEvent[3].face = FACE_SID;
			strcpy(gameEvent[3].message, "Chris, HURRY!!!");

			gameEvent[4].time = 180;
			gameEvent[4].face = FACE_SID;
			strcpy(gameEvent[4].message, "Oh my god...");
			gameEvent[4].entity = ALIEN_BOSS;
			gameEvent[4].flag = FL_LEAVESECTOR | FL_IMMORTAL;

			break;

		case MISN_NEROD:
			gameEvent[0].time = 2;
			gameEvent[0].face = FACE_PHOEBE;
			strcpy(gameEvent[0].message, "Hey, over here! Destroy this tug so I can break free!");

			break;

		case MISN_ALLEZ:
			gameEvent[0].time = 2;
			gameEvent[0].face = FACE_CREW;
			strcpy(gameEvent[0].message, "Thank God! Please, help us! If they destroy this ship...");

			gameEvent[1].time = 6;
			gameEvent[1].face = FACE_CHRIS;
			strcpy(gameEvent[1].message, "How long do you need?");

			gameEvent[2].time = 10;
			gameEvent[2].face = FACE_CREW;
			strcpy(gameEvent[2].message, "Just a few minutes!");

			gameEvent[3].time = 60;
			gameEvent[3].face = FACE_CREW;
			strcpy(gameEvent[3].message, "Alright! We've got the weapons working again! Now to look at the engines...");
			gameEvent[3].entity = ALIEN_GOODTRANSPORT;
			gameEvent[3].flag = -FL_NOFIRE;

			gameEvent[4].time = 80;
			gameEvent[4].face = FACE_CREW;
			strcpy(gameEvent[4].message, "DAMN! The guns are offline again! What you guys doing back there?!");
			gameEvent[4].entity = ALIEN_GOODTRANSPORT;
			gameEvent[4].flag = FL_NOFIRE;

			gameEvent[5].time = 100;
			gameEvent[5].face = FACE_CREW;
			strcpy(gameEvent[5].message, "Chris, HELP!! We've lost all system power! We're a sitting duck out here!");
			gameEvent[5].entity = ALIEN_GOODTRANSPORT;
			gameEvent[5].flag = FL_DISABLED;

			gameEvent[6].time = 120;
			gameEvent[6].face = FACE_CREW;
			strcpy(gameEvent[6].message, "Just a little longer...");

			gameEvent[7].time = 140;
			gameEvent[7].face = FACE_CREW;
			strcpy(gameEvent[7].message, "Okay! We've fixed that one. We think we've found the problem with the engines too...");
			gameEvent[7].entity = ALIEN_GOODTRANSPORT;
			gameEvent[7].flag = -FL_DISABLED;

			gameEvent[8].time = 177;
			gameEvent[8].face = FACE_CREW;
			strcpy(gameEvent[8].message, "Engines fixed! Let's move!");

			gameEvent[9].time = 180;
			gameEvent[9].entity = ALIEN_GOODTRANSPORT;
			gameEvent[9].flag = FL_LEAVESECTOR | FL_IMMORTAL;

			break;

		case MISN_ELAMALE:
			gameEvent[0].time = 5;
			gameEvent[0].face = FACE_PHOEBE;
			strcpy(gameEvent[0].message, "Ummm... something about this doesn't look right! They should be offering more resistance than this!");

			gameEvent[1].time = -8;
			gameEvent[1].face = FACE_PHOEBE;
			strcpy(gameEvent[1].message, "DAMMIT! It's a trap! Come on, Chris, we have to leave NOW!!");

			gameEvent[2].time = -9;
			gameEvent[2].entity = ALIEN_PHOEBE;
			gameEvent[2].flag = FL_LEAVESECTOR;

			gameEvent[3].time = -13;
			gameEvent[3].face = FACE_SID;
			strcpy(gameEvent[3].message, "Chris, wh...");

			gameEvent[4].time = -15;
			gameEvent[4].face = FACE_KLINE;
			strcpy(gameEvent[4].message, "So finally I meet the man who has been causing us so much trouble!");

			gameEvent[5].time = -20;
			gameEvent[5].face = FACE_CHRIS;
			strcpy(gameEvent[5].message, "I see you've jammed up my warp drive. Who are you?");

			gameEvent[6].time = -25;
			gameEvent[6].face = FACE_KLINE;
			strcpy(gameEvent[6].message, "I am Kline Kethlan. And I am here to put to a stop to your worthless little crusade!");

			gameEvent[7].time = -31;
			gameEvent[7].face = FACE_CHRIS;
			strcpy(gameEvent[7].message, "Kline Kethlan, huh? You sure sound confident.");

			gameEvent[8].time = -36;
			gameEvent[8].face = FACE_KLINE;
			strcpy(gameEvent[8].message, "Nowhere to run, nowhere to hide, and no one to help you! This will certainly be interesting...");

			gameEvent[9].time = -42;
			gameEvent[9].entity = ALIEN_KLINE;
			gameEvent[9].flag = -(FL_IMMORTAL | FL_NOFIRE);

			break;

		case MISN_ODEON:
			gameEvent[0].time = 2;
			gameEvent[0].face = FACE_URSULA;
			strcpy(gameEvent[0].message, "Unidentified craft sighted. Will proceed to destroy all targets.");

			gameEvent[1].time = 8;
			gameEvent[1].face = FACE_CHRIS;
			strcpy(gameEvent[1].message, "Looks like she's got one of those homing missile launchers too! Any advice, Phoebe?");

			gameEvent[2].time = 14;
			gameEvent[2].face = FACE_PHOEBE;
			strcpy(gameEvent[2].message, "Just try and dodge them!");

			break;

		case MISN_ALMARTHA:
			gameEvent[0].time = 1;
			gameEvent[0].face = FACE_CHRIS;
			strcpy(gameEvent[0].message, "Nice ship!");

			gameEvent[1].time = 7;
			gameEvent[1].face = FACE_KRASS;
			strcpy(gameEvent[1].message, "Thanks, boy. Made it myself!");

			gameEvent[2].time = 60;
			gameEvent[2].face = FACE_KRASS;
			strcpy(gameEvent[2].message, "Hey, boy, we've got company! Looks like your friends didn't do a very good job after all!");
			gameEvent[2].entity = 10;
			gameEvent[2].flag = -FL_ACTIVATE;

			gameEvent[3].time = 90;
			gameEvent[3].entity = 11;
			gameEvent[3].flag = -FL_ACTIVATE;

			gameEvent[4].time = 93;
			gameEvent[4].face = FACE_KRASS;
			strcpy(gameEvent[4].message, "Keep those things off my back or it'll cost you extra!!!");

			gameEvent[5].time = 120;
			gameEvent[5].entity = 12;
			gameEvent[5].flag = -FL_ACTIVATE;

			gameEvent[6].time = 140;
			gameEvent[6].face = FACE_PHOEBE;
			strcpy(gameEvent[6].message, "Hey! Did we miss anything exciting?");
			gameEvent[6].entity = ALIEN_PHOEBE;
			gameEvent[6].flag = -FL_ACTIVATE;

			gameEvent[7].time = 140;
			gameEvent[7].entity = ALIEN_URSULA;
			gameEvent[7].flag = -FL_ACTIVATE;

			gameEvent[8].time = 150;
			gameEvent[8].face = FACE_KRASS;
			strcpy(gameEvent[8].message, "I've earned my fee. I'll see you around, boy!");
			gameEvent[8].entity = ALIEN_GOODTRANSPORT;
			gameEvent[8].flag = FL_LEAVESECTOR;

			break;

		case MISN_POSWIC:
			gameEvent[0].time = 90;
			gameEvent[0].face = FACE_SID;
			strcpy(gameEvent[0].message, "DAMMIT! It's getting away! We've got to stop it!");
			gameEvent[0].entity = ALIEN_BOSS;
			gameEvent[0].flag = FL_LEAVESECTOR;

			break;

		case MISN_SATURN:
			gameEvent[0].time = 45;
			gameEvent[0].face = FACE_SID;
			strcpy(gameEvent[0].message, "Chris! Another two of those ray cannons just arrived in your sector!");
			gameEvent[0].entity = 11;
			gameEvent[0].flag = -FL_ACTIVATE;

			gameEvent[1].time = 45;
			gameEvent[1].entity = 12;
			gameEvent[1].flag = -FL_ACTIVATE;

			gameEvent[2].time = 90;
			gameEvent[2].face = FACE_SID;
			strcpy(gameEvent[2].message, "Two more!");
			gameEvent[2].entity = 13;
			gameEvent[2].flag = -FL_ACTIVATE;

			gameEvent[3].time = 90;
			gameEvent[3].entity = 14;
			gameEvent[3].flag = -FL_ACTIVATE;

			break;

		case MISN_JUPITER:
			gameEvent[0].time = 1;
			gameEvent[0].entity = 10;
			gameEvent[0].flag = -FL_ACTIVATE;

			gameEvent[1].time = 2;
			gameEvent[1].face = FACE_URSULA;
			strcpy(gameEvent[1].message, "It's a trap! My God! It's Tyler!");

			gameEvent[2].time = 8;
			gameEvent[2].face = FACE_KRASS;
			strcpy(gameEvent[2].message, "I'm a mercenary, what do you expect?! WEAPCO hired me to do a job, just like you have been doing.");

			gameEvent[3].time = 14;
			gameEvent[3].face = FACE_CHRIS;
			strcpy(gameEvent[3].message, "Good point. It would be foolish to expect anything else.");

			gameEvent[4].time = 15;
			gameEvent[4].entity = 11;
			gameEvent[4].flag = -FL_ACTIVATE;

			gameEvent[5].time = 20;
			gameEvent[5].face = FACE_KRASS;
			strcpy(gameEvent[5].message, "Now if you'll assist me in dying quickly, I have a cheque to earn, boy...");

			gameEvent[6].time = 25;
			gameEvent[6].face = FACE_CHRIS;
			strcpy(gameEvent[6].message, "I have a name, you know. Do you remember it?");

			gameEvent[4].time = 45;
			gameEvent[4].entity = 12;
			gameEvent[4].flag = -FL_ACTIVATE;

			gameEvent[4].time = 45;
			gameEvent[4].entity = 13;
			gameEvent[4].flag = -FL_ACTIVATE;

			break;

		case MISN_EARTH:
			gameEvent[0].time = 2;
			gameEvent[0].face = FACE_KLINE;
			strcpy(gameEvent[0].message, "That's far enough, Bainfield. You've been lucky so far, but your luck is about to run out!");

			gameEvent[1].time = 8;
			gameEvent[1].face = FACE_CHRIS;
			strcpy(gameEvent[1].message, "Yeah, right! Like I'd lose to you after coming this far!");

			break;

		case MISN_VENUS:
			gameEvent[0].time = 2;
			gameEvent[0].face = FACE_CHRIS;
			strcpy(gameEvent[0].message, "WEAPCO is finished, Kethlan! You have nowhere to run, nowhere to hide, and no one to help you.");

			gameEvent[1].time = 8;
			gameEvent[1].face = FACE_KLINE;
			strcpy(gameEvent[1].message, "You underestimate me, Bainfield.");

			break;
	}
}

void event_check()
{
	for (int i = 0 ; i < 20 ; i++)
	{
		if (engine.timeTaken == gameEvent[i].time)
		{
			if (strcmp(gameEvent[i].message, "@none@") != 0)
			{
				setRadioMessage(gameEvent[i].face, gameEvent[i].message, 1);
			}

			if (gameEvent[i].entity > -1)
			{
				if (gameEvent[i].flag != -FL_ACTIVATE)
				{
					aliens[gameEvent[i].entity].flags += gameEvent[i].flag;
				}
				else
				{
					aliens[gameEvent[i].entity].active = true;
					aliens[gameEvent[i].entity].x = ((int)player.x +
						RANDRANGE(400, 800));
					aliens[gameEvent[i].entity].y = ((int)player.y +
						RANDRANGE(-400, 800));
				}
			}

			gameEvent[i].time = 0;
		}
	}
}

void event_sync()
{
	for (int i = 0 ; i < 20 ; i++)
	{
		if (gameEvent[i].time < 0)
		{
			gameEvent[i].time = engine.timeTaken + abs(gameEvent[i].time);
		}
	}
}
