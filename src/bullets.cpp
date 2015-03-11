/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012 Guus Sliepen
Copyright (C) 2015 Julian Marchant

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

void addBullet(object *theWeapon, object *attacker, int y, int dy)
{
	object *bullet;
	signed char imageIndex;
	int tempX, tempY, steps;

	bullet = new object;

	if (attacker == &player)
		currentGame.shots++;

	bullet->next = NULL;
	bullet->active = true;
	bullet->x = attacker->x + (attacker->image[0]->w / 2) -
		(theWeapon->image[0]->w * attacker->face);
	bullet->y = attacker->y + y;
	bullet->flags = theWeapon->flags;
	bullet->shield = 300; // bullets live for (approximately) 5 seconds

	// Timed explosions live between 1 and 3 seconds
	if (bullet->flags & WF_TIMEDEXPLOSION)
		bullet->shield = 60 + ((rand() % 3) * 60);

	if (attacker->face == 0)
	{
		bullet->dx = theWeapon->speed;
		if ((currentGame.area == 18) || (currentGame.area == 24))
			bullet->dx += fabsf(engine.ssx + engine.smx);
	}
	else
	{
		bullet->dx = (0 - theWeapon->speed);
	}

	if (bullet->flags & WF_VARIABLE_SPEED)
	{
		bullet->dx = rrand(100, 200);
		bullet->dx /= 10;
		if (attacker->face == 1)
			bullet->dx = 0 - bullet->dx;
	}

	bullet->dy = dy;

	if (bullet->flags & WF_SCATTER)
	{
		bullet->dy = rrand(-200, 200);
		if (bullet->dy != 0)
			bullet->dy /= 200;
	}

	if (attacker->flags & FL_WEAPCO)
		bullet->flags |= WF_WEAPCO;
	else
		bullet->flags |= WF_FRIEND;

	bullet->owner = attacker->owner;

	bullet->id = theWeapon->id;

	bullet->damage = theWeapon->damage;

	if (bullet->id == WT_CHARGER)
	{
		bullet->damage = attacker->ammo[1] / 2;
		if (bullet->damage < 15)
		{
			bullet->damage = 1;
			bullet->id = WT_PLASMA;
		}
	}

	bullet->target = NULL;

	if (attacker->flags & FL_FRIEND)
		imageIndex = 0;
	else
		imageIndex = 1;

	// Use the enemy's images if applicable
	if (bullet->id != WT_ROCKET)
		bullet->image[0] = theWeapon->image[imageIndex];
	else
		bullet->image[0] = theWeapon->image[attacker->face];

	if (bullet->flags & WF_AIMED)
	{
		tempX = (int)fabsf(attacker->target->x - attacker->x);
		tempY = (int)fabsf(attacker->target->y - attacker->y);
		steps = max(tempX, tempY);

		if (steps < 12)
			steps = 12;

		if (!(bullet->flags & WF_TIMEDEXPLOSION))
			steps /= 8;
		else
			steps /= 6 + (rand() % 6);

		tempX = (int)(attacker->target->x - attacker->x);
		tempY = (int)(attacker->target->y - attacker->y);

		bullet->dx = tempX / steps;
		bullet->dy = tempY / steps;
	}

	if (attacker->classDef == CD_ASTEROID)
	{
		bullet->dx = rrand(-20, 20);
		bullet->dy = rrand(-20, 20);
		bullet->image[0] = shape[4];
	}

	engine.bulletTail->next = bullet;
	engine.bulletTail = bullet;
}

/*
Fill in later...
*/
void fireBullet(object *attacker, int weaponType)
{
	if (attacker->reload[weaponType] > 0)
		return;

	int y = (attacker->image[0]->h) / 5;

	// Remove some ammo from the player
	if ((attacker == &player) && (weaponType == 1) && (!engine.cheatAmmo))
		player.ammo[1]--;

	object *theWeapon = &weapon[attacker->weaponType[weaponType]];

	switch(theWeapon->id)
	{
		case WT_PLASMA:
		case WT_SPREAD:
		case WT_DIRECTIONAL:
			audio_playSound(SFX_PLASMA, attacker->x);
			break;
		case WT_ROCKET:
			audio_playSound(SFX_MISSILE, attacker->x);
			break;
		case WT_LASER:
			audio_playSound(SFX_LASER, attacker->x);
			break;
		case WT_CHARGER:
			audio_playSound(SFX_PLASMA3, attacker->x);
			break;
	}

	if (theWeapon->flags & WF_SPREAD && theWeapon->ammo[0] >= 3)
	{
		addBullet(theWeapon, attacker, y * 1, -2);

		if(theWeapon->ammo[0] != 4)
			addBullet(theWeapon, attacker, y * 3, 0);

		if(theWeapon->ammo[0] != 3)
		{
			addBullet(theWeapon, attacker, y * 2, -1);
			addBullet(theWeapon, attacker, y * 4, 1);
		}

		addBullet(theWeapon, attacker, y * 5, 2);
	}
	else
	{
		if(theWeapon->ammo[0] & 1)
			addBullet(theWeapon, attacker, y * 3, 0);

		if(theWeapon->ammo[0] >= 2)
		{
			addBullet(theWeapon, attacker, y * 2, 0);
			addBullet(theWeapon, attacker, y * 4, 0);
		}

		if(theWeapon->ammo[0] >= 4)
		{
			addBullet(theWeapon, attacker, y * 1, 0);
			addBullet(theWeapon, attacker, y * 5, 0);
		}
	}

	// Reset the weapon reload time. Double it if it is not friendly or
	// a boss or Kline
	attacker->reload[weaponType] = theWeapon->reload[0];
	if ((attacker->flags & FL_WEAPCO) && (attacker != &aliens[ALIEN_BOSS]) &&
			(attacker != &aliens[ALIEN_KLINE]) && (theWeapon->id != W_LASER))
		attacker->reload[weaponType] *= 2;

	if ((engine.cheatAmmo) || (theWeapon->id == WT_LASER))
		return;

	if ((attacker == &player) && (weaponType == 0))
	{
		if (player.ammo[0] > 0)
		{
			player.ammo[0]--;
			if (player.ammo[0] <= 0)
			{
				weapon[W_PLAYER_WEAPON].ammo[0] = currentGame.minPlasmaOutput;
				weapon[W_PLAYER_WEAPON].damage = currentGame.minPlasmaDamage;
				weapon[W_PLAYER_WEAPON].reload[0] = rate2reload[currentGame.minPlasmaRate];
			}
		}
	}
}

/*
Used for homing missiles. When a missile is active and it is told to home in
on an enemy, it will attempt to randomly grab one every frame if it does not
already have a target. If the target it is currently chasing is killed, it will
begin to look for a new one (done in doBullets()). The homing missile will make
one attempt per call (one call per frame) to find a suitable target. If the target
it picks is dead or outside the screen range, then it returns NULL. A suitable
target will be returned as the object address.
*/
static object *getRandomEnemy(object *bullet)
{
	int i;

	if (bullet->owner->flags & FL_WEAPCO)
	{
		i = (rand() % 10);

		if (i < 1)
			return &player;
	}

	i = rand() % ALIEN_MAX;

	if ((aliens[i].shield < 1) || (!aliens[i].active))
		return NULL;

	if (aliens[i].flags & FL_IMMORTAL)
		return NULL;

	if ((bullet->owner->flags & FL_WEAPCO) && (aliens[i].flags & FL_WEAPCO))
		return NULL;

	if ((bullet->owner->flags & FL_FRIEND) && (aliens[i].flags & FL_FRIEND))
		return NULL;

	if (abs((int)bullet->x - (int)aliens[i].target->x) > 800)
		return NULL;

	if (abs((int)bullet->y - (int)aliens[i].target->y) > 200)
		return NULL;

	return &aliens[i];
}

char checkPlayerShockDamage(float x, float y)
{
	// Don't let the player be hurt by an explosion after they have completed
	// all the mission objectives. That would be *really* annoying!
	if ((engine.cheatShield) || (engine.missionCompleteTimer != 0))
		return 0;

	float distX = fabsf(x - player.x);
	float distY = fabsf(y - player.y);

	if ((distX <= 50) && (distY <= 50))
	{
		if (distX >= 1)
			distX /= 5;

		if (distY >= 1)
			distY /= 5;

		player.shield -= (int)(10 - distX);
		player.shield -= (int)(10 - distY);
		limitInt(&player.shield, 0, player.maxShield);
		player.hit = 10;

		return 1;
	}

	return 0;
}

/*
Fill in later...
*/
void fireRay(object *attacker)
{
	SDL_Rect ray;

	if (attacker->face == 0)
	{
		ray.x = (int)(attacker->x + attacker->image[0]->w);
	}
	else
	{
		ray.x = (int)(attacker->x - 800);
	}
	ray.y = (int)(attacker->y + attacker->engineY - 1);
	ray.h = 3;
	ray.w = 800;

	int red = SDL_MapRGB(screen->format, rand() % 256, 0x00, 0x00);
	SDL_FillRect(screen, &ray, red);
	addBuffer(ray.x, ray.y, ray.w, ray.h);

	if (attacker != &player)
	{
		if (player.shield > 0)
		{
			if (collision(player.x, player.y, player.image[0]->w, player.image[0]->h, ray.x, ray.y, ray.w, ray.h) && (!engine.cheatShield))
			{
				if (player.shield > engine.lowShield)
				{
					if (player.shield - 1 <= engine.lowShield)
					{
						setInfoLine("!!! WARNING: SHIELD LOW !!!", FONT_RED);
					}
				}
				player.shield--;

				addExplosion(player.x, player.y, E_SMALL_EXPLOSION);
				audio_playSound(SFX_HIT, player.x);
				if (player.shield < 1)
				{
					audio_playSound(SFX_DEATH, player.x);
					audio_playSound(SFX_EXPLOSION, player.x);
				}
			}
		}
	}

	object *anEnemy = aliens;

	for (int i = 0 ; i < ALIEN_MAX ; i++)
	{
		if (anEnemy->flags & FL_IMMORTAL)
			continue;

		if ((anEnemy->shield > 0) && (attacker != anEnemy) && (attacker->classDef != anEnemy->classDef))
		{
			if (collision(anEnemy->x, anEnemy->y, anEnemy->image[0]->w, anEnemy->image[0]->h, ray.x, ray.y, ray.w, ray.h))
			{
				anEnemy->shield--;
				addExplosion(anEnemy->x, anEnemy->y, E_SMALL_EXPLOSION);
				audio_playSound(SFX_HIT, anEnemy->x);
				if (anEnemy->shield < 1)
				{
					alien_destroy(anEnemy, attacker->owner);
				}
			}
		}

		anEnemy++;
	}

	attacker->ammo[0]--;
	if (attacker->ammo[0] < 1)
		attacker->flags &= ~FL_FIRERAY;
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
void doBullets()
{
	object *bullet = engine.bulletHead;
	object *prevBullet = engine.bulletHead;
	engine.bulletTail = engine.bulletHead;

	object *alien, *theCargo;

	bool okayToHit = false;
	float homingMissileSpeed = 0;

	while (bullet->next != NULL)
	{
		bullet = bullet->next;

		if (bullet->active)
		{
			if (bullet->flags & WF_HOMING)
			{
			  	if (bullet->target == NULL)
					bullet->target = getRandomEnemy(bullet);

				if (bullet->owner->flags & FL_FRIEND)
					homingMissileSpeed = 0.25;
				else
					homingMissileSpeed = 0.05;
			}

			if (bullet->id == WT_ROCKET)
			{
				addExplosion(bullet->x, bullet->y, E_SMALL_EXPLOSION);
			}
			else if (bullet->id == WT_MICROROCKET)
			{
				addExplosion(bullet->x, bullet->y, E_TINY_EXPLOSION);
			}

			if ((bullet->flags & WF_AIMED))
			{
				blit(bullet->image[0], (int)(bullet->x - bullet->dx), (int)(bullet->y - bullet->dy));
			}

			if (bullet->id == WT_CHARGER)
			{
				for (int i = 0 ; i < bullet->damage * 2 ; i++)
					blit(bullet->image[0],
						(int)(bullet->x - rrand(-(bullet->damage * 2 / 3), 0)),
						(int)(bullet->y + rrand(-3, 3)));
			}

			blit(bullet->image[0], (int)bullet->x, (int)bullet->y);
			bullet->x += bullet->dx;
			bullet->y += bullet->dy;

			if (bullet->target != NULL)
			{
				if (bullet->x < bullet->target->x)
					limitFloat(&(bullet->dx += homingMissileSpeed), -15, 15);
				if (bullet->x > bullet->target->x)
					limitFloat(&(bullet->dx -= homingMissileSpeed), -15, 15);

				//Rocket is (more or less) inline with target. Fly straight
				if ((bullet->x > bullet->target->x - 1) && (bullet->x < bullet->target->x + 5))
					bullet->dx = 0;

				if (bullet->y < bullet->target->y)
					limitFloat(&(bullet->dy += homingMissileSpeed), -15, 15);
				if (bullet->y > bullet->target->y)
					limitFloat(&(bullet->dy -= homingMissileSpeed), -15, 15);

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
				alien = &aliens[i];

				if ((alien->shield < 1) || (!alien->active))
					continue;

				okayToHit = false;

				if ((bullet->flags & WF_FRIEND) && (alien->flags & FL_WEAPCO))
					okayToHit = true;
				if ((bullet->flags & WF_WEAPCO) && (alien->flags & FL_FRIEND))
					okayToHit = true;
				if ((bullet->id == WT_ROCKET) || (bullet->id == WT_LASER) ||
						(bullet->id == WT_CHARGER))
					okayToHit = true;

				if (bullet->owner == alien->owner)
					okayToHit = false;

				if (okayToHit)
				{
					if ((bullet->active) && (collision(bullet, alien)))
					{
						if (bullet->owner == &player)
						{
							currentGame.hits++;
							if ((alien->classDef == CD_PHOEBE) ||
									(alien->classDef == CD_URSULA))
								getMissFireMessage(alien);
						}

						if (!(alien->flags & FL_IMMORTAL))
						{
							if (!(bullet->flags & WF_DISABLE))
								alien->shield -= bullet->damage;
							else
								alien->systemPower -= bullet->damage;

							alien->hit = 5;
						}

						if (alien->classDef == CD_KLINE)
						{
							if (currentGame.area == 11)
							{
								if ((alien->shield <= alien->maxShield - 500) &&
									!(alien->flags & FL_LEAVESECTOR))
								{
									alien->flags |= FL_LEAVESECTOR;
									alien->flags &= ~FL_CIRCLES;
									setRadioMessage(FACE_KLINE, "Seems I underestimated you, Bainfield. We'll meet again!", 1);
								}
							}
							else if (currentGame.area == 25)
							{
								if ((alien->shield <= alien->maxShield - 750) &&
									!(alien->flags & FL_LEAVESECTOR))
								{
									alien->flags |= FL_LEAVESECTOR;
									alien->flags &= ~FL_CIRCLES;
									setRadioMessage(FACE_SID, "Chris, Kethlan is getting away!", 1);
								}
							}
							else if (currentGame.area == 26)
							{
								if (alien->shield + bullet->damage > 1500 &&
										alien->shield <= 1500)
									alien_setKlineAttackMethod(alien);
								else if (alien->shield + bullet->damage > 1000 &&
										alien->shield <= 1000)
									alien_setKlineAttackMethod(alien);
								else if (alien->shield + bullet->damage > 500 &&
										alien->shield <= 500)
									alien_setKlineAttackMethod(alien);
							}
							else
							{
								if ((alien->shield <= alien->maxShield - 100) &&
									!(alien->flags & FL_LEAVESECTOR))
								{
									alien->flags |= FL_LEAVESECTOR;
									alien->flags &= ~FL_CIRCLES;
								}
							}
						}

						if ((alien->flags & FL_RUNSAWAY) && ((rand() % 50) == 0))
						{
							alien->flags |= FL_LEAVESECTOR;
						}

						if (bullet->id == WT_CHARGER)
						{
							bullet->shield -= alien->shield;
							if (bullet->shield <= 0)
								bullet->active = false;
						}
						else
						{
							bullet->active = false;
							bullet->shield = 0;
						}

						audio_playSound(SFX_HIT, alien->x);
						if (alien->AIType == AI_EVASIVE)
							alien->thinktime = 0;

						if (alien->shield < 1)
							alien_destroy(alien, bullet->owner);

						if (alien->systemPower < 1)
						{
							if (!(alien->flags & FL_DISABLED))
							{
								alien->flags += FL_DISABLED;
								updateMissionRequirements(M_DISABLE_TARGET,
									alien->classDef, 1);
							}

							alien->systemPower = 0;
							if (alien->classDef == CD_KLINE)
								alien->systemPower = alien->maxShield;
						}

						if (bullet->id == WT_ROCKET)
							addExplosion(bullet->x, bullet->y, E_BIG_EXPLOSION);
						else
							addExplosion(bullet->x, bullet->y, E_SMALL_EXPLOSION);
					}
				}
			}

			// Check for bullets hitting player
			if ((bullet->flags & WF_WEAPCO) || (bullet->id == WT_ROCKET) ||
				(bullet->id == WT_LASER) || (bullet->id == WT_CHARGER))
			{
				if ((bullet->active) && (player.shield > 0) &&
					(collision(bullet, &player)) && (bullet->owner != &player))
				{
					if ((!engine.cheatShield) || (engine.missionCompleteTimer != 0))
					{
						if ((player.shield > engine.lowShield) &&
								(player.shield - bullet->damage <= engine.lowShield))
							setInfoLine("!!! WARNING: SHIELD LOW !!!", FONT_RED);

						player.shield -= bullet->damage;
						limitInt(&player.shield, 0, player.maxShield);
						player.hit = 5;
					}

					if ((bullet->owner->classDef == CD_PHOEBE) ||
							(bullet->owner->classDef == CD_URSULA))
						getPlayerHitMessage(bullet->owner);

					if (bullet->id != WT_CHARGER)
					{
						bullet->active = false;
						bullet->shield = 0;
					}
					else if (bullet->id == WT_CHARGER)
					{
						bullet->shield -= alien->shield;
						if (bullet->shield < 0)
							bullet->active = false;
					}

					audio_playSound(SFX_HIT, player.x);

					if (bullet->id == WT_ROCKET)
						addExplosion(bullet->x, bullet->y, E_BIG_EXPLOSION);
					else
						addExplosion(bullet->x, bullet->y, E_SMALL_EXPLOSION);
				}
			}
		}

		if ((currentGame.difficulty > DIFFICULTY_EASY) &&
			((bullet->owner == &player) || (bullet->id == WT_ROCKET)))
		{
			for (int j = 0 ; j < 20 ; j++)
			{
				theCargo = &cargo[j];
				if (theCargo->active)
				{
					if (collision(bullet, theCargo))
					{
						bullet->active = false;
						addExplosion(bullet->x, bullet->y, E_SMALL_EXPLOSION);
						audio_playSound(SFX_HIT, theCargo->x);
						if (theCargo->collectType != P_PHOEBE)
						{
							theCargo->active = false;
							audio_playSound(SFX_EXPLOSION, theCargo->x);
							for (int i = 0 ; i < 10 ; i++)
								addExplosion(theCargo->x + rrand(-15, 15), theCargo->y + rrand(-15, 15), E_BIG_EXPLOSION);
							updateMissionRequirements(M_PROTECT_PICKUP, P_CARGO, 1);
						}
					}
				}
			}
		}

		// check to see if a bullet (on any side) hits a mine
		checkMineBulletCollisions(bullet);

		bullet->shield--;

		if (bullet->shield < 1)
		{
			if ((bullet->flags & WF_TIMEDEXPLOSION) || (bullet->id == WT_CHARGER))
			{
				audio_playSound(SFX_EXPLOSION, bullet->x);
				for (int i = 0 ; i < 10 ; i++)
					addExplosion(bullet->x + rrand(-35, 35),
						bullet->y + rrand(-35, 35), E_BIG_EXPLOSION);

				if (bullet->flags & WF_TIMEDEXPLOSION)
					if (checkPlayerShockDamage(bullet->x, bullet->y))
						setInfoLine("Warning: Missile Shockwave Damage!!", FONT_RED);
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

