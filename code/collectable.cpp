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
		value = 1;

	/*
	Cash is rare on interceptions. Stops people from point leeching(!)
	*/
	if ((currentGame.area == MAX_MISSIONS - 1) && (type == P_CASH))
	{
		if (rand() % 10 > 0)
			return;
	}

	if (value == 0)
		return; // don't bother!

	// If the player has a charge cannon or a laser cannon, don't give them
	// rockets. Causes problems otherwise :)
	if (type == P_ROCKET)
	{
		if ((player.weaponType[1] == W_CHARGER) || (player.weaponType[1] == W_LASER))
		{
			type = P_CASH;
		}
	}

	collectables *collectable = new collectables;

	collectable->next = NULL;
	collectable->active = true;
	collectable->x = x;
	collectable->y = y;

	collectable->dx = Math::rrand(-100, 100);
	if (collectable->dx != 0)
		collectable->dx /= 100;

	collectable->dy = Math::rrand(-100, 100);
	if (collectable->dy != 0)
		collectable->dy /= 100;

	collectable->type = type;
	collectable->value = value;
	collectable->life = life;

	switch(type)
	{
		case P_CASH:
			collectable->image = graphics.shape[24];
			break;

		case P_ROCKET:
			collectable->image = graphics.shape[49];
			break;

		case P_PLASMA_AMMO:
			collectable->image = graphics.shape[25];
			break;

		case P_SHIELD:
			collectable->image = graphics.shape[26];
			break;

		case P_PLASMA_SHOT:
			collectable->image = graphics.shape[27];
			break;

		case P_PLASMA_RATE:
			collectable->image = graphics.shape[28];
			break;

		case P_PLASMA_DAMAGE:
			collectable->image = graphics.shape[29];
			break;

		case P_CARGO:
			collectable->image = graphics.shape[32];
			break;

		case P_SUPER:
			collectable->image = graphics.shape[50];
			break;

		case P_MINE:
			collectable->image = graphics.shape[31];
			break;

		case P_SLAVES:
		case P_ESCAPEPOD:
			collectable->image = graphics.shape[45];
			break;

		case P_ORE:
			collectable->image = graphics.shape[46 + rand() % 3];
			break;
	}

	engine.collectableTail->next = collectable;
	engine.collectableTail = collectable;
}

static void explodeMine(collectables *collectable)
{
	if ((collectable->x >= 0) && (collectable->x <= 800) && (collectable->y >= 0) && (collectable->y <= 600))
		playSound(SFX_EXPLOSION);

	for (int i = 0 ; i < 10 ; i++)
		addExplosion(collectable->x + rand() % 25 - rand() % 25, collectable->y + rand() % 25 - rand() % 25, E_BIG_EXPLOSION);

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
			if (Collision::collision(collectable, bullet))
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
			if ((collectable->x + collectable->image->w > 0) && (collectable->x < 800) && (collectable->y + collectable->image->h > 0) && (collectable->y < 600))
				graphics.blit(collectable->image, (int)collectable->x, (int)collectable->y);

			collectable->x += engine.ssx;
			collectable->y += engine.ssy;
			collectable->x += collectable->dx;
			collectable->y += collectable->dy;

			collectable->life--;

			if ((player.shield > 0) && (Collision::collision(collectable, &player)))
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
						Math::limitCharAdd(&player.ammo[1], collectable->value, 0, currentGame.maxRocketAmmo);
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
						Math::limitInt(&(player.shield += 10), 0, player.maxShield);
						currentGame.shieldPickups ++;
						sprintf(temp, "Restored 10 shield points");
						break;

					case P_PLASMA_RATE:
						Math::limitCharAdd(&weapon[1].reload[0], -2, currentGame.maxPlasmaRate, 15);
						player.weaponType[0] = 1;
						if (player.ammo[0] < 50)
							player.ammo[0] = 50;
						Math::limitChar(&(player.ammo[0]), 0, currentGame.maxPlasmaAmmo);
						if (weapon[1].reload[0] == currentGame.maxPlasmaRate)
							sprintf(temp, "Firing Rate at Maximum");
						else
							sprintf(temp, "Firing rate increased");
						currentGame.powerups++;
						break;

					case P_PLASMA_SHOT:
						Math::limitCharAdd(&weapon[1].ammo[0], 1, 1, currentGame.maxPlasmaOutput);
						if (player.ammo[0] < 50)
							player.ammo[0] = 50;
						Math::limitChar(&(player.ammo[0]), 0, currentGame.maxPlasmaAmmo);
						if (weapon[1].ammo[0] == currentGame.maxPlasmaOutput)
							sprintf(temp, "Plasma output at Maximum");
						else
							sprintf(temp, "Plasma output increased");
						player.weaponType[0] = 1;
						currentGame.powerups++;
						break;

					case P_PLASMA_DAMAGE:
						Math::limitCharAdd(&weapon[1].damage, 1, 1, currentGame.maxPlasmaDamage);
						if (player.ammo[0] < 50)
							player.ammo[0] = 50;
						Math::limitChar(&(player.ammo[0]), 0, currentGame.maxPlasmaAmmo);
						if (weapon[1].damage == currentGame.maxPlasmaDamage)
							sprintf(temp, "Plasma damage at Maximum");
						else
							sprintf(temp, "Plasma damage increased");
						player.weaponType[0] = 1;
						currentGame.powerups++;
						break;

					case P_SUPER:
						weapon[1].ammo[0] = 5;
						weapon[1].damage = 5;
						weapon[1].reload[0] = 7;

						if (weapon[1].flags & WF_STRAIGHT)
							weapon[1].flags -= WF_STRAIGHT;

						if (weapon[1].flags & WF_THIN_SPREAD)
							weapon[1].flags -= WF_THIN_SPREAD;

						if (!(weapon[1].flags & WF_WIDE_SPREAD))
							weapon[1].flags += WF_WIDE_SPREAD;
							
						sprintf(temp, "Picked up a Super Charge!!");

						if (player.ammo[0] < 50)
							player.ammo[0] = 50;
						player.weaponType[0] = 1;
						currentGame.powerups++;
						break;

					case P_PLASMA_AMMO:
						Math::limitCharAdd(&player.ammo[0], collectable->value, 0, currentGame.maxPlasmaAmmo);
						if (player.ammo[0] == currentGame.maxPlasmaAmmo)
							sprintf(temp, "Plasma cells at Maximum");
						else
						{
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
						player.weaponType[0] = 1;
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

				updateMissionRequirements(M_COLLECT, collectable->type, collectable->value);

				collectable->active = false;
				if (collectable->type != P_MINE)
				{
					setInfoLine(temp, FONT_WHITE);
					if (collectable->type == P_SHIELD)
						playSound(SFX_SHIELDUP);
					else
						playSound(SFX_PICKUP);
				}
			}

			// stop people from exploiting a weapon check condition
			if (player.ammo[0] == 0)
			{
				player.weaponType[0] = 0;
				weapon[1] = weapon[0]; // reset to weapon 1 defaults
			}
		}

		if (collectable->life < 1)
		{
			collectable->active = false;
			if ((collectable->type == P_CARGO) || (collectable->type == P_ESCAPEPOD) || (collectable->type == P_SLAVES))
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

