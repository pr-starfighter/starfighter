/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012, 2013 Guus Sliepen
Copyright (C) 2015, 2016 onpon4 <onpon4@riseup.net>

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

void loadBackground(const char *filename)
{
	if (gfx_background != NULL)
	{
		SDL_FreeSurface(gfx_background);
		gfx_background = NULL;
	}
	gfx_background = gfx_loadImage(filename);
	SDL_SetColorKey(gfx_background, 0, 0);
}

void loadGameGraphics()
{
	Uint32 *p32;
	Uint16 *p16;
	Uint8 *p8;

	gfx_free();

	// Faces
	gfx_faceSprites[FS_CHRIS] = gfx_loadImage("gfx/face_chris.png");
	gfx_faceSprites[FS_SID] = gfx_loadImage("gfx/face_sid.png");
	gfx_faceSprites[FS_KRASS] = gfx_loadImage("gfx/face_krass.png");
	gfx_faceSprites[FS_PHOEBE] = gfx_loadImage("gfx/face_phoebe.png");
	gfx_faceSprites[FS_URSULA] = gfx_loadImage("gfx/face_ursula.png");
	gfx_faceSprites[FS_KLINE] = gfx_loadImage("gfx/face_kline.png");
	gfx_faceSprites[FS_CREW] = gfx_loadImage("gfx/face_crew.png");

	// Ships
	gfx_shipSprites[SS_FIREFLY] = gfx_loadImage("gfx/firefly1.png");
	gfx_shipSprites[SS_FIREFLY_L] = gfx_loadImage("gfx/firefly2.png");
	gfx_shipSprites[SS_SID] = gfx_loadImage("gfx/sid1.png");
	gfx_shipSprites[SS_SID_L] = gfx_loadImage("gfx/sid2.png");
	gfx_shipSprites[SS_FRIEND] = gfx_loadImage("gfx/wingmate1.png");
	gfx_shipSprites[SS_FRIEND_L] = gfx_loadImage("gfx/wingmate2.png");
	gfx_shipSprites[SS_GOODTRANSPORT] = gfx_loadImage("gfx/goodTrans1.png");
	gfx_shipSprites[SS_GOODTRANSPORT_L] = gfx_loadImage("gfx/goodTrans2.png");
	gfx_shipSprites[SS_REBELCARRIER] = gfx_loadImage("gfx/rebelCarrier1.png");
	gfx_shipSprites[SS_REBELCARRIER_L] = gfx_loadImage("gfx/rebelCarrier2.png");
	gfx_shipSprites[SS_DUALFIGHTER] = gfx_loadImage("gfx/dualFighter1.png");
	gfx_shipSprites[SS_DUALFIGHTER_L] = gfx_loadImage("gfx/dualFighter2.png");
	gfx_shipSprites[SS_MISSILEBOAT] = gfx_loadImage("gfx/missileBoat1.png");
	gfx_shipSprites[SS_MISSILEBOAT_L] = gfx_loadImage("gfx/missileBoat2.png");
	gfx_shipSprites[SS_PROTOFIGHTER] = gfx_loadImage("gfx/eliteFighter1.png");
	gfx_shipSprites[SS_PROTOFIGHTER_L] = gfx_loadImage("gfx/eliteFighter2.png");
	gfx_shipSprites[SS_AIMFIGHTER] = gfx_loadImage("gfx/aimFighter1.png");
	gfx_shipSprites[SS_AIMFIGHTER_L] = gfx_loadImage("gfx/aimFighter2.png");
	gfx_shipSprites[SS_DRONE] = gfx_loadImage("gfx/drone1.png");
	gfx_shipSprites[SS_DRONE_L] = gfx_loadImage("gfx/drone2.png");
	gfx_shipSprites[SS_MINER] = gfx_loadImage("gfx/miner1.png");
	gfx_shipSprites[SS_MINER_L] = gfx_loadImage("gfx/miner2.png");
	gfx_shipSprites[SS_ESCORT] = gfx_loadImage("gfx/escort1.png");
	gfx_shipSprites[SS_ESCORT_L] = gfx_loadImage("gfx/escort2.png");
	gfx_shipSprites[SS_MOBILE_RAY] = gfx_loadImage("gfx/mobileCannon1.png");
	gfx_shipSprites[SS_MOBILE_RAY_L] = gfx_loadImage("gfx/mobileCannon2.png");
	gfx_shipSprites[SS_TRANSPORTSHIP] = gfx_loadImage("gfx/transport1.png");
	gfx_shipSprites[SS_TRANSPORTSHIP_L] = gfx_loadImage("gfx/transport2.png");
	gfx_shipSprites[SS_CARGOSHIP] = gfx_loadImage("gfx/tug1.png");
	gfx_shipSprites[SS_CARGOSHIP_L] = gfx_loadImage("gfx/tug2.png");
	gfx_shipSprites[SS_SLAVETRANSPORT] = gfx_loadImage("gfx/slaveTrans1.png");
	gfx_shipSprites[SS_SLAVETRANSPORT_L] = gfx_loadImage("gfx/slaveTrans2.png");
	gfx_shipSprites[SS_BARRIER] = gfx_loadImage("gfx/barrier.png");
	gfx_shipSprites[SS_MOBILESHIELD] = gfx_loadImage("gfx/mobileShield1.png");
	gfx_shipSprites[SS_MOBILESHIELD_L] = gfx_loadImage("gfx/mobileShield2.png");
	gfx_shipSprites[SS_ASTEROID] = gfx_loadImage("gfx/asteroid1.png");
	gfx_shipSprites[SS_ASTEROID_SMALL] = gfx_loadImage("gfx/asteroid2.png");
	gfx_shipSprites[SS_ASTEROID_SMALL_L] = gfx_loadImage("gfx/asteroid3.png");
	gfx_shipSprites[SS_CLOAKFIGHTER] = gfx_loadImage("gfx/cloakShip1.png");
	gfx_shipSprites[SS_CLOAKFIGHTER_L] = gfx_loadImage("gfx/cloakShip2.png");
	gfx_shipSprites[SS_EVILURSULA] = gfx_loadImage("gfx/evilUrsula1.png");
	gfx_shipSprites[SS_EVILURSULA_L] = gfx_loadImage("gfx/evilUrsula2.png");
	gfx_shipSprites[SS_KRASS] = gfx_loadImage("gfx/merc1.png");
	gfx_shipSprites[SS_KRASS_L] = gfx_loadImage("gfx/merc2.png");
	gfx_shipSprites[SS_FRIGATE] = gfx_loadImage("gfx/frigateBody1.png");
	gfx_shipSprites[SS_FRIGATE_L] = gfx_loadImage("gfx/frigateBody2.png");
	gfx_shipSprites[SS_FRIGATE_WING1] = gfx_loadImage("gfx/frigateGun11.png");
	gfx_shipSprites[SS_FRIGATE_WING1_L] = gfx_loadImage("gfx/frigateGun12.png");
	gfx_shipSprites[SS_FRIGATE_WING2] = gfx_loadImage("gfx/frigateGun21.png");
	gfx_shipSprites[SS_FRIGATE_WING2_L] = gfx_loadImage("gfx/frigateGun22.png");
	gfx_shipSprites[SS_MINERBOSS] = gfx_loadImage("gfx/mineBoss1.png");
	gfx_shipSprites[SS_MINERBOSS_L] = gfx_loadImage("gfx/mineBoss2.png");
	gfx_shipSprites[SS_MINERBOSS_WING1] = gfx_loadImage("gfx/mineBossWing11.png");
	gfx_shipSprites[SS_MINERBOSS_WING1_L] = gfx_loadImage("gfx/mineBossWing12.png");
	gfx_shipSprites[SS_MINERBOSS_WING2] = gfx_loadImage("gfx/mineBossWing21.png");
	gfx_shipSprites[SS_MINERBOSS_WING2_L] = gfx_loadImage("gfx/mineBossWing22.png");
	gfx_shipSprites[SS_MINERBOSS_WING3] = gfx_loadImage("gfx/mineBossWing31.png");
	gfx_shipSprites[SS_MINERBOSS_WING3_L] = gfx_loadImage("gfx/mineBossWing32.png");
	gfx_shipSprites[SS_MINERBOSS_WING4] = gfx_loadImage("gfx/mineBossWing41.png");
	gfx_shipSprites[SS_MINERBOSS_WING4_L] = gfx_loadImage("gfx/mineBossWing42.png");
	gfx_shipSprites[SS_EXEC] = gfx_loadImage("gfx/execTrans1.png");
	gfx_shipSprites[SS_EXEC_L] = gfx_loadImage("gfx/execTrans2.png");
	gfx_shipSprites[SS_PLUTOBOSS] = gfx_loadImage("gfx/plutoBoss1.png");
	gfx_shipSprites[SS_PLUTOBOSS_L] = gfx_loadImage("gfx/plutoBoss2.png");
	gfx_shipSprites[SS_URANUSBOSS] = gfx_loadImage("gfx/splitBoss11.png");
	gfx_shipSprites[SS_URANUSBOSS_L] = gfx_loadImage("gfx/splitBoss12.png");
	gfx_shipSprites[SS_URANUSBOSS_WING1] = gfx_loadImage("gfx/splitBoss21.png");
	gfx_shipSprites[SS_URANUSBOSS_WING1_L] = gfx_loadImage("gfx/splitBoss22.png");
	gfx_shipSprites[SS_URANUSBOSS_WING2] = gfx_loadImage("gfx/splitBoss31.png");
	gfx_shipSprites[SS_URANUSBOSS_WING2_L] = gfx_loadImage("gfx/splitBoss32.png");
	gfx_shipSprites[SS_KLINE] = gfx_loadImage("gfx/kline11.png");
	gfx_shipSprites[SS_KLINE_L] = gfx_loadImage("gfx/kline12.png");

	/*
	Create images of ships being hit that show a lot of red
	*/
	for (int i = SS_HIT_INDEX ; i < SS_MAX ; i++)
	{
		if (gfx_shipSprites[i - SS_HIT_INDEX] == NULL)
			continue;
		gfx_shipSprites[i] = gfx_createSurface(gfx_shipSprites[i - SS_HIT_INDEX]->w,
			gfx_shipSprites[i - SS_HIT_INDEX]->h);
		SDL_SetSurfaceBlendMode(gfx_shipSprites[i - SS_HIT_INDEX], SDL_BLENDMODE_NONE);
		gfx_blit(gfx_shipSprites[i - SS_HIT_INDEX], 0, 0, gfx_shipSprites[i]);
		SDL_SetSurfaceBlendMode(gfx_shipSprites[i - SS_HIT_INDEX], SDL_BLENDMODE_BLEND);

		switch (gfx_shipSprites[i]->format->BitsPerPixel)
		{
			case 32:
				SDL_LockSurface(gfx_shipSprites[i]);
				p32 = (Uint32 *)gfx_shipSprites[i]->pixels;
				for (int j = 0; j < gfx_shipSprites[i]->w * gfx_shipSprites[i]->h; j++)
				{
					if (p32[j])
						p32[j] |= gfx_shipSprites[i]->format->Rmask;
				}
				SDL_UnlockSurface(gfx_shipSprites[i]);
				break;

			case 16:
				SDL_LockSurface(gfx_shipSprites[i]);
				p16 = (Uint16 *)gfx_shipSprites[i]->pixels;
				for (int j = 0; j < gfx_shipSprites[i]->w * gfx_shipSprites[i]->h; j++)
				{
					if (p16[j])
						p16[j] |= gfx_shipSprites[i]->format->Rmask;
				}
				SDL_UnlockSurface(gfx_shipSprites[i]);
				break;

			case 8:
				SDL_LockSurface(gfx_shipSprites[i]);
				p8 = (Uint8 *)gfx_shipSprites[i]->pixels;
				for (int j = 0; j < gfx_shipSprites[i]->w * gfx_shipSprites[i]->h; j++)
				{
					if (p8[j])
						p8[j] = SDL_MapRGB(gfx_shipSprites[i]->format, 255, 0, 0);
				}
				SDL_UnlockSurface(gfx_shipSprites[i]);
				break;
		}

		SDL_SetColorKey(gfx_shipSprites[i], SDL_TRUE,
			SDL_MapRGB(gfx_shipSprites[i]->format, 0, 0, 0));
	}

	// Other sprites
	gfx_sprites[SP_PLASMA_GREEN] = gfx_loadImage("gfx/plasmaGreen.png");
	gfx_sprites[SP_PLASMA_RED] = gfx_loadImage("gfx/plasmaRed.png");
	gfx_sprites[SP_DIR_PLASMA_GREEN] = gfx_loadImage("gfx/greenDir.png");
	gfx_sprites[SP_DIR_PLASMA_RED] = gfx_loadImage("gfx/redDir.png");
	gfx_sprites[SP_ROCKET] = gfx_loadImage("gfx/rocket1.png");
	gfx_sprites[SP_ROCKET_L] = gfx_loadImage("gfx/rocket2.png");
	gfx_sprites[SP_SMALL_EXPLOSION] = gfx_loadImage("gfx/explode1.png");
	gfx_sprites[SP_SMALL_EXPLOSION_2] = gfx_loadImage("gfx/explode2.png");
	gfx_sprites[SP_SMALL_EXPLOSION_3] = gfx_loadImage("gfx/explode3.png");
	gfx_sprites[SP_SMALL_EXPLOSION_L] = gfx_loadImage("gfx/explode4.png");
	gfx_sprites[SP_BIG_EXPLOSION] = gfx_loadImage("gfx/explode05.png");
	gfx_sprites[SP_BIG_EXPLOSION_2] = gfx_loadImage("gfx/explode06.png");
	gfx_sprites[SP_BIG_EXPLOSION_3] = gfx_loadImage("gfx/explode07.png");
	gfx_sprites[SP_BIG_EXPLOSION_L] = gfx_loadImage("gfx/explode08.png");
	gfx_sprites[SP_SMOKE] = gfx_loadImage("gfx/explode9.png");
	gfx_sprites[SP_SMOKE_2] = gfx_loadImage("gfx/explode10.png");
	gfx_sprites[SP_SMOKE_3] = gfx_loadImage("gfx/explode11.png");
	gfx_sprites[SP_SMOKE_L] = gfx_loadImage("gfx/explode12.png");
	gfx_sprites[SP_TINY_EXPLOSION] = gfx_loadImage("gfx/explode13.png");
	gfx_sprites[SP_TINY_EXPLOSION_2] = gfx_loadImage("gfx/explode14.png");
	gfx_sprites[SP_TINY_EXPLOSION_3] = gfx_loadImage("gfx/explode15.png");
	gfx_sprites[SP_TINY_EXPLOSION_L] = gfx_loadImage("gfx/explode16.png");
	gfx_sprites[SP_ELECTRICAL] = gfx_loadImage("gfx/elec1.png");
	gfx_sprites[SP_ELECTRICAL_2] = gfx_loadImage("gfx/elec2.png");
	gfx_sprites[SP_ELECTRICAL_3] = gfx_loadImage("gfx/elec3.png");
	gfx_sprites[SP_ELECTRICAL_L] = gfx_loadImage("gfx/elec4.png");
	gfx_sprites[SP_PICKUP_MONEY] = gfx_loadImage("gfx/dollar.png");
	gfx_sprites[SP_PICKUP_PLASMA] = gfx_loadImage("gfx/rocket.png");
	gfx_sprites[SP_PICKUP_SHIELD] = gfx_loadImage("gfx/heart.png");
	gfx_sprites[SP_PICKUP_PLASMA_OUTPUT] = gfx_loadImage("gfx/plasmaAmmo.png");
	gfx_sprites[SP_PICKUP_PLASMA_RATE] = gfx_loadImage("gfx/plasmaRate.png");
	gfx_sprites[SP_PICKUP_PLASMA_POWER] = gfx_loadImage("gfx/plasmaDamage.png");
	gfx_sprites[SP_CHAIN_LINK] = gfx_loadImage("gfx/chainLink.png");
	gfx_sprites[SP_MINE] = gfx_loadImage("gfx/mine.png");
	gfx_sprites[SP_CARGO] = gfx_loadImage("gfx/cargo1.png");
	gfx_sprites[SP_ION] = gfx_loadImage("gfx/stunBolt.png");
	gfx_sprites[SP_ARROW_NORTH] = gfx_loadImage("gfx/arrowNorth.png");
	gfx_sprites[SP_ARROW_NORTHEAST] = gfx_loadImage("gfx/arrowNorthEast.png");
	gfx_sprites[SP_ARROW_EAST] = gfx_loadImage("gfx/arrowEast.png");
	gfx_sprites[SP_ARROW_SOUTHEAST] = gfx_loadImage("gfx/arrowSouthEast.png");
	gfx_sprites[SP_ARROW_SOUTH] = gfx_loadImage("gfx/arrowSouth.png");
	gfx_sprites[SP_ARROW_SOUTHWEST] = gfx_loadImage("gfx/arrowSouthWest.png");
	gfx_sprites[SP_ARROW_WEST] = gfx_loadImage("gfx/arrowWest.png");
	gfx_sprites[SP_ARROW_NORTHWEST] = gfx_loadImage("gfx/arrowNorthWest.png");
	gfx_sprites[SP_ARROW_FRIEND_NORTH] = gfx_loadImage("gfx/friendArrowNorth.png");
	gfx_sprites[SP_ARROW_FRIEND_NORTHEAST] = gfx_loadImage("gfx/friendArrowNorthEast.png");
	gfx_sprites[SP_ARROW_FRIEND_EAST] = gfx_loadImage("gfx/friendArrowEast.png");
	gfx_sprites[SP_ARROW_FRIEND_SOUTHEAST] = gfx_loadImage("gfx/friendArrowSouthEast.png");
	gfx_sprites[SP_ARROW_FRIEND_SOUTH] = gfx_loadImage("gfx/friendArrowSouth.png");
	gfx_sprites[SP_ARROW_FRIEND_SOUTHWEST] = gfx_loadImage("gfx/friendArrowSouthWest.png");
	gfx_sprites[SP_ARROW_FRIEND_WEST] = gfx_loadImage("gfx/friendArrowWest.png");
	gfx_sprites[SP_ARROW_FRIEND_NORTHWEST] = gfx_loadImage("gfx/friendArrowNorthWest.png");
	gfx_sprites[SP_INDICATOR_TARGET] = gfx_loadImage("gfx/targetText.png");
	gfx_sprites[SP_INDICATOR_SID] = gfx_loadImage("gfx/sidText.png");
	gfx_sprites[SP_INDICATOR_PHOEBE] = gfx_loadImage("gfx/phoebeText.png");
	gfx_sprites[SP_INDICATOR_URSULA] = gfx_loadImage("gfx/ursulaText.png");
	gfx_sprites[SP_ESCAPE_POD] = gfx_loadImage("gfx/pod.png");
	gfx_sprites[SP_ORE] = gfx_loadImage("gfx/ore1.png");
	gfx_sprites[SP_ORE_2] = gfx_loadImage("gfx/ore2.png");
	gfx_sprites[SP_ORE_L] = gfx_loadImage("gfx/ore3.png");
	gfx_sprites[SP_PICKUP_ROCKETS] = gfx_loadImage("gfx/rocketAmmo.png");
	gfx_sprites[SP_SUPERCHARGE] = gfx_loadImage("gfx/superCharge.png");

	loadBackground(systemBackground[game.system]);

	for (int i = 0 ; i < CD_MAX ; i++)
	{
		if (gfx_shipSprites[alien_defs[i].imageIndex[0]] != NULL)
		{
			alien_defs[i].image[0] = gfx_shipSprites[alien_defs[i].imageIndex[0]];
			alien_defs[i].image[1] = gfx_shipSprites[alien_defs[i].imageIndex[1]];
			alien_defs[i].engineX = alien_defs[i].image[0]->w;
			alien_defs[i].engineY = (alien_defs[i].image[0]->h / 2);
		}
	}

	for (int i = 0 ; i < W_MAX ; i++)
	{
		weapon[i].image[0] = gfx_sprites[weapon[i].imageIndex[0]];
		weapon[i].image[1] = gfx_sprites[weapon[i].imageIndex[1]];
	}
}


/*
Custom loading to alter the font color before doing
all other things
*/
void loadFont()
{
	SDL_Surface *image, *newImage;

	for (int i = 0 ; i < FONT_MAX ; i++)
	{
		image = IMG_Load("gfx/smallFont.png");

		if (image == NULL) {
			printf("Couldn't load game font! (%s) Exitting.\n", SDL_GetError());
		   exit(1);
		}

		switch(i)
		{
			case FONT_RED:
				SDL_SetSurfaceColorMod(image, 255, 0, 0);
				break;
			case FONT_YELLOW:
				SDL_SetSurfaceColorMod(image, 255, 255, 0);
				break;
			case FONT_GREEN:
				SDL_SetSurfaceColorMod(image, 0, 255, 0);
				break;
			case FONT_CYAN:
				SDL_SetSurfaceColorMod(image, 0, 255, 255);
				break;
			case FONT_OUTLINE:
				SDL_SetSurfaceColorMod(image, 0, 0, 10);
				break;
		}

		newImage = SDL_ConvertSurface(image, screen->format, 0);

		gfx_fontSprites[i] = gfx_setTransparent(newImage);

		SDL_FreeSurface(image);
 	}
}
