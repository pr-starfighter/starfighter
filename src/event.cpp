/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012, 2013 Guus Sliepen
Copyright (C) 2012, 2015-2017 Julie Marchant <onpon4@riseup.net>

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

#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "structs.h"

#include "alien.h"
#include "engine.h"
#include "event.h"
#include "game.h"
#include "player.h"
#include "radio.h"

static Event events[MAX_EVENTS];

#define NMSG_KLINE_GREETING 4
static const char *klineGreeting[NMSG_KLINE_GREETING] = {
	"How nice to see you again, Bainfield!",
	"It all ends here, rebel!",
	"I hope you won't disappoint me this time...",
	"Do you really think you can defeat us?!"
};

void events_init()
{
	for (int i = 0 ; i < MAX_EVENTS ; i++)
	{
		events[i].time = 0;
		events[i].face = -1;
		strcpy(events[i].message, "");
		events[i].entity = -1;
		events[i].flag = 0;
	}

	switch (game.area)
	{
		case MISN_INTERCEPTION:
			if ((aliens[ALIEN_KLINE].classDef == CD_KLINE) &&
					(aliens[ALIEN_KLINE].active))
			{
				events[0].time = 2;
				events[0].face = FS_KLINE;
				strcpy(events[0].message, klineGreeting[rand() % NMSG_KLINE_GREETING]);
			}

			break;

		case MISN_MOEBO:
			events[0].time = 2;
			events[0].face = FS_SID;
			strcpy(events[0].message, "You should try and destroy some of the frigate's weaponary, it'll help!");

			events[1].time = 120;
			events[1].face = FS_SID;
			strcpy(events[1].message, "Chris, get a move on! The frigate is almost in range!");

			events[2].time = 150;
			events[2].face = FS_SID;
			strcpy(events[2].message, "The frigate is charging its cannon!");

			events[3].time = 170;
			events[3].face = FS_SID;
			strcpy(events[3].message, "Chris, HURRY!");

			events[4].time = 180;
			events[4].face = FS_SID;
			strcpy(events[4].message, "Oh my god...");
			events[4].entity = ALIEN_BOSS;
			events[4].flag = FL_LEAVESECTOR | FL_IMMORTAL;

			break;

		case MISN_NEROD:
			events[0].time = 2;
			events[0].face = FS_PHOEBE;
			strcpy(events[0].message, "Hey, over here! Destroy this tug so I can break free!");

			break;

		case MISN_ALLEZ:
			events[0].time = 2;
			events[0].face = FS_CREW;
			strcpy(events[0].message, "Thank God! Please, help us! If they destroy this ship...");

			events[1].time = 6;
			events[1].face = FS_CHRIS;
			strcpy(events[1].message, "How long do you need?");

			events[2].time = 10;
			events[2].face = FS_CREW;
			strcpy(events[2].message, "Just a few minutes!");

			events[3].time = 60;
			events[3].face = FS_CREW;
			strcpy(events[3].message, "Alright! We've got the weapons working again! Now to look at the engines...");
			events[3].entity = ALIEN_FRIEND1;
			events[3].flag = -FL_NOFIRE;

			events[4].time = 80;
			events[4].face = FS_CREW;
			strcpy(events[4].message, "DAMN! The guns are offline again! What you guys doing back there?!");
			events[4].entity = ALIEN_FRIEND1;
			events[4].flag = FL_NOFIRE;

			events[5].time = 100;
			events[5].face = FS_CREW;
			strcpy(events[5].message, "Chris, HELP! We've lost all system power! We're a sitting duck out here!");
			events[5].entity = ALIEN_FRIEND1;
			events[5].flag = FL_DISABLED;

			events[6].time = 120;
			events[6].face = FS_CREW;
			strcpy(events[6].message, "Just a little longer...");

			events[7].time = 140;
			events[7].face = FS_CREW;
			strcpy(events[7].message, "Okay! We've fixed that one. We think we've found the problem with the engines too...");
			events[7].entity = ALIEN_FRIEND1;
			events[7].flag = -FL_DISABLED;

			events[8].time = 177;
			events[8].face = FS_CREW;
			strcpy(events[8].message, "Engines fixed! Let's move!");

			events[9].time = 180;
			events[9].entity = ALIEN_FRIEND1;
			events[9].flag = FL_LEAVESECTOR | FL_IMMORTAL;

			break;

		case MISN_ELAMALE:
			events[0].time = 5;
			events[0].face = FS_PHOEBE;
			strcpy(events[0].message, "Ummm... something about this doesn't look right! They should be offering more resistance than this!");

			events[1].time = -8;
			events[1].face = FS_PHOEBE;
			strcpy(events[1].message, "DAMMIT! It's a trap! Come on, Chris, we have to leave NOW!");

			events[2].time = -9;
			events[2].entity = ALIEN_PHOEBE;
			events[2].flag = FL_LEAVESECTOR;

			events[3].time = -13;
			events[3].face = FS_SID;
			strcpy(events[3].message, "Chris, wh...");

			events[4].time = -15;
			events[4].face = FS_KLINE;
			strcpy(events[4].message, "So finally I meet the man who has been causing us so much trouble!");

			events[5].time = -20;
			events[5].face = FS_CHRIS;
			strcpy(events[5].message, "I see you've jammed up my warp drive. Who are you?");

			events[6].time = -25;
			events[6].face = FS_KLINE;
			strcpy(events[6].message, "I am Kline Kethlan. And I am here to put to a stop to your worthless little crusade!");

			events[7].time = -31;
			events[7].face = FS_CHRIS;
			strcpy(events[7].message, "Kline Kethlan, huh? You sure sound confident.");

			events[8].time = -36;
			events[8].face = FS_KLINE;
			strcpy(events[8].message, "Nowhere to run, nowhere to hide, and no one to help you! This will certainly be interesting...");

			events[9].time = -42;
			events[9].entity = ALIEN_KLINE;
			events[9].flag = -(FL_IMMORTAL | FL_NOFIRE);

			break;

		case MISN_ODEON:
			events[0].time = 2;
			events[0].face = FS_URSULA;
			strcpy(events[0].message, "Unidentified craft sighted. Will proceed to destroy all targets.");

			events[1].time = 8;
			events[1].face = FS_CHRIS;
			strcpy(events[1].message, "Looks like she's got one of those homing missile launchers too! Any advice, Phoebe?");

			events[2].time = 14;
			events[2].face = FS_PHOEBE;
			strcpy(events[2].message, "Just try and dodge them!");

			break;

		case MISN_ALMARTHA:
			events[0].time = 1;
			events[0].face = FS_CHRIS;
			strcpy(events[0].message, "Nice ship!");

			events[1].time = 7;
			events[1].face = FS_KRASS;
			strcpy(events[1].message, "Thanks, boy. Made it myself!");

			events[2].time = 60;
			events[2].face = FS_KRASS;
			strcpy(events[2].message, "Hey, boy, we've got company! Looks like your friends didn't do a very good job after all!");
			events[2].entity = ALIEN_BOSS_PART1;
			events[2].flag = -FL_ACTIVATE;

			events[3].time = 90;
			events[3].entity = ALIEN_BOSS_PART2;
			events[3].flag = -FL_ACTIVATE;

			events[4].time = 93;
			events[4].face = FS_KRASS;
			strcpy(events[4].message, "Keep those things off my back or it'll cost you extra!");

			events[5].time = 120;
			events[5].entity = ALIEN_BOSS_PART3;
			events[5].flag = -FL_ACTIVATE;

			events[6].time = 140;
			events[6].face = FS_PHOEBE;
			strcpy(events[6].message, "Hey! Did we miss anything exciting?");
			events[6].entity = ALIEN_PHOEBE;
			events[6].flag = -FL_ACTIVATE;

			events[7].time = 140;
			events[7].entity = ALIEN_URSULA;
			events[7].flag = -FL_ACTIVATE;

			events[8].time = 150;
			events[8].face = FS_KRASS;
			strcpy(events[8].message, "I've earned my fee. I'll see you around, boy!");
			events[8].entity = ALIEN_FRIEND1;
			events[8].flag = FL_LEAVESECTOR;

			break;

		case MISN_POSWIC:
			events[0].time = 90;
			events[0].face = FS_SID;
			strcpy(events[0].message, "DAMMIT! It's getting away! We've got to stop it!");
			events[0].entity = ALIEN_BOSS;
			events[0].flag = FL_LEAVESECTOR;

			break;

		case MISN_SATURN:
			events[0].time = 45;
			events[0].face = FS_SID;
			strcpy(events[0].message, "Chris! Another two of those ray cannons just arrived in your sector!");
			events[0].entity = ALIEN_BOSS_PART3;
			events[0].flag = -FL_ACTIVATE;

			events[1].time = 45;
			events[1].entity = ALIEN_BOSS_PART4;
			events[1].flag = -FL_ACTIVATE;

			events[2].time = 90;
			events[2].face = FS_SID;
			strcpy(events[2].message, "Two more!");
			events[2].entity = ALIEN_BOSS_PART5;
			events[2].flag = -FL_ACTIVATE;

			events[3].time = 90;
			events[3].entity = ALIEN_BOSS_PART6;
			events[3].flag = -FL_ACTIVATE;

			break;

		case MISN_JUPITER:
			events[0].time = 1;
			events[0].entity = ALIEN_BOSS_PART1;
			events[0].flag = -FL_ACTIVATE;

			events[1].time = 2;
			events[1].face = FS_URSULA;
			strcpy(events[1].message, "It's a trap! My God! It's Tyler!");

			events[2].time = 8;
			events[2].face = FS_KRASS;
			strcpy(events[2].message, "I'm a mercenary, what do you expect?! WEAPCO hired me to do a job, just like you have been doing.");

			events[3].time = 14;
			events[3].face = FS_CHRIS;
			strcpy(events[3].message, "Good point. It would be foolish to expect anything else.");

			events[4].time = 15;
			events[4].entity = ALIEN_BOSS_PART2;
			events[4].flag = -FL_ACTIVATE;

			events[5].time = 20;
			events[5].face = FS_KRASS;
			strcpy(events[5].message, "Now if you'll assist me in dying quickly, I have a cheque to earn, boy...");

			events[6].time = 25;
			events[6].face = FS_CHRIS;
			strcpy(events[6].message, "I have a name, you know. Do you remember it?");

			events[7].time = 45;
			events[7].entity = ALIEN_BOSS_PART3;
			events[7].flag = -FL_ACTIVATE;

			events[8].time = 45;
			events[8].entity = ALIEN_BOSS_PART4;
			events[8].flag = -FL_ACTIVATE;

			break;

		case MISN_EARTH:
			events[0].time = 2;
			events[0].face = FS_KLINE;
			strcpy(events[0].message, "That's far enough, Bainfield. You've been lucky so far, but your luck is about to run out!");

			events[1].time = 8;
			events[1].face = FS_CHRIS;
			strcpy(events[1].message, "Yeah, right! Like I'd lose to you after coming this far!");

			break;

		case MISN_VENUS:
			events[0].time = 2;
			events[0].face = FS_CHRIS;
			strcpy(events[0].message, "WEAPCO is finished, Kethlan! You have nowhere to run, nowhere to hide, and no one to help you.");

			events[1].time = 8;
			events[1].face = FS_KLINE;
			strcpy(events[1].message, "You underestimate me, Bainfield.");

			break;
	}
}

void events_check()
{
	for (int i = 0 ; i < MAX_EVENTS ; i++)
	{
		if (engine.timeTaken == events[i].time)
		{
			if (strcmp(events[i].message, "") != 0)
			{
				radio_setMessage(events[i].face, events[i].message, 1);
			}

			if (events[i].entity > -1)
			{
				if (events[i].flag != -FL_ACTIVATE)
				{
					aliens[events[i].entity].flags += events[i].flag;
				}
				else
				{
					aliens[events[i].entity].active = 1;
					aliens[events[i].entity].x = ((int)player.x +
						RANDRANGE(400, 800));
					aliens[events[i].entity].y = ((int)player.y +
						RANDRANGE(-400, 800));
				}
			}

			events[i].time = 0;
		}
	}
}

void events_sync()
{
	for (int i = 0 ; i < MAX_EVENTS ; i++)
	{
		if (events[i].time < 0)
		{
			events[i].time = engine.timeTaken + abs(events[i].time);
		}
	}
}

#undef NMSG_KLINE_GREETING
