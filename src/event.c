/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012, 2013 Guus Sliepen
Copyright (C) 2012, 2015-2020 The Diligent Circle <diligentcircle@riseup.net>

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

#include <libintl.h>
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
			if ((aliens[ALIEN_KLINE].classDef == CD_KLINE)
					&& (aliens[ALIEN_KLINE].active))
			{
				events[0].time = 2;
				events[0].face = FS_KLINE;
				radio_getRandomMessage(events[0].message, _(
					/// This is a list of taunts broadcast by Kline Kethlan when he shows
					/// up at interceptions.  Taunts are separated by a single "\n".  Instead
					/// of directly translating these, please populate the list with taunts
					/// that work well in the target language, following the English
					/// version only as a general guideline.  Any number of taunts is
					/// permitted.
					"How nice to see you again, Bainfield!\n"
					"It all ends here, rebel!\n"
					"I hope you won't disappoint me this time...\n"
					"Do you really think you can defeat us?!"
				));
			}

			break;

		case MISN_START:
			events[0].time = 2;
			events[0].face = FS_SID;
			/// Dialog (Sid Wilson)
			strcpy(events[0].message, _("You should be able to move with"
					" arrow keys or an analog stick, depending on the type of"
					" controls you have."));

			events[1].time = 3 + engine.radioLife/60;
			events[1].face = FS_SID;
			/// Dialog (Sid Wilson)
			strcpy(events[1].message, _("Try to stay clear of enemy fire while"
					" I take a look at the Firefly's manual..."));

			events[2].time = 5 + 3*engine.radioLife/60;
			events[2].face = FS_SID;
			/// Dialog (Sid Wilson)
			strcpy(events[2].message, _("OK, it looks like you have two"
					" weapons: a standard plasma cannon and rockets. Dammit,"
					" it doesn't say what the controls are!"));

			events[3].time = 6 + 4*engine.radioLife/60;
			events[3].face = FS_SID;
			/// Dialog (Sid Wilson)
			strcpy(events[3].message, _("If I had to guess, it's probably"
					" a Ctrl key or a B button for the plasma cannon."));

			events[4].time = 7 + 5*engine.radioLife/60;
			events[4].face = FS_SID;
			/// Dialog (Sid Wilson)
			strcpy(events[4].message, _("Rockets... those would probably be a"
					" space bar or an A button."));

			events[5].time = 9 + 7*engine.radioLife/60;
			events[5].face = FS_SID;
			/// Dialog (Sid Wilson)
			strcpy(events[5].message, _("Interesting... It says right here"
					" that you can temporarily improve the plasma cannon with powerups."));

			events[6].time = 10 + 8*engine.radioLife/60;
			events[6].face = FS_SID;
			/// Dialog (Sid Wilson)
			strcpy(events[6].message, _("Try shooting down those WEAPCO cargo"
					" ships and picking up the items they drop."));

			break;

		case MISN_MOEBO:
			events[0].time = 2;
			events[0].face = FS_SID;
			/// Dialog (Sid Wilson)
			strcpy(events[0].message, _("You should try and destroy some of the frigate's weaponry, it'll help!"));

			events[1].time = 120;
			events[1].face = FS_SID;
			/// Dialog (Sid Wilson)
			strcpy(events[1].message, _("Chris, get a move on! The frigate is almost in range!"));

			events[2].time = 150;
			events[2].face = FS_SID;
			/// Dialog (Sid Wilson)
			strcpy(events[2].message, _("The frigate is charging its cannon!"));

			events[3].time = 170;
			events[3].face = FS_SID;
			/// Dialog (Sid Wilson)
			strcpy(events[3].message, _("Chris, HURRY!"));

			events[4].time = 180;
			events[4].face = FS_SID;
			/// Dialog (Sid Wilson)
			strcpy(events[4].message, _("Oh my god..."));
			events[4].entity = ALIEN_BOSS;
			events[4].flag = FL_LEAVESECTOR | FL_IMMORTAL;

			break;

		case MISN_NEROD:
			events[0].time = 2;
			events[0].face = FS_PHOEBE;
			/// Dialog (Phoebe Lexx)
			strcpy(events[0].message, _("Hey, over here! Destroy this tug so I can break free!"));

			break;

		case MISN_ALLEZ:
			events[0].time = 2;
			events[0].face = FS_CREW;
			/// Dialog (friendly transport from Eyananth, Allez mission)
			strcpy(events[0].message, _("Thank God! Please, help us! If they destroy this ship..."));

			events[1].time = 3 + engine.radioLife/60;
			events[1].face = FS_CHRIS;
			/// Dialog (Chris Bainfield)
			strcpy(events[1].message, _("How long do you need?"));

			events[2].time = 4 + 2*engine.radioLife/60;
			events[2].face = FS_CREW;
			/// Dialog (friendly transport from Eyananth, Allez mission)
			strcpy(events[2].message, _("Just a few minutes!"));

			events[3].time = 60;
			events[3].face = FS_CREW;
			/// Dialog (friendly transport from Eyananth, Allez mission)
			strcpy(events[3].message, _("Alright! We've got the weapons working again! Now to look at the engines..."));
			events[3].entity = ALIEN_FRIEND1;
			events[3].flag = -FL_NOFIRE;

			events[4].time = 80;
			events[4].face = FS_CREW;
			/// Dialog (friendly transport from Eyananth, Allez mission)
			strcpy(events[4].message, _("DAMN! The guns are offline again! What you guys doing back there?!"));
			events[4].entity = ALIEN_FRIEND1;
			events[4].flag = FL_NOFIRE;

			events[5].time = 100;
			events[5].face = FS_CREW;
			/// Dialog (friendly transport from Eyananth, Allez mission)
			strcpy(events[5].message, _("Chris, HELP! We've lost all system power! We're a sitting duck out here!"));
			events[5].entity = ALIEN_FRIEND1;
			events[5].flag = FL_DISABLED;

			events[6].time = 120;
			events[6].face = FS_CREW;
			/// Dialog (friendly transport from Eyananth, Allez mission)
			strcpy(events[6].message, _("Just a little longer..."));

			events[7].time = 140;
			events[7].face = FS_CREW;
			/// Dialog (friendly transport from Eyananth, Allez mission)
			strcpy(events[7].message, _("Okay! We've fixed that one. We think we've found the problem with the engines too..."));
			events[7].entity = ALIEN_FRIEND1;
			events[7].flag = -FL_DISABLED;

			events[8].time = 177;
			events[8].face = FS_CREW;
			/// Dialog (friendly transport from Eyananth, Allez mission)
			strcpy(events[8].message, _("Engines fixed! Let's move!"));

			events[9].time = 180;
			events[9].entity = ALIEN_FRIEND1;
			events[9].flag = FL_LEAVESECTOR | FL_IMMORTAL;

			break;

		case MISN_ELAMALE:
			/* It would be trivial to modify these radio times for
			 * engine.radioLife like with everything else, but we don't
			 * do that here so that the music cue stays the same.  A bit
			 * inconsistent, but the dialog isn't terribly important,
			 * just flavor text really, and you can read it at your
			 * leasure if you want by pausing.  We might change this
			 * later if we decide that preserving the music cue isn't
			 * important after all.
			 */
			events[0].time = 5;
			events[0].face = FS_PHOEBE;
			/// Dialog (Phoebe Lexx)
			strcpy(events[0].message, _("Ummm... something about this doesn't look right! They should be offering more resistance than this!"));

			events[1].time = -8;
			events[1].face = FS_PHOEBE;
			/// Dialog (Phoebe Lexx)
			strcpy(events[1].message, _("DAMMIT! It's a trap! Come on, Chris, we have to leave NOW!"));

			events[2].time = -9;
			events[2].entity = ALIEN_PHOEBE;
			events[2].flag = FL_LEAVESECTOR;

			events[3].time = -13;
			events[3].face = FS_SID;
			/// Dialog (Sid Wilson)
			strcpy(events[3].message, _("Chris, wh..."));

			events[4].time = -15;
			events[4].face = FS_KLINE;
			/// Dialog (Kline Kethlan)
			strcpy(events[4].message, _("So finally I meet the man who has been causing us so much trouble!"));

			events[5].time = -20;
			events[5].face = FS_CHRIS;
			/// Dialog (Chris Bainfield)
			strcpy(events[5].message, _("I see you've jammed up my warp drive. Who are you?"));

			events[6].time = -25;
			events[6].face = FS_KLINE;
			/// Dialog (Kline Kethlan)
			strcpy(events[6].message, _("I am Kline Kethlan. And I am here to put to a stop to your worthless little crusade!"));

			events[7].time = -31;
			events[7].face = FS_CHRIS;
			/// Dialog (Chris Bainfield)
			strcpy(events[7].message, _("Kline Kethlan, huh? You sure sound confident."));

			events[8].time = -36;
			events[8].face = FS_KLINE;
			/// Dialog (Kline Kethlan)
			strcpy(events[8].message, _("Nowhere to run, nowhere to hide, and no one to help you! This will certainly be interesting..."));

			events[9].time = -42;
			events[9].entity = ALIEN_KLINE;
			events[9].flag = -(FL_IMMORTAL | FL_NOFIRE);

			break;

		case MISN_ODEON:
			events[0].time = 2;
			events[0].face = FS_URSULA;
			/// Dialog (Ursula Lexx under mind control)
			/// Translation note: This should appear robotic or at least monotone.
			strcpy(events[0].message, _("Unidentified craft sighted. Will proceed to destroy all targets."));

			events[1].time = 4 + engine.radioLife/60;
			events[1].face = FS_CHRIS;
			/// Dialog (Chris Bainfield)
			strcpy(events[1].message, _("Looks like she's got one of those homing missile launchers too! Any advice, Phoebe?"));

			events[2].time = 5 + 2*engine.radioLife/60;
			events[2].face = FS_PHOEBE;
			/// Dialog (Phoebe Lexx)
			strcpy(events[2].message, _("Just try and dodge them!"));

			break;

		case MISN_ALMARTHA:
			events[0].time = 1;
			events[0].face = FS_CHRIS;
			/// Dialog (Chris Bainfield)
			strcpy(events[0].message, _("That's a nice ship you got there, old-timer!"));

			events[1].time = 2 + engine.radioLife/60;
			events[1].face = FS_KRASS;
			/// Dialog (Krass Tyler)
			strcpy(events[1].message, _("Thanks, boy. Made it myself!"));

			events[2].time = 60;
			events[2].face = FS_KRASS;
			/// Dialog (Krass Tyler)
			strcpy(events[2].message, _("Hey, boy, we've got company! Looks like your friends didn't do a very good job after all!"));
			events[2].entity = ALIEN_BOSS_PART1;
			events[2].flag = -FL_ACTIVATE;

			events[3].time = 90;
			events[3].entity = ALIEN_BOSS_PART2;
			events[3].flag = -FL_ACTIVATE;

			events[4].time = 93;
			events[4].face = FS_KRASS;
			/// Dialog (Krass Tyler)
			strcpy(events[4].message, _("Keep those things off my back or it'll cost you extra!"));

			events[5].time = 120;
			events[5].entity = ALIEN_BOSS_PART3;
			events[5].flag = -FL_ACTIVATE;

			events[6].time = 140;
			events[6].face = FS_PHOEBE;
			/// Dialog (Phoebe Lexx)
			strcpy(events[6].message, _("Hey! Did we miss anything exciting?"));
			events[6].entity = ALIEN_PHOEBE;
			events[6].flag = -FL_ACTIVATE;

			events[7].time = 140;
			events[7].entity = ALIEN_URSULA;
			events[7].flag = -FL_ACTIVATE;

			events[8].time = 150;
			events[8].face = FS_KRASS;
			/// Dialog (Krass Tyler)
			strcpy(events[8].message, _("I've earned my fee. I'll see you around, boy!"));
			events[8].entity = ALIEN_FRIEND1;
			events[8].flag = FL_LEAVESECTOR;

			break;

		case MISN_POSWIC:
			events[0].time = 90;
			events[0].face = FS_SID;
			/// Dialog (Sid Wilson)
			strcpy(events[0].message, _("DAMMIT! It's getting away! We've got to stop it!"));
			events[0].entity = ALIEN_BOSS;
			events[0].flag = FL_LEAVESECTOR;

			break;

		case MISN_SATURN:
			events[0].time = 45;
			events[0].face = FS_SID;
			/// Dialog (Sid Wilson)
			strcpy(events[0].message, _("Chris! Another two of those ray cannons just arrived in your sector!"));
			events[0].entity = ALIEN_BOSS_PART3;
			events[0].flag = -FL_ACTIVATE;

			events[1].time = 45;
			events[1].entity = ALIEN_BOSS_PART4;
			events[1].flag = -FL_ACTIVATE;

			events[2].time = 90;
			events[2].face = FS_SID;
			/// Dialog (Sid Wilson)
			strcpy(events[2].message, _("Two more!"));
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
			/// Dialog (Ursula Lexx)
			strcpy(events[1].message, _("It's a trap! My God! It's Tyler!"));

			events[2].time = 3 + engine.radioLife/60;
			events[2].face = FS_KRASS;
			/// Dialog (Krass Tyler)
			strcpy(events[2].message, _("I'm a mercenary, what do you expect?! WEAPCO hired me to do a job, just like you have been doing."));

			events[3].time = 4 + 2*engine.radioLife/60;
			events[3].face = FS_CHRIS;
			/// Dialog (Chris Bainfield)
			strcpy(events[3].message, _("Wow, that's cold. Just goes to show never to trust a mercenary, eh?"));

			events[4].time = 15;
			events[4].entity = ALIEN_BOSS_PART2;
			events[4].flag = -FL_ACTIVATE;

			events[5].time = 5 + 3*engine.radioLife/60;
			events[5].face = FS_KRASS;
			/// Dialog (Krass Tyler)
			strcpy(events[5].message, _("Now if you'll assist me in dying quickly, I have a cheque to earn, boy..."));

			events[6].time = 6 + 4*engine.radioLife/60;
			events[6].face = FS_CHRIS;
			/// Dialog (Chris Bainfield)
			strcpy(events[6].message, _("I have a name, you know. Do you remember it?"));

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
			/// Dialog (Kline Kethlan)
			strcpy(events[0].message, _("That's far enough, Bainfield. You've been lucky so far, but your luck is about to run out!"));

			events[1].time = 3 + engine.radioLife/60;
			events[1].face = FS_CHRIS;
			/// Dialog (Chris Bainfield)
			strcpy(events[1].message, _("Yeah, right! Like I'd lose to you after coming this far!"));

			break;

		case MISN_VENUS:
			events[0].time = 2;
			events[0].face = FS_CHRIS;
			/// Dialog (Chris Bainfield)
			strcpy(events[0].message, _("WEAPCO is finished, Kethlan! You have nowhere to run, nowhere to hide, and no one to help you."));

			events[1].time = 3 + engine.radioLife/60;
			events[1].face = FS_KLINE;
			/// Dialog (Kline Kethlan)
			strcpy(events[1].message, _("You underestimate me, Bainfield."));
			if (game.difficulty == DIFFICULTY_ORIGINAL) {
				events[1].entity = ALIEN_KLINE;
				events[1].flag = -(FL_IMMORTAL | FL_NOMOVE | FL_NOFIRE);
			}

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
