/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012, 2013 Guus Sliepen
Copyright (C) 2015-2019 Julie Marchant <onpon4@riseup.net>

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
#include <stdio.h>

#include "SDL.h"

#include "defs.h"
#include "structs.h"

#include "engine.h"
#include "game.h"
#include "gfx.h"
#include "intermission.h"
#include "player.h"
#include "save.h"
#include "screen.h"
#include "shop.h"
#include "weapons.h"

typedef struct ShopItem_ {

	int x, y;
	int price;
	char name[50];
	char description[STRMAX];
	int image;

} ShopItem;

static ShopItem shopItems[SHOP_MAX];
static int shopSelectedItem;

static void sell(int i);

static void drawSecondaryWeaponSurface()
{
	char description[50] = "";

	gfx_renderUnicode(_("Secondary Weapon"), 10, 3, FONT_WHITE, 0, gfx_shopSprites[SHOP_S_SECONDARY]);

	switch (player.weaponType[1])
	{
		case W_NONE:
			strcpy(description, _("None"));
			break;
		case W_ROCKETS:
			strcpy(description, _("Rockets"));
			break;
		case W_DOUBLE_ROCKETS:
			strcpy(description, _("Dbl Rockets"));
			break;
		case W_MICRO_ROCKETS:
			strcpy(description, _("Micro Rockets"));
			break;
		case W_LASER:
			strcpy(description, _("Laser"));
			break;
		case W_CHARGER:
			strcpy(description, _("Charger"));
			break;
		case W_HOMING_MISSILE:
			strcpy(description, _("Homing Missile"));
			break;
		case W_DOUBLE_HOMING_MISSILES:
			strcpy(description, _("Dbl Homing Missiles"));
			break;
		case W_MICRO_HOMING_MISSILES:
			strcpy(description, _("Mcr Homing Missiles"));
			break;
	}
	gfx_renderUnicode(description, 10, 22, FONT_WHITE, 0, gfx_shopSprites[SHOP_S_SECONDARY]);

	if ((player.weaponType[1] != W_LASER) &&
		(player.weaponType[1] != W_CHARGER) && (player.weaponType[1] != W_NONE))
	{
		/// Retain "%d" as-is.  It is replaced with the rocket capacity of the Firefly.
		sprintf(description, _("Capacity : %d"), game.maxRocketAmmo);
		gfx_renderUnicode(description, 10, 37, FONT_WHITE, 0, gfx_shopSprites[SHOP_S_SECONDARY]);
	}
}

static void adjustShopPrices()
{
	if (game.difficulty == DIFFICULTY_ORIGINAL)
	{
		shopItems[SHOP_PLASMA_MAX_OUTPUT].price = 500 * game.maxPlasmaOutput;
		shopItems[SHOP_PLASMA_MAX_DAMAGE].price = 500 * game.maxPlasmaDamage;
		shopItems[SHOP_PLASMA_MAX_RATE].price = 500 * (game.maxPlasmaRate * 2 - 1);

		shopItems[SHOP_PLASMA_MIN_OUTPUT].price = 2000 * game.minPlasmaOutput;
		shopItems[SHOP_PLASMA_MIN_DAMAGE].price = 2000 * game.minPlasmaDamage;
		shopItems[SHOP_PLASMA_MIN_RATE].price = 2000 * (game.minPlasmaRate * 2 - 1);
	}
	else
	{
		shopItems[SHOP_PLASMA_MAX_OUTPUT].price = 1000 * (game.maxPlasmaOutput + 1);
		shopItems[SHOP_PLASMA_MAX_DAMAGE].price = 1000 * (game.maxPlasmaDamage + 1);
		shopItems[SHOP_PLASMA_MAX_RATE].price = 1000 * (game.maxPlasmaRate + 1);

		shopItems[SHOP_PLASMA_MIN_OUTPUT].price = 1500 * (game.minPlasmaOutput + 1);
		shopItems[SHOP_PLASMA_MIN_DAMAGE].price = 1500 * (game.minPlasmaDamage + 1);
		shopItems[SHOP_PLASMA_MIN_RATE].price = 1500 * (game.minPlasmaRate + 1);

		if (game.maxPlasmaOutput <= game.minPlasmaOutput)
			shopItems[SHOP_PLASMA_MIN_OUTPUT].price += shopItems[SHOP_PLASMA_MAX_OUTPUT].price;
		if (game.maxPlasmaDamage <= game.minPlasmaDamage)
			shopItems[SHOP_PLASMA_MIN_DAMAGE].price += shopItems[SHOP_PLASMA_MAX_DAMAGE].price;
		if (game.maxPlasmaRate <= game.minPlasmaRate)
			shopItems[SHOP_PLASMA_MIN_RATE].price += shopItems[SHOP_PLASMA_MAX_RATE].price;
	}

	if (game.difficulty == DIFFICULTY_ORIGINAL)
		shopItems[SHOP_PLASMA_MAX_AMMO].price = (5 * game.maxPlasmaAmmo);
	else
		shopItems[SHOP_PLASMA_MAX_AMMO].price = (10 * (game.maxPlasmaAmmo - 75));

	shopItems[SHOP_ROCKET_MAX_AMMO].price = (25 * game.maxRocketAmmo);

	if (game.maxPlasmaOutput >= game.maxPlasmaOutputLimit)
		shopItems[SHOP_PLASMA_MAX_OUTPUT].price = 0;

	if (game.maxPlasmaDamage >= game.maxPlasmaDamageLimit)
		shopItems[SHOP_PLASMA_MAX_DAMAGE].price = 0;

	if (game.maxPlasmaRate >= game.maxPlasmaRateLimit)
		shopItems[SHOP_PLASMA_MAX_RATE].price = 0;

	if (game.minPlasmaOutput >= game.minPlasmaOutputLimit)
		shopItems[SHOP_PLASMA_MIN_OUTPUT].price = 0;

	if (game.minPlasmaDamage >= game.minPlasmaDamageLimit)
		shopItems[SHOP_PLASMA_MIN_DAMAGE].price = 0;

	if (game.minPlasmaRate >= game.minPlasmaRateLimit)
		shopItems[SHOP_PLASMA_MIN_RATE].price = 0;

	if (game.maxPlasmaAmmo >= game.maxPlasmaAmmoLimit)
		shopItems[SHOP_PLASMA_MAX_AMMO].price = 0;

	if (game.maxRocketAmmo >= game.maxRocketAmmoLimit)
		shopItems[SHOP_ROCKET_MAX_AMMO].price = 0;
}

static void drawShop()
{
	char description[100];
	int icons = SHOP_MAX;

	adjustShopPrices();

	shopItems[SHOP_PLASMA_MAX_OUTPUT].x = SHOP_X;
	shopItems[SHOP_PLASMA_MAX_OUTPUT].y = SHOP_Y + 70;

	shopItems[SHOP_PLASMA_MAX_DAMAGE].x = SHOP_X + 50;
	shopItems[SHOP_PLASMA_MAX_DAMAGE].y = SHOP_Y + 70;

	shopItems[SHOP_PLASMA_MAX_RATE].x = SHOP_X + 100;
	shopItems[SHOP_PLASMA_MAX_RATE].y = SHOP_Y + 70;

	shopItems[SHOP_PLASMA_MIN_OUTPUT].x = SHOP_X;
	shopItems[SHOP_PLASMA_MIN_OUTPUT].y = SHOP_Y + 130;

	shopItems[SHOP_PLASMA_MIN_DAMAGE].x = SHOP_X + 50;
	shopItems[SHOP_PLASMA_MIN_DAMAGE].y = SHOP_Y + 130;

	shopItems[SHOP_PLASMA_MIN_RATE].x = SHOP_X + 100;
	shopItems[SHOP_PLASMA_MIN_RATE].y = SHOP_Y + 130;

	shopItems[SHOP_PLASMA_AMMO].x = SHOP_X + 250;
	shopItems[SHOP_PLASMA_AMMO].y = SHOP_Y + 70;

	shopItems[SHOP_ROCKET_AMMO].x = SHOP_X + 300;
	shopItems[SHOP_ROCKET_AMMO].y = SHOP_Y + 70;

	shopItems[SHOP_PLASMA_MAX_AMMO].x = SHOP_X + 350;
	shopItems[SHOP_PLASMA_MAX_AMMO].y = SHOP_Y + 70;

	shopItems[SHOP_ROCKET_MAX_AMMO].x = SHOP_X + 400;
	shopItems[SHOP_ROCKET_MAX_AMMO].y = SHOP_Y + 70;

	shopItems[SHOP_DOUBLE_ROCKETS].x = SHOP_X + 250;
	shopItems[SHOP_DOUBLE_ROCKETS].y = SHOP_Y + 130;

	shopItems[SHOP_MICRO_ROCKETS].x = SHOP_X + 300;
	shopItems[SHOP_MICRO_ROCKETS].y = SHOP_Y + 130;

	shopItems[SHOP_LASER].x = SHOP_X + 350;
	shopItems[SHOP_LASER].y = SHOP_Y + 130;

	shopItems[SHOP_HOMING_MISSILE].x = SHOP_X + 400;
	shopItems[SHOP_HOMING_MISSILE].y = SHOP_Y + 130;

	shopItems[SHOP_CHARGER].x = SHOP_X + 450;
	shopItems[SHOP_CHARGER].y = SHOP_Y + 130;

	shopItems[SHOP_DOUBLE_HOMING_MISSILES].x = SHOP_X + 500;
	shopItems[SHOP_DOUBLE_HOMING_MISSILES].y = SHOP_Y + 130;

	shopItems[SHOP_MICRO_HOMING_MISSILES].x = SHOP_X + 550;
	shopItems[SHOP_MICRO_HOMING_MISSILES].y = SHOP_Y + 130;

	for (int i = 0 ; i < SHOP_S_MAX ; i++)
	{
		if (gfx_shopSprites[i] != NULL)
		{
			SDL_FreeSurface(gfx_shopSprites[i]);
		}
	}

	gfx_shopSprites[SHOP_S_PRIMARY] = gfx_createSurface(190, 91);
	gfx_shopSprites[SHOP_S_POWERUP] = gfx_createSurface(190, 91);
	gfx_shopSprites[SHOP_S_SECONDARY] = gfx_createSurface(190, 91);

	gfx_drawRect(gfx_shopSprites[SHOP_S_PRIMARY], 0, 0, 189, 90, 0x00, 0x00, 0x55);
	gfx_drawRect(gfx_shopSprites[SHOP_S_PRIMARY], 0, 0, 189, 20, 0x00, 0x00, 0x99);
	gfx_drawRect(gfx_shopSprites[SHOP_S_POWERUP], 0, 0, 189, 90, 0x00, 0x00, 0x55);
	gfx_drawRect(gfx_shopSprites[SHOP_S_POWERUP], 0, 0, 189, 20, 0x00, 0x00, 0x99);
	gfx_drawRect(gfx_shopSprites[SHOP_S_SECONDARY], 0, 0, 189, 90, 0x00, 0x00, 0x55);
	gfx_drawRect(gfx_shopSprites[SHOP_S_SECONDARY], 0, 0, 189, 20, 0x00, 0x00, 0x99);

	gfx_shopSprites[SHOP_S_SHIP_INFO] = gfx_createAlphaRect(601, 41, 0x00, 0x00, 0x00);
	gfx_drawRect(gfx_shopSprites[SHOP_S_SHIP_INFO], 0, 0, 600, 40, 0x00, 0x00, 0x33);

	switch (shopSelectedItem)
	{
		case SHOP_NOTHING:
		case SHOP_ERROR_INSUFFICIENT_FUNDS:
		case SHOP_ERROR_CANNOT_UPGRADE:
		case SHOP_ERROR_AMMO_LIMIT:
		case SHOP_ERROR_CANNOT_SELL:
		case SHOP_ERROR_NOTHING_TO_SELL:
			break;
		case SHOP_PLASMA_MAX_OUTPUT:
		case SHOP_PLASMA_MAX_DAMAGE:
		case SHOP_PLASMA_MAX_RATE:
		case SHOP_PLASMA_MAX_AMMO:
			gfx_drawRect(gfx_shopSprites[SHOP_S_POWERUP], 0, 0, 189, 90, 0x55, 0x00, 0x00);
			gfx_drawRect(gfx_shopSprites[SHOP_S_POWERUP], 0, 0, 189, 20, 0x99, 0x00, 0x00);
			break;
		case SHOP_PLASMA_MIN_OUTPUT:
		case SHOP_PLASMA_MIN_DAMAGE:
		case SHOP_PLASMA_MIN_RATE:
			gfx_drawRect(gfx_shopSprites[SHOP_S_PRIMARY], 0, 0, 189, 90, 0x55, 0x00, 0x00);
			gfx_drawRect(gfx_shopSprites[SHOP_S_PRIMARY], 0, 0, 189, 20, 0x99, 0x00, 0x00);
			break;
		case SHOP_PLASMA_AMMO:
		case SHOP_ROCKET_AMMO:
			gfx_drawRect(gfx_shopSprites[SHOP_S_SHIP_INFO], 0, 0, 600, 40, 0x33, 0x00, 0x00);
			break;
		default:
			gfx_drawRect(gfx_shopSprites[SHOP_S_SECONDARY], 0, 0, 189, 90, 0x55, 0x00, 0x00);
			gfx_drawRect(gfx_shopSprites[SHOP_S_SECONDARY], 0, 0, 189, 20, 0x99, 0x00, 0x00);
			break;
	}

	strcpy(description, "");

	gfx_renderUnicode(_("Primary Weapon"), 10, 3, FONT_WHITE, 0, gfx_shopSprites[SHOP_S_PRIMARY]);
	/// Retain "%d" as-is.  It is replaced with the min plasma output.
	sprintf(description, _("Cannons : %d"), game.minPlasmaOutput);
	gfx_renderUnicode(description, 10, 22, FONT_WHITE, 0, gfx_shopSprites[SHOP_S_PRIMARY]);
	/// Retain "%d" as-is.  It is replaced with the min plasma damage.
	sprintf(description, _("Power   : Stage %d"),
		game.minPlasmaDamage);
	gfx_renderUnicode(description, 10, 37, FONT_WHITE, 0, gfx_shopSprites[SHOP_S_PRIMARY]);
	/// Retain "%d" as-is.  It is replaced with the min plasma cooling.
	sprintf(description, _("Cooling : Stage %d"),
		game.minPlasmaRate);
	gfx_renderUnicode(description, 10, 52, FONT_WHITE, 0, gfx_shopSprites[SHOP_S_PRIMARY]);

	gfx_renderUnicode(_("Powerup Weapon"), 10, 3, FONT_WHITE, 0, gfx_shopSprites[SHOP_S_POWERUP]);
	/// Retain "%d" as-is.  It is replaced with the max plasma output.
	sprintf(description, _("Splitter  : Stage %d"),
		game.maxPlasmaOutput);
	gfx_renderUnicode(description, 10, 22, FONT_WHITE, 0, gfx_shopSprites[SHOP_S_POWERUP]);
	/// Retain "%d" as-is.  It is replaced with the max plasma damage.
	sprintf(description, _("Condensor : Stage %d"),
		game.maxPlasmaDamage);
	gfx_renderUnicode(description, 10, 37, FONT_WHITE, 0, gfx_shopSprites[SHOP_S_POWERUP]);
	/// Retain "%d" as-is.  It is replaced with the max plasma cooling.
	sprintf(description, _("L.Nitrogen: Stage %d"),
		game.maxPlasmaRate);
	gfx_renderUnicode(description, 10, 52, FONT_WHITE, 0, gfx_shopSprites[SHOP_S_POWERUP]);
	/// Retain "%d" as-is.  It is replaced with the Firefly's plasma ammo capacity.
	sprintf(description, _("Capacity  : %d"), game.maxPlasmaAmmo);
	gfx_renderUnicode(description, 10, 67, FONT_WHITE, 0, gfx_shopSprites[SHOP_S_POWERUP]);

	drawSecondaryWeaponSurface();

	gfx_shopSprites[SHOP_S_CATALOG] = gfx_createSurface(601, 121);

	gfx_drawRect(gfx_shopSprites[SHOP_S_CATALOG], 0, 0, 600, 120, 0x00, 0x00, 0x22);

	gfx_renderUnicode(_("Temporary Weapons"), 10, 2, FONT_WHITE, 0, gfx_shopSprites[SHOP_S_CATALOG]);
	gfx_renderUnicode(_("Ammo and Storage"), 260, 2, FONT_WHITE, 0, gfx_shopSprites[SHOP_S_CATALOG]);

	gfx_renderUnicode(_("Primary Weapons"), 10, 62, FONT_WHITE, 0, gfx_shopSprites[SHOP_S_CATALOG]);

	gfx_renderUnicode(_("Secondary Weapons"), 260, 62, FONT_WHITE, 0, gfx_shopSprites[SHOP_S_CATALOG]);

	if (game.system == 0)
		icons = SHOP_DOUBLE_ROCKETS + 1;
	else if (game.system == 1)
		icons = SHOP_LASER + 1;
	else if (game.system == 2)
		icons = SHOP_CHARGER + 1;

	for (int i = 0 ; i < icons ; i++)
	{
		gfx_blit(gfx_sprites[shopItems[i].image], shopItems[i].x + 10 - SHOP_X,
			shopItems[i].y - SHOP_Y - 48, gfx_shopSprites[SHOP_S_CATALOG]);
	}

	/// Retain "%d" as-is.  It is replaced with the Firefly's max shield.
	sprintf(description, _("Shield : %d"), player.maxShield);
	gfx_renderUnicode(description, 10, 6, FONT_WHITE, 0, gfx_shopSprites[SHOP_S_SHIP_INFO]);
	/// Retain "%d" as-is.  It is replaced with the player's current cash.
	sprintf(description, _("  Cash : $%d"), game.cash);
	gfx_renderUnicode(description, 10, 22, FONT_WHITE, 0, gfx_shopSprites[SHOP_S_SHIP_INFO]);
	/// Retain "%.3d".  It is replaced with the ship's current number of plasma cells.
	/// "%.3d" can be changed to "%d" if you wish to not fill in space with zeroes,
	/// e.g. render the number 5 as "5" rather than "005".
	sprintf(description, _("Plasma Cells : %.3d"), player.ammo[0]);
	gfx_renderUnicode(description, 430, 6, FONT_WHITE, 0, gfx_shopSprites[SHOP_S_SHIP_INFO]);
	/// Retain "%.2d".  It is replaced with the ship's current number of rockets.
	/// "%.2d" can be changed to "%d" if you wish to not fill in space with zeroes,
	/// e.g. render the number 3 as "3" rather than "03".
	sprintf(description, _("Rockets :  %.2d"), player.ammo[1]);
	gfx_renderUnicode(description, 475, 22, FONT_WHITE, 0, gfx_shopSprites[SHOP_S_SHIP_INFO]);

	gfx_shopSprites[SHOP_S_ITEM_INFO] = gfx_createSurface(601, 56);
	gfx_drawRect(gfx_shopSprites[SHOP_S_ITEM_INFO], 0, 0, 600, 35, 0x00, 0x99, 0x00);
	gfx_drawRect(gfx_shopSprites[SHOP_S_ITEM_INFO], 0, 20, 600, 35, 0x00, 0x33, 0x00);
	gfx_renderUnicode(_("Information"), 5, 4, FONT_WHITE, 0, gfx_shopSprites[SHOP_S_ITEM_INFO]);

	switch (shopSelectedItem)
	{
		case SHOP_NOTHING:
			break;
		case SHOP_ERROR_INSUFFICIENT_FUNDS:
			/// For when the player attempts to buy something they can't afford.
			gfx_renderUnicode(_("You don't have enough money"), 20, 30, FONT_WHITE, 0, gfx_shopSprites[SHOP_S_ITEM_INFO]);
			break;
		case SHOP_ERROR_CANNOT_UPGRADE:
			/// For when the player attempts an upgrade beyond the maximum (line 1 of 2).
			gfx_renderUnicode(_("Cannot upgrade ship"), 5, 22, FONT_WHITE, 0, gfx_shopSprites[SHOP_S_ITEM_INFO]);

			/// For when the player attempts an upgrade beyond the maximum (line 2 of 2).
			gfx_renderUnicode(_("Hardware capacity has been reached"), 20, 38, FONT_CYAN, 0, gfx_shopSprites[SHOP_S_ITEM_INFO]);
			break;
		case SHOP_ERROR_AMMO_LIMIT:
			/// For when the player attempts to buy more ammo than the ship can hold.
			gfx_renderUnicode(_("Ammunition limit reached"), 20, 30, FONT_WHITE, 0, gfx_shopSprites[SHOP_S_ITEM_INFO]);
			break;
		case SHOP_ERROR_CANNOT_SELL:
			/// For when the player attempts to sell an item they aren't allowed to sell.
			gfx_renderUnicode(_("You cannot sell that item"), 20, 30, FONT_WHITE, 0, gfx_shopSprites[SHOP_S_ITEM_INFO]);
			break;
		case SHOP_ERROR_NOTHING_TO_SELL:
			/// For when the player attempts to sell an item they don't have any of.
			gfx_renderUnicode(_("Nothing to sell"), 20, 30, FONT_WHITE, 0, gfx_shopSprites[SHOP_S_ITEM_INFO]);
			break;
		case SHOP_ERROR_IS_NOT_ROCKETS:
			/// For when the player attempts to buy rockets or rocket capacity
			/// while secondary weapon is either laser or charge cannon.
			gfx_renderUnicode(_("Rockets cannot be bought for Laser or Charger Cannon"), 5, 30, FONT_WHITE, 0, gfx_shopSprites[SHOP_S_ITEM_INFO]);
			break;
		case SHOP_ERROR_ALREADY_OWNED:
			/// For when the player attempts to buy a weapon they already have.
			gfx_renderUnicode(_("You already have that weapon"), 20, 30, FONT_WHITE, 0, gfx_shopSprites[SHOP_S_ITEM_INFO]);
			break;
		case SHOP_ERROR_WEAPON_CAPACITY:
			/// For when the player attempts to increase rocket capacity beyond
			/// what is allowed for the weapon (used for homing missiles).
			gfx_renderUnicode(_("This weapon's ammo limit has been reached"), 20, 30, FONT_WHITE, 0, gfx_shopSprites[SHOP_S_ITEM_INFO]);
			break;
		default:
			if (shopItems[shopSelectedItem].price != 0)
			{
				/// Used to put a shop item's name next to its price.
				/// "%s" is replaced with the item name, and "%d" is replaced
				/// with the item price.
				sprintf(description, _("%s ($%d)"),
					shopItems[shopSelectedItem].description,
					shopItems[shopSelectedItem].price);
			}
			else
			{
				/// Used for shop items that cannot be bought.
				/// "%s" is replaced with the item name.
				sprintf(description, _("%s (N/A)"),
					shopItems[shopSelectedItem].description);
			}
			gfx_renderUnicode(shopItems[shopSelectedItem].name, 5, 22,
				FONT_WHITE, 0, gfx_shopSprites[SHOP_S_ITEM_INFO]);
			gfx_renderUnicode(description, 20, 38, FONT_CYAN, 0, gfx_shopSprites[SHOP_S_ITEM_INFO]);
			break;
	}
}

void shop_init()
{
	/* ----------- Temporary Items ----------- */

	shopItems[SHOP_PLASMA_MAX_OUTPUT].price = 0; // Overwritten later
	strcpy(shopItems[SHOP_PLASMA_MAX_OUTPUT].name, _("Plasma Channel Splitter"));
	/// Shop item description: Plasma Channel Splitter (PLASMA_MAX_OUTPUT)
	strcpy(shopItems[SHOP_PLASMA_MAX_OUTPUT].description, _("Improves poweredup plasma output"));
	shopItems[SHOP_PLASMA_MAX_OUTPUT].image = SP_PLASMA_MAX_OUTPUT;

	shopItems[SHOP_PLASMA_MAX_DAMAGE].price = 0; // Overwritten later
	strcpy(shopItems[SHOP_PLASMA_MAX_DAMAGE].name, _("Plasma Capacity Condensor"));
	/// Shop item description: Plasma Capacity Condensor (PLASMA_MAX_DAMAGE)
	strcpy(shopItems[SHOP_PLASMA_MAX_DAMAGE].description, _("Increases poweredup plasma damage"));
	shopItems[SHOP_PLASMA_MAX_DAMAGE].image = SP_PLASMA_MAX_POWER;

	shopItems[SHOP_PLASMA_MAX_RATE].price = 0; // Overwritten later
	strcpy(shopItems[SHOP_PLASMA_MAX_RATE].name, _("Liquid Nitrogen Capsules"));
	/// Shop item description: Liquid Nitrogen Capsules (PLASMA_MAX_RATE)
	strcpy(shopItems[SHOP_PLASMA_MAX_RATE].description, _("Increases plasma firing rate"));
	shopItems[SHOP_PLASMA_MAX_RATE].image = SP_PLASMA_MAX_RATE;

	if (game.difficulty == DIFFICULTY_ORIGINAL)
		shopItems[SHOP_PLASMA_AMMO].price = 50;
	else
		shopItems[SHOP_PLASMA_AMMO].price = 1;

	strcpy(shopItems[SHOP_PLASMA_AMMO].name, _("Plasma Cells"));
	/// Shop item description: Plasma Cells
	strcpy(shopItems[SHOP_PLASMA_AMMO].description, _("Plasma ammunition (10 cells each)"));
	shopItems[SHOP_PLASMA_AMMO].image = SP_PLASMA_AMMO;

	if (game.difficulty == DIFFICULTY_ORIGINAL)
		shopItems[SHOP_ROCKET_AMMO].price = 50;
	else
		shopItems[SHOP_ROCKET_AMMO].price = 1;

	strcpy(shopItems[SHOP_ROCKET_AMMO].name, _("Rocket Ammo"));
	/// Shop item description: Rocket Ammo
	strcpy(shopItems[SHOP_ROCKET_AMMO].description, _("High velocity dumb fire rocket"));
	shopItems[SHOP_ROCKET_AMMO].image = SP_ROCKET_AMMO;

	/* ----------- Permanent Items ----------- */

	shopItems[SHOP_PLASMA_MIN_OUTPUT].price = 0; // Overwritten later
	strcpy(shopItems[SHOP_PLASMA_MIN_OUTPUT].name, _("Additional Plasma Cannon"));
	/// Shop item description: Additional Plasma Cannon (PLASMA_MIN_OUTPUT)
	strcpy(shopItems[SHOP_PLASMA_MIN_OUTPUT].description, _("Adds an extra plasma cannon to the Firefly"));
	shopItems[SHOP_PLASMA_MIN_OUTPUT].image = SP_PLASMA_MIN_OUTPUT;

	shopItems[SHOP_PLASMA_MIN_DAMAGE].price = 0; // Overwritten later
	strcpy(shopItems[SHOP_PLASMA_MIN_DAMAGE].name, _("Plasma Power Booster"));
	/// Shop item description: Plasma Power Booster (PLASMA_MIN_DAMAGE)
	strcpy(shopItems[SHOP_PLASMA_MIN_DAMAGE].description, _("Increases power of plasma shots"));
	shopItems[SHOP_PLASMA_MIN_DAMAGE].image = SP_PLASMA_MIN_POWER;

	shopItems[SHOP_PLASMA_MIN_RATE].price = 0; // Overwritten later
	strcpy(shopItems[SHOP_PLASMA_MIN_RATE].name, _("Plasma Cooling Booster"));
	/// Shop item description: Plasma Cooling Booster (PLASMA_MIN_RATE)
	strcpy(shopItems[SHOP_PLASMA_MIN_RATE].description, _("Permanently increases firing rate"));
	shopItems[SHOP_PLASMA_MIN_RATE].image = SP_PLASMA_MIN_RATE;

	/* ----------- Ammo Items -------------- */

	shopItems[SHOP_PLASMA_MAX_AMMO].price = 0; // Overwritten later
	strcpy(shopItems[SHOP_PLASMA_MAX_AMMO].name, _("Plasma Compressor"));
	/// Shop item description: Plasma Compressor (PLASMA_MAX_AMMO)
	strcpy(shopItems[SHOP_PLASMA_MAX_AMMO].description, _("Increases plasma ammo capacity"));
	shopItems[SHOP_PLASMA_MAX_AMMO].image = SP_PLASMA_MAX_AMMO;

	shopItems[SHOP_ROCKET_MAX_AMMO].price = 0; // Overwritten later
	strcpy(shopItems[SHOP_ROCKET_MAX_AMMO].name, _("Rocket Pod"));
	/// Shop item description: Rocket Pod (ROCKET_MAX_AMMO)
	strcpy(shopItems[SHOP_ROCKET_MAX_AMMO].description, _("Allows for an additional 5 rockets to be carried"));
	shopItems[SHOP_ROCKET_MAX_AMMO].image = SP_ROCKET_MAX_AMMO;

	/* ---------- Weaponary --------------- */

	shopItems[SHOP_DOUBLE_ROCKETS].price = 2000;
	strcpy(shopItems[SHOP_DOUBLE_ROCKETS].name, _("Dual Rocket Launcher"));
	/// Shop item description: Dual Rocket Launcher
	strcpy(shopItems[SHOP_DOUBLE_ROCKETS].description, _("Launches two rockets at once"));
	shopItems[SHOP_DOUBLE_ROCKETS].image = SP_DOUBLE_ROCKETS;

	shopItems[SHOP_MICRO_ROCKETS].price = 2500;
	strcpy(shopItems[SHOP_MICRO_ROCKETS].name, _("Micro Rocket Launcher"));
	/// Shop item description: Micro Rocket Launcher
	strcpy(shopItems[SHOP_MICRO_ROCKETS].description, _("Launches several less powerful rockets at once"));
	shopItems[SHOP_MICRO_ROCKETS].image = SP_MICRO_ROCKETS;

	shopItems[SHOP_LASER].price = 5000;
	strcpy(shopItems[SHOP_LASER].name, _("Laser Cannon"));
	/// Shop item description: Laser Cannon
	strcpy(shopItems[SHOP_LASER].description, _("Fires a continuous stream of energy particles"));
	shopItems[SHOP_LASER].image = SP_LASER;

	shopItems[SHOP_HOMING_MISSILE].price = 7500;
	strcpy(shopItems[SHOP_HOMING_MISSILE].name, _("Homing Missile Launcher"));
	/// Shop item description: Homing Missile Launcher
	/// %i must be retained.  It is replaced by the maximum missile
	/// capacity of the weapon.
	sprintf(shopItems[SHOP_HOMING_MISSILE].description, _("Fires homing missile (max %i missiles)"), MAX_HOMING);
	shopItems[SHOP_HOMING_MISSILE].image = SP_HOMING_MISSILE;

	shopItems[SHOP_CHARGER].price = 10000;
	strcpy(shopItems[SHOP_CHARGER].name, _("Charge Cannon"));
	/// Shop item description: Charge Cannon
	strcpy(shopItems[SHOP_CHARGER].description, _("Compacts plasma into clusters for greater damage"));
	shopItems[SHOP_CHARGER].image = SP_CHARGER;

	shopItems[SHOP_DOUBLE_HOMING_MISSILES].price = 10000;
	strcpy(shopItems[SHOP_DOUBLE_HOMING_MISSILES].name, _("Dual Homing Missile Launcher"));
	/// Shop item description: Dual Homing Missile Launcher
	/// %i must be retained.  It is replaced by the maximum missile
	/// capacity of the weapon.
	sprintf(shopItems[SHOP_DOUBLE_HOMING_MISSILES].description, _("Fires two homing missiles (max %i missiles)"), MAX_DOUBLE_HOMING);
	shopItems[SHOP_DOUBLE_HOMING_MISSILES].image = SP_DOUBLE_HOMING_MISSILES;

	shopItems[SHOP_MICRO_HOMING_MISSILES].price = 15000;
	strcpy(shopItems[SHOP_MICRO_HOMING_MISSILES].name, _("Micro Homing Missile Launcher"));
	/// Shop item description: Micro Homing Missile Launcher
	/// %i must be retained.  It is replaced by the maximum missile
	/// capacity of the weapon.
	sprintf(shopItems[SHOP_MICRO_HOMING_MISSILES].description, _("Fires several small homing missiles (max %i missiles)"), MAX_MICRO_HOMING);
	shopItems[SHOP_MICRO_HOMING_MISSILES].image = SP_MICRO_HOMING_MISSILES;

 	shopSelectedItem = SHOP_NOTHING;

	drawShop();
}

static void shop_sellSecondaryWeapon()
{
	switch (player.weaponType[1])
	{
		case W_DOUBLE_ROCKETS:
			sell(SHOP_DOUBLE_ROCKETS);
			break;

		case W_MICRO_ROCKETS:
			sell(SHOP_MICRO_ROCKETS);
			break;

		case W_LASER:
			sell(SHOP_LASER);
			break;

		case W_HOMING_MISSILE:
			sell(SHOP_HOMING_MISSILE);
			break;

		case W_CHARGER:
			sell(SHOP_CHARGER);
			break;

		case W_DOUBLE_HOMING_MISSILES:
			sell(SHOP_DOUBLE_HOMING_MISSILES);
			break;

		case W_MICRO_HOMING_MISSILES:
			sell(SHOP_MICRO_HOMING_MISSILES);
			break;
	}
}

static void buy(int i)
{
	if ((game.cash < shopItems[i].price) && (!engine.cheatCash))
	{
		shopSelectedItem = SHOP_ERROR_INSUFFICIENT_FUNDS;
		drawShop();
		return;
	}

	switch (i)
	{
		case SHOP_PLASMA_MAX_OUTPUT:
			if (game.maxPlasmaOutput >= game.maxPlasmaOutputLimit)
			{
				shopSelectedItem = SHOP_ERROR_CANNOT_UPGRADE;
				return;
			}
			game.maxPlasmaOutput++;
			break;

		case SHOP_PLASMA_MAX_DAMAGE:
			if (game.maxPlasmaDamage >= game.maxPlasmaDamageLimit)
			{
				shopSelectedItem = SHOP_ERROR_CANNOT_UPGRADE;
				return;
			}
			game.maxPlasmaDamage++;
			break;

		case SHOP_PLASMA_MAX_RATE:
			if (game.maxPlasmaRate >= game.maxPlasmaRateLimit)
			{
				shopSelectedItem = SHOP_ERROR_CANNOT_UPGRADE;
				return;
			}
			game.maxPlasmaRate++;
			break;

		case SHOP_PLASMA_AMMO:
			if (player.ammo[0] >= game.maxPlasmaAmmo)
			{
				shopSelectedItem = SHOP_ERROR_AMMO_LIMIT;
				return;
			}
			LIMIT_ADD(player.ammo[0], 10, 0, game.maxPlasmaAmmo);
			break;

		case SHOP_ROCKET_AMMO:
			if ((player.weaponType[1] == W_CHARGER) ||
				(player.weaponType[1] == W_LASER))
			{
				shopSelectedItem = SHOP_ERROR_IS_NOT_ROCKETS;
				return;
			}
			if (player.ammo[1] == game.maxRocketAmmo)
			{
				shopSelectedItem = SHOP_ERROR_AMMO_LIMIT;
				return;
			}
			if ((player.weaponType[1] == W_HOMING_MISSILE) &&
				(player.ammo[1] >= MAX_HOMING))
			{
				shopSelectedItem = SHOP_ERROR_WEAPON_CAPACITY;
				return;
			}
			if ((player.weaponType[1] == W_DOUBLE_HOMING_MISSILES) &&
				(player.ammo[1] >= MAX_DOUBLE_HOMING))
			{
				shopSelectedItem = SHOP_ERROR_WEAPON_CAPACITY;
				return;
			}
			if ((player.weaponType[1] == W_MICRO_HOMING_MISSILES) &&
				(player.ammo[1] >= MAX_MICRO_HOMING))
			{
				shopSelectedItem = SHOP_ERROR_WEAPON_CAPACITY;
				return;
			}
			player.ammo[1]++;
			break;

		case SHOP_PLASMA_MIN_OUTPUT:
			if (game.minPlasmaOutput >= game.minPlasmaOutputLimit)
			{
				shopSelectedItem = SHOP_ERROR_CANNOT_UPGRADE;
				return;
			}
			game.minPlasmaOutput++;
			if (game.maxPlasmaOutput < game.minPlasmaOutput)
				game.maxPlasmaOutput = game.minPlasmaOutput;
			break;

		case SHOP_PLASMA_MIN_DAMAGE:
			if (game.minPlasmaDamage >= game.minPlasmaDamageLimit)
			{
				shopSelectedItem = SHOP_ERROR_CANNOT_UPGRADE;
				return;
			}
			game.minPlasmaDamage++;
			if (game.maxPlasmaDamage < game.minPlasmaDamage)
				game.maxPlasmaDamage = game.minPlasmaDamage;
			break;

		case SHOP_PLASMA_MIN_RATE:
			if (game.minPlasmaRate >= game.minPlasmaRateLimit)
			{
				shopSelectedItem = SHOP_ERROR_CANNOT_UPGRADE;
				return;
			}
			game.minPlasmaRate++;
			if (game.maxPlasmaRate < game.minPlasmaRate)
				game.maxPlasmaRate = game.minPlasmaRate;
			break;

		case SHOP_PLASMA_MAX_AMMO:
			if (game.maxPlasmaAmmo >= game.maxPlasmaAmmoLimit)
			{
				shopSelectedItem = SHOP_ERROR_CANNOT_UPGRADE;
				return;
			}
			if (game.difficulty == DIFFICULTY_ORIGINAL)
				game.maxPlasmaAmmo += 10;
			else
				game.maxPlasmaAmmo += 25;
			break;

		case SHOP_ROCKET_MAX_AMMO:
			if ((player.weaponType[1] == W_CHARGER) ||
				(player.weaponType[1] == W_LASER))
			{
				shopSelectedItem = SHOP_ERROR_IS_NOT_ROCKETS;
				return;
			}
			if ((player.weaponType[1] == W_HOMING_MISSILE) &&
				(game.maxRocketAmmo >= MAX_HOMING))
			{
				shopSelectedItem = SHOP_ERROR_WEAPON_CAPACITY;
				return;
			}
			if ((player.weaponType[1] == W_DOUBLE_HOMING_MISSILES) &&
				(game.maxRocketAmmo >= MAX_DOUBLE_HOMING))
			{
				shopSelectedItem = SHOP_ERROR_WEAPON_CAPACITY;
				return;
			}
			if ((player.weaponType[1] == W_MICRO_HOMING_MISSILES) &&
				(game.maxRocketAmmo >= MAX_MICRO_HOMING))
			{
				shopSelectedItem = SHOP_ERROR_WEAPON_CAPACITY;
				return;
			}
			if (game.maxRocketAmmo >= game.maxRocketAmmoLimit)
			{
				shopSelectedItem = SHOP_ERROR_WEAPON_CAPACITY;
				return;
			}
			game.maxRocketAmmo += 5;
			break;

		case SHOP_DOUBLE_ROCKETS:
			if (player.weaponType[1] == W_DOUBLE_ROCKETS)
			{
				shopSelectedItem = SHOP_ERROR_ALREADY_OWNED;
				return;
			}
			shop_sellSecondaryWeapon();
			player.weaponType[1] = W_DOUBLE_ROCKETS;
			LIMIT(game.maxRocketAmmo, 5, 50);
			shopSelectedItem = SHOP_NOTHING;
			break;

		case SHOP_MICRO_ROCKETS:
			if (player.weaponType[1] == W_MICRO_ROCKETS)
			{
				shopSelectedItem = SHOP_ERROR_ALREADY_OWNED;
				return;
			}
			shop_sellSecondaryWeapon();
			player.weaponType[1] = W_MICRO_ROCKETS;
			LIMIT(game.maxRocketAmmo, 5, 50);
			shopSelectedItem = SHOP_NOTHING;
			break;

		case SHOP_LASER:
			if (player.weaponType[1] == W_LASER)
			{
				shopSelectedItem = SHOP_ERROR_ALREADY_OWNED;
				return;
			}
			shop_sellSecondaryWeapon();
			player.weaponType[1] = W_LASER;

			if (game.difficulty != DIFFICULTY_ORIGINAL)
			{
				while (game.maxRocketAmmo > 5)
					sell(SHOP_ROCKET_MAX_AMMO);
			}
			while (player.ammo[1] > 0)
				sell(SHOP_ROCKET_AMMO);

			shopSelectedItem = SHOP_NOTHING;
			break;

		case SHOP_HOMING_MISSILE:
			if (player.weaponType[1] == W_HOMING_MISSILE)
			{
				shopSelectedItem = SHOP_ERROR_ALREADY_OWNED;
				return;
			}
			shop_sellSecondaryWeapon();
			player.weaponType[1] = W_HOMING_MISSILE;

			while (game.maxRocketAmmo > MAX_HOMING)
				sell(SHOP_ROCKET_MAX_AMMO);

			LIMIT(game.maxRocketAmmo, 5, MAX_HOMING);
			shopSelectedItem = SHOP_NOTHING;
			break;

		case SHOP_CHARGER:
			if (player.weaponType[1] == W_CHARGER)
			{
				shopSelectedItem = SHOP_ERROR_ALREADY_OWNED;
				return;
			}
			shop_sellSecondaryWeapon();
			player.weaponType[1] = W_CHARGER;

			if (game.difficulty != DIFFICULTY_ORIGINAL)
			{
				while (game.maxRocketAmmo > 5)
					sell(SHOP_ROCKET_MAX_AMMO);
			}
			while (player.ammo[1] > 0)
				sell(SHOP_ROCKET_AMMO);

			shopSelectedItem = SHOP_NOTHING;
			break;

		case SHOP_DOUBLE_HOMING_MISSILES:
			if (player.weaponType[1] == W_DOUBLE_HOMING_MISSILES)
			{
				shopSelectedItem = SHOP_ERROR_ALREADY_OWNED;
				return;
			}
			shop_sellSecondaryWeapon();
			player.weaponType[1] = W_DOUBLE_HOMING_MISSILES;

			while (game.maxRocketAmmo > MAX_DOUBLE_HOMING)
				sell(SHOP_ROCKET_MAX_AMMO);

			LIMIT(game.maxRocketAmmo, 5, MAX_DOUBLE_HOMING);
			shopSelectedItem = SHOP_NOTHING;
			break;

		case SHOP_MICRO_HOMING_MISSILES:
			if (player.weaponType[1] == W_MICRO_HOMING_MISSILES)
			{
				shopSelectedItem = SHOP_ERROR_ALREADY_OWNED;
				return;
			}
			shop_sellSecondaryWeapon();
			player.weaponType[1] = W_MICRO_HOMING_MISSILES;

			while (game.maxRocketAmmo > MAX_MICRO_HOMING)
				sell(SHOP_ROCKET_MAX_AMMO);

			LIMIT(game.maxRocketAmmo, 5, MAX_MICRO_HOMING);
			shopSelectedItem = SHOP_NOTHING;
			break;
	}

	if (!engine.cheatCash)
		game.cash -= shopItems[i].price;
	save(0);
}

static void sell(int i)
{
	switch (i)
	{
		case SHOP_PLASMA_MAX_OUTPUT:
			if (game.maxPlasmaOutput <= ((game.difficulty != DIFFICULTY_ORIGINAL) ? 1 : 2))
			{
				shopSelectedItem = SHOP_ERROR_CANNOT_SELL;
				return;
			}

			while (game.minPlasmaOutput >= game.maxPlasmaOutput)
				sell(SHOP_PLASMA_MIN_OUTPUT);

			game.maxPlasmaOutput--;
			if (weapons[W_PLAYER_WEAPON].ammo[0] <= game.maxPlasmaOutput + 1)
				weapons[W_PLAYER_WEAPON].ammo[0] = MIN(
					weapons[W_PLAYER_WEAPON].ammo[0],
					game.maxPlasmaOutput);
			break;

		case SHOP_PLASMA_MAX_DAMAGE:
			if (game.maxPlasmaDamage <= ((game.difficulty != DIFFICULTY_ORIGINAL) ? 1 : 2))
			{
				shopSelectedItem = SHOP_ERROR_CANNOT_SELL;
				return;
			}

			while (game.minPlasmaDamage >= game.maxPlasmaDamage)
				sell(SHOP_PLASMA_MIN_DAMAGE);

			game.maxPlasmaDamage--;
			if (weapons[W_PLAYER_WEAPON].damage <= game.maxPlasmaDamage + 1)
				weapons[W_PLAYER_WEAPON].damage = MIN(
					weapons[W_PLAYER_WEAPON].damage,
					game.maxPlasmaDamage);
			break;

		case SHOP_PLASMA_MAX_RATE:
			if (game.maxPlasmaRate <= ((game.difficulty != DIFFICULTY_ORIGINAL) ? 1 : 2))
			{
				shopSelectedItem = SHOP_ERROR_CANNOT_SELL;
				return;
			}

			while (game.minPlasmaRate >= game.maxPlasmaRate)
				sell(SHOP_PLASMA_MIN_RATE);

			game.maxPlasmaRate--;
			if (weapons[W_PLAYER_WEAPON].reload[0] >= rate2reload[game.maxPlasmaRate + 1])
				weapons[W_PLAYER_WEAPON].reload[0] = MAX(
					weapons[W_PLAYER_WEAPON].reload[0],
					rate2reload[game.maxPlasmaRate]);
			break;

		case SHOP_PLASMA_MIN_OUTPUT:
			if (game.minPlasmaOutput <= 1)
			{
				shopSelectedItem = SHOP_ERROR_CANNOT_SELL;
				return;
			}
			game.minPlasmaOutput--;
			if (player.ammo[0] <= 0)
				weapons[W_PLAYER_WEAPON].ammo[0] = game.minPlasmaOutput;
			break;

		case SHOP_PLASMA_MIN_DAMAGE:
			if (game.minPlasmaDamage <= 1)
			{
				shopSelectedItem = SHOP_ERROR_CANNOT_SELL;
				return;
			}
			game.minPlasmaDamage--;
			if (player.ammo[0] <= 0)
				weapons[W_PLAYER_WEAPON].damage = game.minPlasmaDamage;
			break;

		case SHOP_PLASMA_MIN_RATE:
			if (game.minPlasmaRate <= 1)
			{
				shopSelectedItem = SHOP_ERROR_CANNOT_SELL;
				return;
			}
			game.minPlasmaRate--;
			if (player.ammo[0] <= 0)
				weapons[W_PLAYER_WEAPON].reload[0] = rate2reload[game.minPlasmaRate];
			break;

		case SHOP_PLASMA_AMMO:
			if (player.ammo[0] <= 0)
			{
				shopSelectedItem = SHOP_ERROR_NOTHING_TO_SELL;
				return;
			}
			if (player.ammo[0] > 10)
				player.ammo[0] -= 10;
			else
			{
				player.ammo[0] = 0;
				weapons[W_PLAYER_WEAPON].ammo[0] = game.minPlasmaOutput;
				weapons[W_PLAYER_WEAPON].damage = game.minPlasmaDamage;
				weapons[W_PLAYER_WEAPON].reload[0] = rate2reload[game.minPlasmaRate];
			}
			break;

		case SHOP_ROCKET_AMMO:
			if (player.ammo[1] <= 0)
			{
				shopSelectedItem = SHOP_ERROR_NOTHING_TO_SELL;
				return;
			}
			player.ammo[1]--;
			break;

		case SHOP_PLASMA_MAX_AMMO:
			if (game.maxPlasmaAmmo <= 100)
			{
				shopSelectedItem = SHOP_ERROR_CANNOT_SELL;
				return;
			}
			if (game.difficulty == DIFFICULTY_ORIGINAL)
				game.maxPlasmaAmmo -= 10;
			else
				game.maxPlasmaAmmo -= 25;

			while (player.ammo[0] > game.maxPlasmaAmmo)
				sell(SHOP_PLASMA_AMMO);

			break;

		case SHOP_ROCKET_MAX_AMMO:
			if (game.maxRocketAmmo <= 5)
			{
				shopSelectedItem = SHOP_ERROR_CANNOT_SELL;
				return;
			}
			game.maxRocketAmmo -= 5;

			while (player.ammo[1] > game.maxRocketAmmo)
				sell(SHOP_ROCKET_AMMO);

			break;

		case SHOP_DOUBLE_ROCKETS:
			if (player.weaponType[1] != W_DOUBLE_ROCKETS)
			{
				shopSelectedItem = SHOP_ERROR_NOTHING_TO_SELL;
				return;
			}
			player.weaponType[1] = (game.difficulty == DIFFICULTY_ORIGINAL ? W_NONE : W_ROCKETS);
			shopSelectedItem = SHOP_NOTHING;
			break;

		case SHOP_MICRO_ROCKETS:
			if (player.weaponType[1] != W_MICRO_ROCKETS)
			{
				shopSelectedItem = SHOP_ERROR_NOTHING_TO_SELL;
				return;
			}
			player.weaponType[1] = (game.difficulty == DIFFICULTY_ORIGINAL ? W_NONE : W_ROCKETS);
			shopSelectedItem = SHOP_NOTHING;
			break;

		case SHOP_LASER:
			if (player.weaponType[1] != W_LASER)
			{
				shopSelectedItem = SHOP_ERROR_NOTHING_TO_SELL;
				return;
			}
			player.weaponType[1] = (game.difficulty == DIFFICULTY_ORIGINAL ? W_NONE : W_ROCKETS);
			player.ammo[1] = 0;
			shopSelectedItem = SHOP_NOTHING;
			break;

		case SHOP_HOMING_MISSILE:
			if (player.weaponType[1] != W_HOMING_MISSILE)
			{
				shopSelectedItem = SHOP_ERROR_NOTHING_TO_SELL;
				return;
			}
			player.weaponType[1] = (game.difficulty == DIFFICULTY_ORIGINAL ? W_NONE : W_ROCKETS);
			shopSelectedItem = SHOP_NOTHING;
			break;

		case SHOP_CHARGER:
			if (player.weaponType[1] != W_CHARGER)
			{
				shopSelectedItem = SHOP_ERROR_NOTHING_TO_SELL;
				return;
			}
			player.weaponType[1] = (game.difficulty == DIFFICULTY_ORIGINAL ? W_NONE : W_ROCKETS);
			player.ammo[1] = 0;
			shopSelectedItem = SHOP_NOTHING;
			break;

		case SHOP_DOUBLE_HOMING_MISSILES:
			if (player.weaponType[1] != W_DOUBLE_HOMING_MISSILES)
			{
				shopSelectedItem = SHOP_ERROR_NOTHING_TO_SELL;
				return;
			}
			player.weaponType[1] = (game.difficulty == DIFFICULTY_ORIGINAL ? W_NONE : W_ROCKETS);
			shopSelectedItem = SHOP_NOTHING;
			break;

		case SHOP_MICRO_HOMING_MISSILES:
			if (player.weaponType[1] != W_MICRO_HOMING_MISSILES)
			{
				shopSelectedItem = SHOP_ERROR_NOTHING_TO_SELL;
				return;
			}
			player.weaponType[1] = (game.difficulty == DIFFICULTY_ORIGINAL ? W_NONE : W_ROCKETS);
			shopSelectedItem = SHOP_NOTHING;
			break;
	}

	adjustShopPrices();

	if (game.difficulty == DIFFICULTY_ORIGINAL)
		game.cash += shopItems[i].price / 2;
	else
		game.cash += shopItems[i].price;

	save(0);
}

void shop_show()
{
	int icons = SHOP_MAX;
	int sell_x = SHOP_X + SHOP_WIDTH - gfx_sprites[SP_SELL]->w - 2;
	int sell_y = SHOP_Y + 183;
	int buy_x = sell_x - gfx_sprites[SP_BUY]->w - 5;
	int buy_y = SHOP_Y + 183;

	drawShop();

	screen_blit(gfx_shopSprites[SHOP_S_SHIP_INFO], SHOP_X, SHOP_Y);
	screen_blit(gfx_shopSprites[SHOP_S_CATALOG], SHOP_X, SHOP_Y + 50);
	screen_blit(gfx_shopSprites[SHOP_S_ITEM_INFO], SHOP_X, SHOP_Y + 180);
	screen_blit(gfx_shopSprites[SHOP_S_PRIMARY], SHOP_X, SHOP_Y + 245);
	screen_blit(gfx_shopSprites[SHOP_S_POWERUP], SHOP_X + 203, SHOP_Y + 245);
	screen_blit(gfx_shopSprites[SHOP_S_SECONDARY], SHOP_X + 406, SHOP_Y + 245);

	if (shopSelectedItem > SHOP_NOTHING)
	{
		screen_blit(gfx_sprites[SP_BUY], buy_x, buy_y);
		screen_blit(gfx_sprites[SP_SELL], sell_x, sell_y);
	}

	screen_blit(gfx_sprites[SP_FIREFLY], SHOP_X + 280, SHOP_Y + 15);

	if (game.system == 0)
		icons = SHOP_DOUBLE_ROCKETS + 1;
	else if (game.system == 1)
		icons = SHOP_LASER + 1;
	else if (game.system == 2)
		icons = SHOP_CHARGER + 1;

	if ((engine.keyState[KEY_FIRE]))
	{
		for (int i = 0 ; i < icons ; i++)
		{
			if (game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6,
				shopItems[i].x, shopItems[i].y, 32, 25))
			{
				shopSelectedItem = i;
				engine.keyState[KEY_FIRE] = 0;
				drawShop();
			}
		}

		if (shopSelectedItem > SHOP_NOTHING)
		{
			if (game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, buy_x, buy_y, 24, 16))
			{
				buy(shopSelectedItem);
				engine.keyState[KEY_FIRE] = 0;
				drawShop();
			}

			if (game_collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, sell_x, sell_y, 24, 16))
			{
				sell(shopSelectedItem);
				engine.keyState[KEY_FIRE] = 0;
				drawShop();
			}
		}
	}
}
