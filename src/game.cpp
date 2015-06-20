/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012, 2013 Guus Sliepen
Copyright (C) 2012, 2014, 2015 Julian Marchant

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

Game game;

static Star stars[STARS_NUM];

void game_init()
{
	game.system = 0;
	game.area = MISN_START;
	game.sfxVolume = 0;
	game.musicVolume = 0;

	if (!engine.useAudio)
	{
		engine.useSound = false;
		engine.useMusic = false;
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

	for (int i = 0 ; i < STARS_NUM ; i++)
	{
		stars[i].x = rand() % screen->w;
		stars[i].y = rand() % screen->h;
		stars[i].speed = 1 + (rand() % 3);
	}

	initWeapons();
	initMissions();
	initPlanetMissions(game.system);
}

static void game_addDebris(int x, int y, int amount)
{
	if ((rand() % 2) == 0)
		audio_playSound(SFX_DEBRIS, x);
	else
		audio_playSound(SFX_DEBRIS2, x);

	object *debris;
	
	amount = RANDRANGE(3, amount);
	LIMIT(amount, 3, 8);

	for (int i = 0 ; i < amount ; i++)
	{
		debris = new object;

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
Simply draws the stars in their positions on screen and moves
them around.
*/
void game_doStars()
{
	/* Lock the screen for direct access to the pixels */
	if (SDL_MUSTLOCK(screen))
	{
		if (SDL_LockSurface(screen) < 0 )
			showErrorAndExit(2, "");
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

		putpixel(screen, (int)stars[i].x, (int)stars[i].y, color);
		r.x = (int)stars[i].x;
		r.y = (int)stars[i].y;
		r.w = 1;
		r.h = 1;

		addBuffer(r.x, r.y, r.w, r.h);
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
	collectables *collectable = engine.collectableHead;
	collectables *prevCollectable = engine.collectableHead;
	engine.collectableTail = engine.collectableHead;
	char temp[40];

	while (collectable->next != NULL)
	{
		collectable = collectable->next;

		if (collectable->active)
		{
			if ((collectable->x + collectable->image->w > 0) &&
					(collectable->x < screen->w) &&
					(collectable->y + collectable->image->h > 0) &&
					(collectable->y < screen->h))
				blit(collectable->image, (int)collectable->x, (int)collectable->y);

			collectable->x += engine.ssx + engine.smx;
			collectable->y += engine.ssy + engine.smy;
			collectable->x += collectable->dx;
			collectable->y += collectable->dy;

			collectable->life--;

			if ((player.shield > 0) && (collision(collectable, &player)))
			{
				switch(collectable->type)
				{
					case P_CASH:
						if (game.difficulty == DIFFICULTY_ORIGINAL)
							collectable->value *= 2;

						game.cash += collectable->value;
						game.cashEarned += collectable->value;
						sprintf(temp, "Got $%d ", collectable->value);
						break;

					case P_ROCKET:
						LIMIT_ADD(player.ammo[1], collectable->value, 0,
							game.maxRocketAmmo);
						if (player.ammo[1] == game.maxRocketAmmo)
							sprintf(temp, "Rocket Ammo at Maximum");
						else
						{
							if (collectable->value > 1)
								sprintf(temp, "Got %d rockets", collectable->value);
							else
								sprintf(temp, "Got a rocket");
						}
						game.rocketPickups += collectable->value;
						break;

					case P_SHIELD:
						LIMIT_ADD(player.shield, 10, 0, player.maxShield);
						game.shieldPickups ++;
						sprintf(temp, "Restored 10 shield points");
						break;

					case P_PLASMA_RATE:
						game.powerups++;
						if ((game.area != MISN_INTERCEPTION) ||
							(game.difficulty == DIFFICULTY_ORIGINAL) ||
							(player.ammo[0] > 0))
						{
							if ((game.area != MISN_INTERCEPTION) ||
									(game.difficulty == DIFFICULTY_ORIGINAL))
								LIMIT_ADD(player.ammo[0], collectable->value,
									0, game.maxPlasmaAmmo);

							if (weapon[W_PLAYER_WEAPON].reload[0] <= rate2reload[game.maxPlasmaRate])
								sprintf(temp, "Firing rate already at maximum");
							else
							{
								weapon[W_PLAYER_WEAPON].reload[0] -= 2;
								sprintf(temp, "Firing rate increased");
							}
						}
						else
						{
							sprintf(temp, "Upgrade failed (no plasma ammo)");
						}
						break;

					case P_PLASMA_SHOT:
						game.powerups++;
						if ((game.area != MISN_INTERCEPTION) ||
							(game.difficulty == DIFFICULTY_ORIGINAL) ||
							(player.ammo[0] > 0))
						{
							if ((game.area != MISN_INTERCEPTION) ||
									(game.difficulty == DIFFICULTY_ORIGINAL))
								LIMIT_ADD(player.ammo[0], collectable->value,
									0, game.maxPlasmaAmmo);

							if (weapon[W_PLAYER_WEAPON].ammo[0] >= game.maxPlasmaOutput)
								sprintf(temp, "Plasma output already at maximum");
							else
							{
								weapon[W_PLAYER_WEAPON].ammo[0]++;
								sprintf(temp, "Plasma output increased");
							}
						}
						else
						{
							sprintf(temp, "Upgrade failed (no plasma ammo)");
						}
						break;

					case P_PLASMA_DAMAGE:
						game.powerups++;
						if ((game.area != MISN_INTERCEPTION) ||
							(game.difficulty == DIFFICULTY_ORIGINAL) ||
							(player.ammo[0] > 0))
						{
							if ((game.area != MISN_INTERCEPTION) ||
									(game.difficulty == DIFFICULTY_ORIGINAL))
								LIMIT_ADD(player.ammo[0], collectable->value,
									0, game.maxPlasmaAmmo);

							if (weapon[W_PLAYER_WEAPON].damage >= game.maxPlasmaDamage)
								sprintf(temp, "Plasma damage already at maximum");
							else {
								weapon[W_PLAYER_WEAPON].damage++;
								sprintf(temp, "Plasma damage increased");
							}
						}
						else
						{
							sprintf(temp, "Upgrade failed (no plasma ammo)");
						}
						break;

					case P_SUPER:
						game.powerups++;
						if ((game.area != MISN_INTERCEPTION) ||
							(game.difficulty == DIFFICULTY_ORIGINAL) ||
							(player.ammo[0] > 0))
						{
							if ((game.area != MISN_INTERCEPTION) ||
									(game.difficulty == DIFFICULTY_ORIGINAL))
								LIMIT_ADD(player.ammo[0], collectable->value,
									0, game.maxPlasmaAmmo);

							weapon[W_PLAYER_WEAPON].ammo[0] = 5;
							weapon[W_PLAYER_WEAPON].damage = 5;
							weapon[W_PLAYER_WEAPON].reload[0] = rate2reload[5];
							weapon[W_PLAYER_WEAPON].flags |= WF_SPREAD;

							sprintf(temp, "Picked up a Super Charge!!");
						}
						else
						{
							sprintf(temp, "Damn! Upgrade failed (no plasma ammo)");
						}
						break;

					case P_PLASMA_AMMO:
						if (player.ammo[0] >= game.maxPlasmaAmmo)
							sprintf(temp, "Plasma cells already at Maximum");
						else
						{
							LIMIT_ADD(player.ammo[0], collectable->value,
								0, game.maxPlasmaAmmo);
							if (collectable->value > 1)
							{
								sprintf(temp, "Got %d plasma cells", collectable->value);
							}
							else
							{
								sprintf(temp, "Got a plasma cell");
								if ((rand() % 25) == 0)
									sprintf(temp, "Got one whole plasma cell (wahoo!)");
							}
						}
						game.cellPickups += collectable->value;
						break;

					case P_CARGO:
						strcpy(temp, "Picked up some Cargo");
						game.cargoPickups++;
						break;

					case P_SLAVES:
						sprintf(temp, "Rescued %d slaves", collectable->value);
						game.slavesRescued += collectable->value;
						break;

					case P_ESCAPEPOD:
						sprintf(temp, "Picked up an Escape Pod");
						break;

					case P_ORE:
						sprintf(temp, "Picked up some Ore");
						break;
				}

				updateMissionRequirements(M_COLLECT, collectable->type,
					collectable->value);

				collectable->active = false;
				if (collectable->type != P_MINE)
				{
					setInfoLine(temp, FONT_WHITE);
					if (collectable->type == P_SHIELD)
						audio_playSound(SFX_SHIELDUP, player.x);
					else
						audio_playSound(SFX_PICKUP, player.x);
				}
			}

			// stop people from exploiting a weapon check condition
			if (player.ammo[0] == 0)
			{
				weapon[W_PLAYER_WEAPON].ammo[0] = game.minPlasmaOutput;
				weapon[W_PLAYER_WEAPON].damage = game.minPlasmaDamage;
				weapon[W_PLAYER_WEAPON].reload[0] = rate2reload[game.minPlasmaRate];
			}
		}

		if (collectable->life < 1)
		{
			collectable->active = false;
			if ((collectable->type == P_CARGO) ||
					(collectable->type == P_ESCAPEPOD) ||
					(collectable->type == P_SLAVES))
				updateMissionRequirements(M_PROTECT_PICKUP, collectable->type, 1);
		}

		if (collectable->active)
		{
			prevCollectable = collectable;
			engine.collectableTail = collectable;
		}
		else
		{
			if (collectable->type == P_MINE)
				collectable_explode(collectable);
			prevCollectable->next = collectable->next;
			delete collectable;
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
	object *bullet = engine.bulletHead;
	object *prevBullet = engine.bulletHead;

	collectables *collectable;
	collectables *prevCollectable;

	bool okayToHit = false;
	int old_shield;
	float homingMissileSpeed = 0;
	int charger_num;

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
				explosion_add(bullet->x, bullet->y, E_SMALL_EXPLOSION);
			}
			else if (bullet->id == WT_MICROROCKET)
			{
				explosion_add(bullet->x, bullet->y, E_TINY_EXPLOSION);
			}

			if ((bullet->flags & WF_AIMED))
			{
				blit(bullet->image[0], (int)(bullet->x - bullet->dx),
					(int)(bullet->y - bullet->dy));
			}

			if (bullet->id == WT_CHARGER)
			{
				if (game.difficulty == DIFFICULTY_ORIGINAL)
					charger_num = bullet->damage;
				else
					charger_num = bullet->damage * 2;

				for (int i = 0 ; i < charger_num ; i++)
					blit(bullet->image[0],
						(int)(bullet->x - RANDRANGE(
							-(charger_num / 6), charger_num / 6)),
						(int)(bullet->y + RANDRANGE(-3, 3)));
			}

			blit(bullet->image[0], (int)bullet->x, (int)bullet->y);
			bullet->x += bullet->dx;
			bullet->y += bullet->dy;

			if (bullet->target != NULL)
			{
				if (bullet->x < bullet->target->x)
					LIMIT_ADD(bullet->dx, homingMissileSpeed, -15, 15);
				else if (bullet->x > bullet->target->x)
					LIMIT_ADD(bullet->dx, -homingMissileSpeed, -15, 15);

				//Rocket is (more or less) inline with target. Fly straight
				if ((bullet->x > bullet->target->x - 1) && (bullet->x < bullet->target->x + 5))
					bullet->dx = 0;

				if (bullet->y < bullet->target->y)
					LIMIT_ADD(bullet->dy, homingMissileSpeed, -15, 15);
				else if (bullet->y > bullet->target->y)
					LIMIT_ADD(bullet->dy, -homingMissileSpeed, -15, 15);

				//Rocket is (more or less) inline with target. Fly straight
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

				okayToHit = false;

				if ((bullet->flags & WF_FRIEND) && (aliens[i].flags & FL_WEAPCO))
					okayToHit = true;
				if ((bullet->flags & WF_WEAPCO) && (aliens[i].flags & FL_FRIEND))
					okayToHit = true;
				if ((bullet->id == WT_ROCKET) || (bullet->id == WT_LASER) ||
						(bullet->id == WT_CHARGER))
					okayToHit = true;

				if (bullet->owner == aliens[i].owner)
					okayToHit = false;

				if (okayToHit)
				{
					if ((bullet->active) && (collision(bullet, &aliens[i])))
					{
						old_shield = aliens[i].shield;

						if (bullet->owner == &player)
						{
							game.hits++;
							if ((aliens[i].classDef == CD_PHOEBE) ||
									(aliens[i].classDef == CD_URSULA))
								getMissFireMessage(&aliens[i]);
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
							bullet->damage -= old_shield;
							if (bullet->damage <= 0)
							{
								bullet->active = false;
								bullet->shield = 0;
								audio_playSound(SFX_EXPLOSION, bullet->x);
								for (int i = 0 ; i < 10 ; i++)
									explosion_add(bullet->x + RANDRANGE(-35, 35),
										bullet->y + RANDRANGE(-35, 35),
										E_BIG_EXPLOSION);
							}
						}
						else
						{
							bullet->active = false;
							bullet->shield = 0;
						}

						if (bullet->id == WT_ROCKET)
							explosion_add(bullet->x, bullet->y, E_BIG_EXPLOSION);
						else
							explosion_add(bullet->x, bullet->y, E_SMALL_EXPLOSION);
					}
				}
			}

			// Check for bullets hitting player
			if ((bullet->flags & WF_WEAPCO) || (bullet->id == WT_ROCKET) ||
				(bullet->id == WT_LASER) || (bullet->id == WT_CHARGER))
			{
				if (bullet->active && (player.shield > 0) &&
					(bullet->owner != &player) && collision(bullet, &player))
				{
					old_shield = player.shield;

					if ((!engine.cheatShield) || (engine.missionCompleteTimer != 0))
					{
						if ((player.shield > engine.lowShield) &&
								(player.shield - bullet->damage <= engine.lowShield))
							setInfoLine("!!! WARNING: SHIELD LOW !!!", FONT_RED);

						player.shield -= bullet->damage;
						LIMIT(player.shield, 0, player.maxShield);
						player.hit = 5;
					}

					if ((bullet->owner->classDef == CD_PHOEBE) ||
							(bullet->owner->classDef == CD_URSULA))
						getPlayerHitMessage(bullet->owner);

					if (bullet->id == WT_CHARGER)
					{
						bullet->damage -= old_shield;
						if (bullet->damage <= 0)
						{
							bullet->active = false;
							bullet->shield = 0;
							audio_playSound(SFX_EXPLOSION, bullet->x);
							for (int i = 0 ; i < 10 ; i++)
								explosion_add(bullet->x + RANDRANGE(-35, 35),
									bullet->y + RANDRANGE(-35, 35), E_BIG_EXPLOSION);
						}
					}
					else
					{
						bullet->active = false;
						bullet->shield = 0;
					}

					audio_playSound(SFX_HIT, player.x);

					if (bullet->id == WT_ROCKET)
						explosion_add(bullet->x, bullet->y, E_BIG_EXPLOSION);
					else
						explosion_add(bullet->x, bullet->y, E_SMALL_EXPLOSION);
				}
			}
		}

		if ((game.difficulty != DIFFICULTY_EASY) &&
			((bullet->owner == &player) || (bullet->id == WT_ROCKET)))
		{
			for (int j = 0 ; j < 20 ; j++)
			{
				if (cargo[j].active)
				{
					if (collision(bullet, &cargo[j]))
					{
						bullet->active = false;
						explosion_add(bullet->x, bullet->y, E_SMALL_EXPLOSION);
						audio_playSound(SFX_HIT, cargo[j].x);
						if (cargo[j].collectType != P_PHOEBE)
						{
							cargo[j].active = false;
							audio_playSound(SFX_EXPLOSION, cargo[j].x);
							for (int i = 0 ; i < 10 ; i++)
								explosion_add(cargo[j].x + RANDRANGE(-15, 15),
									cargo[j].y + RANDRANGE(-15, 15),
									E_BIG_EXPLOSION);
							updateMissionRequirements(M_PROTECT_PICKUP,
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
				if (collision(collectable, bullet))
				{
					collectable->active = false;
					
					if (bullet->id != WT_CHARGER)
					{
						bullet->active = false;
					}
					else
					{
						bullet->shield--;
						if (bullet->shield < 0)
							bullet->active = false;
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
				delete collectable;
				collectable = prevCollectable;
			}
		}

		bullet->shield--;

		if (bullet->shield < 1)
		{
			if (bullet->flags & WF_TIMEDEXPLOSION)
			{
				audio_playSound(SFX_EXPLOSION, bullet->x);
				for (int i = 0 ; i < 10 ; i++)
					explosion_add(bullet->x + RANDRANGE(-35, 35),
						bullet->y + RANDRANGE(-35, 35), E_BIG_EXPLOSION);

				player_checkShockDamage(bullet->x, bullet->y);
			}
			bullet->active = false;
		}

		if (bullet->active)
		{
			prevBullet = bullet;
			engine.bulletTail = bullet;
		}
		else
		{
			prevBullet->next = bullet->next;
			delete bullet;
			bullet = prevBullet;
		}
	}
}

static void game_doAliens()
{
	static float barrierLoop = 0;

	int shapeToUse;
	bool canFire;
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
						aliens[i].x = aliens[i].owner->x + aliens[i].owner->image[0]->w + aliens[i].dx - aliens[i].image[0]->w;

					aliens[i].y = (aliens[i].owner->y + aliens[i].dy);

					if (aliens[i].owner->shield < 1)
					{
						if ((aliens[i].classDef != CD_URANUSBOSSWING1) &&
							(aliens[i].classDef != CD_URANUSBOSSWING2))
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

				canFire = true; // The alien is allowed to fire

				LIMIT_ADD(aliens[i].thinktime, -1, 0, 250);

				if (aliens[i].target->shield < 1)
					aliens[i].target = &aliens[i];

				// Specific to Sid to stop him pissing about(!)
				if ((aliens[i].classDef == CD_SID) &&
						(aliens[i].target->flags & FL_DISABLED))
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

				if ((!(aliens[i].flags & FL_DISABLED)) &&
					(aliens[i].thinktime == 0) && (aliens[i].target != &aliens[i]) &&
					(aliens[i].owner == &aliens[i]))
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

				if ((game.area == MISN_ELLESH) &&
						((aliens[i].classDef == CD_BOSS) ||
						    game.difficulty != DIFFICULTY_ORIGINAL))
					aliens[i].face = 0;

				if ((aliens[i].flags & FL_DEPLOYDRONES) && ((rand() % 300) == 0))
					alien_addDrone(&aliens[i]);

				if (aliens[i].flags & FL_LEAVESECTOR)
				{
					// Note: The original version of this line incorrectly
					// specified -15 as the *maximum* instead of the
					// *minimum*, which at the time was equivalent to
					// ``aliens[i].dx = -15``.
					LIMIT_ADD(aliens[i].dx, -0.5, -15, 0);
					aliens[i].dy = 0;
					aliens[i].thinktime = 999;
					aliens[i].face = 0;

					if (aliens[i].x >= 5000)
					{
						aliens[i].flags -= FL_LEAVESECTOR;
						aliens[i].flags += FL_ESCAPED;
						aliens[i].active = false;

						if (aliens[i].classDef == CD_CLOAKFIGHTER)
						{
							game.experimentalShield = aliens[i].shield;
							setInfoLine("Experimental Fighter has fled",
								FONT_CYAN);
						}

						aliens[i].shield = 0;
						updateMissionRequirements(M_ESCAPE_TARGET,
							aliens[i].classDef, 1);
					
						if (aliens[i].classDef != CD_CLOAKFIGHTER)
							updateMissionRequirements(M_DESTROY_TARGET_TYPE,
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
					audio_playSound(SFX_CLOAK, aliens[i].x);
				}

				if (aliens[i].classDef == CD_BARRIER)
				{
					aliens[i].dx = -10 + (sinf(barrierLoop + aliens[i].speed) * 60);
					aliens[i].dy = 20 + (cosf(barrierLoop + aliens[i].speed) * 40);
				}

				if (aliens[i].classDef == CD_MOBILESHIELD)
				{
					LIMIT_ADD(aliens[ALIEN_BOSS].shield, 1, 0,
						aliens[ALIEN_BOSS].maxShield);
				}

				LIMIT_ADD(aliens[i].reload[0], -1, 0, 999);
				LIMIT_ADD(aliens[i].reload[1], -1, 0, 999);

				if ((!(aliens[i].flags & FL_DISABLED)) &&
					(!(aliens[i].flags & FL_NOFIRE)))
				{
					if ((aliens[i].target->shield > 0))
						canFire = alien_checkTarget(&aliens[i]);

					if (((aliens[i].thinktime % 2) == 0) &&
							(aliens[i].flags & FL_FRIEND))
						canFire = alien_enemiesInFront(&aliens[i]);
				}
				else
				{
					canFire = false;
				}

				if (canFire)
				{
					if ((aliens[i].reload[0] == 0) &&
						((rand() % 1000 < aliens[i].chance[0]) ||
							(aliens[i].flags & FL_CONTINUOUS_FIRE)))
					{
						ship_fireBullet(&aliens[i], 0);
					}
					if ((aliens[i].reload[1] == 0) &&
						((rand() % 1000 < aliens[i].chance[1]) ||
							(aliens[i].flags & FL_CONTINUOUS_FIRE)))
					{
						if ((aliens[i].weaponType[1] != W_ENERGYRAY) &&
							(aliens[i].weaponType[1] != W_LASER))
						{
							if (aliens[i].weaponType[1] == W_CHARGER)
								aliens[i].ammo[1] = 50 + rand() % 150;
							ship_fireBullet(&aliens[i], 1);
						}
						else if (aliens[i].weaponType[1] == W_LASER)
						{
							aliens[i].flags += FL_FIRELASER;
						}
						else if ((aliens[i].weaponType[1] == W_ENERGYRAY) &&
							(aliens[i].ammo[0] == 250))
						{
							aliens[i].flags += FL_FIRERAY;
							audio_playSound(SFX_ENERGYRAY, aliens[i].x);
						}
					}
				}

				if (aliens[i].flags & FL_FIRERAY)
				{
					ship_fireRay(&aliens[i]);
				}
				else
				{
					LIMIT_ADD(aliens[i].ammo[0], 1, 0, 250);
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
					shapeToUse += SHIP_HIT_INDEX;

				LIMIT_ADD(aliens[i].hit, -1, 0, 100);

				if ((aliens[i].x + aliens[i].image[0]->w > 0) &&
					(aliens[i].x < screen->w) &&
					(aliens[i].y + aliens[i].image[0]->h > 0) &&
					(aliens[i].y < screen->h))
				{
					if ((!(aliens[i].flags & FL_DISABLED)) &&
							(aliens[i].classDef != CD_ASTEROID) &&
							(aliens[i].classDef != CD_ASTEROID2))
						explosion_addEngine(&aliens[i]);
					if ((!(aliens[i].flags & FL_ISCLOAKED)) || (aliens[i].hit > 0))
						blit(shipShape[shapeToUse], (int)aliens[i].x,
							(int)aliens[i].y);
					if (aliens[i].flags & FL_DISABLED)
					{
						if ((rand() % 10) == 0)
							explosion_add(aliens[i].x + (rand() % aliens[i].image[0]->w),
								aliens[i].y + (rand() % aliens[i].image[0]->h),
								E_ELECTRICAL);
					}
				}

				if ((game.area == MISN_MARS) && (aliens[i].x < -60))
					aliens[i].active = false;
			}
			else
			{
				aliens[i].shield--;
				if ((aliens[i].x > 0) && (aliens[i].x < screen->w) &&
					(aliens[i].y > 0) && (aliens[i].y < screen->h))
				{
					blit(aliens[i].image[aliens[i].face], (int)aliens[i].x,
						(int)aliens[i].y);
					explosion_add(aliens[i].x + (rand() % aliens[i].image[0]->w),
						aliens[i].y + (rand() % aliens[i].image[0]->h),
						E_BIG_EXPLOSION);
				}
				if (aliens[i].shield < aliens[i].deathCounter)
				{
					aliens[i].active = false;
					if ((aliens[i].classDef == CD_BOSS) ||
							(aliens[i].owner == &aliens[ALIEN_BOSS]) ||
							(aliens[i].flags & FL_FRIEND) ||
							(aliens[i].classDef == CD_ASTEROID) ||
							(aliens[i].classDef == CD_KLINE))
						game_addDebris((int)aliens[i].x, (int)aliens[i].y,
							aliens[i].maxShield);

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
			if ((!(aliens[i].flags & FL_NOMOVE)) &&
					(!(aliens[i].flags & FL_DISABLED)))
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
	// This causes the motion to slow
	engine.ssx *= 0.99;
	engine.ssy *= 0.99;

	engine.smx = 0;
	engine.smy = 0;

	int shapeToUse;
	float cd;
	float cc;
	bool xmoved = false;
	bool ymoved = false;

	if (player.shield > -100)
	{
		if (player.shield > 0)
		{
			if ((engine.keyState[KEY_FIRE]))
				ship_fireBullet(&player, 0);

			if ((engine.keyState[KEY_ALTFIRE]) && (player.weaponType[1] != W_NONE))
			{
				if ((player.weaponType[1] != W_CHARGER) &&
					(player.weaponType[1] != W_LASER) && (player.ammo[1] > 0))
				{
					ship_fireBullet(&player, 1);
				}

				if (player.weaponType[1] == W_LASER)
				{
					if (player.ammo[1] < 100)
					{
						ship_fireBullet(&player, 1);

						if (!engine.cheatAmmo)
							player.ammo[1] += 2;

						if (player.ammo[1] >= 100)
						{
							player.ammo[1] = 200;
							setInfoLine("Laser Overheat!!", FONT_WHITE);
						}
					}
				}
			}

			if (player.weaponType[1] == W_CHARGER)
			{
				if (engine.keyState[KEY_ALTFIRE] &&
					((game.difficulty == DIFFICULTY_ORIGINAL) ||
						!(engine.keyState[KEY_FIRE])))
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
								player_chargerFired = true;
							}
						}
					}
				}
				else
				{
					if (player.ammo[1] > 0)
						ship_fireBullet(&player, 1);
					player.ammo[1] = 0;
					player_chargerFired = false;
				}
			}

			if ((engine.keyState[KEY_SWITCH]))
			{
				if ((weapon[W_PLAYER_WEAPON].ammo[0] >= 3) &&
					(weapon[W_PLAYER_WEAPON].ammo[0] <= game.maxPlasmaOutput))
				{
					weapon[W_PLAYER_WEAPON].flags ^= WF_SPREAD;

					if (weapon[W_PLAYER_WEAPON].flags & WF_SPREAD)
					{
						setInfoLine("Weapon set to Spread", FONT_WHITE);
					}
					else
					{
						setInfoLine("Weapon set to Concentrate", FONT_WHITE);
					}
				}

				engine.keyState[KEY_SWITCH] = 0;
			}

			LIMIT_ADD(player.reload[0], -1, 0, 999);
			LIMIT_ADD(player.reload[1], -1, 0, 999);

			if (engine.keyState[KEY_UP])
			{
				player.y -= player.speed;
				engine.ssy += 0.1;
				ymoved = true;
			}

			if (engine.keyState[KEY_DOWN])
			{
				player.y += player.speed;
				engine.ssy -= 0.1;
				ymoved = true;
			}

			if (engine.keyState[KEY_LEFT])
			{
				player.x -= player.speed;
				engine.ssx += 0.1;
				player.face = 1;
				xmoved = true;
			}

			if (engine.keyState[KEY_RIGHT])
			{
				player.x += player.speed;
				engine.ssx -= 0.1;
				player.face = 0;
				xmoved = true;
			}

			if (engine.keyState[KEY_ESCAPE])
			{
				if ((engine.done == 0) && (engine.gameSection == SECTION_GAME) &&
					(currentMission.remainingObjectives1 == 0))
				{
					audio_playSound(SFX_FLY, screen->w / 2);
					engine.done = 2;
					engine.missionCompleteTimer = (SDL_GetTicks() - 1);
				}
			}

			if (engine.keyState[KEY_PAUSE])
			{
				engine.paused = true;
				engine.keyState[KEY_PAUSE] = 0;
			}

			if (((game.area == MISN_ELLESH) &&
					(aliens[ALIEN_BOSS].shield > 0)) ||
				(game.area == MISN_MARS))
			{
				player.face = 0;
				xmoved = true;
				ymoved = true;
			}

			if (engine.done == 0)
			{
				if ((game.difficulty != DIFFICULTY_ORIGINAL) ||
					((game.area != MISN_ELLESH) &&
						(game.area != MISN_MARS)))
				{
					if (xmoved)
					{
						if (player.x < xViewBorder)
						{
							engine.smx += xViewBorder - player.x;
							player.x = xViewBorder;
						}
						else if (player.x > screen->w - xViewBorder)
						{
							engine.smx += (screen->w - xViewBorder) - player.x;
							player.x = screen->w - xViewBorder;
						}
					}
					else if (game.difficulty != DIFFICULTY_ORIGINAL)
					{
						cd = player.x - screen->w / 2;
						if (cd < 0)
						{
							cc = MAX(cd / 10, MAX(0, engine.ssx) - cameraMaxSpeed);
							player.x -= cc;
							engine.smx -= cc;
						}
						else if (cd > 0)
						{
							cc = MIN(cd / 10, cameraMaxSpeed + MIN(0, engine.ssx));
							player.x -= cc;
							engine.smx -= cc;
						}
					}

					if (ymoved)
					{
						if (player.y < yViewBorder)
						{
							engine.smy += yViewBorder - player.y;
							player.y = yViewBorder;
						}
						else if (player.y > screen->h - yViewBorder)
						{
							engine.smy += (screen->h - yViewBorder) - player.y;
							player.y = screen->h - yViewBorder;
						}
					}
					else if (game.difficulty != DIFFICULTY_ORIGINAL)
					{
						cd = player.y - screen->h / 2;
						if (cd < 0)
						{
							cc = MAX(cd / 10, MAX(0, engine.ssy) - cameraMaxSpeed);
							player.y -= cc;
							engine.smy -= cc;
						}
						else if (cd > 0)
						{
							cc = MIN(cd / 10, cameraMaxSpeed + MIN(0, engine.ssy));
							player.y -= cc;
							engine.smy -= cc;
						}
					}
				}
				else
				{
					LIMIT(player.x, xViewBorder, screen->w - xViewBorder);
					LIMIT(player.y, yViewBorder, screen->h - yViewBorder);
				}
			}

			if ((player.maxShield <= 1) || (player.shield > engine.lowShield))
				explosion_addEngine(&player);

			shapeToUse = player.face;

			if (player.hit)
				shapeToUse += SHIP_HIT_INDEX;

			LIMIT_ADD(player.hit, -1, 0, 100);

			blit(shipShape[shapeToUse], (int)player.x, (int)player.y);
			if ((player.maxShield > 1) && (player.shield <= engine.lowShield) &&
					(rand() % 5 < 1))
				explosion_add(player.x + RANDRANGE(-10, 10),
					player.y + RANDRANGE(-10, 20), E_SMOKE);
		}
		else
		{
			player.active = false;
			player.shield--;
			if (player.shield == -1)
			{
				if ((game.hasWingMate1) || (aliens[ALIEN_KLINE].active))
					getPlayerDeathMessage();

				// Make it look like the ships are all still moving...
				if (game.area == MISN_ELLESH)
				{
					for (int i = 0 ; i < ALIEN_MAX ; i++)
						aliens[i].flags |= FL_LEAVESECTOR;
				}

				audio_playSound(SFX_DEATH, player.x);
				audio_playSound(SFX_EXPLOSION, player.x);
			}

			engine.keyState[KEY_UP] = engine.keyState[KEY_DOWN] = engine.keyState[KEY_LEFT] = engine.keyState[KEY_RIGHT] = 0;
			if ((rand() % 3) == 0)
				explosion_add(player.x + RANDRANGE(-10, 10),
					player.y + RANDRANGE(-10, 10), E_BIG_EXPLOSION);
			if (player.shield == -99)
				game_addDebris((int)player.x, (int)player.y, player.maxShield);
		}
	}

	LIMIT(engine.ssx, -cameraMaxSpeed, cameraMaxSpeed);
	LIMIT(engine.ssy, -cameraMaxSpeed, cameraMaxSpeed);

	// Specific for the mission were you have to chase the Executive Transport
	if ((game.area == MISN_ELLESH) && (aliens[ALIEN_BOSS].shield > 0) &&
		(player.shield > 0))
	{
		engine.ssx = -6;
		engine.ssy = 0;
	}
	
	if (game.area == MISN_MARS)
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

			blit(cargo[i].image[0], (int)cargo[i].x, (int)cargo[i].y);

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
				blit(shape[30], (int)chainX, (int)chainY);
				chainX -= dx;
				chainY -= dy;
			}
		}
	}
}

static void game_doDebris()
{
	object *prevDebris = engine.debrisHead;
	object *debris = engine.debrisHead;
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

			explosion_add(debris->x + RANDRANGE(-10, 10), debris->y + RANDRANGE(-10, 10), E_BIG_EXPLOSION);
		}

		if (debris->thinktime < 1)
		{
			prevDebris->next = debris->next;
			delete debris;
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
	object *prevExplosion = engine.explosionHead;
	object *explosion = engine.explosionHead;
	engine.explosionTail = engine.explosionHead;
	
	while (explosion->next != NULL)
	{
		explosion = explosion->next;

		if (explosion->active)
		{
			explosion->x += engine.ssx + engine.smx;
			explosion->y += engine.ssy + engine.smy;

			blit(explosion->image[0], (int)explosion->x, (int)explosion->y);

			if(rand() % 7 == 0)
			{
				explosion->thinktime -= 7;

				if(explosion->thinktime < 1)
				{
					explosion->active = false;
				}
				else
				{
					explosion->face++;
					explosion->image[0] = shape[explosion->face];
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
			delete explosion;
			explosion = prevExplosion;
		}
	}
}

/*
Draw an arrow at the edge of the screen for each enemy ship that is not visible.
*/
static void game_doArrow(int i)
{
	if (i < 0 || !aliens[i].active || aliens[i].shield <= 0 || aliens[i].flags & FL_ISCLOAKED)
		return;

	int x = aliens[i].x + aliens[i].image[0]->w / 2;
	int y = aliens[i].y + aliens[i].image[0]->h / 2;

	float sx = fabsf((x - (screen->w / 2)) / (screen->w / 2.0));
	float sy = fabsf((y - (screen->h / 2)) / (screen->h / 2.0));
	float sxy = MAX(sx, sy);

	if (sxy < 1) 
		return;

	x = screen->w / 2 + (x - screen->w / 2) / sxy;
	y = screen->h / 2 + (y - screen->h / 2) / sxy;

	int arrow;

	if (sxy == sx) {
		arrow = x < screen->w / 2 ? 42 : 38;
		x -= x > screen->w / 2 ? shape[arrow]->w : 0;
		y -= shape[arrow]->h / 2;
	} else {
		arrow = y < screen->h / 2 ? 36 : 40;
		x -= shape[arrow]->w / 2;
		y -= y > screen->h / 2 ? shape[arrow]->h : 0;
	}

	blit(shape[arrow], x, y);

	if (i != engine.targetIndex)
		return;

	if (textShape[3].life > 0)
		return;

	if (sxy == sx) {
		x -= x > screen->w / 2 ? 5 + shape[44]->w : -5 - shape[arrow]->w;
		y -= (shape[44]->h - shape[arrow]->h) / 2;
	} else {
		x -= (shape[44]->w - shape[arrow]->w) / 2;
		y -= y > screen->h / 2 ? 5 + shape[44]->h : -5 - shape[arrow]->h;
	}

	blit(shape[44], x, y);
}

static void game_doHud()
{
	int shieldColor = 0;
	SDL_Rect bar;
	signed char fontColor;
	char text[25];

	addBuffer(0, 20, 800, 25);
	addBuffer(0, 550, 800, 34);

	if (engine.minutes > -1)
	{
		if ((engine.minutes == 0) && (engine.seconds <= 29))
			fontColor = FONT_RED;
		else if ((engine.minutes == 0) && (engine.seconds > 29))
			fontColor = FONT_YELLOW;
		else
			fontColor = FONT_WHITE;
		blitText(10); // time remaining
		sprintf(text, "%.2d:%.2d", engine.minutes, engine.seconds);
		textSurface(30, text, 410, 21, fontColor);
		blitText(30);
	}

	if (game.area != MISN_INTERCEPTION)
	{
		blitText(9); // mission objectives
		sprintf(text, "%d", (currentMission.remainingObjectives1 + currentMission.remainingObjectives2));
		textSurface(39, text, 745, 21, FONT_WHITE);
		blitText(39);
	}

	blitText(8); // cash
	sprintf(text, "%.6d", game.cash);
	textSurface(38, text, 90, 21, FONT_WHITE);
	blitText(38);

	for (int i = 0; i < ALIEN_MAX; i++)
		game_doArrow(i);

	fontColor = FONT_WHITE;
	if (player.ammo[0] > 0)
	{
		if (player.ammo[0] <= 25) fontColor = FONT_YELLOW;
		if (player.ammo[0] <= 10) fontColor = FONT_RED;
	}
	blitText(5); // plasma ammo
	sprintf(text, "%.3d", player.ammo[0]);
	textSurface(35, text, 320, 551, fontColor);
	blitText(35);

	blitText(6);

	if ((player.weaponType[1] != W_CHARGER) && (player.weaponType[1] != W_LASER))
	{
		if (player.ammo[1] == 1)
			fontColor = FONT_RED;
		else
			fontColor = FONT_WHITE;
		sprintf(text, "%.3d", player.ammo[1]); // rocket ammo
		textSurface(36, text, 465, 551, fontColor);
		blitText(36);
	}

	if (((player.weaponType[1] == W_CHARGER) || (player.weaponType[1] == W_LASER)) && (player.ammo[1] > 0))
	{
		int c = white;
		if (player.ammo[1] > 100)
			c = red;

		bar.x = 450;
		bar.y = 550;
		bar.h = 12;

		for (int i = 0 ; i < (player.ammo[1] / 5) ; i++)
		{
			bar.w = 1;
			SDL_FillRect(screen, &bar, c);
			bar.x += 2;
		}
	}

	if ((!allMissionsCompleted()) && (SDL_GetTicks() >= engine.counter2))
	{
		engine.timeTaken++;
		engine.counter2 = SDL_GetTicks() + 1000;
		if (engine.missionCompleteTimer == 0)
			checkScriptEvents();
	}

	if ((engine.timeMission) && (!engine.cheatTime) && (player.shield > 0))
	{
		if (SDL_GetTicks() >= engine.counter)
		{
			if ((engine.seconds > 1) && (engine.seconds <= 11) && (engine.minutes == 0))
			{
				audio_playSound(SFX_CLOCK, screen->w / 2);
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
					if (currentMission.timeLimit1[i] > -1)
						currentMission.timeLimit1[i]--;
					if (currentMission.timeLimit2[i] > -1)
						currentMission.timeLimit2[i]--;
				}
				checkTimer();
				checkScriptEvents();
			}

			if ((engine.seconds == 0) && (engine.minutes == 0))
			{
				for (int i = 0 ; i < 3 ; i++)
				{
					if (currentMission.timeLimit1[i] > -1)
						currentMission.timeLimit1[i]--;
					if (currentMission.timeLimit2[i] > -1)
						currentMission.timeLimit2[i]--;
				}
				checkTimer();
				checkScriptEvents();
				engine.counter = (SDL_GetTicks() + 1000);
			}
		}
	}

	for (int i = 0 ; i < 3 ; i++)
	{
		if (textShape[i].life > 0)
		{
			textShape[i].y = (525 - (i * 20));
			blitText(i);
			textShape[i].life--;

			if (textShape[i].life == 0)
			{
				copyInfoLine(i + 1, i);
				copyInfoLine(i + 2, i + 1);
				textShape[2].life = 0;
			}
		}
	}

	// Show the radio message if there is one
	if (textShape[3].life > 0)
	{
		blit(messageBox, (800 - messageBox->w) / 2, 50);
		textShape[3].life--;
	}

	// Do the target's remaining shield (if required)
	if (game.area != MISN_DORIM)
	{
		if ((engine.targetIndex > -1) && (aliens[engine.targetIndex].shield > 0) && (engine.targetIndex > 9))
		{
			blitText(7);
			bar.w = 1;
			bar.h = 12;
			bar.x = 620;
			bar.y = 550;

			for (float i = 0 ; i < (engine.targetShield * aliens[engine.targetIndex].shield) ; i++)
			{
				if (i > 50)
					shieldColor = green;
				else if ((i >= 25) && (i <= 50))
					shieldColor = yellow;
				else
					shieldColor = red;
				SDL_FillRect(screen, &bar, shieldColor);
				bar.x += 2;
			}
		}
	}

	blitText(11);

	bar.w = 25;
	bar.h = 12;
	bar.x = 80;
	bar.y = 571;

	for (int i = 1 ; i <= 5 ; i++)
	{
		if (weapon[W_PLAYER_WEAPON].damage >= i) {
			if(i <= game.maxPlasmaDamage || (SDL_GetTicks() % 1000 > (unsigned)i * 100))
			{
				SDL_FillRect(screen, &bar, green);
			}
		} else if (i <= game.maxPlasmaDamage)
			SDL_FillRect(screen, &bar, darkGreen);
		bar.x += 30;
	}

	blitText(12);

	bar.w = 25;
	bar.h = 12;
	bar.x = 315;
	bar.y = 571;
	SDL_FillRect(screen, &bar, yellow);

	for (int i = 1 ; i <= 5 ; i++)
	{
		if (weapon[W_PLAYER_WEAPON].ammo[0] >= i) {
			if(i <= game.maxPlasmaOutput || (SDL_GetTicks() % 1000 > (unsigned)i * 100))
			{
				SDL_FillRect(screen, &bar, yellow);
			}
		}
		else if (i <= game.maxPlasmaOutput)
			SDL_FillRect(screen, &bar, darkYellow);
		bar.x += 30;
	}

	blitText(13);

	bar.w = 25;
	bar.h = 12;
	bar.x = 550;
	bar.y = 571;

	for (int i = 1 ; i <= 5 ; i++)
	{
		if (weapon[W_PLAYER_WEAPON].reload[0] <= rate2reload[i]) {
			if(i <= game.maxPlasmaRate || (SDL_GetTicks() % 1000 > (unsigned)i * 100))
			{
				SDL_FillRect(screen, &bar, blue);
			}
		}
		else if (i <= game.maxPlasmaRate)
			SDL_FillRect(screen, &bar, darkerBlue);
		bar.x += 30;
	}

	blitText(4);
	if (player.shield < 1)
		return;

	if ((!engine.keyState[KEY_ALTFIRE]) && (player.weaponType[1] == W_LASER) && (engine.eventTimer % 8 == 1))
		LIMIT_ADD(player.ammo[1], -1, 1, 255);

	if ((engine.eventTimer < 30) && (player.shield <= engine.lowShield))
		return;

	signed char blockSize = 1;

	bar.w = blockSize;
	bar.h = 12;
	bar.x = 95;
	bar.y = 550;

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
		if (player.maxShield < 75)
			bar.x++;
	}
}

/*
Checked during the main game loop. When the game is paused
it goes into a constant loop checking this routine. If escape is
pressed, the game automatically ends and goes back to the title screen
*/
static bool game_checkPauseRequest()
{
	getPlayerInput();
		
	if (engine.keyState[KEY_ESCAPE])
	{
		engine.paused = false;
		engine.done = 1;
		player.shield = 0;
		return true;
	}
	
	if (engine.keyState[KEY_PAUSE])
	{
		engine.paused = false;
		engine.keyState[KEY_PAUSE] = 0;
	}

	return false;
}

int game_mainLoop()
{
	resetLists();

	setMission(game.area);
	missionBriefScreen();

	cargo_init();
	initPlayer();
	aliens_init();

	// specific for Phoebe being captured!
	if (game.area == MISN_NEROD)
		game.hasWingMate1 = 1;

	if (game.area == MISN_ELAMALE)
		aliens[ALIEN_KLINE].active = false;

	for (int i = 0 ; i < engine.maxAliens ; i++)
		alien_add();

	if (game.hasWingMate1)
		alien_addFriendly(ALIEN_PHOEBE);

	if (game.hasWingMate2)
		alien_addFriendly(ALIEN_URSULA);

	if ((game.area == MISN_URUSOR) ||
			(game.area == MISN_POSWIC) ||
			(game.area == MISN_EARTH))
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
			aliens[ALIEN_PHOEBE].active = false;
			aliens[ALIEN_URSULA].active = false;
			break;
	}

	if (game.area == MISN_DORIM)
	{
		aliens[0].collectChance = 100;
		aliens[0].collectType = P_ESCAPEPOD;
	}

	// Some specifics for interception missions
	if (game.area == MISN_INTERCEPTION)
	{
		if ((game.system > 1) && ((rand() % 5) == 0))
		{
			aliens[ALIEN_KLINE] = alien_defs[CD_KLINE];
			aliens[ALIEN_KLINE].owner = &aliens[ALIEN_KLINE];
			aliens[ALIEN_KLINE].target = &player;
			aliens[ALIEN_KLINE].active = true;
			aliens[ALIEN_KLINE].x = player.x + 1000;
			aliens[ALIEN_KLINE].y = player.y;
			player_setTarget(ALIEN_KLINE);
		}

		if ((game.system == 2) && (game.experimentalShield > 0))
		{
			if ((rand() % 5) > 0)
			{
				aliens[10] = alien_defs[CD_CLOAKFIGHTER];
				aliens[10].owner = &aliens[10];
				aliens[10].target = &aliens[10];
				aliens[10].shield = 1000;
				aliens[10].active = true;
				aliens[10].x = player.x - 1000;
				aliens[10].y = player.y;
				player_setTarget(10);
				aliens[10].shield = game.experimentalShield;
			}
		}
	}

	if (game.area == MISN_VENUS)
	{
		aliens[ALIEN_KLINE].x = player.x + 1000;
		aliens[ALIEN_KLINE].y = player.y;
	}

	for (int i = 0 ; i < ALIEN_MAX ; i++)
	{
		aliens[i].systemPower = aliens[i].maxShield;
		aliens[i].deathCounter = 0 - (aliens[i].maxShield * 3);
		LIMIT(aliens[i].deathCounter, -350, 0);
	}

	// Set target energy meter
	switch (game.area)
	{
		case MISN_MOEBO:
		case MISN_ELAMALE:
		case MISN_ODEON:
		case MISN_FELLON:
		case MISN_POSWIC:
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
			player_setTarget(ALIEN_GOODTRANSPORT);
			break;
		case MISN_URUSOR:
			player_setTarget(ALIEN_SID);
			break;
		case MISN_EARTH:
		case MISN_VENUS:
			player_setTarget(ALIEN_KLINE);
			break;
	}

	clearInfoLines();

	loadScriptEvents();

	engine.ssx = 0;
	engine.ssy = 0;
	engine.smx = 0;
	engine.smy = 0;

	engine.done = 0;

	engine.counter = (SDL_GetTicks() + 1000);
	engine.counter2 = (SDL_GetTicks() + 1000);

	engine.missionCompleteTimer = 0;
	engine.musicVolume = 100;

	int rtn = 0;

	int allowableAliens = 999999999;

	for (int i = 0 ; i < 3 ; i++)
	{
		if ((currentMission.primaryType[i] == M_DESTROY_TARGET_TYPE) &&
				(currentMission.target1[i] == CD_ANY))
			allowableAliens = currentMission.targetValue1[i];

		if (currentMission.primaryType[i] == M_DESTROY_ALL_TARGETS)
			allowableAliens = 999999999;
	}

	for (int i = 0 ; i < ALIEN_MAX ; i++)
	{
		if ((aliens[i].active) && (aliens[i].flags & FL_WEAPCO))
		{
			allowableAliens--;
		}
	}

	drawBackGround();
	flushBuffer();

	// Default to no aliens dead...
	engine.allAliensDead = 0;

	engine.keyState[KEY_FIRE] = 0;
	engine.keyState[KEY_ALTFIRE] = 0;
	flushInput();

	while (engine.done != 1)
	{
		updateScreen();

		if ((allMissionsCompleted()) && (engine.missionCompleteTimer == 0))
		{
			engine.missionCompleteTimer = SDL_GetTicks() + 4000;
		}

		if ((missionFailed()) && (engine.missionCompleteTimer == 0))
		{
			if (game.area != MISN_MOEBO)
				engine.missionCompleteTimer = SDL_GetTicks() + 4000;
		}

		if (engine.missionCompleteTimer != 0)
		{
			engine.gameSection = SECTION_INTERMISSION;
			if (player.shield > 0)
			{
				if (SDL_GetTicks() >= engine.missionCompleteTimer)
				{
					if ((!missionFailed()) && (game.area != MISN_VENUS))
					{
						leaveSector();
						if ((engine.done == 2) &&
							(game.area != MISN_DORIM) &&
							(game.area != MISN_SIVEDI))
						{
							if ((aliens[ALIEN_PHOEBE].shield > 0) &&
								(game.area != MISN_EARTH))
							{
								aliens[ALIEN_PHOEBE].x = player.x - 40;
								aliens[ALIEN_PHOEBE].y = player.y - 35;
								aliens[ALIEN_PHOEBE].face = 0;
							}

							if ((aliens[ALIEN_URSULA].shield > 0) &&
								(game.area != MISN_EARTH))
							{
								aliens[ALIEN_URSULA].x = player.x - 40;
								aliens[ALIEN_URSULA].y = player.y + 45;
								aliens[ALIEN_URSULA].face = 0;
							}

							if ((game.area == MISN_URUSOR) ||
								(game.area == MISN_POSWIC))
							{
								aliens[ALIEN_SID].x = player.x - 100;
								aliens[ALIEN_SID].y = player.y;
								aliens[ALIEN_SID].face = 0;
							}
						}
					}
					else if ((game.area == MISN_VENUS) &&
						(engine.musicVolume > 0))
					{
						engine.keyState[KEY_UP] = 0;
						engine.keyState[KEY_DOWN] = 0;
						engine.keyState[KEY_LEFT] = 0;
						engine.keyState[KEY_RIGHT] = 0;
						engine.keyState[KEY_FIRE] = 0;
						engine.keyState[KEY_ALTFIRE] = 0;
						LIMIT_ADD(engine.musicVolume, -0.2, 0, 100);
						audio_setMusicVolume(engine.musicVolume);
					}
					else
					{
						engine.done = 1;
					}
				}
				else
				{
					getPlayerInput();
				}
			}
			else
			{
				LIMIT_ADD(engine.musicVolume, -0.2, 0, 100);
				audio_setMusicVolume(engine.musicVolume);
				if (SDL_GetTicks() >= engine.missionCompleteTimer)
				{
					engine.done = 1;
				}
			}
		}
		else
		{
			getPlayerInput();
		}

		unBuffer();
		game_doStars();
		game_doCollectables();
		game_doBullets();
		game_doAliens();
		game_doPlayer();
		game_doCargo();
		game_doDebris();
		game_doExplosions();
		game_doHud();

		WRAP_ADD(engine.eventTimer, -1, 0, 60);

		if (engine.paused)
		{
			textSurface(22, "PAUSED", -1, screen->h / 2, FONT_WHITE);
			blitText(22);
			updateScreen();
			audio_pauseMusic();

			while (engine.paused)
			{
				engine.done = game_checkPauseRequest();
				delayFrame();
			}

			audio_resumeMusic();
		}

		if ((game.area == MISN_MARS) && (engine.addAliens > -1))
		{
			if ((rand() % 5) == 0)
				// XXX: The originally specified range for x was [800, 100],
				// which with the original rrand function caused the result
				// returned to be `800 + rand() % -699`. Clearly a mistake,
				// but I'm not entirely sure what the original intention was.
				// For now, I've set the range to [800, 1500], which
				// approximately replicates the original's results.
				collectable_add(RANDRANGE(800, 1500),
					RANDRANGE(-screen->h / 3, (4 * screen->h) / 3), P_MINE, 25,
					180 + rand() % 60);
		}

		if (engine.addAliens > -1)
		{
			WRAP_ADD(engine.addAliens, -1, 0, currentMission.addAliens);
			if ((engine.addAliens == 0) && (allowableAliens > 0))
			{
				allowableAliens -= alien_add();
			}
		}

		if ((player.shield <= 0) && (engine.missionCompleteTimer == 0))
			engine.missionCompleteTimer = SDL_GetTicks() + 7000;

		// specific to Boss 1
		if ((game.area == MISN_MOEBO) &&
			(aliens[ALIEN_BOSS].flags & FL_ESCAPED))
		{
			audio_playSound(SFX_DEATH, aliens[ALIEN_BOSS].x);
			clearScreen(white);
			updateScreen();
			for (int i = 0 ; i < 300 ; i++)
			{
				SDL_Delay(10);
				if ((rand() % 25) == 0)
					audio_playSound(SFX_EXPLOSION, aliens[ALIEN_BOSS].x);
			}
			SDL_Delay(1000);
			break;
		}

		delayFrame();
	}

	flushBuffer();

	if ((player.shield > 0) && (!missionFailed()))
	{
		if (game.area < MISN_VENUS)
			missionFinishedScreen();

		switch (game.area)
		{
			case MISN_MOEBO:
				doCutscene(1);
				doCutscene(2);
				break;
			case MISN_NEROD:
				doCutscene(3);
				break;
			case MISN_ELAMALE:
				doCutscene(4);
				break;
			case MISN_ODEON:
				doCutscene(5);
				break;
			case MISN_ELLESH:
				doCutscene(6);
				break;
			case MISN_VENUS:
				doCredits();
				break;
		}
		
		if (game.area < MISN_VENUS)
		{
			updateSystemStatus();
			saveGame(0);
		}

		rtn = 1;
		
		if (game.area == MISN_VENUS)
			rtn = 0;
	}
	else
	{
		gameover();
		rtn = 0;
	}

	exitPlayer();

	return rtn;
}
