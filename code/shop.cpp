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

#include "shop.h"

void drawSecondaryWeaponSurface()
{
	char description[50];
	strcpy(description, "");

	graphics.drawString("Secondary Weapon", 10, 3, FONT_WHITE, graphics.shopSurface[2]);

	switch (player.weaponType[1])
	{
		case W_NONE:
			strcpy(description, "Type : None");
			break;
		case W_ROCKETS:
			strcpy(description, "Type : Rockets");
			break;
		case W_DOUBLE_ROCKETS:
			strcpy(description, "Type : Dbl Rockets");
			break;
		case W_MICRO_ROCKETS:
			strcpy(description, "Type : Micro Rockets");
			break;
		case W_LASER:
			strcpy(description, "Type : Laser");
			break;
		case W_CHARGER:
			strcpy(description, "Type : Charger");
			break;
		case W_HOMING_MISSILE:
			strcpy(description, "Type : Homing Missile");
			break;
		case W_DOUBLE_HOMING_MISSILES:
			strcpy(description, "Type : Dbl Homing Missiles");
			break;
		case W_MICRO_HOMING_MISSILES:
			strcpy(description, "Type : Mcr Homing Missiles");
			break;
	}
	graphics.drawString(description, 10, 22, FONT_WHITE, graphics.shopSurface[2]);

	if ((player.weaponType[1] != W_LASER) && (player.weaponType[1] != W_CHARGER) && (player.weaponType[1] != W_NONE))
	{
		sprintf(description, "Capacity : %d", currentGame.maxRocketAmmo);
		graphics.drawString(description, 10, 37, FONT_WHITE, graphics.shopSurface[2]);
	}
}

void adjustShopPrices()
{
	shopItems[0].price = (500 * currentGame.maxPlasmaOutput);
	shopItems[1].price = (500 * currentGame.maxPlasmaDamage);
	shopItems[2].price = (500 * (16 - currentGame.maxPlasmaRate));

	shopItems[5].price = (2000 * weapon[0].ammo[0]);
	shopItems[6].price = (2000 * weapon[0].damage);
	shopItems[7].price = (2000 * (16 - weapon[0].reload[0]));

	shopItems[8].price = (5 * currentGame.maxPlasmaAmmo);
	shopItems[9].price = (25 * currentGame.maxRocketAmmo);
	
	if (currentGame.maxPlasmaOutput == 5)
		shopItems[0].price = 0;

	if (currentGame.maxPlasmaDamage == 5)
		shopItems[1].price = 0;

	if (currentGame.maxPlasmaRate == 7)
		shopItems[2].price = 0;
		
	if (weapon[0].ammo[0] == 3)
		shopItems[5].price = 0;

	if (weapon[0].damage == 3)
		shopItems[6].price = 0;

	if (weapon[0].reload[0] == 11)
		shopItems[7].price = 0;
}

void drawShop()
{
	adjustShopPrices();

	for (int i = 0 ; i < MAX_SHOPSHAPES ; i++)
	{
		if (graphics.shopSurface[i] != NULL)
		{
			SDL_FreeSurface(graphics.shopSurface[i]);
		}
	}

	for (int i = 0 ; i < 3 ; i++)
		graphics.shopSurface[i] = graphics.createSurface(246, 91);

	for (int i = 0 ; i < 3 ; i++)
	{
		graphics.blevelRect(graphics.shopSurface[i], 0, 0, 245, 90, 0x00, 0x00, 0x55);
		graphics.blevelRect(graphics.shopSurface[i], 0, 0, 245, 20, 0x00, 0x00, 0x99);
	}

	graphics.shopSurface[4] = graphics.alphaRect(601, 101, 0x00, 0x00, 0x00);
	graphics.blevelRect(graphics.shopSurface[4], 0, 0, 600, 100, 0x00, 0x00, 0x33);

	switch (shopSelectedItem)
	{
		case -1:
		case -2:
		case -3:
		case -4:
		case -5:
		case -6:
			break;
		case 0:
		case 1:
		case 2:
		case 8:
			graphics.blevelRect(graphics.shopSurface[1], 0, 0, 245, 90, 0x55, 0x00, 0x00);
			graphics.blevelRect(graphics.shopSurface[1], 0, 0, 245, 20, 0x99, 0x00, 0x00);
			break;
		case 3:
		case 4:
			graphics.blevelRect(graphics.shopSurface[4], 0, 0, 600, 100, 0x33, 0x00, 0x00);
			break;
		case 5:
		case 6:
		case 7:
			graphics.blevelRect(graphics.shopSurface[0], 0, 0, 245, 90, 0x55, 0x00, 0x00);
			graphics.blevelRect(graphics.shopSurface[0], 0, 0, 245, 20, 0x99, 0x00, 0x00);
			break;
		default:
			graphics.blevelRect(graphics.shopSurface[2], 0, 0, 245, 90, 0x55, 0x00, 0x00);
			graphics.blevelRect(graphics.shopSurface[2], 0, 0, 245, 20, 0x99, 0x00, 0x00);
			break;
	}

	char description[100];
	strcpy(description, "");

	graphics.drawString("Primary Weapon", 10, 3, FONT_WHITE, graphics.shopSurface[0]);
	sprintf(description, "Plasma Cannons : %d", weapon[0].ammo[0]);
	graphics.drawString(description, 10, 22, FONT_WHITE, graphics.shopSurface[0]);
	sprintf(description, "Plasma Power   : Stage %d", weapon[0].damage);
	graphics.drawString(description, 10, 37, FONT_WHITE, graphics.shopSurface[0]);
	sprintf(description, "Cooler         : Stage %d", ((15 - weapon[0].reload[0]) / 2) + 1);
	graphics.drawString(description, 10, 52, FONT_WHITE, graphics.shopSurface[0]);

	graphics.drawString("Powerup Weapon", 10, 3, FONT_WHITE, graphics.shopSurface[1]);
	sprintf(description, "Plasma Output    : Stage %d", currentGame.maxPlasmaOutput);
	graphics.drawString(description, 10, 22, FONT_WHITE, graphics.shopSurface[1]);
	sprintf(description, "Plasma Condensor : Stage %d", currentGame.maxPlasmaDamage);
	graphics.drawString(description, 10, 37, FONT_WHITE, graphics.shopSurface[1]);
	sprintf(description, "Liquid Nitrogen  : Stage %d", ((15 - currentGame.maxPlasmaRate) / 2) + 1);
	graphics.drawString(description, 10, 52, FONT_WHITE, graphics.shopSurface[1]);
	sprintf(description, "Plasma Capacity  : %d", currentGame.maxPlasmaAmmo);
	graphics.drawString(description, 10, 67, FONT_WHITE, graphics.shopSurface[1]);

	drawSecondaryWeaponSurface();

	graphics.shopSurface[3] = graphics.createSurface(601, 121);

	graphics.blevelRect(graphics.shopSurface[3], 0, 0, 600, 120, 0x00, 0x00, 0x22);

	graphics.drawString("Temporary Weapons", 10, 2, FONT_WHITE, graphics.shopSurface[3]);
	graphics.drawString("Ammo and Storage", 260, 2, FONT_WHITE, graphics.shopSurface[3]);

	graphics.drawString("Primary Weapons", 10, 62, FONT_WHITE, graphics.shopSurface[3]);

	graphics.drawString("Secondary Weapons", 260, 62, FONT_WHITE, graphics.shopSurface[3]);

	signed char icons = MAX_SHOPITEMS;

	if (currentGame.system == 0)
		icons = 11;
	else if (currentGame.system == 1)
		icons = 13;
	else if (currentGame.system == 2)
		icons = 15;

	for (int i = 0 ; i < icons ; i++)
		graphics.blit(graphics.shape[shopItems[i].image], shopItems[i].x - 90, shopItems[i].y - 178, graphics.shopSurface[3]);

	sprintf(description, "Shield Units : %d", currentGame.shieldUnits * 25);
	graphics.drawString(description, 10, 4, FONT_WHITE, graphics.shopSurface[4]);
	sprintf(description, "Cash : $%d", currentGame.cash);
	graphics.drawString(description, 10, 80, FONT_WHITE, graphics.shopSurface[4]);
	sprintf(description, "Plasma Cells : %.3d", player.ammo[0]);
	graphics.drawString(description, 430, 4, FONT_WHITE, graphics.shopSurface[4]);
	sprintf(description, "Rockets : %.3d", player.ammo[1]);
	graphics.drawString(description, 475, 80, FONT_WHITE, graphics.shopSurface[4]);

	graphics.shopSurface[5] = graphics.createSurface(601, 56);
	graphics.blevelRect(graphics.shopSurface[5], 0, 0, 600, 35, 0x00, 0x99, 0x00);
	graphics.blevelRect(graphics.shopSurface[5], 0, 20, 600, 35, 0x00, 0x33, 0x00);
	graphics.drawString("Information", 5, 4, FONT_WHITE, graphics.shopSurface[5]);

	switch (shopSelectedItem)
	{
		case -1:
			break;
		case -2:
			graphics.drawString("You don't have enough money", 20, 30, FONT_WHITE, graphics.shopSurface[5]);
			break;
		case -3:
			graphics.drawString("Cannot upgrade ship", 5, 22, FONT_WHITE, graphics.shopSurface[5]);
			graphics.drawString("Hardware capacity has been reached", 20, 38, FONT_CYAN, graphics.shopSurface[5]);
			break;
		case -4:
			graphics.drawString("Ammunition limit reached", 20, 30, FONT_WHITE, graphics.shopSurface[5]);
			break;
		case -5:
			graphics.drawString("You cannot sell that item", 20, 30, FONT_WHITE, graphics.shopSurface[5]);
			break;
		case -6:
			graphics.drawString("Nothing to sell", 20, 30, FONT_WHITE, graphics.shopSurface[5]);
			break;
		case -7:
			graphics.drawString("Rockets cannot be bought for Laser or Charger Cannon", 5, 30, FONT_WHITE, graphics.shopSurface[5]);
			break;
		case -8:
			graphics.drawString("You already have that weapon", 20, 30, FONT_WHITE, graphics.shopSurface[5]);
			break;
		case -9:
			graphics.drawString("This weapon's ammo limit has been reached", 20, 30, FONT_WHITE, graphics.shopSurface[5]);
			break;
		default:
			if (shopItems[shopSelectedItem].price != 0)
			{
				sprintf(description, "%s ($%d)", shopItems[shopSelectedItem].description, shopItems[shopSelectedItem].price);
			}
			else
			{
				sprintf(description, "%s (N/A)", shopItems[shopSelectedItem].description);
			}
			graphics.drawString(shopItems[shopSelectedItem].name, 5, 22, FONT_WHITE, graphics.shopSurface[5]);
			graphics.drawString(description, 20, 38, FONT_CYAN, graphics.shopSurface[5]);
			break;
	}
}

#if USEPACK

void loadShop()
{
	char name[255], description[255];
	int price, image, x, y;

	FILE *fp;

	#if USEPACK
	int dataLocation = locateDataInPak("data/shop.dat", 1);
	fp = fopen(PACKLOCATION, "rb");
	fseek(fp, dataLocation, SEEK_SET);
	#else
	fp = fopen("data/shop.dat", "rb");
	#endif

	for (int i = 0 ; i < MAX_SHOPITEMS ; i++)
	{
		fscanf(fp, "%[^\n]%*c", name);
		fscanf(fp, "%[^\n]%*c", description);
		fscanf(fp, "%d", &price);
		fscanf(fp, "%d", &image);
		fscanf(fp, "%d", &x);
		fscanf(fp, "%d%*c", &y);

		strcpy(shopItems[i].name, name);
		strcpy(shopItems[i].description, description);
		shopItems[i].price = price;
		shopItems[i].image = image;
		shopItems[i].x = x;
		shopItems[i].y = y;
	}

	fclose(fp);

	shopSelectedItem = -1;

	player.image[0] = graphics.shape[0];
	player.x = 380;
	player.y = 95;

	drawShop();
}

void initShop(){loadShop();}

#else

void saveShop()
{
	FILE *fp;

	fp = fopen("data/shop.dat", "wb");
	if (fp == NULL)
	{
		printf("Unable to write Shop Data File\n");
		exit(1);
	}

	for (int i = 0 ; i < MAX_SHOPITEMS ; i++)
	{
		fprintf(fp, "%s\n", shopItems[i].name);
		fprintf(fp, "%s\n", shopItems[i].description);
		fprintf(fp, "%d ", shopItems[i].price);
		fprintf(fp, "%d ", shopItems[i].image);
		fprintf(fp, "%d ", shopItems[i].x);
		fprintf(fp, "%d\n", shopItems[i].y);
	}

	// Put an extra line for the PAK file "just in case"
	fprintf(fp, "\n");

	fclose(fp);
}

/*
Throw into a data file in final build
*/
void initShop()
{
	/* ----------- Temporary Items ----------- */

	shopItems[0].price = 1000;
	strcpy(shopItems[0].name, "Plasma channel splitter");
	strcpy(shopItems[0].description, "Improves poweredup plasma output");
	shopItems[0].image = 9;

	shopItems[1].price = 1000;
	strcpy(shopItems[1].name, "Plasma capacity condensor");
	strcpy(shopItems[1].description, "Increases poweredup plasma damage");
	shopItems[1].image = 10;

	shopItems[2].price = 1000;
	strcpy(shopItems[2].name, "Liquid nitrogen capsules");
	strcpy(shopItems[2].description, "Increases plasma firing rate");
	shopItems[2].image = 11;

	shopItems[3].price = 50;
	strcpy(shopItems[3].name, "10 Plasma cells");
	strcpy(shopItems[3].description, "Plasma ammunition");
	shopItems[3].image = 12;

	shopItems[4].price = 50;
	strcpy(shopItems[4].name, "Rocket Ammo");
	strcpy(shopItems[4].description, "High velocity dumb fire rocket");
	shopItems[4].image = 13;

	/* ----------- Permanent Items ----------- */

	shopItems[5].price = 2000;
	strcpy(shopItems[5].name, "Additional Plasma Cannon");
	strcpy(shopItems[5].description, "Adds an extra plasma cannon to the Firefly");
	shopItems[5].image = 14;

	shopItems[6].price = 2000;
	strcpy(shopItems[6].name, "Plasma Power Booster");
	strcpy(shopItems[6].description, "Increases power of plasma shots");
	shopItems[6].image = 15;

	shopItems[7].price = 2000;
	strcpy(shopItems[7].name, "Plasma Cooling Booster");
	strcpy(shopItems[7].description, "Permanently increases firing rate");
	shopItems[7].image = 16;

	/* ----------- Ammo Items -------------- */

	shopItems[8].price = 250;
	strcpy(shopItems[8].name, "Plasma compressor");
	strcpy(shopItems[8].description, "Increases plasma ammo capacity");
	shopItems[8].image = 17;

	shopItems[9].price = 250;
	strcpy(shopItems[9].name, "Rocket Pod");
	strcpy(shopItems[9].description, "Allows for an additional 5 rockets to be carried");
	shopItems[9].image = 18;

	/* ---------- Weaponary --------------- */

	shopItems[10].price = 2000;
	strcpy(shopItems[10].name, "Dual Rocket Launcher");
	strcpy(shopItems[10].description, "Allows for two rockets to be fired at once");
	shopItems[10].image = 19;

	shopItems[11].price = 2500;
	strcpy(shopItems[11].name, "Micro Rocket Launcher");
	strcpy(shopItems[11].description, "Launches several less powerful rockets at once");
	shopItems[11].image = 20;

	shopItems[12].price = 5000;
	strcpy(shopItems[12].name, "Laser Cannon");
	strcpy(shopItems[12].description, "Laser Cannon");
	shopItems[12].image = 21;

	shopItems[13].price = 7500;
	strcpy(shopItems[13].name, "Homing Missile Launcher");
	strcpy(shopItems[13].description, "Fires homing missile");
	shopItems[13].image = 22;

	shopItems[14].price = 10000;
	strcpy(shopItems[14].name, "Charge Cannon");
	strcpy(shopItems[14].description, "A charge up cannon");
	shopItems[14].image = 23;

	shopItems[15].price = 20000;
	strcpy(shopItems[15].name, "Dual Homing Missile Launcher");
	strcpy(shopItems[15].description, "Fires two homing missiles");
	shopItems[15].image = 24;

	shopItems[16].price = 25000;
	strcpy(shopItems[16].name, "Homing Micro Missile Launcher");
	strcpy(shopItems[16].description, "Fires several small homing missiles");
	shopItems[16].image = 25;

	for (int i = 0 ; i < 3 ; i++)
	{
		shopItems[i].x = 100 + (i * 50);
		shopItems[i].y = 200;
	}

	shopItems[3].x = 350;
	shopItems[3].y = 200;

	shopItems[4].x = 400;
	shopItems[4].y = 200;

	for (int i = 0 ; i < 3 ; i++)
	{
		shopItems[i + 5].x = 100 + (i * 50);
		shopItems[i + 5].y = 260;
	}

	for (int i = 0 ; i < 2 ; i++)
	{
		shopItems[i + 8].x = 450 + (i * 50);
		shopItems[i + 8].y = 200;
	}

	for (int i = 0 ; i < 8 ; i++)
	{
		shopItems[i + 10].x = 350 + (i * 50);
		shopItems[i + 10].y = 260;
	}

 	shopSelectedItem = -1;

	player.image[0] = graphics.shape[0];
	player.x = 380;
	player.y = 95;

	saveShop();

	drawShop();
}

#endif

void buy(int i)
{
	if ((currentGame.cash < shopItems[i].price) && (!engine.cheatCash))
	{
		shopSelectedItem = -2;
		drawShop();
		return;
	}

	switch(i)
	{
		case 0:
			if (currentGame.maxPlasmaOutput == 5)
				{shopSelectedItem = -3; return;}
			currentGame.maxPlasmaOutput++;
			break;
		case 1:
			if (currentGame.maxPlasmaDamage == 5)
				{shopSelectedItem = -3; return;}
			currentGame.maxPlasmaDamage++;
			break;
		case 2:
			if (currentGame.maxPlasmaRate == 7)
				{shopSelectedItem = -3; return;}
			currentGame.maxPlasmaRate -= 2;
			break;
		case 3:
			if (player.ammo[0] == currentGame.maxPlasmaAmmo)
				{shopSelectedItem = -4; return;}
			Math::limitCharAdd(&player.ammo[0], 10, 0, currentGame.maxPlasmaAmmo);
			break;
		case 4:
			if ((player.weaponType[1] == W_CHARGER) || (player.weaponType[1] == W_LASER))
				{shopSelectedItem = -7; return;}
			if (player.ammo[1] == currentGame.maxRocketAmmo)
				{shopSelectedItem = -4; return;}
			if ((player.weaponType[1] == W_HOMING_MISSILE) && (player.ammo[1] == 20))
				{shopSelectedItem = -9; return;}
			if ((player.weaponType[1] == W_DOUBLE_HOMING_MISSILES) && (player.ammo[1] == 10))
				{shopSelectedItem = -9; return;}
			if ((player.weaponType[1] == W_MICRO_HOMING_MISSILES) && (player.ammo[1] == 10))
				{shopSelectedItem = -9; return;}
			player.ammo[1]++;
			break;
		case 5:
			if (weapon[0].ammo[0] == 3)
				{shopSelectedItem = -3; return;}
			weapon[0].ammo[0]++;
			if (currentGame.maxPlasmaOutput < weapon[0].ammo[0])
				currentGame.maxPlasmaOutput = weapon[0].ammo[0];
			break;
		case 6:
			if (weapon[0].damage == 3)
				{shopSelectedItem = -3; return;}
			weapon[0].damage++;
			if (currentGame.maxPlasmaDamage < weapon[0].damage)
				currentGame.maxPlasmaDamage = weapon[0].damage;
			break;
		case 7:
			if (weapon[0].reload[0] == 11)
				{shopSelectedItem = -3; return;}
			weapon[0].reload[0] -= 2;
			if (currentGame.maxPlasmaRate > weapon[0].reload[0])
				currentGame.maxPlasmaRate = weapon[0].reload[0];
			break;
		case 8:
			if (currentGame.maxPlasmaAmmo == 250)
				{shopSelectedItem = -3; return;}
			Math::limitCharAdd(&currentGame.maxPlasmaAmmo, 10, 0, 250);
			break;
		case 9:
			if ((player.weaponType[1] == W_CHARGER) || (player.weaponType[1] == W_LASER))
				{shopSelectedItem = -7; return;}
			if (currentGame.maxRocketAmmo == 50)
				{shopSelectedItem = -3; return;}
			currentGame.maxRocketAmmo += 5;
			break;
		case 10:
			if (player.weaponType[1] == W_DOUBLE_ROCKETS)
				{shopSelectedItem = -8; return;}
			player.weaponType[1] = W_DOUBLE_ROCKETS;
			shopSelectedItem = -1;
			break;
		case 11:
			if (player.weaponType[1] == W_MICRO_ROCKETS)
				{shopSelectedItem = -8; return;}
			player.weaponType[1] = W_MICRO_ROCKETS;
			shopSelectedItem = -1;
			break;
		case 12:
			if (player.weaponType[1] == W_LASER)
				{shopSelectedItem = -8; return;}
			player.weaponType[1] = W_LASER;
			player.ammo[1] = 0;
			shopSelectedItem = -1;
			break;
		case 13:
			if (player.weaponType[1] == W_HOMING_MISSILE)
				{shopSelectedItem = -8; return;}
			player.weaponType[1] = W_HOMING_MISSILE;
			Math::limitChar(&player.ammo[1], 0, 20);
			shopSelectedItem = -1;
			break;
		case 14:
			if (player.weaponType[1] == W_CHARGER)
				{shopSelectedItem = -8; return;}
			player.weaponType[1] = W_CHARGER;
			player.ammo[1] = 0;
			shopSelectedItem = -1;
			break;
		case 15:
			if (player.weaponType[1] == W_DOUBLE_HOMING_MISSILES)
				{shopSelectedItem = -8; return;}
			player.weaponType[1] = W_DOUBLE_HOMING_MISSILES;
			Math::limitChar(&player.ammo[1], 0, 10);
			shopSelectedItem = -1;
			break;
		case 16:
			if (player.weaponType[1] == W_MICRO_HOMING_MISSILES)
				{shopSelectedItem = -8; return;}
			player.weaponType[1] = W_MICRO_HOMING_MISSILES;
			Math::limitChar(&player.ammo[1], 0, 10);
			shopSelectedItem = -1;
			break;
	}

	if (!engine.cheatCash)
		currentGame.cash -= shopItems[i].price;
}

void sell(int i)
{
	switch (i)
	{
		case 0:
			if (currentGame.maxPlasmaOutput == 2)
				{shopSelectedItem = -5; return;}
			currentGame.maxPlasmaOutput--;
			break;
		case 1:
			if (currentGame.maxPlasmaDamage == 2)
				{shopSelectedItem = -5; return;}
			currentGame.maxPlasmaDamage--;
			break;
		case 2:
			if (currentGame.maxPlasmaRate == 13)
				{shopSelectedItem = -5; return;}
			currentGame.maxPlasmaRate += 2;
			break;
		case 3:
			if (player.ammo[0] == 0)
				{shopSelectedItem = -6; return;}
			if (player.ammo[0] > 9)
				Math::limitCharAdd(&player.ammo[0], -10, 0, currentGame.maxPlasmaAmmo);
			else
				player.ammo[0] = 0;
			break;
		case 4:
			if (player.ammo[1] == 0)
				{shopSelectedItem = -6; return;}
			player.ammo[1]--;
			break;
		case 5:
			if (weapon[0].ammo[0] == 1)
				{shopSelectedItem = -5; return;}
			weapon[0].ammo[0]--;
			break;
		case 6:
			if (weapon[0].damage == 1)
				{shopSelectedItem = -5; return;}
			weapon[0].damage--;
			break;
		case 7:
			if (weapon[0].reload[0] == 15)
				{shopSelectedItem = -5; return;}
			weapon[0].reload[0] += 2;
			break;
		case 8:
			if (currentGame.maxPlasmaAmmo == 100)
				{shopSelectedItem = -1; return;}
			currentGame.maxPlasmaAmmo -= 10;
			Math::limitChar(&player.ammo[0], 0, currentGame.maxPlasmaAmmo);
			break;
		case 9:
			if (currentGame.maxRocketAmmo == 0)
				{shopSelectedItem = -1; return;}
			currentGame.maxRocketAmmo -= 5;
			Math::limitChar(&player.ammo[1], 0, currentGame.maxRocketAmmo);
			break;
		case 10:
			if (player.weaponType[1] != W_DOUBLE_ROCKETS)
				{shopSelectedItem = -1; return;}
			player.weaponType[1] = W_NONE;
			shopSelectedItem = -1;
			break;
		case 11:
			if (player.weaponType[1] != W_MICRO_ROCKETS)
				{shopSelectedItem = -1; return;}
			player.weaponType[1] = W_NONE;
			shopSelectedItem = -1;
			break;
		case 12:
			if (player.weaponType[1] != W_LASER)
				{shopSelectedItem = -1; return;}
			player.weaponType[1] = W_NONE;
			player.ammo[1] = 0;
			shopSelectedItem = -1;
			break;
		case 13:
			if (player.weaponType[1] != W_HOMING_MISSILE)
				{shopSelectedItem = -1; return;}
			player.weaponType[1] = W_NONE;
			shopSelectedItem = -1;
			break;
		case 14:
			if (player.weaponType[1] != W_CHARGER)
				{shopSelectedItem = -1; return;}
			player.weaponType[1] = W_NONE;
			player.ammo[1] = 0;
			shopSelectedItem = -1;
			break;
		case 15:
			if (player.weaponType[1] != W_DOUBLE_HOMING_MISSILES)
				{shopSelectedItem = -1; return;}
			player.weaponType[1] = W_NONE;
			shopSelectedItem = -1;
			break;
		case 16:
			if (player.weaponType[1] != W_MICRO_HOMING_MISSILES)
				{shopSelectedItem = -1; return;}
			player.weaponType[1] = W_NONE;
			shopSelectedItem = -1;
			break;
	}

	adjustShopPrices();
	currentGame.cash += (shopItems[i].price / 2);
}

void showShop()
{
 	graphics.blit(graphics.shopSurface[0], 20, 395);
	graphics.blit(graphics.shopSurface[1], 275, 395);
	graphics.blit(graphics.shopSurface[2], 530, 395);
	graphics.blit(graphics.shopSurface[3], 100, 180);
	graphics.blit(graphics.shopSurface[4], 100, 50);
	graphics.blit(graphics.shopSurface[5], 100, 320);

	if (shopSelectedItem > -1)
	{
		graphics.blit(graphics.shape[27], 60, 350);
		graphics.blit(graphics.shape[28], 710, 350);
	}

	graphics.blit(graphics.shape[29], (int)player.x, (int)player.y);

	signed char icons = MAX_SHOPITEMS;

	if (currentGame.system == 0)
		icons = 11;
	else if (currentGame.system == 1)
		icons = 13;
	else if (currentGame.system == 2)
		icons = 15;

	if ((engine.keyState[SDLK_LCTRL]) || (engine.keyState[SDLK_RCTRL]))
	{
		for (int i = 0 ; i < icons ; i++)
		{
			if (Collision::collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, shopItems[i].x, shopItems[i].y, 32, 25))
			{
				shopSelectedItem = i;
				engine.keyState[SDLK_LCTRL] = engine.keyState[SDLK_RCTRL] = 0;
				drawShop();
			}
		}

		if (shopSelectedItem > -1)
		{
			if (Collision::collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, 60, 350, 24, 16))
			{
				buy(shopSelectedItem);
				engine.keyState[SDLK_LCTRL] = engine.keyState[SDLK_RCTRL] = 0;
				drawShop();
			}

			if (Collision::collision(engine.cursor_x + 13, engine.cursor_y + 13, 6, 6, 700, 350, 24, 16))
			{
				sell(shopSelectedItem);
				engine.keyState[SDLK_LCTRL] = engine.keyState[SDLK_RCTRL] = 0;
				drawShop();
			}
		}
	}
}

