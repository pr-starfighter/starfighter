/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012, 2013 Guus Sliepen
Copyright (C) 2012, 2014-2020 The Diligent Circle <diligentcircle@riseup.net>

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
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"

#ifndef NOSOUND
#include "SDL_mixer.h"
#endif

#include "colors.h"
#include "defs.h"
#include "structs.h"

#include "alien.h"
#include "audio.h"
#include "bullet.h"
#include "cargo.h"
#include "collectable.h"
#include "cutscene.h"
#include "engine.h"
#include "event.h"
#include "explosion.h"
#include "game.h"
#include "gfx.h"
#include "info.h"
#include "intermission.h"
#include "mission.h"
#include "player.h"
#include "radio.h"
#include "renderer.h"
#include "save.h"
#include "screen.h"
#include "ship.h"
#include "title.h"
#include "weapons.h"

typedef struct Star_ {

	float x, y, dx, dy;
	int speed; // How fast the star moves

} Star;

Game game;
char game_systemNames[SYSTEM_MAX][STRMAX_SHORT];

static Star stars[STARS_NUM];
static Uint32 frameLimit = 0;
static int thirds = 0;


void game_init()
{
	strcpy(game_systemNames[SYSTEM_SPIRIT], _("Spirit"));
	strcpy(game_systemNames[SYSTEM_EYANANTH], _("Eyananth"));
	strcpy(game_systemNames[SYSTEM_MORDOR], _("Mordor"));
	strcpy(game_systemNames[SYSTEM_SOL], _("Sol"));

	game.system = SYSTEM_SPIRIT;
	game.area = MISN_START;
	game.sfxVolume = 0;
	game.musicVolume = 0;

	if (!engine.useAudio)
	{
		engine.useSound = 0;
		engine.useMusic = 0;
	}

	game.cash = 0;
	game.cashEarned = 0;
	game.shots = 0;
	game.hits = 0;
	game.accuracy = 0;
	game.totalKills = game.wingMate1Kills = game.wingMate2Kills = 0;
	game.totalOtherKills = 0;
	game.hasWingMate1 = game.hasWingMate2 = 0;
	game.wingMate1Ejects = game.wingMate2Ejects = 0;
	game.secondaryMissions = game.secondaryMissionsCompleted = 0;
	game.shieldPickups = game.rocketPickups = game.cellPickups = 0;
	game.powerups = game.minesKilled = game.cargoPickups = 0;

	game.slavesRescued = 0;
	game.experimentalShield = 1000;

	game.forceMisnTarget = 1;

	game.timeTaken = 0;

	game.stationedPlanet = -1;
	game.destinationPlanet = -1;
	for (int i = 0 ; i < 10 ; i++)
		game.missionCompleted[i] = 0;
	game.distanceCovered = 0;

	game.minPlasmaRate = 1;
	game.minPlasmaOutput = 1;
	game.minPlasmaDamage = 1;
	game.maxPlasmaRate = 2;
	game.maxPlasmaOutput = 2;
	game.maxPlasmaDamage = 2;
	game.maxPlasmaAmmo = 100;
	game.maxRocketAmmo = 10;

	game.minPlasmaRateLimit = 2;
	game.minPlasmaDamageLimit = 2;
	game.minPlasmaOutputLimit = 2;
	game.maxPlasmaRateLimit = 3;
	game.maxPlasmaDamageLimit = 3;
	game.maxPlasmaOutputLimit = 3;
	game.maxPlasmaAmmoLimit = 250;
	game.maxRocketAmmoLimit = 50;

	player.maxShield = 50;

	switch (game.difficulty)
	{
		case DIFFICULTY_SUPEREASY:
		case DIFFICULTY_EASY:
			player.maxShield = 100;

			game.minPlasmaRate = 2;
			game.minPlasmaOutput = 2;
			game.minPlasmaDamage = 2;
			game.maxPlasmaRate = 3;
			game.maxPlasmaOutput = 3;
			game.maxPlasmaDamage = 3;
			game.maxPlasmaAmmo = 150;
			game.maxRocketAmmo = 20;

			game.minPlasmaRateLimit = 3;
			game.minPlasmaDamageLimit = 3;
			game.minPlasmaOutputLimit = 3;
			game.maxPlasmaRateLimit = 5;
			game.maxPlasmaDamageLimit = 5;
			game.maxPlasmaOutputLimit = 5;
			break;
		case DIFFICULTY_HARD:
			player.maxShield = 25;
			break;
		case DIFFICULTY_NIGHTMARE:
			player.maxShield = 1;
			game.maxRocketAmmo = 5;
			break;
		case DIFFICULTY_ORIGINAL:
			player.maxShield = 25;

			game.minPlasmaRateLimit = 3;
			game.minPlasmaDamageLimit = 3;
			game.minPlasmaOutputLimit = 3;
			game.maxPlasmaRateLimit = 5;
			game.maxPlasmaDamageLimit = 5;
			game.maxPlasmaOutputLimit = 5;
			break;
	}

	player.shield = player.maxShield;
	player.ammo[0] = 0;
	player.ammo[1] = game.maxRocketAmmo / 2;
	player.weaponType[0] = W_PLAYER_WEAPON;
	player.weaponType[1] = W_ROCKETS;

	game_setStars();

	weapons_init();
	mission_init();
	intermission_initPlanets(game.system);
}

static void game_addDebris(int x, int y, int amount)
{
	if ((rand() % 2) == 0)
		audio_playSound(SFX_DEBRIS, x, y);
	else
		audio_playSound(SFX_DEBRIS2, x, y);

	Object *debris;

	amount = RANDRANGE(3, amount);
	LIMIT(amount, 3, 8);

	for (int i = 0 ; i < amount ; i++)
	{
		debris = malloc(sizeof(*debris));
		if (debris == NULL)
		{
			engine_warn("Failed to allocate memory for debris");
			return;
		}

		debris->next = NULL;
		debris->x = x;
		debris->y = y;

		debris->thinktime = RANDRANGE(60, 180);

		debris->dx = RANDRANGE(-500, 500);
		debris->dy = RANDRANGE(-500, 500);

		if (debris->dx != 0)
			debris->dx /= 100;

		if (debris->dy != 0)
			debris->dy /= 100;

		engine.debrisTail->next = debris;
		engine.debrisTail = debris;
	}
}

/*
Sets star positions. Must do this any time the window size changes.
*/
void game_setStars()
{
	for (int i = 0 ; i < STARS_NUM ; i++)
	{
		stars[i].x = rand() % screen->w;
		stars[i].y = rand() % screen->h;
		stars[i].speed = 1 + (rand() % 3);
	}
}

/*
Simply draws the stars in their positions on screen and moves
them around.
*/
void game_doStars()
{
	/* Lock the screen for direct access to the pixels */
	if (SDL_MUSTLOCK(screen))
	{
		if (SDL_LockSurface(screen) < 0)
			engine_showError(2, "");
	}

	int color = 0;

	SDL_Rect r;

	for (int i = 0 ; i < STARS_NUM ; i++)
	{
		if (stars[i].speed == 3)
			color = white;
		else if (stars[i].speed == 2)
			color = lightGrey;
		else if (stars[i].speed == 1)
			color = darkGrey;

		WRAP_ADD(stars[i].x, (engine.ssx + engine.smx) * stars[i].speed, 0,
			screen->w - 1);
		WRAP_ADD(stars[i].y, (engine.ssy + engine.smy) * stars[i].speed, 0,
			screen->h - 1);

		gfx_putPixel(screen, (int)stars[i].x, (int)stars[i].y, color);
		r.x = (int)stars[i].x;
		r.y = (int)stars[i].y;
		r.w = 1;
		r.h = 1;

		screen_addBuffer(r.x, r.y, r.w, r.h);
	}

	if (SDL_MUSTLOCK(screen))
	{
		SDL_UnlockSurface(screen);
	}
}

/*
Loops through the currently active collectables (in a linked list). The collectable
will travel in the direction that was defined when it was made. Its life will decreased
whilst it remains active. It will be removed if the player touches it or if its life
reaches 0. When it is picked up, depending on the type of collectable it is, mission requirements
will be updated. Information will be displayed and appropriate player variables altered.
*/
static void game_doCollectables()
{
	Collectable *collectable = engine.collectableHead;
	Collectable *prevCollectable = engine.collectableHead;
	engine.collectableTail = engine.collectableHead;
	char temp[STRMAX_SHORT];

	while (collectable->next != NULL)
	{
		collectable = collectable->next;

		if (collectable->active)
		{
			if ((collectable->x + collectable->image->w > 0)
					&& (collectable->x < screen->w)
					&& (collectable->y + collectable->image->h > 0)
					&& (collectable->y < screen->h))
				screen_blit(collectable->image, (int)collectable->x, (int)collectable->y);

			collectable->x += engine.ssx + engine.smx;
			collectable->y += engine.ssy + engine.smy;
			collectable->x += collectable->dx;
			collectable->y += collectable->dy;

			collectable->life--;

			if ((player.shield > 0) && (collectable_collision(collectable, &player)))
			{
				switch(collectable->type)
				{
					case P_CASH:
						game.cash += collectable->value;
						game.cashEarned += collectable->value;
						snprintf(temp, STRMAX_SHORT, ngettext(
							/// "%d" must be retained. It is replaced with the amount of money that
							/// was picked up.
							"Got $%d",
							"Got $%d",
							collectable->value), collectable->value);
						break;

					case P_ROCKET:
						LIMIT_ADD(player.ammo[1], collectable->value, 0,
							game.maxRocketAmmo);
						if (player.ammo[1] == game.maxRocketAmmo)
							strcpy(temp, _("Rocket Ammo at Maximum"));
						else
						{
							snprintf(temp, STRMAX_SHORT, ngettext(
								/// "%d" must be retained. It is replaced with the number of rockets
								/// picked up.
								"Got %d rocket",
								"Got %d rockets",
								collectable->value), collectable->value);
						}
						game.rocketPickups += collectable->value;
						break;

					case P_SHIELD:
						LIMIT_ADD(player.shield, 10, 0, player.maxShield);
						game.shieldPickups ++;
						strcpy(temp, _("Restored 10 shield points"));
						break;

					case P_PLASMA_RATE:
						game.powerups++;
						if (game.difficulty == DIFFICULTY_ORIGINAL)
						{
							player.ammo[0] = MAX(player.ammo[0], 50);
							weapons[W_PLAYER_WEAPON].reload[0] = MAX(
								rate2reload[game.maxPlasmaRate],
								weapons[W_PLAYER_WEAPON].reload[0] - 2);

							if (weapons[W_PLAYER_WEAPON].reload[0] <= rate2reload[game.maxPlasmaRate])
								strcpy(temp, _("Firing rate already at maximum"));
							else
							{
								weapons[W_PLAYER_WEAPON].reload[0] -= 2;
								strcpy(temp, _("Firing rate increased"));
							}
						}
						else if ((game.difficulty == DIFFICULTY_SUPEREASY)
							|| (game.area != MISN_INTERCEPTION)
							|| (player.ammo[0] > 0))
						{
							if ((game.difficulty == DIFFICULTY_SUPEREASY)
								|| (game.area != MISN_INTERCEPTION))
							{
								LIMIT_ADD(player.ammo[0], collectable->value,
									0, game.maxPlasmaAmmo);
							}

							if (weapons[W_PLAYER_WEAPON].reload[0] <= rate2reload[game.maxPlasmaRate])
								strcpy(temp, _("Firing rate already at maximum"));
							else
							{
								weapons[W_PLAYER_WEAPON].reload[0] -= 2;
								strcpy(temp, _("Firing rate increased"));
							}
						}
						else
						{
							strcpy(temp, _("Upgrade failed (no plasma ammo)"));
						}
						break;

					case P_PLASMA_SHOT:
						game.powerups++;
						if (game.difficulty == DIFFICULTY_ORIGINAL)
						{
							player.ammo[0] = MAX(player.ammo[0], 50);
							weapons[W_PLAYER_WEAPON].ammo[0] = MIN(
								game.maxPlasmaOutput, weapons[W_PLAYER_WEAPON].ammo[0] + 1);

							if (weapons[W_PLAYER_WEAPON].ammo[0] >= game.maxPlasmaOutput)
								strcpy(temp, _("Plasma output already at maximum"));
							else
							{
								weapons[W_PLAYER_WEAPON].ammo[0]++;
								strcpy(temp, _("Plasma output increased"));
							}
						}
						else if ((game.difficulty == DIFFICULTY_SUPEREASY)
							|| (game.area != MISN_INTERCEPTION)
							|| (player.ammo[0] > 0))
						{
							if ((game.difficulty == DIFFICULTY_SUPEREASY)
								|| (game.area != MISN_INTERCEPTION))
							{
								LIMIT_ADD(player.ammo[0], collectable->value,
									0, game.maxPlasmaAmmo);
							}

							if (weapons[W_PLAYER_WEAPON].ammo[0] >= game.maxPlasmaOutput)
								strcpy(temp, _("Plasma output already at maximum"));
							else
							{
								weapons[W_PLAYER_WEAPON].ammo[0]++;
								strcpy(temp, _("Plasma output increased"));
							}
						}
						else
						{
							strcpy(temp, _("Upgrade failed (no plasma ammo)"));
						}
						break;

					case P_PLASMA_DAMAGE:
						game.powerups++;
						if (game.difficulty == DIFFICULTY_ORIGINAL)
						{
							player.ammo[0] = MAX(player.ammo[0], 50);
							weapons[W_PLAYER_WEAPON].damage = MIN(
								game.maxPlasmaDamage, weapons[W_PLAYER_WEAPON].damage + 1);

							if (weapons[W_PLAYER_WEAPON].damage >= game.maxPlasmaDamage)
								strcpy(temp, _("Plasma damage already at maximum"));
							else
							{
								weapons[W_PLAYER_WEAPON].damage++;
								strcpy(temp, _("Plasma damage increased"));
							}
						}
						else if ((game.difficulty == DIFFICULTY_SUPEREASY)
							|| (game.area != MISN_INTERCEPTION)
							|| (player.ammo[0] > 0))
						{
							if ((game.difficulty == DIFFICULTY_SUPEREASY)
								|| (game.area != MISN_INTERCEPTION))
							{
								LIMIT_ADD(player.ammo[0], collectable->value,
									0, game.maxPlasmaAmmo);
							}

							if (weapons[W_PLAYER_WEAPON].damage >= game.maxPlasmaDamage)
								strcpy(temp, _("Plasma damage already at maximum"));
							else
							{
								weapons[W_PLAYER_WEAPON].damage++;
								strcpy(temp, _("Plasma damage increased"));
							}
						}
						else
						{
							strcpy(temp, _("Upgrade failed (no plasma ammo)"));
						}
						break;

					case P_SUPER:
						game.powerups++;
						if ((game.difficulty == DIFFICULTY_ORIGINAL)
							|| (game.difficulty == DIFFICULTY_SUPEREASY)
							|| (game.area != MISN_INTERCEPTION)
							|| (player.ammo[0] > 0))
						{
							if (game.difficulty == DIFFICULTY_ORIGINAL)
								player.ammo[0] = MAX(player.ammo[0], 50);
							else if ((game.difficulty == DIFFICULTY_SUPEREASY)
								|| (game.area != MISN_INTERCEPTION))
							{
								LIMIT_ADD(player.ammo[0], collectable->value,
									0, game.maxPlasmaAmmo);
							}

							weapons[W_PLAYER_WEAPON].ammo[0] = 5;
							weapons[W_PLAYER_WEAPON].damage = 5;
							weapons[W_PLAYER_WEAPON].reload[0] = rate2reload[5];
							weapons[W_PLAYER_WEAPON].flags |= WF_SPREAD;

							strcpy(temp, _("Picked up a Super Charge!"));
						}
						else
						{
							/// Rare case of grabbing the super charge when you have no ammo at an
							/// interception (which means it won't take effect). The "Damn!" serves
							/// as a little sympathetic easter egg for players unfortunate enough to
							/// have this happen to them.
							strcpy(temp, _("Damn! Upgrade failed (no plasma ammo)"));
						}
						break;

					case P_PLASMA_AMMO:
						if (player.ammo[0] >= game.maxPlasmaAmmo)
							strcpy(temp, _("Plasma cells already at Maximum"));
						else
						{
							LIMIT_ADD(player.ammo[0], collectable->value,
								0, game.maxPlasmaAmmo);
							snprintf(temp, STRMAX_SHORT, ngettext(
								/// "%d" must be retained. It is replaced with the number of plasma
								/// cells picked up.
								"Got %d plasma cell",
								"Got %d plasma cells",
								collectable->value), collectable->value);
						}
						game.cellPickups += collectable->value;
						break;

					case P_CARGO:
						strcpy(temp, _("Picked up some Cargo"));
						game.cargoPickups++;
						break;

					case P_SLAVES:
						snprintf(temp, STRMAX_SHORT, ngettext(
							"Rescued %d slave",
							"Rescued %d slaves",
							collectable->value), collectable->value);
						game.slavesRescued += collectable->value;
						break;

					case P_ESCAPEPOD:
						strcpy(temp, _("Picked up an Escape Pod"));
						break;

					case P_ORE:
						strcpy(temp, _("Picked up some Ore"));
						break;
				}

				mission_updateRequirements(M_COLLECT, collectable->type,
					collectable->value);

				collectable->active = 0;
				if (collectable->type != P_MINE)
				{
					info_setLine(temp, FONT_WHITE);
					if (collectable->type == P_SHIELD)
						audio_playSound(SFX_SHIELDUP, player.x, player.y);
					else
						audio_playSound(SFX_PICKUP, player.x, player.y);
				}
			}

			// stop people from exploiting a weapon check condition
			if (player.ammo[0] == 0)
			{
				weapons[W_PLAYER_WEAPON].ammo[0] = game.minPlasmaOutput;
				weapons[W_PLAYER_WEAPON].damage = game.minPlasmaDamage;
				weapons[W_PLAYER_WEAPON].reload[0] = rate2reload[game.minPlasmaRate];
			}
		}

		if (collectable->life < 1)
		{
			collectable->active = 0;
			if ((collectable->type == P_CARGO)
					|| (collectable->type == P_ESCAPEPOD)
					|| (collectable->type == P_SLAVES))
				mission_updateRequirements(M_PROTECT_PICKUP, collectable->type, 1);
		}

		if (collectable->active)
		{
			prevCollectable = collectable;
			engine.collectableTail = collectable;
		}
		else
		{
			if ((collectable->type == P_MINE) && (collectable->x >= 0)
					&& (collectable->x <= screen->w) && (collectable->y >= 0)
					&& (collectable->y <= screen->h))
				collectable_explode(collectable);
			prevCollectable->next = collectable->next;
			free(collectable);
			collectable = prevCollectable;
		}
	}
}

/*
This handles active bullets in a linked list. The current bullet and
previous bullet pointers are first assigned to the main header bullet
and each successive bullet is pulled out. Bullets are moved in their
delta coordinates, with rockets having fire trails added to them. Seperate
collision checks are done for a bullet that belongs to the enemy and one
that belongs to a player. However rockets can hit anyone. Upon an enemy
being killed, mission requirements are checked and collectables are randomly
spawned.
*/
static void game_doBullets()
{
	Object *bullet = engine.bulletHead;
	Object *prevBullet = engine.bulletHead;

	Collectable *collectable;
	Collectable *prevCollectable;

	int okayToHit = 0;
	int old_shield;
	float homingMissileSpeed = 0;
	int charger_num;

	char msg[STRMAX];

	bullet = engine.bulletHead;
	prevBullet = engine.bulletHead;
	engine.bulletTail = engine.bulletHead;

	while (bullet->next != NULL)
	{
		bullet = bullet->next;

		if (bullet->active)
		{
			if (bullet->flags & WF_HOMING)
			{
				if (bullet->target == NULL)
					bullet->target = bullet_getTarget(bullet);

				if (bullet->owner->flags & FL_FRIEND)
					homingMissileSpeed = 0.25;
				else
					homingMissileSpeed = 0.05;
			}

			if (bullet->id == WT_ROCKET)
			{
				explosion_add(bullet->x, bullet->y, SP_SMALL_EXPLOSION);
			}
			else if (bullet->id == WT_MICROROCKET)
			{
				explosion_add(bullet->x, bullet->y, SP_TINY_EXPLOSION);
			}

			if ((bullet->flags & WF_AIMED))
			{
				screen_blit(bullet->image[0], (int)(bullet->x - bullet->dx),
					(int)(bullet->y - bullet->dy));
			}

			if (bullet->id == WT_CHARGER)
			{
				if (game.difficulty == DIFFICULTY_ORIGINAL)
					charger_num = bullet->damage;
				else
					charger_num = bullet->damage * 2;

				for (int i = 0 ; i < charger_num ; i++)
					screen_blit(bullet->image[0],
						(int)(bullet->x - RANDRANGE(
							-(charger_num / 6), charger_num / 6)),
						(int)(bullet->y + RANDRANGE(-3, 3)));
			}

			screen_blit(bullet->image[0], (int)bullet->x, (int)bullet->y);
			bullet->x += bullet->dx;
			bullet->y += bullet->dy;

			if (bullet->target != NULL)
			{
				if (bullet->x < bullet->target->x)
					LIMIT_ADD(bullet->dx, homingMissileSpeed, -15, 15);
				else if (bullet->x > bullet->target->x)
					LIMIT_ADD(bullet->dx, -homingMissileSpeed, -15, 15);

				//Rocket is (more or less) in line with target. Fly straight
				if ((bullet->x > bullet->target->x - 1) && (bullet->x < bullet->target->x + 5))
					bullet->dx = 0;

				if (bullet->y < bullet->target->y)
					LIMIT_ADD(bullet->dy, homingMissileSpeed, -15, 15);
				else if (bullet->y > bullet->target->y)
					LIMIT_ADD(bullet->dy, -homingMissileSpeed, -15, 15);

				//Rocket is (more or less) in line with target. Fly straight
				if ((bullet->y > bullet->target->y - 1) && (bullet->y < bullet->target->y + 5))
					bullet->dy = 0;

				if ((bullet->target->shield < 1) || (bullet->target->flags & FL_ISCLOAKED))
					bullet->target = NULL;
			}

			bullet->x += engine.ssx + engine.smx;
			bullet->y += engine.ssy + engine.smy;

			for (int i = 0 ; i < ALIEN_MAX ; i++)
			{
				if ((aliens[i].shield < 1) || (!aliens[i].active))
					continue;

				okayToHit = 0;

				if ((bullet->flags & WF_FRIEND) && (aliens[i].flags & FL_WEAPCO))
					okayToHit = 1;
				if ((bullet->flags & WF_WEAPCO) && (aliens[i].flags & FL_FRIEND))
					okayToHit = 1;
				if ((bullet->id == WT_ROCKET) || (bullet->id == WT_LASER)
						|| (bullet->id == WT_CHARGER))
					okayToHit = 1;

				if (bullet->owner == aliens[i].owner)
					okayToHit = 0;

				if (okayToHit)
				{
					if ((bullet->active) && (bullet_collision(bullet, &aliens[i])))
					{
						old_shield = aliens[i].shield;

						if (bullet->owner == &player)
						{
							game.hits++;
							if (aliens[i].classDef == CD_PHOEBE)
							{
								radio_getRandomMessage(msg, _(
									/// Chris: Phoebe Hit Messages
									/// This is a list of messages separated by "\n".  They are randomly
									/// broadcast by Phoebe when Chris (the player) damages her.
									/// Instead of directly translating these, please populate the list
									/// with messages that work well in the target language,
									/// following the English version only as a general guideline.  Any
									/// number of messages is permitted.
									"OW! I hope that was an accident!\n"
									"Chris, please be more careful!\n"
									"Ouch! What are you shooting at me for?"
								));
								radio_setMessage(FS_PHOEBE, msg, 0);
							}
							else if (aliens[i].classDef == CD_URSULA)
							{
								radio_getRandomMessage(msg, _(
									/// Chris: Ursula Hit Messages
									/// This is a list of messages separated by "\n".  They are randomly
									/// broadcast by Phoebe when Chris (the player) damages her.
									/// Instead of directly translating these, please populate the list
									/// with messages that work well in the target language,
									/// following the English version only as a general guideline.  Any
									/// number of messages is permitted.
									"I am NOT your enemy!\n"
									"Hey! Watch it!\n"
									"What are you doing?! Shoot THEM!\n"
									"Pay some damn attention!"
								));
								radio_setMessage(FS_URSULA, msg, 0);
							}
						}

						if (!(aliens[i].flags & FL_IMMORTAL))
						{
							alien_hurt(&aliens[i], bullet->owner,
								bullet->damage, (bullet->flags & WF_DISABLE));

							aliens[i].hit = 5;
						}
						else if (aliens[i].flags & FL_DAMAGEOWNER)
						{
							alien_hurt(aliens[i].owner, bullet->owner,
								bullet->damage, (bullet->flags & WF_DISABLE));

							aliens[i].owner->hit = 5;
						}

						if (bullet->id == WT_CHARGER)
						{
							if (game.difficulty == DIFFICULTY_ORIGINAL)
							{
								bullet->shield -= old_shield;
								if (bullet->shield < 0)
									bullet->active = 0;
							}
							else
							{
								bullet->damage -= old_shield;
								if (bullet->damage <= 0)
								{
									bullet->active = 0;
									bullet->shield = 0;
								}
							}
							if (!bullet->active)
							{
								audio_playSound(SFX_EXPLOSION, bullet->x, bullet->y);
								for (int i = 0 ; i < 10 ; i++)
									explosion_add(bullet->x + RANDRANGE(-35, 35),
										bullet->y + RANDRANGE(-35, 35),
										SP_BIG_EXPLOSION);
							}
						}
						else
						{
							bullet->active = 0;
							bullet->shield = 0;
						}

						if (bullet->id == WT_ROCKET)
							explosion_add(bullet->x, bullet->y, SP_BIG_EXPLOSION);
						else
							explosion_add(bullet->x, bullet->y, SP_SMALL_EXPLOSION);
					}
				}
			}

			// Check for bullets hitting player
			if ((bullet->flags & WF_WEAPCO) || (bullet->id == WT_ROCKET)
				|| (bullet->id == WT_LASER) || (bullet->id == WT_CHARGER))
			{
				if (bullet->active && (player.shield > 0)
						&& (bullet->owner != &player)
						&& bullet_collision(bullet, &player))
				{
					old_shield = player.shield;

					player_damage(bullet->damage, 0);

					if (player.shield > 0)
					{
						if (bullet->owner->classDef == CD_PHOEBE)
						{
							radio_getRandomMessage(msg, _(
								/// Phoebe: Player Hit Messages
								/// This is a list of messages separated by "\n".  They are randomly
								/// broadcast when Phoebe accidentally damages Chris (the player).
								/// Instead of directly translating these, please populate the list
								/// with messages that work well in the target language,
								/// following the English version only as a general guideline.  Any
								/// number of messages is permitted.
								"Oops! Sorry!\n"
								"Whoops! Are you OK, Chris?\n"
								"Oh, sorry! I didn't see you there!"
							));
							radio_setMessage(FS_PHOEBE, msg, 0);
						}
						else if (bullet->owner->classDef == CD_URSULA)
						{
							radio_getRandomMessage(msg, _(
								/// Ursula: Player Hit Messages
								/// This is a list of messages separated by "\n".  They are randomly
								/// broadcast when Ursula accidentally damages Chris (the player).
								/// Instead of directly translating these, please populate the list
								/// with messages that work well in the target language,
								/// following the English version only as a general guideline.  Any
								/// number of messages is permitted.
								"Get out of the way!\n"
								"Don't fly into my missiles!\n"
								"Dammit, Chris, you made me miss!"
							));
							radio_setMessage(FS_URSULA, msg, 0);
						}
					}

					if (bullet->id == WT_CHARGER)
					{
						bullet->damage -= old_shield;
						if (bullet->damage <= 0)
						{
							bullet->active = 0;
							bullet->shield = 0;
							audio_playSound(SFX_EXPLOSION, bullet->x, bullet->y);
							for (int i = 0 ; i < 10 ; i++)
								explosion_add(bullet->x + RANDRANGE(-35, 35),
									bullet->y + RANDRANGE(-35, 35), SP_BIG_EXPLOSION);
						}
					}
					else
					{
						bullet->active = 0;
						bullet->shield = 0;
					}

					if (bullet->id == WT_ROCKET)
						explosion_add(bullet->x, bullet->y, SP_BIG_EXPLOSION);
					else
						explosion_add(bullet->x, bullet->y, SP_SMALL_EXPLOSION);
				}
			}
		}

		if ((game.difficulty != DIFFICULTY_SUPEREASY)
			&& (game.difficulty != DIFFICULTY_EASY)
			&& ((bullet->owner == &player) || (bullet->id == WT_ROCKET)))
		{
			for (int j = 0 ; j < 20 ; j++)
			{
				if (cargo[j].active)
				{
					if (bullet_collision(bullet, &cargo[j]))
					{
						bullet->active = 0;
						explosion_add(bullet->x, bullet->y, SP_SMALL_EXPLOSION);
						audio_playSound(SFX_HIT, cargo[j].x, cargo[j].y);
						if (cargo[j].collectType != P_PHOEBE)
						{
							cargo[j].active = 0;
							audio_playSound(SFX_EXPLOSION, cargo[j].x, cargo[j].y);
							for (int i = 0 ; i < 10 ; i++)
								explosion_add(cargo[j].x + RANDRANGE(-15, 15),
									cargo[j].y + RANDRANGE(-15, 15),
									SP_BIG_EXPLOSION);
							mission_updateRequirements(M_PROTECT_PICKUP,
								P_CARGO, 1);
						}
					}
				}
			}
		}

		// check to see if a bullet (on any side) hits a mine
		collectable = engine.collectableHead;
		prevCollectable = engine.collectableHead;
		engine.collectableTail = engine.collectableHead;
		while (collectable->next != NULL)
		{
			collectable = collectable->next;

			if (collectable->type == P_MINE)
			{
				if (collectable_collision(collectable, bullet))
				{
					collectable->active = 0;

					if (bullet->id != WT_CHARGER)
					{
						bullet->active = 0;
					}
					else
					{
						bullet->shield--;
						if (bullet->shield < 0)
							bullet->active = 0;
					}

					if (bullet->owner == &player)
					{
						game.minesKilled++;
						game.hits++;
					}
				}
			}

			if (collectable->active)
			{
				prevCollectable = collectable;
				engine.collectableTail = collectable;
			}
			else
			{
				collectable_explode(collectable);
				prevCollectable->next = collectable->next;
				free(collectable);
				collectable = prevCollectable;
			}
		}

		bullet->shield--;

		if (bullet->shield < 1)
		{
			if (bullet->flags & WF_TIMEDEXPLOSION)
			{
				audio_playSound(SFX_EXPLOSION, bullet->x, bullet->y);
				for (int i = 0 ; i < 10 ; i++)
					explosion_add(bullet->x + RANDRANGE(-35, 35),
						bullet->y + RANDRANGE(-35, 35), SP_BIG_EXPLOSION);

				player_checkShockDamage(bullet->x, bullet->y);
			}
			bullet->active = 0;
		}

		if (bullet->active)
		{
			prevBullet = bullet;
			engine.bulletTail = bullet;
		}
		else
		{
			prevBullet->next = bullet->next;
			free(bullet);
			bullet = prevBullet;
		}
	}
}

static void game_doAliens()
{
	static float barrierLoop = 0;

	int shapeToUse;
	int canFire;
	int n;

	barrierLoop += 0.2;

	// A global variable for checking if all the aliens are dead
	engine.allAliensDead = 1;

	for (int i = 0 ; i < ALIEN_MAX ; i++)
	{
		if (aliens[i].active)
		{
			if (aliens[i].shield > 0)
			{
				if ((aliens[i].flags & FL_WEAPCO) && (!(aliens[i].flags & FL_DISABLED)))
					engine.allAliensDead = 0;

				// Set part attributes
				if (aliens[i].owner != &aliens[i])
				{
					aliens[i].face = aliens[i].owner->face;

					if (aliens[i].face == 0)
						aliens[i].x = aliens[i].owner->x - aliens[i].dx;
					else
						aliens[i].x = (aliens[i].owner->x + aliens[i].dx
								+ aliens[i].owner->image[0]->w
								- aliens[i].image[0]->w);

					aliens[i].y = (aliens[i].owner->y + aliens[i].dy);

					if (aliens[i].owner->shield < 1)
					{
						if ((aliens[i].classDef != CD_URANUSBOSSWING1)
								&& (aliens[i].classDef != CD_URANUSBOSSWING2))
						{
							aliens[i].shield = 0;
						}
						else
						{
							aliens[i].flags &= ~FL_IMMORTAL;
							aliens[i].owner = &aliens[i];
							aliens[i].chance[0] = 25;
						}
					}
				}

				canFire = 1; // The alien is allowed to fire

				LIMIT_ADD(aliens[i].thinktime, -1, 0, 250);

				if (aliens[i].target->shield < 1)
					aliens[i].target = &aliens[i];

				// Specific to Sid to stop him pissing about(!)
				if ((aliens[i].classDef == CD_SID)
						&& (aliens[i].target->flags & FL_DISABLED))
					aliens[i].target = &aliens[i];

 				if (aliens[i].target == &aliens[i])
				{
					if (engine.missionCompleteTimer == 0)
					{
						alien_searchForTarget(&aliens[i]);
					}
					else
					{
						if (aliens[i].flags & FL_FRIEND)
						{
							aliens[i].target = &player;
							aliens[i].thinktime = 1;
						}
					}
				}

				if ((!(aliens[i].flags & FL_DISABLED))
					&& (aliens[i].thinktime == 0) && (aliens[i].target != &aliens[i])
					&& (aliens[i].owner == &aliens[i]))
				{
					if (aliens[i].classDef == CD_KLINE)
						alien_setKlineAI(&aliens[i]);
					else
						alien_setAI(&aliens[i]);

					aliens[i].thinktime = (rand() % 25) * 10;

					// Face direction of movement unless you always face
					// your target(!)
					if (!(aliens[i].flags & FL_ALWAYSFACE))
					{
						aliens[i].face = (aliens[i].dx > 0);
					}

					LIMIT(aliens[i].dx, -aliens[i].speed, aliens[i].speed);
					LIMIT(aliens[i].dy, -aliens[i].speed, aliens[i].speed);

				}

				if (aliens[i].flags & FL_ALWAYSFACE)
				{
					aliens[i].face = 0;
					if (aliens[i].x > aliens[i].target->x) aliens[i].face = 1;
				}

				if ((game.area == MISN_ELLESH)
						&& ((aliens[i].classDef == CD_BOSS)
						    || (game.difficulty != DIFFICULTY_ORIGINAL)))
					aliens[i].face = 0;

				if ((aliens[i].flags & FL_DEPLOYDRONES) && ((rand() % 300) == 0))
					alien_addDrone(&aliens[i]);

				if (aliens[i].flags & FL_LEAVESECTOR)
				{
					// Note: The original version of this line incorrectly
					// specified -15 as the *maximum* instead of the
					// *minimum*, which at the time was equivalent to
					// ``aliens[i].dx = -15``.
					LIMIT_ADD(aliens[i].dx, ALIEN_WARP_ACCEL, ALIEN_WARP_SPEED, 0);
					aliens[i].dy = 0;
					aliens[i].thinktime = 999;
					aliens[i].face = 0;

					if (aliens[i].x >= 5000)
					{
						aliens[i].flags &= ~FL_LEAVESECTOR;
						aliens[i].flags |= FL_ESCAPED;
						aliens[i].active = 0;

						if (aliens[i].classDef == CD_CLOAKFIGHTER)
						{
							game.experimentalShield = aliens[i].shield;
							info_setLine("Experimental Fighter has fled",
								FONT_CYAN);
						}

						aliens[i].shield = 0;
						mission_updateRequirements(M_ESCAPE_TARGET,
							aliens[i].classDef, 1);

						if (aliens[i].classDef != CD_CLOAKFIGHTER)
							mission_updateRequirements(M_DESTROY_TARGET_TYPE,
								aliens[i].classDef, 1);
					}
				}

				// This deals with the Experimental Fighter in Mordor
				// (and Kline on the final mission)
				// It can cloak and uncloak at random. When cloaked,
				// its sprite is not displayed. However the engine
				// trail is still visible!
				if ((aliens[i].flags & FL_CANCLOAK) && ((rand() % 500) == 0))
				{
					if (aliens[i].flags & FL_ISCLOAKED)
						aliens[i].flags -= FL_ISCLOAKED;
					else
						aliens[i].flags += FL_ISCLOAKED;
					audio_playSound(SFX_CLOAK, aliens[i].x, aliens[i].y);
				}

				if (aliens[i].classDef == CD_BARRIER)
				{
					aliens[i].dx = -10 + (sinf(barrierLoop + aliens[i].speed) * 60);
					aliens[i].dy = 20 + (cosf(barrierLoop + aliens[i].speed) * 40);
				}

				if ((aliens[i].classDef == CD_MOBILESHIELD)
						&& (aliens[ALIEN_BOSS].active)
						&& (aliens[ALIEN_BOSS].shield > 0))
				{
					LIMIT_ADD(aliens[ALIEN_BOSS].shield, 1, 0,
						aliens[ALIEN_BOSS].maxShield);
				}

				LIMIT_ADD(aliens[i].reload[0], -1, 0, 999);
				LIMIT_ADD(aliens[i].reload[1], -1, 0, 999);

				if ((!(aliens[i].flags & FL_DISABLED))
						&& (!(aliens[i].flags & FL_NOFIRE)))
				{
					if ((aliens[i].target->shield > 0))
						canFire = alien_checkTarget(&aliens[i]);

					if (((aliens[i].thinktime % 2) == 0)
							&& (aliens[i].flags & FL_FRIEND))
						canFire = alien_enemiesInFront(&aliens[i]);
				}
				else
				{
					canFire = 0;
				}

				if (canFire)
				{
					for (int j = 0 ; j < 2 ; j++)
					{
						if ((aliens[i].reload[j] == 0)
								&& ((rand() % 1000 < aliens[i].chance[j])
									|| (aliens[i].flags & FL_CONTINUOUS_FIRE)))
						{
							if ((aliens[i].weaponType[j] != W_ENERGYRAY)
									&& (aliens[i].weaponType[j] != W_LASER))
							{
								if (aliens[i].weaponType[j] == W_CHARGER)
									aliens[i].ammo[j] = 50 + rand() % 150;
								ship_fireBullet(&aliens[i], j);
							}
							else if (aliens[i].weaponType[j] == W_LASER)
							{
								aliens[i].flags += FL_FIRELASER;
							}
							// Note: ammo[0] is required whether the ray is primary
							// or secondary because futher below, ammo[0] increases on
							// any alien that isn't currently firing a ray.
							else if ((aliens[i].weaponType[j] == W_ENERGYRAY)
									&& (aliens[i].ammo[0] >= RAY_INTERVAL))
							{
								aliens[i].flags += FL_FIRERAY;
								audio_playSound(SFX_ENERGYRAY, aliens[i].x, aliens[i].y);
							}
						}
					}
				}

				if (aliens[i].flags & FL_FIRERAY)
				{
					ship_fireRay(&aliens[i]);
				}
				else
				{
					LIMIT_ADD(aliens[i].ammo[0], 1, 0, RAY_INTERVAL);
				}

				if (aliens[i].flags & FL_FIRELASER)
				{
					ship_fireBullet(&aliens[i], 1);
					if ((rand() % 25) == 0)
						aliens[i].flags -= FL_FIRELASER;
				}

				if (aliens[i].flags & FL_DROPMINES)
				{
					if ((rand() % 150) == 0)
						collectable_add(aliens[i].x, aliens[i].y, P_MINE, 25,
							600 + rand() % 2400);

					// Kline drops mines a lot more often
					if ((&aliens[i] == &aliens[ALIEN_KLINE]))
					{
						if ((rand() % 10) == 0)
							collectable_add(aliens[i].x, aliens[i].y, P_MINE, 25,
								600 + rand() % 2400);
					}
				}

				shapeToUse = aliens[i].imageIndex[aliens[i].face];

				if (aliens[i].hit)
					shapeToUse += SS_HIT_INDEX;

				LIMIT_ADD(aliens[i].hit, -1, 0, 100);

				if ((aliens[i].x + aliens[i].image[0]->w > 0)
						&& (aliens[i].x < screen->w)
						&& (aliens[i].y + aliens[i].image[0]->h > 0)
						&& (aliens[i].y < screen->h))
				{
					if ((!(aliens[i].flags & FL_DISABLED))
							&& (aliens[i].classDef != CD_ASTEROID)
							&& (aliens[i].classDef != CD_ASTEROID2))
						explosion_addEngine(&aliens[i]);
					if ((!(aliens[i].flags & FL_ISCLOAKED))
							|| (aliens[i].hit > 0))
						screen_blit(gfx_shipSprites[shapeToUse], (int)aliens[i].x,
							(int)aliens[i].y);
					if (aliens[i].flags & FL_DISABLED)
					{
						if ((rand() % 10) == 0)
							explosion_add(aliens[i].x + (rand() % aliens[i].image[0]->w),
								aliens[i].y + (rand() % aliens[i].image[0]->h),
								SP_ELECTRICAL);
					}
				}

				if ((game.area == MISN_MARS) && (aliens[i].x < -60))
					aliens[i].active = 0;
			}
			else
			{
				aliens[i].shield--;
				if ((aliens[i].x > 0) && (aliens[i].x < screen->w)
					&& (aliens[i].y > 0) && (aliens[i].y < screen->h))
				{
					screen_blit(aliens[i].image[aliens[i].face], (int)aliens[i].x,
						(int)aliens[i].y);
					explosion_add(aliens[i].x + (rand() % aliens[i].image[0]->w),
						aliens[i].y + (rand() % aliens[i].image[0]->h),
						SP_BIG_EXPLOSION);
				}
				if (aliens[i].shield < aliens[i].deathCounter)
				{
					aliens[i].active = 0;
					if ((aliens[i].classDef == CD_BOSS)
						|| (aliens[i].owner == &aliens[ALIEN_BOSS])
						|| (aliens[i].flags & FL_FRIEND)
						|| (aliens[i].classDef == CD_ASTEROID)
						|| (aliens[i].classDef == CD_KLINE))
					{
						game_addDebris((int)aliens[i].x, (int)aliens[i].y,
							aliens[i].maxShield);
					}

					if (aliens[i].classDef == CD_ASTEROID)
					{
						n = 1 + (rand() % 3);
						for (int j = 0 ; j < n ; j++)
						{
							alien_addSmallAsteroid(&aliens[i]);
						}
					}
				}
			}

			// Adjust the movement even whilst exploding
			if ((!(aliens[i].flags & FL_NOMOVE))
					&& (!(aliens[i].flags & FL_DISABLED)))
				alien_move(&aliens[i]);

			if ((game.area != MISN_ELLESH) || (aliens[i].shield < 0))
				aliens[i].x += engine.ssx;

			aliens[i].x += engine.smx;
			aliens[i].y += engine.ssy + engine.smy;
		}
	}
}

static void game_doPlayer()
{
	int shapeToUse;
	float cd;
	float cc;
	double xm = 0;
	double ym = 0;
	int xmoved = 0;
	int ymoved = 0;
	char msg[STRMAX];

	// This causes the motion to slow
	engine.ssx *= 0.99;
	engine.ssy *= 0.99;

	engine.smx = 0;
	engine.smy = 0;

	if (player.shield > -100)
	{
		if (player.shield > 0)
		{
			if ((engine.keyState[KEY_FIRE]))
				ship_fireBullet(&player, 0);

			if ((engine.keyState[KEY_ALTFIRE]) && (player.weaponType[1] != W_NONE))
			{
				if ((player.weaponType[1] != W_CHARGER)
						&& (player.weaponType[1] != W_LASER)
						&& (player.ammo[1] > 0))
				{
					ship_fireBullet(&player, 1);
				}

				if (player.weaponType[1] == W_LASER)
				{
					if (player.ammo[1] < 100)
					{
						ship_fireBullet(&player, 1);

						if (!engine.cheatAmmo)
							player.ammo[1] += 1;

						if (player.ammo[1] >= 100)
						{
							player.ammo[1] = 200;
							info_setLine("Laser Overheat!", FONT_WHITE);
						}
					}
				}
			}

			if (player.weaponType[1] == W_CHARGER)
			{
				if (engine.keyState[KEY_ALTFIRE]
						&& ((game.difficulty == DIFFICULTY_ORIGINAL)
							|| !(engine.keyState[KEY_FIRE])))
				{
					if (!player_chargerFired)
					{
						// With ammo cheat, cause the charge cannon to
						// fire at full blast immediately.
						if (engine.cheatAmmo)
							player.ammo[1] = 200;

						if (game.difficulty == DIFFICULTY_ORIGINAL)
						{
							LIMIT_ADD(player.ammo[1], 1, 0, 200);
						}
						else
						{
							LIMIT_ADD(player.ammo[1], 1, 0, 150);
							if (player.ammo[1] >= 150)
							{
								ship_fireBullet(&player, 1);
								player.ammo[1] = 0;
								player_chargerFired = 1;
							}
						}
					}
				}
				else
				{
					if (player.ammo[1] > 0)
						ship_fireBullet(&player, 1);
					player.ammo[1] = 0;
					player_chargerFired = 0;
				}
			}

			if ((engine.keyState[KEY_SWITCH]))
			{
				if ((weapons[W_PLAYER_WEAPON].ammo[0] >= 3)
					&& ((weapons[W_PLAYER_WEAPON].ammo[0] <= game.maxPlasmaOutput)
						|| (game.difficulty == DIFFICULTY_ORIGINAL)))
				{
					weapons[W_PLAYER_WEAPON].flags ^= WF_SPREAD;

					if (weapons[W_PLAYER_WEAPON].flags & WF_SPREAD)
					{
						info_setLine("Weapon set to Spread", FONT_WHITE);
					}
					else
					{
						info_setLine("Weapon set to Concentrate", FONT_WHITE);
					}
				}

				engine.keyState[KEY_SWITCH] = 0;
			}

			LIMIT_ADD(player.reload[0], -1, 0, 999);
			LIMIT_ADD(player.reload[1], -1, 0, 999);

			xm = engine.keyState[KEY_RIGHT] - engine.keyState[KEY_LEFT];
			if (!xm)
				xm = engine.xaxis;
			
			if (xm)
			{
				player.x += xm * player.speed;
				engine.ssx -= xm * 0.1;
				player.face = (xm < 0 ? 1 : 0);
				xmoved = 1;
			}

			ym = engine.keyState[KEY_DOWN] - engine.keyState[KEY_UP];
			if (!ym)
				ym = engine.yaxis;
			
			if (ym)
			{
				player.y += ym * player.speed;
				engine.ssy -= ym * 0.1;
				ymoved = 1;
			}

			if (engine.keyState[KEY_ESCAPE])
			{
				if ((engine.done == ENGINE_RUNNING)
					&& (engine.gameSection == SECTION_GAME)
					&& (mission.remainingObjectives1 == 0))
				{
					audio_playSound(SFX_FLY, screen->w / 2, screen->h / 2);
					engine.done = ENGINE_SYSEXIT;
					engine.missionCompleteTimer = (SDL_GetTicks() - 1);
				}
			}

			if (engine.keyState[KEY_PAUSE])
			{
				engine.paused = 1;
				engine.keyState[KEY_PAUSE] = 0;
			}

			if ((game.area == MISN_ELLESH)
					|| (game.area == MISN_MARS))
			{
				player.face = 0;
				xmoved = 1;
				ymoved = 1;
			}

			if (engine.done == ENGINE_RUNNING)
			{
				if (game.difficulty == DIFFICULTY_ORIGINAL)
				{
					LIMIT(player.x, X_VIEW_BORDER, screen->w - X_VIEW_BORDER);
					LIMIT(player.y, Y_VIEW_BORDER, screen->h - Y_VIEW_BORDER);
				}
				else
				{
					if (xmoved)
					{
						if (player.x < X_VIEW_BORDER)
						{
							engine.smx += X_VIEW_BORDER - player.x;
							player.x = X_VIEW_BORDER;
						}
						else if (player.x > screen->w - X_VIEW_BORDER)
						{
							engine.smx += (screen->w - X_VIEW_BORDER) - player.x;
							player.x = screen->w - X_VIEW_BORDER;
						}
					}
					else if (game.difficulty != DIFFICULTY_ORIGINAL)
					{
						cd = player.x - screen->w / 2;
						if (cd < 0)
						{
							cc = MAX(cd / 10, MAX(0, engine.ssx) - CAMERA_MAX_SPEED);
							player.x -= cc;
							engine.smx -= cc;
						}
						else if (cd > 0)
						{
							cc = MIN(cd / 10, CAMERA_MAX_SPEED + MIN(0, engine.ssx));
							player.x -= cc;
							engine.smx -= cc;
						}
					}

					if (ymoved)
					{
						if (player.y < Y_VIEW_BORDER)
						{
							engine.smy += Y_VIEW_BORDER - player.y;
							player.y = Y_VIEW_BORDER;
						}
						else if (player.y > screen->h - Y_VIEW_BORDER)
						{
							engine.smy += (screen->h - Y_VIEW_BORDER) - player.y;
							player.y = screen->h - Y_VIEW_BORDER;
						}
					}
					else if (game.difficulty != DIFFICULTY_ORIGINAL)
					{
						cd = player.y - screen->h / 2;
						if (cd < 0)
						{
							cc = MAX(cd / 10, MAX(0, engine.ssy) - CAMERA_MAX_SPEED);
							player.y -= cc;
							engine.smy -= cc;
						}
						else if (cd > 0)
						{
							cc = MIN(cd / 10, CAMERA_MAX_SPEED + MIN(0, engine.ssy));
							player.y -= cc;
							engine.smy -= cc;
						}
					}
				}
			}

			explosion_addEngine(&player);

			shapeToUse = player.face;

			if (player.hit)
				shapeToUse += SS_HIT_INDEX;

			LIMIT_ADD(player.hit, -1, 0, 100);

			screen_blit(gfx_shipSprites[shapeToUse], (int)player.x, (int)player.y);
			if ((player.maxShield > 1) && (player.shield <= engine.lowShield)
					&& CHANCE(player.shield > 1 ? 1. / 10 : 3. / 10))
				explosion_add(player.x + RANDRANGE(-10, 10),
					player.y + RANDRANGE(-10, 20), SP_SMOKE);
		}
		else
		{
			// Player is dead. At this point, the shield counts down to
			// -100 and does death and explosion stuff along the way.
			player.active = 0;
			player.shield--;
			if (player.shield == -1)
			{
				if (aliens[ALIEN_KLINE].active)
				{
					if (game.area == MISN_VENUS)
					{
						radio_getRandomMessage(msg, _(
							/// Kline Venus insult messages
							/// This is a list of insults separated by "\n".  They are randomly
							/// broadcast when the player dies in the Venus mission.
							/// Instead of directly translating these, please populate the list
							/// with insults that work well in the target language,
							/// following the English version only as a general guideline.  Any
							/// number of insults is permitted.
							"Fool.\n"
							"And now you're nothing but a DEAD hero."
						));
						radio_setMessage(FS_KLINE, msg, 1);
					}
					else
					{
						radio_getRandomMessage(msg, _(
							/// Kline insult messages
							/// This is a list of insults separated by "\n".  They are randomly
							/// broadcast when the player dies in a mission Kline is in (except Venus).
							/// Instead of directly translating these, please populate the list
							/// with insults that work well in the target language,
							/// following the English version only as a general guideline.  Any
							/// number of insults is permitted.
							"Pathetic.\n"
							"How very disappointing...\n"
							"Heroic. And foolish."
						));
						radio_setMessage(FS_KLINE, msg, 1);
					}
				}
				else if ((aliens[ALIEN_BOSS].active) && (aliens[ALIEN_BOSS].classDef == CD_KRASS))
				{
					/// Dialog: Krass Tyler
					/// Used when the player is killed in the Jupiter mission.
					radio_setMessage(FS_KRASS, _("That was the easiest $90,000,000 I've ever earned! Bwah! Ha! Ha! Ha!"), 1);
				}

				// Make it look like the ships are all still moving...
				if (game.area == MISN_ELLESH)
				{
					for (int i = 0 ; i < ALIEN_MAX ; i++)
						aliens[i].flags |= FL_LEAVESECTOR;
				}

				audio_playSound(SFX_DEATH, player.x, player.y);
				audio_playSound(SFX_EXPLOSION, player.x, player.y);
			}

			engine.keyState[KEY_UP] = 0;
			engine.keyState[KEY_DOWN] = 0;
			engine.keyState[KEY_LEFT] = 0;
			engine.keyState[KEY_RIGHT] = 0;
			engine.xaxis = 0;
			engine.yaxis = 0;
			if (CHANCE(1. / 3.))
				explosion_add(player.x + RANDRANGE(-10, 10),
					player.y + RANDRANGE(-10, 10), SP_BIG_EXPLOSION);
			if (player.shield == -99)
				game_addDebris((int)player.x, (int)player.y, player.maxShield);
		}
	}

	LIMIT(engine.ssx, -CAMERA_MAX_SPEED, CAMERA_MAX_SPEED);
	LIMIT(engine.ssy, -CAMERA_MAX_SPEED, CAMERA_MAX_SPEED);

	// Specific for the mission were you have to chase the Executive Transport
	if (((game.area == MISN_ELLESH) && (player.shield > 0))
			|| (game.area == MISN_MARS))
	{
		engine.ssx = -6;
		engine.ssy = 0;
	}

	player.dx = engine.ssx;
	player.dy = engine.ssy;
}

static void game_doCargo()
{
	float dx, dy, chainX, chainY;

	for (int i = 0 ; i < MAX_CARGO ; i++)
	{
		if (cargo[i].active)
		{
			if (!cargo[i].owner->active)
			{
				cargo_becomeCollectable(i);
				continue;
			}

			screen_blit(cargo[i].image[0], (int)cargo[i].x, (int)cargo[i].y);

			cargo[i].x += engine.ssx + engine.smx;
			cargo[i].y += engine.ssy + engine.smy;

			LIMIT(cargo[i].x, cargo[i].owner->x - 50, cargo[i].owner->x + 50);
			LIMIT(cargo[i].y, cargo[i].owner->y - 50, cargo[i].owner->y + 50);

			dx = (cargo[i].x - cargo[i].owner->x) / 10;
			dy = (cargo[i].y - cargo[i].owner->y) / 10;
			chainX = cargo[i].x - cargo[i].dx;
			chainY = cargo[i].y - cargo[i].dy;

			// draw the chain link line
			for (int j = 0 ; j < 10 ; j++)
			{
				screen_blit(gfx_sprites[SP_CHAIN_LINK], (int)chainX, (int)chainY);
				chainX -= dx;
				chainY -= dy;
			}
		}
	}
}

static void game_doDebris()
{
	Object *prevDebris = engine.debrisHead;
	Object *debris = engine.debrisHead;
	engine.debrisTail = engine.debrisHead;

	while (debris->next != NULL)
	{
		debris = debris->next;

		if (debris->thinktime > 0)
		{
			debris->thinktime--;

			debris->x += engine.ssx + engine.smx;
			debris->y += engine.ssy + engine.smy;
			debris->x += debris->dx;
			debris->y += debris->dy;

			explosion_add(debris->x + RANDRANGE(-10, 10), debris->y + RANDRANGE(-10, 10), SP_BIG_EXPLOSION);
		}

		if (debris->thinktime < 1)
		{
			prevDebris->next = debris->next;
			free(debris);
			debris = prevDebris;
		}
		else
		{
			prevDebris = debris;
			engine.debrisTail = debris;
		}

	}
}

/*
Loops through active explosions and decrements their think time.
If their thinktime is divisable by 5, then the frame is changed to
the next one up (for example 0->1->2-3). When their think time is 0,
the explosion is killed off.
*/
void game_doExplosions()
{
	Object *prevExplosion = engine.explosionHead;
	Object *explosion = engine.explosionHead;
	engine.explosionTail = engine.explosionHead;

	while (explosion->next != NULL)
	{
		explosion = explosion->next;

		if (explosion->active)
		{
			explosion->x += engine.ssx + engine.smx;
			explosion->y += engine.ssy + engine.smy;

			screen_blit(explosion->image[0], (int)explosion->x, (int)explosion->y);

			if(CHANCE(1. / 7.))
			{
				explosion->thinktime -= 7;

				if(explosion->thinktime < 1)
				{
					explosion->active = 0;
				}
				else
				{
					explosion->face++;
					explosion->image[0] = gfx_sprites[explosion->face];
				}
			}
		}

		if (explosion->active)
		{
			prevExplosion = explosion;
			engine.explosionTail = explosion;
		}
		else
		{
			prevExplosion->next = explosion->next;
			free(explosion);
			explosion = prevExplosion;
		}
	}
}

/*
Draw an arrow at the edge of the screen for each enemy ship that is not visible.
*/
static void game_doArrow(int i)
{
	int arrow = -1;
	int arrowX;
	int arrowY;

	int indicator = -1;
	int indicatorX;
	int indicatorY;

	if (i < 0 || !aliens[i].active || aliens[i].shield <= 0 || aliens[i].flags & FL_ISCLOAKED)
		return;

	if (aliens[i].x + aliens[i].image[0]->w < 0)
	{
		if (aliens[i].y + aliens[i].image[0]->h < 0)
			arrow = (((game.difficulty != DIFFICULTY_ORIGINAL)
					&& (aliens[i].flags & FL_FRIEND)) ?
				SP_ARROW_FRIEND_NORTHWEST : SP_ARROW_NORTHWEST);
		else if (aliens[i].y > screen->h)
			arrow = (((game.difficulty != DIFFICULTY_ORIGINAL)
					&& (aliens[i].flags & FL_FRIEND)) ?
				SP_ARROW_FRIEND_SOUTHWEST : SP_ARROW_SOUTHWEST);
		else
			arrow = (((game.difficulty != DIFFICULTY_ORIGINAL)
					&& (aliens[i].flags & FL_FRIEND)) ?
				SP_ARROW_FRIEND_WEST : SP_ARROW_WEST);
	}
	else if (aliens[i].x > screen->w)
	{
		if (aliens[i].y + aliens[i].image[0]->h < 0)
			arrow = (((game.difficulty != DIFFICULTY_ORIGINAL)
					&& (aliens[i].flags & FL_FRIEND)) ?
				SP_ARROW_FRIEND_NORTHEAST : SP_ARROW_NORTHEAST);
		else if (aliens[i].y > screen->h)
			arrow = (((game.difficulty != DIFFICULTY_ORIGINAL)
					&& (aliens[i].flags & FL_FRIEND)) ?
				SP_ARROW_FRIEND_SOUTHEAST : SP_ARROW_SOUTHEAST);
		else
			arrow = (((game.difficulty != DIFFICULTY_ORIGINAL)
					&& (aliens[i].flags & FL_FRIEND)) ?
				SP_ARROW_FRIEND_EAST : SP_ARROW_EAST);
	}
	else if (aliens[i].y + aliens[i].image[0]->h < 0)
		arrow = (((game.difficulty != DIFFICULTY_ORIGINAL)
				&& (aliens[i].flags & FL_FRIEND)) ?
			SP_ARROW_FRIEND_NORTH : SP_ARROW_NORTH);
	else if (aliens[i].y > screen->h)
		arrow = (((game.difficulty != DIFFICULTY_ORIGINAL)
				&& (aliens[i].flags & FL_FRIEND)) ?
			SP_ARROW_FRIEND_SOUTH : SP_ARROW_SOUTH);

	if (arrow != -1)
	{
		if (game.difficulty == DIFFICULTY_ORIGINAL)
		{
			indicator = SP_INDICATOR_TARGET;
			indicatorX = (screen->w / 2) - (gfx_sprites[indicator]->w / 2);
			indicatorY = 120;
			arrowX = indicatorX + gfx_sprites[indicator]->w / 2 - (gfx_sprites[arrow]->w / 2);
			arrowY = indicatorY + gfx_sprites[indicator]->h + 5;
			screen_blit(gfx_sprites[arrow], arrowX, arrowY);
			screen_blit(gfx_sprites[indicator], indicatorX, indicatorY);
		}
		else
		{
			arrowX = aliens[i].x + aliens[i].image[0]->w / 2 - gfx_sprites[arrow]->w;
			arrowX = MAX(0, MIN(arrowX, screen->w - gfx_sprites[arrow]->w));
			arrowY = aliens[i].y + aliens[i].image[0]->h / 2 - gfx_sprites[arrow]->h;
			arrowY = MAX(0, MIN(arrowY, screen->h - gfx_sprites[arrow]->h));
			screen_blit(gfx_sprites[arrow], arrowX, arrowY);

			if (i == ALIEN_SID)
				indicator = SP_INDICATOR_SID;
			else if (i == ALIEN_PHOEBE)
				indicator = SP_INDICATOR_PHOEBE;
			else if (i == ALIEN_URSULA)
				indicator = SP_INDICATOR_URSULA;
			else if (i == ALIEN_KLINE)
				indicator = SP_INDICATOR_KLINE;
			else if (i == engine.targetIndex)
				indicator = SP_INDICATOR_TARGET;

			if (indicator != -1)
			{
				indicatorX = arrowX + gfx_sprites[arrow]->w / 2 - gfx_sprites[indicator]->w / 2;
				indicatorX = MAX(indicatorX, gfx_sprites[arrow]->w + 5);
				indicatorX = MIN(indicatorX, screen->w - gfx_sprites[arrow]->w - gfx_sprites[indicator]->w - 5);
				indicatorY = arrowY + gfx_sprites[arrow]->h / 2 - gfx_sprites[indicator]->h / 2;
				indicatorY = MAX(indicatorY, gfx_sprites[arrow]->h + 5);
				indicatorY = MIN(indicatorY, screen->h - gfx_sprites[arrow]->h - gfx_sprites[indicator]->h - 5);
				screen_blit(gfx_sprites[indicator], indicatorX, indicatorY);
			}
		}
	}
}

static void game_doHud()
{
	static int last_arrow = -1;
	int shieldColor = 0;
	SDL_Rect bar;
	SDL_Rect bar_foutline;
	SDL_Rect bar_fcolor;
	int fontColor;
	int tTextIndex;
	char text[STRMAX_SHORT];
	float nbars; // A float for the sake of float division
	float min_shield, max_shield;
	float shield_pct;
	int i;
	int c;

	screen_addBuffer(0, 20, screen->w, 25);
	screen_addBuffer(0, screen->h - 50, screen->w, 34);

	if (engine.minutes > -1)
	{
		if (game.area == MISN_MARS)
		{
			fontColor = FONT_WHITE;
		}
		else
		{
			if ((engine.minutes == 0) && (engine.seconds <= 29))
				fontColor = FONT_RED;
			else if ((engine.minutes == 0) && (engine.seconds > 29))
				fontColor = FONT_YELLOW;
			else
				fontColor = FONT_WHITE;
		}

		/// Each "%.2d" must be retained.  They are replaced with the minutes and seconds left
		/// to complete the mission, respectively (or, in the case of the Mars mission, the
		/// minutes and seconds, respectively, until the mission is completed).
		/// If you are familiar with C string formatting, they can be modified as long as
		/// the "d" type remains.  For example, you can replace "%.2d" with "%d" to allow the
		/// timer to use single-digit numbers.
		/// The ":" can also be replaced just like any text.  For example, this would be fine:
		///     "Time Remaining - %d minutes and %d seconds"
		snprintf(text, STRMAX_SHORT, _("Time Remaining - %.2d:%.2d"), engine.minutes, engine.seconds);
		gfx_createTextObject(TS_TIME, text, 0, 0, fontColor);
		screen_blitText(TS_TIME, screen->w / 2 - gfx_textSprites[TS_TIME].image->w / 2, 20);
	}

	if (game.area != MISN_INTERCEPTION)
	{
		/// "%d" must be retained.  It is replaced with the number of mission objectives remaining.
		snprintf(text, STRMAX_SHORT, _("Objectives Remaining: %d"), (mission.remainingObjectives1 + mission.remainingObjectives2));
		gfx_createTextObject(TS_OBJECTIVES, text, 0, 0, FONT_WHITE);
		screen_blitText(TS_OBJECTIVES, screen->w - gfx_textSprites[TS_OBJECTIVES].image->w - 25, 20);
	}

	/// "%d" must be retained.  It is replaced with the player's current total cash.
	snprintf(text, STRMAX_SHORT, _("Cash: $%d"), game.cash);
	gfx_createTextObject(TS_CASH, text, 0, 0, FONT_WHITE);
	screen_blitText(TS_CASH, 25, 20);

	if (game.difficulty == DIFFICULTY_ORIGINAL)
	{
		i = engine.targetIndex;
		if ((i >= 0) && aliens[i].active && (aliens[i].shield > 0) && (!(aliens[i].flags & FL_ISCLOAKED)))
			game_doArrow(i);
		else
		{
			i = last_arrow;
			if ((i >= 0) && aliens[i].active && (aliens[i].shield > 0) && (!(aliens[i].flags & FL_ISCLOAKED)))
				game_doArrow(i);
			else
			{
				last_arrow = rand() % ALIEN_MAX;
				if (aliens[last_arrow].flags & FL_FRIEND)
					last_arrow = 0;

				game_doArrow(last_arrow);
			}
		}
	}
	else
	{
		for (i = 0; i < ALIEN_MAX; i++)
			game_doArrow(i);
	}

	fontColor = FONT_WHITE;
	if (player.ammo[0] > 0)
	{
		if (player.ammo[0] <= 25) fontColor = FONT_YELLOW;
		if (player.ammo[0] <= 10) fontColor = FONT_RED;
	}
	/// "%.3d" must be retained.  It is replaced with the amount of plasma ammo.
	snprintf(text, STRMAX_SHORT, _("Plasma: %.3d"), player.ammo[0]);
	gfx_createTextObject(TS_PLASMA, text, 0, 0, fontColor);
	screen_blitText(TS_PLASMA, screen->w * 5 / 16, screen->h - 50);

	if (player.weaponType[1] == W_CHARGER)
	{
		/// Used to indicate the charge meter for the charger cannon in the HUD.
		gfx_createTextObject(TS_AMMO, _("Charge"), 0, 0, FONT_WHITE);
	}
	else if (player.weaponType[1] == W_LASER)
	{
		/// Used to indicate the heat meter for the laser cannon in the HUD.
		gfx_createTextObject(TS_AMMO, _("Heat"), 0, 0, FONT_WHITE);
	}
	else
	{
		/// "%.2d" must be retained.  It is replaced with the amount of rocket ammo.
		snprintf(text, STRMAX_SHORT, _("Rockets: %.2d"), player.ammo[1]);
		gfx_createTextObject(TS_AMMO, text, 0, 0, FONT_WHITE);
	}
	screen_blitText(TS_AMMO, screen->w / 2, screen->h - 50);

	if (((player.weaponType[1] == W_CHARGER) || (player.weaponType[1] == W_LASER)) && (player.ammo[1] > 0))
	{
		c = white;
		if (player.ammo[1] > 100)
			c = red;

		bar.x = screen->w / 2 + gfx_textSprites[TS_AMMO].image->w + 10;
		bar.y = screen->h - 50;
		bar.h = 12;

		for (i = 0 ; i < (player.ammo[1] / 5) ; i++)
		{
			bar.w = MAX(screen->w / 800, 1);
			SDL_FillRect(screen, &bar, c);
			bar.x += bar.w + (screen->w / 800);
		}
	}

	if ((!mission_checkCompleted()) && (SDL_GetTicks() >= engine.counter2))
	{
		engine.timeTaken++;
		engine.counter2 = SDL_GetTicks() + 1000;
		if (engine.missionCompleteTimer == 0)
			events_check();
	}

	if ((engine.timeMission) && (player.shield > 0)
			&& ((!engine.cheatTime) || (game.area == MISN_MARS)))
	{
		if (SDL_GetTicks() >= engine.counter)
		{
			if ((engine.seconds > 1) && (engine.seconds <= 11) && (engine.minutes == 0))
			{
				audio_playSound(SFX_CLOCK, screen->w / 2, screen->h / 2);
			}

			if (engine.seconds > 0)
			{
				engine.seconds--;
				engine.counter = (SDL_GetTicks() + 1000);
			}
			else if ((engine.seconds == 0) && (engine.minutes > 0))
			{
				engine.minutes--;
				engine.seconds = 59;
				engine.counter = (SDL_GetTicks() + 1000);
				for (int i = 0 ; i < 3 ; i++)
				{
					if (mission.timeLimit1[i] > -1)
						mission.timeLimit1[i]--;
					if (mission.timeLimit2[i] > -1)
						mission.timeLimit2[i]--;
				}
				mission_checkTimer();
				events_check();
			}

			if ((engine.seconds == 0) && (engine.minutes == 0))
			{
				for (int i = 0 ; i < 3 ; i++)
				{
					if (mission.timeLimit1[i] > -1)
						mission.timeLimit1[i]--;
					if (mission.timeLimit2[i] > -1)
						mission.timeLimit2[i]--;
				}
				mission_checkTimer();
				events_check();
				engine.counter = (SDL_GetTicks() + 1000);
			}
		}
	}

	for (i = 0 ; i < MAX_INFOLINES ; i++)
	{
		if (gfx_textSprites[i].life > 0)
		{
			screen_blitText(i, -1, screen->h - 75 - (i * MENU_SPACING));
			gfx_textSprites[i].life--;

			if (gfx_textSprites[i].life == 0)
			{
				for (int j = i ; j < MAX_INFOLINES - 1 ; j++)
				{
					info_copyLine(j + 1, j);
				}
				gfx_textSprites[MAX_INFOLINES - 1].life = 0;
			}
		}
	}

	// Show the radio message if there is one
	if ((gfx_textSprites[TS_RADIO].life > 0) && (gfx_messageBox != NULL))
	{
		screen_blit(gfx_messageBox, (screen->w - gfx_messageBox->w) / 2, 50);
		gfx_textSprites[TS_RADIO].life--;
	}

	// Do the target's remaining shield (if required)
	if (game.area != MISN_DORIM)
	{
		if ((engine.targetIndex > -1) && (aliens[engine.targetIndex].shield > 0)
				&& (engine.targetIndex > engine.maxAliens))
		{
			if (game.difficulty == DIFFICULTY_ORIGINAL)
			{
				tTextIndex = TS_TARGET;
			}
			else
			{
				if (engine.targetIndex == ALIEN_SID)
					tTextIndex = TS_TARGET_SID;
				else if (engine.targetIndex == ALIEN_PHOEBE)
					tTextIndex = TS_TARGET_PHOEBE;
				else if (engine.targetIndex == ALIEN_KLINE)
					tTextIndex = TS_TARGET_KLINE;
				else
					tTextIndex = TS_TARGET;
			}

			screen_blitText(tTextIndex, screen->w * 11 / 16, screen->h - 50);

			bar.w = MAX(screen->w / 800, 1);
			bar.h = 12;
			bar.x = screen->w * 11 / 16 + gfx_textSprites[tTextIndex].image->w + 10;
			bar.y = screen->h - 50;
			nbars = 85.;
			if (engine.targetIndex == ALIEN_KLINE
					&& game.difficulty == DIFFICULTY_ORIGINAL)
			{
				if (game.area == MISN_ELAMALE)
				{
					max_shield = aliens[engine.targetIndex].maxShield;
					min_shield = max_shield - KLINE_SHIELD_MEDIUM;
				}
				else if (game.area == MISN_EARTH)
				{
					max_shield = aliens[engine.targetIndex].maxShield;
					min_shield = max_shield - KLINE_SHIELD_SMALL;
				}
				else if (game.area == MISN_VENUS)
				{
					if (aliens[ALIEN_KLINE].shield > KLINE_STAGE1_SHIELD)
					{
						max_shield = aliens[engine.targetIndex].maxShield;
						min_shield = KLINE_STAGE1_SHIELD;
					}
					else if (aliens[ALIEN_KLINE].shield > KLINE_STAGE2_SHIELD)
					{
						max_shield = KLINE_STAGE1_SHIELD;
						min_shield = KLINE_STAGE2_SHIELD;
					}
					else if (aliens[ALIEN_KLINE].shield > KLINE_STAGE3_SHIELD)
					{
						max_shield = KLINE_STAGE2_SHIELD;
						min_shield = KLINE_STAGE3_SHIELD;
					}
					else
					{
						max_shield = KLINE_STAGE3_SHIELD;
						min_shield = 0;
					}
				}
				else
				{
					max_shield = aliens[engine.targetIndex].maxShield;
					min_shield = max_shield - KLINE_SHIELD_TINY;
				}

				if (min_shield > 0)
					shield_pct = (
						MAX((float)aliens[engine.targetIndex].shield - min_shield, 1.)
							/ (max_shield-min_shield));
				else
					shield_pct = (
						((float)aliens[engine.targetIndex].shield - min_shield)
							/ (max_shield-min_shield));
			}
			else
			{
				shield_pct = ((float)aliens[engine.targetIndex].shield
					/ (float)aliens[engine.targetIndex].maxShield);
			}

			for (i = 0 ; i < nbars ; i++)
			{
				if (i / nbars > 2. / 3)
					shieldColor = green;
				else if (i / nbars > 1. / 3)
					shieldColor = yellow;
				else
					shieldColor = red;

				if (i / nbars <= shield_pct)
				{
					SDL_FillRect(screen, &bar, shieldColor);
					bar.x += bar.w + (screen->w / 800);
				}
				else
				{
					break;
				}
			}
		}
	}

	screen_blitText(TS_POWER, screen->w / 32, screen->h - 30);

	bar.w = screen->w / 32;
	bar.h = 12;
	bar.x = screen->w / 32 + gfx_textSprites[TS_POWER].image->w + 10;
	bar.y = screen->h - 29;

	bar_foutline.x = bar.x + 2;
	bar_foutline.y = bar.y + 2;
	bar_foutline.w = bar.w - 4;
	bar_foutline.h = bar.h - 4;

	bar_fcolor.x = bar_foutline.x + 2;
	bar_fcolor.y = bar_foutline.y + 2;
	bar_fcolor.w = bar_foutline.w - 4;
	bar_fcolor.h = bar_foutline.h - 4;

	for (int i = 1 ; i <= 5 ; i++)
	{
		if (weapons[W_PLAYER_WEAPON].damage >= i) {
			if(i <= game.maxPlasmaDamage || (SDL_GetTicks() % 1000 > (unsigned)i * 100))
			{
				SDL_FillRect(screen, &bar, darkGreen);
				SDL_FillRect(screen, &bar_foutline, green);
				SDL_FillRect(screen, &bar_fcolor, lightGreen);
			}
		} else if (i <= game.maxPlasmaDamage)
			SDL_FillRect(screen, &bar, darkGreen);

		bar.x += screen->w * 3 / 80;
		bar_foutline.x = bar.x + 2;
		bar_fcolor.x = bar_foutline.x + 2;
	}

	screen_blitText(TS_OUTPUT, screen->w * 5 / 16, screen->h - 30);

	bar.w = screen->w / 32;
	bar.h = 12;
	bar.x = screen->w * 5 / 16 + gfx_textSprites[TS_OUTPUT].image->w + 10;
	bar.y = screen->h - 29;

	bar_foutline.x = bar.x + 2;
	bar_foutline.y = bar.y + 2;
	bar_foutline.w = bar.w - 4;
	bar_foutline.h = bar.h - 4;

	bar_fcolor.x = bar_foutline.x + 2;
	bar_fcolor.y = bar_foutline.y + 2;
	bar_fcolor.w = bar_foutline.w - 4;
	bar_fcolor.h = bar_foutline.h - 4;

	for (int i = 1 ; i <= 5 ; i++)
	{
		if (weapons[W_PLAYER_WEAPON].ammo[0] >= i)
		{
			if (i <= game.maxPlasmaOutput || (SDL_GetTicks() % 1000 > (unsigned)i * 100))
			{
				SDL_FillRect(screen, &bar, darkYellow);
				SDL_FillRect(screen, &bar_foutline, yellow);
				SDL_FillRect(screen, &bar_fcolor, lightYellow);
			}
		}
		else if (i <= game.maxPlasmaOutput)
			SDL_FillRect(screen, &bar, darkYellow);

		bar.x += screen->w * 3 / 80;
		bar_foutline.x = bar.x + 2;
		bar_fcolor.x = bar_foutline.x + 2;
	}

	screen_blitText(TS_COOLER, screen->w * 97 / 160, screen->h - 30);

	bar.w = screen->w / 32;
	bar.h = 12;
	bar.x = screen->w * 97 / 160 + gfx_textSprites[TS_COOLER].image->w + 10;
	bar.y = screen->h - 29;

	bar_foutline.x = bar.x + 2;
	bar_foutline.y = bar.y + 2;
	bar_foutline.w = bar.w - 4;
	bar_foutline.h = bar.h - 4;

	bar_fcolor.x = bar_foutline.x + 2;
	bar_fcolor.y = bar_foutline.y + 2;
	bar_fcolor.w = bar_foutline.w - 4;
	bar_fcolor.h = bar_foutline.h - 4;

	for (int i = 1 ; i <= 5 ; i++)
	{
		if (weapons[W_PLAYER_WEAPON].reload[0] <= rate2reload[i])
		{
			if (i <= game.maxPlasmaRate || (SDL_GetTicks() % 1000 > (unsigned)i * 100))
			{
				SDL_FillRect(screen, &bar, darkBlue);
				SDL_FillRect(screen, &bar_foutline, lightBlue);
				SDL_FillRect(screen, &bar_fcolor, lighterBlue);
			}
		}
		else if (i <= game.maxPlasmaRate)
			SDL_FillRect(screen, &bar, darkBlue);

		bar.x += screen->w * 3 / 80;
		bar_foutline.x = bar.x + 2;
		bar_fcolor.x = bar_foutline.x + 2;
	}

	screen_blitText(TS_SHIELD, screen->w / 32, screen->h - 50);
	if (player.shield < 1)
		return;

	if ((player.weaponType[1] == W_LASER) && (engine.eventTimer % 8 == 1))
		LIMIT_ADD(player.ammo[1], -1, 0, 200);

	if ((engine.eventTimer < 30) && (player.shield <= engine.lowShield))
		return;

	int blockSize = MAX(screen->w / 800, 1);

	bar.w = blockSize;
	bar.h = 12;
	bar.x = screen->w / 32 + gfx_textSprites[TS_SHIELD].image->w + 10;
	bar.y = screen->h - 50;

	for (int i = 0 ; i < player.shield ; i += blockSize)
	{
		if (i >= engine.averageShield)
			shieldColor = green;
		else if ((i >= engine.lowShield) && (i < engine.averageShield))
			shieldColor = yellow;
		else
			shieldColor = red;
		SDL_FillRect(screen, &bar, shieldColor);
		bar.x += blockSize;
		if (player.maxShield <= 75 || screen->w >= 1200)
			bar.x += screen->w / 800;
	}
}

/*
 * Delay until the next 60 Hz frame
 */
void game_delayFrame()
{
	Uint32 now = SDL_GetTicks();

	// Add 16 2/3 (= 1000 / 60) to frameLimit
	frameLimit += 16;
	thirds += 2;
	while (thirds >= 3)
	{
		thirds -= 3;
		frameLimit++;
	}

	if(now < frameLimit)
		SDL_Delay(frameLimit - now);
	else
		frameLimit = now;
}

/*
Checked during the main game loop. When the game is paused
it goes into a constant loop checking this routine. If escape is
pressed, the game automatically ends and goes back to the title screen
*/
static int game_checkPauseRequest()
{
	player_getInput();

	if (engine.keyState[KEY_ESCAPE])
	{
		engine.paused = 0;
		player.shield = 0;
		return 1;
	}

	if (engine.keyState[KEY_PAUSE])
	{
		engine.paused = 0;
		engine.keyState[KEY_PAUSE] = 0;
	}

	return 0;
}

int game_collision(float x0, float y0, int w0, int h0, float x2, float y2, int w1, int h1)
{
	float x1 = x0 + w0;
	float y1 = y0 + h0;

	float x3 = x2 + w1;
	float y3 = y2 + h1;

	return !(x1<x2 || x3<x0 || y1<y2 || y3<y0);
}

/*
The game over screen :(
*/
static void game_showGameOver()
{
	screen_flushBuffer();
	gfx_free();
	SDL_FillRect(gfx_background, NULL, black);

	engine.keyState[KEY_FIRE] = engine.keyState[KEY_ALTFIRE] = 0;
	engine.gameSection = SECTION_INTERMISSION;

	SDL_Surface *gameover = gfx_loadImage("gfx/gameover.png");

	screen_clear(black);
	renderer_update();
	screen_clear(black);
	SDL_Delay(1000);

#ifdef OLD_MUSIC
	audio_playMusic("music/Wybierak.mod", -1);
#else
	audio_playMusic("music/sleeping_with_androids.ogg", 0);
#endif

	int x = (screen->w - gameover->w) / 2;
	int y = (screen->h - gameover->h) / 2;

	renderer_update();

	player_flushInput();
	engine.keyState[KEY_FIRE] = engine.keyState[KEY_ALTFIRE] = 0;

	while (1)
	{
		player_getInput();

		if (engine.keyState[KEY_FIRE] || engine.keyState[KEY_ALTFIRE])
			break;

		renderer_update();

		screen_unBuffer();
		x = ((screen->w - gameover->w) / 2) - RANDRANGE(-2, 2);
		y = ((screen->h - gameover->h) / 2)  - RANDRANGE(-2, 2);
		screen_blit(gameover, x,  y);

		game_delayFrame();
	}

	SDL_FreeSurface(gameover);
	audio_haltMusic();
	screen_flushBuffer();
}

void game_getDifficultyText(char *dest, int difficulty)
{
	switch (difficulty)
	{
		case DIFFICULTY_SUPEREASY:
			/// DIFFICULTY_SUPEREASY
			strcpy(dest, _("Super-Easy"));
			break;
		case DIFFICULTY_EASY:
			/// DIFFICULTY_EASY
			strcpy(dest, _("Easy"));
			break;
		case DIFFICULTY_NORMAL:
			/// DIFFICULTY_NORMAL
			strcpy(dest, _("Normal"));
			break;
		case DIFFICULTY_HARD:
			/// DIFFICULTY_HARD
			strcpy(dest, _("Hard"));
			break;
		case DIFFICULTY_NIGHTMARE:
			/// DIFFICULTY_NIGHTMARE
			strcpy(dest, _("Nightmare!"));
			break;
		case DIFFICULTY_ORIGINAL:
			/// DIFFICULTY_ORIGINAL (Classic)
			strcpy(dest, _("Classic"));
			break;
		default:
			strcpy(dest, "???");
	}
}

int game_mainLoop()
{
	int spawnedMisnTarget;
	float chance;

	engine_resetLists();

	mission_set(game.area);
	mission_showStartScreen();

	cargo_init();
	player_init();
	aliens_init();

	// specific for Phoebe being captured!
	if (game.area == MISN_NEROD)
		game.hasWingMate1 = 1;

	if (game.area == MISN_ELAMALE)
		aliens[ALIEN_KLINE].active = 0;

	spawnedMisnTarget = 0;
	for (int i = 0 ; i < engine.maxAliens ; i++)
		alien_add(&spawnedMisnTarget);

	if (!spawnedMisnTarget)
		game.forceMisnTarget = 1;

	if (game.hasWingMate1)
		alien_addFriendly(ALIEN_PHOEBE);

	if (game.hasWingMate2)
		alien_addFriendly(ALIEN_URSULA);

	if ((game.area == MISN_URUSOR)
			|| (game.area == MISN_POSWIC)
			|| (game.area == MISN_EARTH))
		alien_addFriendly(ALIEN_SID);

	// Disable Wingmates for certain missions
	switch (game.area)
	{
		case MISN_NEROD:
		case MISN_URUSOR:
		case MISN_DORIM:
		case MISN_SIVEDI:
		case MISN_ALMARTHA:
		case MISN_ELLESH:
		case MISN_MARS:
		case MISN_VENUS:
			aliens[ALIEN_PHOEBE].active = 0;
			aliens[ALIEN_URSULA].active = 0;
			break;
	}

	if (game.area == MISN_DORIM)
	{
		aliens[0].collectChance = 100;
		aliens[0].collectType = P_ESCAPEPOD;
		aliens[0].collectTypeOriginal = P_ESCAPEPOD;
	}

	// Some specifics for interception missions
	if (game.area == MISN_INTERCEPTION)
	{
		if ((game.system > SYSTEM_EYANANTH) && CHANCE(1. / 5.))
		{
			aliens[ALIEN_KLINE] = alien_defs[CD_KLINE];
			aliens[ALIEN_KLINE].owner = &aliens[ALIEN_KLINE];
			aliens[ALIEN_KLINE].target = &player;
			aliens[ALIEN_KLINE].active = 1;
			aliens[ALIEN_KLINE].x = player.x + 1000;
			aliens[ALIEN_KLINE].y = player.y;
			player_setTarget(ALIEN_KLINE);
		}

		if ((game.system == SYSTEM_MORDOR) && (game.experimentalShield > 0))
		{
			if (CHANCE(4. / 5.)
				|| (game.difficulty != DIFFICULTY_ORIGINAL
					&& game.forceMisnTarget))
			{
				aliens[ALIEN_BOSS] = alien_defs[CD_CLOAKFIGHTER];
				aliens[ALIEN_BOSS].owner = &aliens[ALIEN_BOSS];
				aliens[ALIEN_BOSS].target = &aliens[ALIEN_BOSS];
				aliens[ALIEN_BOSS].shield = 1000;
				aliens[ALIEN_BOSS].active = 1;
				aliens[ALIEN_BOSS].x = player.x - 1000;
				aliens[ALIEN_BOSS].y = player.y;
				player_setTarget(ALIEN_BOSS);
				aliens[ALIEN_BOSS].shield = game.experimentalShield;
				game.forceMisnTarget = 0;
			}
			else
				game.forceMisnTarget = 1;
		}

		// Note: music is started here only for interceptions.  For
		// regular missions, it is instead started by
		// mission_showStartScreen().  This is necessary to ensure the
		// proper music can be played when Kline or the cloak fighter
		// are active.
		audio_playRandomTrack();
	}

	if (game.area == MISN_VENUS)
	{
		if (game.difficulty == DIFFICULTY_ORIGINAL)
		{
			aliens[ALIEN_KLINE].flags |= FL_IMMORTAL | FL_NOFIRE | FL_NOMOVE;
			aliens[ALIEN_KLINE].x = screen->w * 3 / 4;
			aliens[ALIEN_KLINE].y = screen->h / 2;
		}
		else
		{
			aliens[ALIEN_KLINE].x = player.x + 1000;
			aliens[ALIEN_KLINE].y = player.y;
		}
	}

	for (int i = 0 ; i < ALIEN_MAX ; i++)
	{
		aliens[i].systemPower = aliens[i].maxShield;
		aliens[i].deathCounter = 0 - (aliens[i].maxShield * 3);
		LIMIT(aliens[i].deathCounter, -350, 0);
		alien_nerf(i);
	}

	// Set target energy meter
	switch (game.area)
	{
		case MISN_MOEBO:
		case MISN_ELAMALE:
		case MISN_ODEON:
		case MISN_FELLON:
		case MISN_ELLESH:
		case MISN_PLUTO:
		case MISN_NEPTUNE:
		case MISN_URANUS:
		case MISN_JUPITER:
			player_setTarget(ALIEN_BOSS);
			break;
		case MISN_NEROD:
			player_setTarget(ALIEN_PHOEBE);
			break;
		case MISN_ALLEZ:
			player_setTarget(ALIEN_FRIEND1);
			break;
		case MISN_URUSOR:
			player_setTarget(ALIEN_SID);
			break;
		case MISN_POSWIC:
			if (game.difficulty == DIFFICULTY_ORIGINAL)
				player_setTarget(ALIEN_BOSS);
			else
				player_setTarget(ALIEN_SID);
			break;
		case MISN_EARTH:
		case MISN_VENUS:
			player_setTarget(ALIEN_KLINE);
			break;
	}

	info_clearLines();

	events_init();

	engine.ssx = 0;
	engine.ssy = 0;
	engine.smx = 0;
	engine.smy = 0;

	engine.done = ENGINE_RUNNING;

	engine.counter = (SDL_GetTicks() + 1000);
	engine.counter2 = (SDL_GetTicks() + 1000);

	engine.missionCompleteTimer = 0;
	engine.musicVolume = MIX_MAX_VOLUME;

	int rtn = 0;

	int allowableAliens = 999999999;

	for (int i = 0 ; i < 3 ; i++)
	{
		if ((mission.primaryType[i] == M_DESTROY_TARGET_TYPE)
				&& (mission.target1[i] == CD_ANY))
			allowableAliens = mission.targetValue1[i];

		if (mission.primaryType[i] == M_DESTROY_ALL_TARGETS)
			allowableAliens = 999999999;
	}

	for (int i = 0 ; i < ALIEN_MAX ; i++)
	{
		if ((aliens[i].active) && (aliens[i].flags & FL_WEAPCO))
		{
			allowableAliens--;
		}
	}

	screen_drawBackground();
	screen_flushBuffer();

	// Default to no aliens dead...
	engine.allAliensDead = 0;

	engine.keyState[KEY_FIRE] = 0;
	engine.keyState[KEY_ALTFIRE] = 0;
	player_flushInput();

	while (engine.done != ENGINE_CLOSING)
	{
		renderer_update();

		if ((mission_checkCompleted()) && (engine.missionCompleteTimer == 0))
		{
			engine.missionCompleteTimer = SDL_GetTicks() + 4000;
		}

		if ((mission_checkFailed()) && (engine.missionCompleteTimer == 0))
		{
			if (game.area != MISN_MOEBO)
				engine.missionCompleteTimer = SDL_GetTicks() + 4000;
		}

		if (engine.missionCompleteTimer != 0)
		{
			engine.gameSection = SECTION_INTERMISSION;
			if (player.shield > 0)
			{
				if ((SDL_GetTicks() >= engine.missionCompleteTimer)
						&& ((game.difficulty == DIFFICULTY_ORIGINAL)
							|| (game.difficulty == DIFFICULTY_NIGHTMARE)
							|| (game.area == MISN_INTERCEPTION)
							|| (game.area == MISN_ELLESH)
							|| (game.area == MISN_MARS)
							|| (mission_checkFailed())
							|| (collectable_numGood() <= 0)
							|| (engine.done == ENGINE_SYSEXIT)))
				{
					if ((!mission_checkFailed()) && (game.area != MISN_VENUS))
					{
						player_leaveSector();
						if ((engine.done == ENGINE_SYSEXIT)
								&& (game.area != MISN_DORIM)
								&& (game.area != MISN_SIVEDI))
						{
							if ((aliens[ALIEN_PHOEBE].shield > 0)
									&& (game.area != MISN_EARTH))
							{
								aliens[ALIEN_PHOEBE].x = player.x - 40;
								aliens[ALIEN_PHOEBE].y = player.y - 35;
								aliens[ALIEN_PHOEBE].face = 0;
							}

							if ((aliens[ALIEN_URSULA].shield > 0)
									&& (game.area != MISN_EARTH))
							{
								aliens[ALIEN_URSULA].x = player.x - 40;
								aliens[ALIEN_URSULA].y = player.y + 45;
								aliens[ALIEN_URSULA].face = 0;
							}

							if ((game.area == MISN_URUSOR)
									|| (game.area == MISN_POSWIC))
							{
								aliens[ALIEN_SID].x = player.x - 100;
								aliens[ALIEN_SID].y = player.y;
								aliens[ALIEN_SID].face = 0;
							}
						}
					}
					else if ((game.area == MISN_VENUS)
							&& (engine.musicVolume > 0))
					{
						player_getInput();
						engine.keyState[KEY_UP] = 0;
						engine.keyState[KEY_DOWN] = 0;
						engine.keyState[KEY_LEFT] = 0;
						engine.keyState[KEY_RIGHT] = 0;
						engine.keyState[KEY_FIRE] = 0;
						engine.keyState[KEY_ALTFIRE] = 0;
						engine.xaxis = 0;
						engine.yaxis = 0;
						LIMIT_ADD(engine.musicVolume, -0.2, 0, MIX_MAX_VOLUME);
						audio_setMusicVolume(engine.musicVolume);
					}
					else
					{
						engine.done = ENGINE_CLOSING;
					}
				}
				else
				{
					player_getInput();
				}
			}
			else
			{
				player_getInput();
				LIMIT_ADD(engine.musicVolume, -0.2, 0, MIX_MAX_VOLUME);
				audio_setMusicVolume(engine.musicVolume);
				if (SDL_GetTicks() >= engine.missionCompleteTimer)
				{
					engine.done = ENGINE_CLOSING;
				}
			}
		}
		else
		{
			player_getInput();
		}

		screen_unBuffer();
		game_doStars();
		game_doCollectables();
		game_doBullets();
		game_doAliens();
		game_doPlayer();
		game_doCargo();
		game_doDebris();
		game_doExplosions();
		game_doHud();

		// Start delaying damage again gradually
		if (player_damageDelay > 0)
		{
			if (player_resetDamageDelay)
			{
				player_damageDelay--;
			}
			else
			{
				player_resetDamageDelay = 1;
			}
		}

		WRAP_ADD(engine.eventTimer, -1, 0, 60);

		if (engine.paused)
		{
			gfx_createTextObject(TS_PAUSED, "PAUSED", 0, 0, FONT_WHITE);
			audio_pauseMusic();

			while (engine.paused)
			{
				screen_blitText(TS_PAUSED, -1, screen->h / 2);
				renderer_update();
				engine.done = game_checkPauseRequest();
				game_delayFrame();
			}

			if (engine.done == ENGINE_RUNNING)
				audio_resumeMusic();
		}

		if ((game.area == MISN_MARS) && (engine.addAliens > -1))
		{
			if (game.difficulty == DIFFICULTY_SUPEREASY)
				chance = 1. / 20.;
			else
				chance = 1. / 5.;

			if (CHANCE(chance))
				// Note: The originally specified range for x was [800, 100],
				// which with the original rrand function caused the result
				// returned to be `800 + rand() % -699`. Clearly a mistake,
				// but I'm not entirely sure what the original intention was.
				// I've set the range to [800, 1500] (modified for screen
				// width), which approximately replicates the original's results.
				collectable_add(screen->w + RANDRANGE(0, 700),
					RANDRANGE(-screen->h / 3, (4 * screen->h) / 3), P_MINE, 25,
					180 * screen->w / 800 + RANDRANGE(0, 60));
		}

		if (engine.addAliens > -1)
		{
			WRAP_ADD(engine.addAliens, -1, 0, mission.addAliens);
			if ((engine.addAliens == 0) && (allowableAliens > 0))
			{
				allowableAliens -= alien_add(NULL);
			}
		}

		if ((player.shield <= 0) && (engine.missionCompleteTimer == 0))
			engine.missionCompleteTimer = SDL_GetTicks() + 7000;

		// specific to Boss 1
		if ((game.area == MISN_MOEBO)
				&& (aliens[ALIEN_BOSS].flags & FL_ESCAPED))
		{
			audio_playSound(SFX_DEATH, aliens[ALIEN_BOSS].x, aliens[ALIEN_BOSS].y);
			screen_clear(white);
			renderer_update();
			for (int i = 0 ; i < 300 ; i++)
			{
				SDL_Delay(10);
				if ((rand() % 25) == 0)
					audio_playSound(SFX_EXPLOSION, aliens[ALIEN_BOSS].x, aliens[ALIEN_BOSS].y);
			}
			SDL_Delay(1000);
			break;
		}

		game_delayFrame();
	}

	screen_flushBuffer();

	if ((player.shield > 0) && (!mission_checkFailed()))
	{
		if (game.area < MISN_VENUS)
			mission_showFinishedScreen();

		switch (game.area)
		{
			case MISN_MOEBO:
				cutscene_init(1);
				cutscene_init(2);
				break;
			case MISN_NEROD:
				cutscene_init(3);
				break;
			case MISN_ELAMALE:
				cutscene_init(4);
				break;
			case MISN_ODEON:
				cutscene_init(5);
				break;
			case MISN_ELLESH:
				cutscene_init(6);
				break;
			case MISN_VENUS:
				title_showCredits();
				break;
		}

		if (game.area < MISN_VENUS)
		{
			intermission_updateSystemStatus();
			save(0);
		}

		rtn = 1;

		if (game.area == MISN_VENUS)
			rtn = 0;
	}
	else
	{
		game_showGameOver();
		rtn = 0;
	}

	player_exit();

	return rtn;
}
