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

/*
Create a new collectable item based on supplied arguments.
*/
void addCollectable(float x, float y, int type, int value, int life)
{
	if (type == P_ANYTHING)
	{
		type = P_CASH;

		int r = rand() % 9;

		switch (r)
		{
			case 0:
				type = P_PLASMA_AMMO;
				break;
			case 1:
				type = P_SHIELD;
				break;
			case 2:
				type = P_ROCKET;
				value /= 10;
				break;
		}
	}
	else if (type == P_WEAPONS)
	{
		type = P_PLASMA_RATE;

		int r = rand() % 61;

		if (r <= 19)
			type = P_PLASMA_DAMAGE;
		else if (r <= 39)
			type = P_PLASMA_SHOT;
		else if (r <= 59)
			type = P_PLASMA_RATE;
		else
			type = P_SUPER;
	}

	if (type == P_SUPER)
		value = max(value, 1);

	if (value == 0)
		return; // don't bother!

	/*
	No cash, powerups, or ammo on interceptions. Stops point leeching(!)
	*/
	if ((currentGame.area == MAX_MISSIONS - 1) &&
		((type == P_CASH) || (type == P_PLASMA_AMMO) || (type == P_ROCKET) ||
			(type == P_PLASMA_DAMAGE) || (type == P_PLASMA_SHOT) ||
			(type == P_PLASMA_RATE) || (type == P_SUPER)))
	{
		return;
	}

	// If plasma rate is at max, convert plasma rate powerup to ammo.
	if (type == P_PLASMA_RATE)
	{
		if (weapon[W_PLAYER_WEAPON].reload[0] <= rate2reload[currentGame.maxPlasmaRate])
		{
			type = P_PLASMA_AMMO;
		}
	}

	// If plasma output is at max, convert plasma output powerup to ammo.
	if (type == P_PLASMA_SHOT)
	{
		if (weapon[W_PLAYER_WEAPON].ammo[0] >= currentGame.maxPlasmaOutput)
		{
			type = P_PLASMA_AMMO;
		}
	}

	// If plasma damage is at max, convert plasma damage powerup to ammo.
	if (type == P_PLASMA_DAMAGE)
	{
		if (weapon[W_PLAYER_WEAPON].damage >= currentGame.maxPlasmaDamage)
		{
			type = P_PLASMA_AMMO;
		}
	}

	// No point in giving the player plasma ammo if the weapons aren't
	// upgraded! Give them money instead.
	if (type == P_PLASMA_AMMO)
	{
		if ((weapon[W_PLAYER_WEAPON].reload[0] >= rate2reload[currentGame.minPlasmaRate]) &&
			(weapon[W_PLAYER_WEAPON].ammo[0] <= currentGame.minPlasmaOutput) &&
			(weapon[W_PLAYER_WEAPON].damage <= currentGame.minPlasmaDamage))
		{
			type = P_CASH;
		}
	}

	// If the player has a charge cannon or a laser cannon, don't give them
	// rockets. Causes problems otherwise :)
	if (type == P_ROCKET)
	{
		if ((player.weaponType[1] == W_CHARGER) || (player.weaponType[1] == W_LASER))
		{
			type = P_CASH;
		}
	}

	// Deny the Super Charge in Nightmare difficulty, and on bosses.
	if (type == P_SUPER)
	{
		if ((currentGame.difficulty >= DIFFICULTY_NIGHTMARE) ||
			((currentGame.difficulty > DIFFICULTY_EASY) &&
				((currentGame.area == 5) || (currentGame.area == 11) ||
					(currentGame.area == 18) || (currentGame.area == 25))))
		{
			type = P_PLASMA_DAMAGE;
		}
	}

	// Shield bonus is useless in Nightmare difficulty; give cash instead.
	if (type == P_SHIELD)
	{
		if (currentGame.difficulty >= DIFFICULTY_NIGHTMARE)
		{
			type = P_CASH;
		}
	}

	collectables *collectable = new collectables;

	collectable->next = NULL;
	collectable->active = true;
	collectable->x = x;
	collectable->y = y;

	collectable->dx = rrand(-100, 100);
	if (collectable->dx != 0)
		collectable->dx /= 100;

	collectable->dy = rrand(-100, 100);
	if (collectable->dy != 0)
		collectable->dy /= 100;

	collectable->type = type;
	collectable->value = value;
	collectable->life = life;

	switch(type)
	{
		case P_CASH:
			collectable->image = shape[24];
			break;

		case P_ROCKET:
			collectable->image = shape[49];
			break;

		case P_PLASMA_AMMO:
			collectable->image = shape[25];
			break;

		case P_SHIELD:
			collectable->image = shape[26];
			break;

		case P_PLASMA_SHOT:
			collectable->image = shape[27];
			break;

		case P_PLASMA_RATE:
			collectable->image = shape[28];
			break;

		case P_PLASMA_DAMAGE:
			collectable->image = shape[29];
			break;

		case P_CARGO:
			collectable->image = shape[32];
			break;

		case P_SUPER:
			collectable->image = shape[50];
			break;

		case P_MINE:
			collectable->image = shape[31];
			break;

		case P_SLAVES:
		case P_ESCAPEPOD:
			collectable->image = shape[45];
			break;

		case P_ORE:
			collectable->image = shape[46 + rand() % 3];
			break;
	}

	engine.collectableTail->next = collectable;
	engine.collectableTail = collectable;
}

static void explodeMine(collectables *collectable)
{
	if ((collectable->x >= 0) && (collectable->x <= screen->w) &&
			(collectable->y >= 0) && (collectable->y <= screen->h))
		playSound(SFX_EXPLOSION, collectable->x);

	for (int i = 0 ; i < 10 ; i++)
		addExplosion(collectable->x + rand() % 25 - rand() % 25,
			collectable->y + rand() % 25 - rand() % 25, E_BIG_EXPLOSION);

	if (checkPlayerShockDamage(collectable->x, collectable->y))
		setInfoLine("Warning: Mine damage to shield!!", FONT_RED);
}

void checkMineBulletCollisions(object *bullet)
{
	collectables *collectable = engine.collectableHead;
	collectables *prevCollectable = engine.collectableHead;
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
					currentGame.minesKilled++;
					currentGame.hits++;
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
			explodeMine(collectable);
			prevCollectable->next = collectable->next;
			delete collectable;
			collectable = prevCollectable;
		}
	}
}

/*
Loops through the currently active collectables (in a linked list). The collectable
will travel in the direction that was defined when it was made. Its life will decreased
whilst it remains active. It will be removed if the player touches it or if its life
reaches 0. When it is picked up, depending on the type of collectable it is, mission requirements
will be updated. Information will be displayed and appropriate player variables altered.
*/
void doCollectables()
{
	collectables *collectable = engine.collectableHead;
	collectables *prevCollectable = engine.collectableHead;
	engine.collectableTail = engine.collectableHead;

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
				char temp[40];
				switch(collectable->type)
				{
					case P_CASH:
						currentGame.cash += collectable->value;
						currentGame.cashEarned += collectable->value;
						sprintf(temp, "Got $%d ", collectable->value);
						break;

					case P_ROCKET:
						limitCharAdd(&player.ammo[1], collectable->value, 0,
							currentGame.maxRocketAmmo);
						if (player.ammo[1] == currentGame.maxRocketAmmo)
							sprintf(temp, "Rocket Ammo at Maximum");
						else
						{
							if (collectable->value > 1)
								sprintf(temp, "Got %d rockets", collectable->value);
							else
								sprintf(temp, "Got a rocket");
						}
						currentGame.rocketPickups += collectable->value;
						break;

					case P_SHIELD:
						limitInt(&(player.shield += 10), 0, player.maxShield);
						currentGame.shieldPickups ++;
						sprintf(temp, "Restored 10 shield points");
						break;

					case P_PLASMA_RATE:
						if (player.ammo[0] < 50)
							player.ammo[0] = 50;
						if (weapon[W_PLAYER_WEAPON].reload[0] <= rate2reload[currentGame.maxPlasmaRate])
							sprintf(temp, "Firing Rate already at Maximum");
						else
						{
							weapon[W_PLAYER_WEAPON].reload[0] -= 2;
							sprintf(temp, "Firing rate increased");
						}
						currentGame.powerups++;
						break;

					case P_PLASMA_SHOT:
						if (player.ammo[0] < 50)
							player.ammo[0] = 50;
						if (weapon[W_PLAYER_WEAPON].ammo[0] >= currentGame.maxPlasmaOutput)
							sprintf(temp, "Plasma output already at Maximum");
						else
						{
							weapon[W_PLAYER_WEAPON].ammo[0]++;
							sprintf(temp, "Plasma output increased");
						}
						currentGame.powerups++;
						break;

					case P_PLASMA_DAMAGE:
						if (player.ammo[0] < 50)
							player.ammo[0] = 50;
						if (weapon[W_PLAYER_WEAPON].damage >= currentGame.maxPlasmaDamage)
							sprintf(temp, "Plasma damage already at Maximum");
						else {
							weapon[W_PLAYER_WEAPON].damage++;
							sprintf(temp, "Plasma damage increased");
						}
						currentGame.powerups++;
						break;

					case P_SUPER:
						weapon[W_PLAYER_WEAPON].ammo[0] = 5;
						weapon[W_PLAYER_WEAPON].damage = 5;
						weapon[W_PLAYER_WEAPON].reload[0] = rate2reload[5];
						weapon[W_PLAYER_WEAPON].flags |= WF_SPREAD;

						sprintf(temp, "Picked up a Super Charge!!");

						if (player.ammo[0] < 50)
							player.ammo[0] = 50;
						currentGame.powerups++;
						break;

					case P_PLASMA_AMMO:
						if (player.ammo[0] >= currentGame.maxPlasmaAmmo)
							sprintf(temp, "Plasma cells already at Maximum");
						else
						{
							limitCharAdd(&player.ammo[0], collectable->value, 0, currentGame.maxPlasmaAmmo);
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
						currentGame.cellPickups += collectable->value;
						break;

					case P_CARGO:
						strcpy(temp, "Picked up some Cargo");
						currentGame.cargoPickups++;
						break;

					case P_SLAVES:
						sprintf(temp, "Rescued %d slaves", collectable->value);
						currentGame.slavesRescued += collectable->value;
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
						playSound(SFX_SHIELDUP, player.x);
					else
						playSound(SFX_PICKUP, player.x);
				}
			}

			// stop people from exploiting a weapon check condition
			if (player.ammo[0] == 0)
			{
				weapon[W_PLAYER_WEAPON].ammo[0] = currentGame.minPlasmaOutput;
				weapon[W_PLAYER_WEAPON].damage = currentGame.minPlasmaDamage;
				weapon[W_PLAYER_WEAPON].reload[0] = rate2reload[currentGame.minPlasmaRate];
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
				explodeMine(collectable);
			prevCollectable->next = collectable->next;
			delete collectable;
			collectable = prevCollectable;
		}
	}
}

