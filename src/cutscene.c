/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012, 2013 Guus Sliepen
Copyright (C) 2012, 2015-2019 Julie Marchant <onpon4@riseup.net>

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

#include "colors.h"
#include "defs.h"
#include "structs.h"

#include "alien.h"
#include "engine.h"
#include "explosion.h"
#include "game.h"
#include "gfx.h"
#include "player.h"
#include "renderer.h"
#include "screen.h"

typedef struct Message_ {

	int face;
	char message[STRMAX];

} Message;


static Message messages[MAX_EVENTS];

void cutscene_init(int scene)
{
	screen_clear(black);
	renderer_update();
	screen_clear(black);

	engine.keyState[KEY_FIRE] = 0;
	engine.keyState[KEY_ALTFIRE] = 0;

	engine.ssx = -0.5;
	engine.ssy = 0;
	engine.smx = 0;
	engine.smy = 0;

	screen_flushBuffer();
	gfx_free();
	engine_resetLists();
	gfx_loadSprites();

	for (int i = 0 ; i < ALIEN_MAX ; i++)
	{
		// 0 is the dualfighter, but there doesn't seem to be any
		// particular reason for choosing this alien def.
		aliens[i] = alien_defs[0];
		aliens[i].face = 0;
		aliens[i].active = 0;
	}

	for (int i = 0 ; i < MAX_EVENTS ; i++)
	{
		strcpy(messages[i].message, "");
		messages[i].face = -1;
	}

	switch (scene)
	{
		case 0:
			gfx_loadBackground("gfx/spirit.jpg");
			engine.ssx = -3;
			engine.ssy = 0;

			aliens[0].image[0] = gfx_shipSprites[SS_FIREFLY];
			aliens[0].x = screen->w * 3 / 5;
			aliens[0].y = screen->h / 2;
			aliens[0].dx = 3.1;
			aliens[0].active = 1;

			for (int i = 1 ; i < 7 ; i++)
			{
				aliens[i].image[0] = gfx_shipSprites[SS_DUALFIGHTER];
				aliens[i].x = RANDRANGE(0, screen->w / 8);
				aliens[i].y = RANDRANGE(50, screen->h - 50);
				aliens[i].dx = 3;
				aliens[i].active = 1;
			}

			messages[0].face = -1;
			/// Cutscene (narration)
			strcpy(messages[0].message, _("While escaping with his newly acquired Firefly, Chris Bainfield is intercepted by a WEAPCO patrol..."));

			messages[1].face = FS_CHRIS;
			/// Cutscene (Chris Bainfield)
			strcpy(messages[1].message, _("These things just won't give up, will they?"));

			messages[2].face = FS_CHRIS;
			/// Cutscene (Chris Bainfield)
			strcpy(messages[2].message, _("What a dilemma! I'm not all that familiar with this ship's controls, but I can't let this patrol reach my rendezvous point..."));

			messages[3].face = FS_CHRIS;
			/// Cutscene (Chris Bainfield)
			strcpy(messages[3].message, _("I guess I'll have to fight them, then. Let's see what this ship can do!"));

			break;

		case 1:
			gfx_loadBackground("gfx/sol.jpg");
			engine.ssx = -0.5;
			engine.ssy = 0;

			aliens[0].image[0] = gfx_shipSprites[SS_KLINE];
			aliens[0].x = screen->w / 2;
			aliens[0].y = screen->h / 2;
			aliens[0].dx = 0.5;
			aliens[0].active = 1;

			for (int i = 1 ; i < 15 ; i++)
			{
				aliens[i].image[0] = gfx_shipSprites[SS_DUALFIGHTER];
				aliens[i].x = RANDRANGE(0, screen->w);
				aliens[i].y = RANDRANGE(50, screen->h - 50);
				aliens[i].dx = RANDRANGE(1, 3);
				aliens[i].active = 1;
			}

			messages[0].face = -1;
			/// Cutscene (narration)
			strcpy(messages[0].message, _("A few hours later, in the Sol system, news has already spread of Chris Bainfield's heroic actions. The commander of WEAPCO's navy considers his options."));

			messages[1].face = FS_KLINE;
			/// Cutscene (Kline Kethlan)
			strcpy(messages[1].message, _("The Emperor will not be pleased. Spirit is now a free star system thanks to that interfering rebel pilot."));

			messages[2].face = FS_KLINE;
			/// Cutscene (Kline Kethlan)
			strcpy(messages[2].message, _("It was reported that he was able to take down one of our most powerful frigates in under 3 minutes!"));

			messages[3].face = FS_KLINE;
			/// Cutscene (Kline Kethlan)
			strcpy(messages[3].message, _("Talent like that does not appear every day. He would be a perfect candidate for our new AI training program!"));

			messages[4].face = FS_KLINE;
			/// Cutscene (Kline Kethlan)
			strcpy(messages[4].message, _("What a pity I must kill him..."));

			break;

		case 2:
			gfx_loadBackground("gfx/spirit.jpg");
			engine.ssx = -1.4;
			engine.ssy = 0;

			aliens[0].image[0] = gfx_shipSprites[SS_FIREFLY];
			aliens[0].x = screen->w / 4;
			aliens[0].y = screen->h / 2;
			aliens[0].dx = 1.5;
			aliens[0].active = 1;

			aliens[1].image[0] = gfx_shipSprites[SS_SID];
			aliens[1].x = screen->w / 4 - 50;
			aliens[1].y = screen->h / 2 - 40;
			aliens[1].dx = 1.5;
			aliens[1].active = 1;

			messages[0].face = FS_SID;
			/// Cutscene (Sid Wilson)
			strcpy(messages[0].message, _("We're nearly ready to make the jump to Eyananth."));

			messages[1].face = FS_CHRIS;
			/// Cutscene (Chris Bainfield)
			strcpy(messages[1].message, _("Aren't there a lot of WEAPCO slaves in this system?"));

			messages[2].face = FS_SID;
			/// Cutscene (Sid Wilson)
			strcpy(messages[2].message, _("Yes. It's got one of the highest mortality rates in the galaxy."));

			messages[3].face = FS_CHRIS;
			/// Cutscene (Chris Bainfield)
			strcpy(messages[3].message, _("You think we can use that to our advantage?"));

			messages[4].face = FS_SID;
			/// Cutscene (Sid Wilson)
			strcpy(messages[4].message, _("I think so. I'll come up with a plan of action."));

			break;

		case 3:
			gfx_loadBackground("gfx/eyananth.jpg");
			engine.ssx = -0.5;
			engine.ssy = 0;

			aliens[0].image[0] = gfx_shipSprites[SS_FIREFLY];
			aliens[0].x = screen->w * 3 / 8;
			aliens[0].y = screen->h / 2;
			aliens[0].dx = 0.5;
			aliens[0].active = 1;

			aliens[1].image[0] = gfx_shipSprites[SS_SID];
			aliens[1].x = screen->w * 3 / 8 - 50;
			aliens[1].y = screen->h / 2 - 40;
			aliens[1].dx = 0.5;
			aliens[1].active = 1;

			aliens[2].image[0] = gfx_shipSprites[SS_FRIEND];
			aliens[2].x = screen->w * 3 / 8 - 50;
			aliens[2].y = screen->h / 2 + 40;
			aliens[2].dx = 0.5;
			aliens[2].active = 1;

			messages[0].face = FS_PHOEBE;
			/// Cutscene (Phoebe Lexx)
			strcpy(messages[0].message, _("Nice head gear! You shop at the same place as me, huh?"));

			messages[1].face = FS_CHRIS;
			/// Cutscene (Chris Bainfield)
			strcpy(messages[1].message, _("More importantly, what were you doing out there? You're lucky I was around!"));

			messages[2].face = FS_PHOEBE;
			/// Cutscene (Phoebe Lexx)
			strcpy(messages[2].message, _("I'm looking for my sister. She vanished about a week ago."));

			messages[3].face = FS_PHOEBE;
			/// Cutscene (Phoebe Lexx)
			strcpy(messages[3].message, _("Hey! Wait a moment! You're that rebel from Spirit! I think you're so cool! Can I come with you?"));

			messages[4].face = FS_CHRIS;
			/// Cutscene (Chris Bainfield)
			strcpy(messages[4].message, _("Extra firepower? I wouldn't mind one bit! What do you think, Sid?"));

			messages[5].face = FS_SID;
			/// Cutscene (Sid Wilson)
			strcpy(messages[5].message, _("I agree. I'm also interested in studying her homing missile launcher; it could come in handy."));

			messages[6].face = FS_CHRIS;
			/// Cutscene (Chris Bainfield)
			strcpy(messages[6].message, _("In that case, welcome aboard, Phoebe!"));

			break;

		case 4:
			gfx_loadBackground("gfx/eyananth.jpg");
			engine.ssx = -1.4;
			engine.ssy = 0;

			aliens[0].image[0] = gfx_shipSprites[SS_FIREFLY];
			aliens[0].x = screen->w / 4;
			aliens[0].y = screen->h / 2;
			aliens[0].dx = 1.5;
			aliens[0].active = 1;

			aliens[1].image[0] = gfx_shipSprites[SS_SID];
			aliens[1].x = screen->w / 4 - 50;
			aliens[1].y = screen->h / 2 - 40;
			aliens[1].dx = 1.5;
			aliens[1].active = 1;

			aliens[2].image[0] = gfx_shipSprites[SS_FRIEND];
			aliens[2].x = screen->w / 4 - 50;
			aliens[2].y = screen->h / 2 + 40;
			aliens[2].dx = 1.5;
			aliens[2].active = 1;

			messages[0].face = FS_SID;
			/// Cutscene (Sid Wilson)
			strcpy(messages[0].message, _("What happened back there, Chris? The video feed was jammed."));

			messages[1].face = FS_CHRIS;
			/// Cutscene (Chris Bainfield)
			strcpy(messages[1].message, _("We took down the WEAPCO mining vessel and then I was jumped by a man claiming to be Kline Kethlan."));

			messages[2].face = FS_SID;
			/// Cutscene (Sid Wilson)
			strcpy(messages[2].message, _("I've heard of him. He's the Commander of WEAPCO's naval forces. One of the best pilots they ever had."));

			messages[3].face = FS_CHRIS;
			/// Cutscene (Chris Bainfield)
			strcpy(messages[3].message, _("He did put up one hell of a fight! He didn't stick around for long, though."));

			messages[4].face = FS_CHRIS;
			/// Cutscene (Chris Bainfield)
			strcpy(messages[4].message, _("Anyway, what's the scoop on Mordor, Sid?"));

			messages[5].face = FS_SID;
			/// Cutscene (Sid Wilson)
			strcpy(messages[5].message, _("I've learned from the scientist we captured that WEAPCO is testing a new fighter craft there."));

			messages[6].face = FS_CHRIS;
			/// Cutscene (Chris Bainfield)
			strcpy(messages[6].message, _("We should probably destroy that craft, then. We might be able to thwart its development somewhat."));

			messages[7].face = FS_SID;
			/// Cutscene (Sid Wilson)
			strcpy(messages[7].message, _("Agreed. Capturing it would be better, but that's probably not going to happen."));

			messages[8].face = FS_PHOEBE;
			/// Cutscene (Phoebe Lexx)
			strcpy(messages[8].message, _("I wonder if my sister will be here..."));

			break;

		case 5:
			gfx_loadBackground("gfx/mordor.jpg");
			engine.ssx = -0.5;
			engine.ssy = 0;

			aliens[0].image[0] = gfx_shipSprites[SS_FIREFLY];
			aliens[0].x = screen->w * 3 / 8;
			aliens[0].y = screen->h / 2;
			aliens[0].dx = 0.5;
			aliens[0].active = 1;

			aliens[1].image[0] = gfx_shipSprites[SS_SID];
			aliens[1].x = screen->w * 3 / 8 - 50;
			aliens[1].y = screen->h / 2 - 40;
			aliens[1].dx = 0.5;
			aliens[1].active = 1;

			aliens[2].image[0] = gfx_shipSprites[SS_FRIEND];
			aliens[2].x = screen->w * 3 / 8 - 50;
			aliens[2].y = screen->h / 2 + 40;
			aliens[2].dx = 0.5;
			aliens[2].active = 1;

			messages[0].face = FS_PHOEBE;
			/// Cutscene (Phoebe Lexx)
			strcpy(messages[0].message, _("Will she be okay?"));

			messages[1].face = FS_SID;
			/// Cutscene (Sid Wilson)
			strcpy(messages[1].message, _("I've had a look at the implants and they should be easy to remove. She'll just have a headache for a while."));

			messages[2].face = FS_CHRIS;
			/// Cutscene (Chris Bainfield)
			strcpy(messages[2].message, _("Will she be able to tell us anything useful?"));

			messages[3].face = FS_SID;
			/// Cutscene (Sid Wilson)
			strcpy(messages[3].message, _("We'll have to wait for her memory to come back. She might not be able to remember anything she did while the implants were in."));

			messages[4].face = FS_SID;
			/// Cutscene (Sid Wilson)
			strcpy(messages[4].message, _("She'll still be able to pilot a ship though."));

			break;

		case 6:
			gfx_loadBackground("gfx/mordor.jpg");
			engine.ssx = -1.4;
			engine.ssy = 0;

			aliens[0].image[0] = gfx_shipSprites[SS_FIREFLY];
			aliens[0].x = screen->w / 4;
			aliens[0].y = screen->h / 2;
			aliens[0].dx = 1.5;
			aliens[0].active = 1;

			aliens[1].image[0] = gfx_shipSprites[SS_SID];
			aliens[1].x = screen->w / 4 - 50;
			aliens[1].y = screen->h / 2 - 40;
			aliens[1].dx = 1.5;
			aliens[1].active = 1;

			aliens[2].image[0] = gfx_shipSprites[SS_FRIEND];
			aliens[2].x = screen->w / 4 - 50;
			aliens[2].y = screen->h / 2 + 40;
			aliens[2].dx = 1.5;
			aliens[2].active = 1;

			aliens[3].image[0] = gfx_shipSprites[SS_FRIEND];
			aliens[3].x = screen->w / 4 - 90;
			aliens[3].y = screen->h / 2;
			aliens[3].dx = 1.5;
			aliens[3].active = 1;

			messages[0].face = FS_CHRIS;
			/// Cutscene (Chris Bainfield)
			strcpy(messages[0].message, _("Sorry folks, we just lost our bargaining chip."));

			messages[1].face = FS_SID;
			/// Cutscene (Sid Wilson)
			strcpy(messages[1].message, _("Don't worry about it. It's not what I hoped for, but it should still make it easier to defeat WEAPCO."));

			messages[2].face = FS_URSULA;
			/// Cutscene (Ursula Lexx)
			strcpy(messages[2].message, _("Sol is going to be difficult. I've heard they have a lot of heavy defenses on the outer planets."));

			messages[3].face = FS_SID;
			/// Cutscene (Sid Wilson)
			strcpy(messages[3].message, _("We'll have to start there, then."));

			messages[4].face = FS_SID;
			/// Cutscene (Sid Wilson)
			strcpy(messages[4].message, _("The forces here will be unlike anything we've met so far. Just be careful, everyone."));

			break;
	}

	/*
		Because we can fiddle with the images, we need to set the engines to
		the correct places on the craft. Otherwise it will look wrong
	*/
	for (int i = 0 ; i < 15 ; i++)
	{
		aliens[i].engineX = aliens[i].image[0]->w;
		aliens[i].engineY = (aliens[i].image[0]->h / 2);
	}

	int showMessage = 0;
	int currentMessage = -1;
	int timer = 60 * 4;

	screen_drawBackground();

	SDL_Surface *face;

	player_flushInput();

	while (1)
	{
		renderer_update();
		screen_unBuffer();
		player_getInput();
		game_doStars();
		game_doExplosions();

		for (int i = 0 ; i < 15 ; i++)
		{
			if (aliens[i].active)
			{
				explosion_addEngine(&aliens[i]);
				if (scene == 0 && i > 0 && (timer % 15) == i) {
					aliens[i].dx += (DRAND - 0.5) * 0.1;
					aliens[i].dy += (DRAND - 0.5) * 0.1;
					if (aliens[i].x > 500 - timer)
						aliens[i].dx -= 0.2;
					if (aliens[i].x < 0)
						aliens[i].dx += 0.2;
				}
				aliens[i].x += aliens[i].dx;
				aliens[i].y += aliens[i].dy;
				aliens[i].x += engine.ssx + engine.smx;
				screen_blit(aliens[i].image[0], (int)aliens[i].x, (int)aliens[i].y);
				if (aliens[i].x > (screen->w + 50))
				{
					aliens[i].x = -50;
					aliens[i].y = rand() % (screen->h - 40);
				}
				if (aliens[i].y < -50)
					aliens[i].y = (screen->h + 50);
				if (aliens[i].y > (screen->h + 50))
					aliens[i].y = -50;
			}
		}

		timer--;
		if (timer == 0)
		{
			showMessage = !showMessage;
			timer = 120;
			if (showMessage)
			{
				timer = 60 * 7;
				currentMessage++;

				if (currentMessage == 10)
					break;

				if (strcmp(messages[currentMessage].message, "") == 0)
					break;

				face = NULL;
				if (messages[currentMessage].face != -1)
					face = gfx_faceSprites[messages[currentMessage].face];
				gfx_createMessageBox(face, messages[currentMessage].message, 0);
			}
		}

		if ((showMessage) && (gfx_messageBox != NULL))
			screen_blit(gfx_messageBox, (screen->w - gfx_messageBox->w) / 2, screen->h - 100);

		screen_renderUnicode(_("Press [Escape] to skip"), -1, screen->h - 20, FONT_WHITE);

		game_delayFrame();

		if ((engine.keyState[KEY_ESCAPE]) || (engine.keyState[KEY_FIRE]) ||
				(engine.keyState[KEY_ALTFIRE]))
			break;
	}

	screen_flushBuffer();
	gfx_free();
	screen_clear(black);
	renderer_update();
}
