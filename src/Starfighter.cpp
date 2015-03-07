/*
Project: Starfighter
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

int main(int argc, char **argv)
{
	bool cheatAttempt;
	int cheatCount;
	int section;

	if (chdir(DATADIR) == -1)
		printf("Warning: failed to change directory to \"%s\"\n", DATADIR);

	defineGlobals(); // Must do this first!

	cheatAttempt = false;
	cheatCount = 0;

	if (argc > 1)
	{
		if (strcmp("--help", argv[1]) == 0)
		{
			printf("\nProject: Starfighter %s\n", VERSION);
			printf("Copyright Parallel Realities 2003\n");
			printf("Copyright Guus Sliepen, Astrid S. de Wijn and others 2012\n");
			printf("Additional Commands\n");
			printf("\t-noaudio     Disables sound and music\n");
			printf("\t-mono        Mono sound output (best for headphones)\n\n");
			printf("http://sourceforge.net/projects/pr-starfighter/\n");
			printf("\n");
			exit(0);
		}
	}

	for (int i = 1 ; i < argc ; i++)
	{
		if (strcmp(argv[i], "-nomove") == 0)
		{
			printf("Enemy movement disabled\n");
			dev.moveAliens = 0;
		}
		if (strcmp(argv[i], "-nofire") == 0)
		{
			printf("Enemy firing disabled\n");
			dev.fireAliens = 0;
		}
		if (strcmp(argv[i], "-cheat") == 0)
			cheatAttempt = true;
		if (strcmp(argv[i], "-noaudio") == 0)
		{
			printf("No Audio\n");
			engine.useAudio = false;
		}
		if (strcmp(argv[i], "-mono") == 0)
		{
			printf("Mono sound output\n");
			engine.useAudio = true;
		}
		if ((strcmp(argv[i], "humans") == 0) && (cheatCount == 0))
			cheatCount = 1;
		if ((strcmp(argv[i], "do") == 0) && (cheatCount == 1))
			cheatCount = 2;
		if ((strcmp(argv[i], "it") == 0) && (cheatCount == 2))
			cheatCount = 3;
		if (((strcmp(argv[i], "better") == 0) && (cheatCount == 3)) ||
			(strcmp(argv[i], "humansdoitbetter") == 0))
		{
			printf("Humans do it better! Cheats enabled.\n");
			engine.cheat = true;
		}
	}

	atexit(cleanUp);

	initGraphics();
	initSystem(); // Opens video mode and sound
	loadFont();

	if (cheatAttempt && !engine.cheat)
	{
		clearScreen(black);
		drawString("That doesn't work anymore", -1, 285, FONT_WHITE);
		drawString("Try harder...", -1, 315, FONT_WHITE);
		updateScreen();
		SDL_Delay(2000);
		clearScreen(black);
		updateScreen();
		SDL_Delay(500);
	}

	freeGraphics();
	loadSound();

	initWeapons();

	initVars();

	// Dual Plasma Fighter.
	alien_defs[CD_DUALFIGHTER].classDef = CD_DUALFIGHTER;
	alien_defs[CD_DUALFIGHTER].AIType = AI_NORMAL;
	alien_defs[CD_DUALFIGHTER].speed = 4;
	alien_defs[CD_DUALFIGHTER].maxShield = 5;
	alien_defs[CD_DUALFIGHTER].shield = 5;
	alien_defs[CD_DUALFIGHTER].imageIndex[0] = 2;
	alien_defs[CD_DUALFIGHTER].imageIndex[1] = 3;
	alien_defs[CD_DUALFIGHTER].weaponType[0] = W_DOUBLE_SHOT;
	alien_defs[CD_DUALFIGHTER].weaponType[1] = W_ROCKETS;
	alien_defs[CD_DUALFIGHTER].chance[0] = 100;
	alien_defs[CD_DUALFIGHTER].chance[1] = 1;
	alien_defs[CD_DUALFIGHTER].score = 25;
	alien_defs[CD_DUALFIGHTER].collectChance = 50;
	alien_defs[CD_DUALFIGHTER].collectType = P_ANYTHING;
	alien_defs[CD_DUALFIGHTER].collectValue = 50;
	alien_defs[CD_DUALFIGHTER].flags = FL_WEAPCO;

	// Missile Boat
	alien_defs[CD_MISSILEBOAT].classDef = CD_MISSILEBOAT;
	alien_defs[CD_MISSILEBOAT].AIType = AI_DEFENSIVE;
	alien_defs[CD_MISSILEBOAT].speed = 2;
	alien_defs[CD_MISSILEBOAT].maxShield = 50;
	alien_defs[CD_MISSILEBOAT].shield = 50;
	alien_defs[CD_MISSILEBOAT].imageIndex[0] = 4;
	alien_defs[CD_MISSILEBOAT].imageIndex[1] = 5;
	alien_defs[CD_MISSILEBOAT].weaponType[0] = W_ROCKETS;
	alien_defs[CD_MISSILEBOAT].weaponType[1] = W_DOUBLE_ROCKETS;
	alien_defs[CD_MISSILEBOAT].chance[0] = 25;
	alien_defs[CD_MISSILEBOAT].chance[1] = 4;
	alien_defs[CD_MISSILEBOAT].score = 250;
	alien_defs[CD_MISSILEBOAT].collectChance = 25;
	alien_defs[CD_MISSILEBOAT].collectType = P_ANYTHING;
	alien_defs[CD_MISSILEBOAT].collectValue = 75;
	alien_defs[CD_MISSILEBOAT].flags = FL_WEAPCO;

	//Prototype fighter
	alien_defs[CD_PROTOFIGHTER].classDef = CD_PROTOFIGHTER;
	alien_defs[CD_PROTOFIGHTER].AIType = AI_DEFENSIVE;
	alien_defs[CD_PROTOFIGHTER].speed = 5;
	alien_defs[CD_PROTOFIGHTER].maxShield = 15;
	alien_defs[CD_PROTOFIGHTER].shield = 15;
	alien_defs[CD_PROTOFIGHTER].imageIndex[0] = 6;
	alien_defs[CD_PROTOFIGHTER].imageIndex[1] = 7;
	alien_defs[CD_PROTOFIGHTER].weaponType[0] = W_TRIPLE_SHOT;
	alien_defs[CD_PROTOFIGHTER].weaponType[1] = P_ANYTHING;
	alien_defs[CD_PROTOFIGHTER].chance[0] = 100;
	alien_defs[CD_PROTOFIGHTER].chance[1] = 1;
	alien_defs[CD_PROTOFIGHTER].score = 50;
	alien_defs[CD_PROTOFIGHTER].collectChance = 50;
	alien_defs[CD_PROTOFIGHTER].collectType = P_ANYTHING;
	alien_defs[CD_PROTOFIGHTER].collectValue = 50;
	alien_defs[CD_PROTOFIGHTER].flags = FL_WEAPCO;

	// Phoebe and Ursula
	alien_defs[CD_FRIEND].classDef = CD_FRIEND;
	alien_defs[CD_FRIEND].AIType = AI_OFFENSIVE;
	alien_defs[CD_FRIEND].speed = 3;
	alien_defs[CD_FRIEND].maxShield = 50;
	alien_defs[CD_FRIEND].shield = 50;
	alien_defs[CD_FRIEND].imageIndex[0] = 20;
	alien_defs[CD_FRIEND].imageIndex[1] = 21;
	alien_defs[CD_FRIEND].weaponType[0] = W_DOUBLE_SHOT;
	alien_defs[CD_FRIEND].weaponType[1] = W_HOMING_MISSILE;
	alien_defs[CD_FRIEND].chance[0] = 100;
	alien_defs[CD_FRIEND].chance[1] = 5;
	alien_defs[CD_FRIEND].score = 0;
	alien_defs[CD_FRIEND].collectChance = 0;
	alien_defs[CD_FRIEND].collectType = P_CASH;
	alien_defs[CD_FRIEND].collectValue = 0;
	alien_defs[CD_FRIEND].flags = FL_FRIEND;

	// Boss 1
	alien_defs[CD_FRIGATE].classDef = CD_BOSS;
	alien_defs[CD_FRIGATE].AIType = AI_NORMAL;
	alien_defs[CD_FRIGATE].speed = 2;
	alien_defs[CD_FRIGATE].maxShield = 550;
	alien_defs[CD_FRIGATE].shield = 550;
	alien_defs[CD_FRIGATE].imageIndex[0] = 8;
	alien_defs[CD_FRIGATE].imageIndex[1] = 9;
	alien_defs[CD_FRIGATE].weaponType[0] = W_MICRO_ROCKETS;
	alien_defs[CD_FRIGATE].weaponType[1] = W_ENERGYRAY;
	alien_defs[CD_FRIGATE].chance[0] = 100;
	alien_defs[CD_FRIGATE].chance[1] = 85;
	alien_defs[CD_FRIGATE].score = 500;
	alien_defs[CD_FRIGATE].collectChance = 100;
	alien_defs[CD_FRIGATE].collectType = P_CASH;
	alien_defs[CD_FRIGATE].collectValue = 250;
	alien_defs[CD_FRIGATE].flags = FL_WEAPCO;

	alien_defs[CD_FRIGATE_WING1].classDef = CD_FRIGATE_WING1;
	alien_defs[CD_FRIGATE_WING1].AIType = AI_NORMAL;
	alien_defs[CD_FRIGATE_WING1].speed = 2;
	alien_defs[CD_FRIGATE_WING1].maxShield = 100;
	alien_defs[CD_FRIGATE_WING1].shield = 100;
	alien_defs[CD_FRIGATE_WING1].imageIndex[0] = 10;
	alien_defs[CD_FRIGATE_WING1].imageIndex[1] = 11;
	alien_defs[CD_FRIGATE_WING1].weaponType[0] = W_TRIPLE_SHOT;
	alien_defs[CD_FRIGATE_WING1].weaponType[1] = W_ROCKETS;
	alien_defs[CD_FRIGATE_WING1].chance[0] = 100;
	alien_defs[CD_FRIGATE_WING1].chance[1] = 10;
	alien_defs[CD_FRIGATE_WING1].score = 500;
	alien_defs[CD_FRIGATE_WING1].collectChance = 100;
	alien_defs[CD_FRIGATE_WING1].collectType = P_ANYTHING;
	alien_defs[CD_FRIGATE_WING1].collectValue = 250;
	alien_defs[CD_FRIGATE_WING1].flags = FL_WEAPCO | FL_DAMAGEOWNER;

	alien_defs[CD_FRIGATE_WING2].classDef = CD_FRIGATE_WING2;
	alien_defs[CD_FRIGATE_WING2].AIType = AI_NORMAL;
	alien_defs[CD_FRIGATE_WING2].speed = 2;
	alien_defs[CD_FRIGATE_WING2].maxShield = 100;
	alien_defs[CD_FRIGATE_WING2].shield = 100;
	alien_defs[CD_FRIGATE_WING2].imageIndex[0] = 12;
	alien_defs[CD_FRIGATE_WING2].imageIndex[1] = 13;
	alien_defs[CD_FRIGATE_WING2].weaponType[0] = W_TRIPLE_SHOT;
	alien_defs[CD_FRIGATE_WING2].weaponType[1] = W_ROCKETS;
	alien_defs[CD_FRIGATE_WING2].chance[0] = 100;
	alien_defs[CD_FRIGATE_WING2].chance[1] = 10;
	alien_defs[CD_FRIGATE_WING2].score = 500;
	alien_defs[CD_FRIGATE_WING2].collectChance = 100;
	alien_defs[CD_FRIGATE_WING2].collectType = P_ANYTHING;
	alien_defs[CD_FRIGATE_WING2].collectValue = 250;
	alien_defs[CD_FRIGATE_WING2].flags = FL_WEAPCO | FL_DAMAGEOWNER;

	// Transport ship
	alien_defs[CD_TRANSPORTSHIP].classDef = CD_TRANSPORTSHIP;
	alien_defs[CD_TRANSPORTSHIP].AIType = AI_EVASIVE;
	alien_defs[CD_TRANSPORTSHIP].speed = 4;
	alien_defs[CD_TRANSPORTSHIP].maxShield = 10;
	alien_defs[CD_TRANSPORTSHIP].shield = 10;
	alien_defs[CD_TRANSPORTSHIP].imageIndex[0] = 14;
	alien_defs[CD_TRANSPORTSHIP].imageIndex[1] = 15;
	alien_defs[CD_TRANSPORTSHIP].weaponType[0] = W_DOUBLE_SHOT;
	alien_defs[CD_TRANSPORTSHIP].weaponType[1] = W_DOUBLE_SHOT;
	alien_defs[CD_TRANSPORTSHIP].chance[0] = 0;
	alien_defs[CD_TRANSPORTSHIP].chance[1] = 0;
	alien_defs[CD_TRANSPORTSHIP].score = 25;
	alien_defs[CD_TRANSPORTSHIP].collectChance = 100;
	alien_defs[CD_TRANSPORTSHIP].collectType = P_WEAPONS;
	alien_defs[CD_TRANSPORTSHIP].collectValue = 30;
	alien_defs[CD_TRANSPORTSHIP].flags = FL_WEAPCO | FL_NOFIRE;

	// Cargo ship
	alien_defs[CD_CARGOSHIP].classDef = CD_CARGOSHIP;
	alien_defs[CD_CARGOSHIP].AIType = AI_EVASIVE;
	alien_defs[CD_CARGOSHIP].speed = 4;
	alien_defs[CD_CARGOSHIP].maxShield = 10;
	alien_defs[CD_CARGOSHIP].shield = 10;
	alien_defs[CD_CARGOSHIP].imageIndex[0] = 22;
	alien_defs[CD_CARGOSHIP].imageIndex[1] = 23;
	alien_defs[CD_CARGOSHIP].weaponType[0] = W_DOUBLE_SHOT;
	alien_defs[CD_CARGOSHIP].weaponType[1] = W_DOUBLE_SHOT;
	alien_defs[CD_CARGOSHIP].chance[0] = 0;
	alien_defs[CD_CARGOSHIP].chance[1] = 0;
	alien_defs[CD_CARGOSHIP].score = 25;
	alien_defs[CD_CARGOSHIP].collectChance = 50;
	alien_defs[CD_CARGOSHIP].collectType = P_ANYTHING;
	alien_defs[CD_CARGOSHIP].collectValue = 100;
	alien_defs[CD_CARGOSHIP].flags = FL_WEAPCO | FL_NOFIRE;

	// Weapco Miner
	alien_defs[CD_MINER].classDef = CD_MINER;
	alien_defs[CD_MINER].AIType = AI_EVASIVE;
	alien_defs[CD_MINER].speed = 4;
	alien_defs[CD_MINER].maxShield = 25;
	alien_defs[CD_MINER].shield = 25;
	alien_defs[CD_MINER].imageIndex[0] = 16;
	alien_defs[CD_MINER].imageIndex[1] = 17;
	alien_defs[CD_MINER].weaponType[0] = W_DOUBLE_SHOT;
	alien_defs[CD_MINER].weaponType[1] = W_DOUBLE_SHOT;
	alien_defs[CD_MINER].chance[0] = 0;
	alien_defs[CD_MINER].chance[1] = 0;
	alien_defs[CD_MINER].score = 100;
	alien_defs[CD_MINER].collectChance = 100;
	alien_defs[CD_MINER].collectType = P_ANYTHING;
	alien_defs[CD_MINER].collectValue = 30;
	alien_defs[CD_MINER].flags = FL_WEAPCO | FL_NOFIRE | FL_DROPMINES;

	// Kline
	alien_defs[CD_KLINE].classDef = CD_KLINE;
	alien_defs[CD_KLINE].AIType = AI_OFFENSIVE;
	alien_defs[CD_KLINE].speed = 5;
	alien_defs[CD_KLINE].maxShield = 750;
	alien_defs[CD_KLINE].shield = 750;
	alien_defs[CD_KLINE].imageIndex[0] = 18;
	alien_defs[CD_KLINE].imageIndex[1] = 19;
	alien_defs[CD_KLINE].weaponType[0] = W_TRIPLE_SHOT;
	alien_defs[CD_KLINE].weaponType[1] = W_MICRO_ROCKETS;
	alien_defs[CD_KLINE].chance[0] = 100;
	alien_defs[CD_KLINE].chance[1] = 2;
	alien_defs[CD_KLINE].score = 0;
	alien_defs[CD_KLINE].collectChance = 0;
	alien_defs[CD_KLINE].collectType = P_ANYTHING;
	alien_defs[CD_KLINE].collectValue = 0;
	alien_defs[CD_KLINE].flags = FL_WEAPCO | FL_CANNOTDIE | FL_ALWAYSFACE | FL_CIRCLES;

	// Aim Fighter
	alien_defs[CD_AIMFIGHTER].classDef = CD_AIMFIGHTER;
	alien_defs[CD_AIMFIGHTER].AIType = AI_NORMAL;
	alien_defs[CD_AIMFIGHTER].speed = 3;
	alien_defs[CD_AIMFIGHTER].maxShield = 15;
	alien_defs[CD_AIMFIGHTER].shield = 15;
	alien_defs[CD_AIMFIGHTER].imageIndex[0] = 8;
	alien_defs[CD_AIMFIGHTER].imageIndex[1] = 9;
	alien_defs[CD_AIMFIGHTER].weaponType[0] = W_AIMED_SHOT;
	alien_defs[CD_AIMFIGHTER].weaponType[1] = W_AIMED_SHOT;
	alien_defs[CD_AIMFIGHTER].chance[0] = 7;
	alien_defs[CD_AIMFIGHTER].chance[1] = 1;
	alien_defs[CD_AIMFIGHTER].score = 50;
	alien_defs[CD_AIMFIGHTER].collectChance = 75;
	alien_defs[CD_AIMFIGHTER].collectType = P_ANYTHING;
	alien_defs[CD_AIMFIGHTER].collectValue = 100;
	alien_defs[CD_AIMFIGHTER].flags = FL_WEAPCO | FL_AIMS;

	// Slave ship
	alien_defs[CD_SLAVETRANSPORT].classDef = CD_SLAVETRANSPORT;
	alien_defs[CD_SLAVETRANSPORT].AIType = AI_EVASIVE;
	alien_defs[CD_SLAVETRANSPORT].speed = 2;
	alien_defs[CD_SLAVETRANSPORT].maxShield = 10;
	alien_defs[CD_SLAVETRANSPORT].shield = 20;
	alien_defs[CD_SLAVETRANSPORT].imageIndex[0] = 10;
	alien_defs[CD_SLAVETRANSPORT].imageIndex[1] = 11;
	alien_defs[CD_SLAVETRANSPORT].weaponType[0] = W_DOUBLE_SHOT;
	alien_defs[CD_SLAVETRANSPORT].weaponType[1] = W_DOUBLE_SHOT;
	alien_defs[CD_SLAVETRANSPORT].chance[0] = 0;
	alien_defs[CD_SLAVETRANSPORT].chance[1] = 0;
	alien_defs[CD_SLAVETRANSPORT].score = 25;
	alien_defs[CD_SLAVETRANSPORT].collectChance = 100;
	alien_defs[CD_SLAVETRANSPORT].collectType = P_SLAVES;
	alien_defs[CD_SLAVETRANSPORT].collectValue = 25;
	alien_defs[CD_SLAVETRANSPORT].flags = FL_WEAPCO | FL_NOFIRE;

	// Good Transport
	alien_defs[CD_GOODTRANSPORT].classDef = CD_GOODTRANSPORT;
	alien_defs[CD_GOODTRANSPORT].AIType = AI_EVASIVE;
	alien_defs[CD_GOODTRANSPORT].speed = 3;
	alien_defs[CD_GOODTRANSPORT].maxShield = 75;
	alien_defs[CD_GOODTRANSPORT].shield = 75;
	alien_defs[CD_GOODTRANSPORT].imageIndex[0] = 12;
	alien_defs[CD_GOODTRANSPORT].imageIndex[1] = 13;
	alien_defs[CD_GOODTRANSPORT].weaponType[0] = W_AIMED_SHOT;
	alien_defs[CD_GOODTRANSPORT].weaponType[1] = W_AIMED_SHOT;
	alien_defs[CD_GOODTRANSPORT].chance[0] = 100;
	alien_defs[CD_GOODTRANSPORT].chance[1] = 100;
	alien_defs[CD_GOODTRANSPORT].score = 0;
	alien_defs[CD_GOODTRANSPORT].collectChance = 0;
	alien_defs[CD_GOODTRANSPORT].collectType = P_ANYTHING;
	alien_defs[CD_GOODTRANSPORT].collectValue = 0;
	alien_defs[CD_GOODTRANSPORT].flags = FL_FRIEND | FL_NOFIRE | FL_AIMS;

	// Sid Wilson
	alien_defs[CD_SID].classDef = CD_SID;
	alien_defs[CD_SID].AIType = AI_NORMAL;
	alien_defs[CD_SID].speed = 3;
	alien_defs[CD_SID].maxShield = 50;
	alien_defs[CD_SID].shield = 50;
	alien_defs[CD_SID].imageIndex[0] = 24;
	alien_defs[CD_SID].imageIndex[1] = 25;
	alien_defs[CD_SID].weaponType[0] = W_IONCANNON;
	alien_defs[CD_SID].weaponType[1] = W_IONCANNON;
	alien_defs[CD_SID].chance[0] = 100;
	alien_defs[CD_SID].chance[1] = 0;
	alien_defs[CD_SID].score = 0;
	alien_defs[CD_SID].collectChance = 0;
	alien_defs[CD_SID].collectType = P_ANYTHING;
	alien_defs[CD_SID].collectValue = 0;
	alien_defs[CD_SID].flags = FL_FRIEND | FL_AIMS;

	// Mining Vessel Boss
	alien_defs[CD_MINEBOSS].classDef = CD_BOSS;
	alien_defs[CD_MINEBOSS].AIType = AI_NORMAL;
	alien_defs[CD_MINEBOSS].speed = 3;
	alien_defs[CD_MINEBOSS].maxShield = 1000;
	alien_defs[CD_MINEBOSS].shield = 1000;
	alien_defs[CD_MINEBOSS].imageIndex[0] = 26;
	alien_defs[CD_MINEBOSS].imageIndex[1] = 27;
	alien_defs[CD_MINEBOSS].weaponType[0] = W_TRIPLE_SHOT;
	alien_defs[CD_MINEBOSS].weaponType[1] = W_SPREADSHOT;
	alien_defs[CD_MINEBOSS].chance[0] = 0;
	alien_defs[CD_MINEBOSS].chance[1] = 0;
	alien_defs[CD_MINEBOSS].score = 1000;
	alien_defs[CD_MINEBOSS].collectChance = 100;
	alien_defs[CD_MINEBOSS].collectType = P_ANYTHING;
	alien_defs[CD_MINEBOSS].collectValue = 255;
	alien_defs[CD_MINEBOSS].flags = FL_WEAPCO | FL_IMMORTAL;

	alien_defs[CD_BOSS2_WING1].classDef = CD_BOSS2_WING1;
	alien_defs[CD_BOSS2_WING1].AIType = AI_NORMAL;
	alien_defs[CD_BOSS2_WING1].speed = 1;
	alien_defs[CD_BOSS2_WING1].maxShield = 250;
	alien_defs[CD_BOSS2_WING1].shield = 250;
	alien_defs[CD_BOSS2_WING1].imageIndex[0] = 28;
	alien_defs[CD_BOSS2_WING1].imageIndex[1] = 29;
	alien_defs[CD_BOSS2_WING1].weaponType[0] = W_TRIPLE_SHOT;
	alien_defs[CD_BOSS2_WING1].weaponType[1] = W_SPREADSHOT;
	alien_defs[CD_BOSS2_WING1].chance[0] = 0;
	alien_defs[CD_BOSS2_WING1].chance[1] = 0;
	alien_defs[CD_BOSS2_WING1].score = 1000;
	alien_defs[CD_BOSS2_WING1].collectChance = 100;
	alien_defs[CD_BOSS2_WING1].collectType = P_ANYTHING;
	alien_defs[CD_BOSS2_WING1].collectValue = 255;
	alien_defs[CD_BOSS2_WING1].flags = FL_WEAPCO | FL_DAMAGEOWNER;

	alien_defs[CD_BOSS2_WING2].classDef = CD_BOSS2_WING2;
	alien_defs[CD_BOSS2_WING2].AIType = AI_NORMAL;
	alien_defs[CD_BOSS2_WING2].speed = 1;
	alien_defs[CD_BOSS2_WING2].maxShield = 500;
	alien_defs[CD_BOSS2_WING2].shield = 500;
	alien_defs[CD_BOSS2_WING2].imageIndex[0] = 30;
	alien_defs[CD_BOSS2_WING2].imageIndex[1] = 31;
	alien_defs[CD_BOSS2_WING2].weaponType[0] = W_TRIPLE_SHOT;
	alien_defs[CD_BOSS2_WING2].weaponType[1] = W_SPREADSHOT;
	alien_defs[CD_BOSS2_WING2].chance[0] = 0;
	alien_defs[CD_BOSS2_WING2].chance[1] = 0;
	alien_defs[CD_BOSS2_WING2].score = 1000;
	alien_defs[CD_BOSS2_WING2].collectChance = 100;
	alien_defs[CD_BOSS2_WING2].collectType = P_ANYTHING;
	alien_defs[CD_BOSS2_WING2].collectValue = 255;
	alien_defs[CD_BOSS2_WING2].flags = FL_WEAPCO | FL_DEPLOYDRONES | FL_DAMAGEOWNER;

	alien_defs[CD_BOSS2_WING3].classDef = CD_BOSS2_WING3;
	alien_defs[CD_BOSS2_WING3].AIType = AI_NORMAL;
	alien_defs[CD_BOSS2_WING3].speed = 1;
	alien_defs[CD_BOSS2_WING3].maxShield = 500;
	alien_defs[CD_BOSS2_WING3].shield = 500;
	alien_defs[CD_BOSS2_WING3].imageIndex[0] = 32;
	alien_defs[CD_BOSS2_WING3].imageIndex[1] = 33;
	alien_defs[CD_BOSS2_WING3].weaponType[0] = W_TRIPLE_SHOT;
	alien_defs[CD_BOSS2_WING3].weaponType[1] = W_SPREADSHOT;
	alien_defs[CD_BOSS2_WING3].chance[0] = 0;
	alien_defs[CD_BOSS2_WING3].chance[1] = 0;
	alien_defs[CD_BOSS2_WING3].score = 1000;
	alien_defs[CD_BOSS2_WING3].collectChance = 100;
	alien_defs[CD_BOSS2_WING3].collectType = P_ANYTHING;
	alien_defs[CD_BOSS2_WING3].collectValue = 255;
	alien_defs[CD_BOSS2_WING3].flags = FL_WEAPCO | FL_DEPLOYDRONES | FL_DAMAGEOWNER;

	alien_defs[CD_BOSS2_WING4].classDef = CD_BOSS2_WING4;
	alien_defs[CD_BOSS2_WING4].AIType = AI_NORMAL;
	alien_defs[CD_BOSS2_WING4].speed = 1;
	alien_defs[CD_BOSS2_WING4].maxShield = 250;
	alien_defs[CD_BOSS2_WING4].shield = 250;
	alien_defs[CD_BOSS2_WING4].imageIndex[0] = 34;
	alien_defs[CD_BOSS2_WING4].imageIndex[1] = 35;
	alien_defs[CD_BOSS2_WING4].weaponType[0] = W_TRIPLE_SHOT;
	alien_defs[CD_BOSS2_WING4].weaponType[1] = W_SPREADSHOT;
	alien_defs[CD_BOSS2_WING4].chance[0] = 0;
	alien_defs[CD_BOSS2_WING4].chance[1] = 0;
	alien_defs[CD_BOSS2_WING4].score = 1000;
	alien_defs[CD_BOSS2_WING4].collectChance = 100;
	alien_defs[CD_BOSS2_WING4].collectType = P_ANYTHING;
	alien_defs[CD_BOSS2_WING4].collectValue = 255;
	alien_defs[CD_BOSS2_WING4].flags = FL_WEAPCO | FL_DAMAGEOWNER;

	// Drone
	alien_defs[CD_DRONE].classDef = CD_DRONE;
	alien_defs[CD_DRONE].AIType = AI_OFFENSIVE;
	alien_defs[CD_DRONE].speed = 8;
	alien_defs[CD_DRONE].maxShield = 5;
	alien_defs[CD_DRONE].shield = 5;
	alien_defs[CD_DRONE].imageIndex[0] = 36;
	alien_defs[CD_DRONE].imageIndex[1] = 37;
	alien_defs[CD_DRONE].weaponType[0] = W_DOUBLE_SHOT;
	alien_defs[CD_DRONE].weaponType[1] = W_LASER;
	alien_defs[CD_DRONE].chance[0] = 100;
	alien_defs[CD_DRONE].chance[1] = 0;
	alien_defs[CD_DRONE].score = 0;
	alien_defs[CD_DRONE].collectChance = 10;
	alien_defs[CD_DRONE].collectType = P_SHIELD;
	alien_defs[CD_DRONE].collectValue = 1;
	alien_defs[CD_DRONE].flags = FL_WEAPCO;

	// Experimental Fighter
	alien_defs[CD_CLOAKFIGHTER].classDef = CD_CLOAKFIGHTER;
	alien_defs[CD_CLOAKFIGHTER].AIType = AI_OFFENSIVE;
	alien_defs[CD_CLOAKFIGHTER].speed = 6;
	alien_defs[CD_CLOAKFIGHTER].maxShield = 1000;
	alien_defs[CD_CLOAKFIGHTER].shield = 1000;
	alien_defs[CD_CLOAKFIGHTER].imageIndex[0] = 10;
	alien_defs[CD_CLOAKFIGHTER].imageIndex[1] = 11;
	alien_defs[CD_CLOAKFIGHTER].weaponType[0] = W_SPREADSHOT;
	alien_defs[CD_CLOAKFIGHTER].weaponType[1] = W_DOUBLE_ROCKETS;
	alien_defs[CD_CLOAKFIGHTER].chance[0] = 100;
	alien_defs[CD_CLOAKFIGHTER].chance[1] = 5;
	alien_defs[CD_CLOAKFIGHTER].score = 550;
	alien_defs[CD_CLOAKFIGHTER].collectChance = 100;
	alien_defs[CD_CLOAKFIGHTER].collectType = P_CASH;
	alien_defs[CD_CLOAKFIGHTER].collectValue = 255;
	alien_defs[CD_CLOAKFIGHTER].flags = FL_WEAPCO | FL_CANCLOAK | FL_RUNSAWAY;

	// Evil Ursula
	alien_defs[CD_EVILURSULA].classDef = CD_EVILURSULA;
	alien_defs[CD_EVILURSULA].AIType = AI_OFFENSIVE;
	alien_defs[CD_EVILURSULA].speed = 5;
	alien_defs[CD_EVILURSULA].maxShield = 500;
	alien_defs[CD_EVILURSULA].shield = 500;
	alien_defs[CD_EVILURSULA].imageIndex[0] = 12;
	alien_defs[CD_EVILURSULA].imageIndex[1] = 13;
	alien_defs[CD_EVILURSULA].weaponType[0] = W_TRIPLE_SHOT;
	alien_defs[CD_EVILURSULA].weaponType[1] = W_HOMING_MISSILE;
	alien_defs[CD_EVILURSULA].chance[0] = 100;
	alien_defs[CD_EVILURSULA].chance[1] = 100;
	alien_defs[CD_EVILURSULA].score = 500;
	alien_defs[CD_EVILURSULA].collectChance = 100;
	alien_defs[CD_EVILURSULA].collectType = P_ESCAPEPOD;
	alien_defs[CD_EVILURSULA].collectValue = 1;
	alien_defs[CD_EVILURSULA].flags = FL_WEAPCO;

	// Mercenary
	alien_defs[CD_KRASS].classDef = CD_KRASS;
	alien_defs[CD_KRASS].AIType = AI_OFFENSIVE;
	alien_defs[CD_KRASS].speed = 5;
	alien_defs[CD_KRASS].maxShield = 1000;
	alien_defs[CD_KRASS].shield = 1000;
	alien_defs[CD_KRASS].imageIndex[0] = 26;
	alien_defs[CD_KRASS].imageIndex[1] = 27;
	alien_defs[CD_KRASS].weaponType[0] = W_SPREADSHOT;
	alien_defs[CD_KRASS].weaponType[1] = W_CHARGER;
	alien_defs[CD_KRASS].chance[0] = 100;
	alien_defs[CD_KRASS].chance[1] = 0;
	alien_defs[CD_KRASS].score = 2000;
	alien_defs[CD_KRASS].collectChance = 100;
	alien_defs[CD_KRASS].collectType = P_ANYTHING;
	alien_defs[CD_KRASS].collectValue = 255;
	alien_defs[CD_KRASS].flags = FL_FRIEND | FL_IMMORTAL;

	// Executive Transport
	alien_defs[CD_EXEC].classDef = CD_BOSS;
	alien_defs[CD_EXEC].AIType = AI_NORMAL;
	alien_defs[CD_EXEC].speed = 5;
	alien_defs[CD_EXEC].maxShield = 1000;
	alien_defs[CD_EXEC].shield = 1000;
	alien_defs[CD_EXEC].imageIndex[0] = 28;
	alien_defs[CD_EXEC].imageIndex[1] = 28;
	alien_defs[CD_EXEC].weaponType[0] = W_SPREADSHOT;
	alien_defs[CD_EXEC].weaponType[1] = W_HOMING_MISSILE;
	alien_defs[CD_EXEC].chance[0] = 0;
	alien_defs[CD_EXEC].chance[1] = 0;
	alien_defs[CD_EXEC].score = 2000;
	alien_defs[CD_EXEC].collectChance = 0;
	alien_defs[CD_EXEC].collectType = P_ANYTHING;
	alien_defs[CD_EXEC].collectValue = 0;
	alien_defs[CD_EXEC].flags = FL_WEAPCO | FL_NOFIRE;

	// Asteroid
	alien_defs[CD_ASTEROID].classDef = CD_ASTEROID;
	alien_defs[CD_ASTEROID].AIType = AI_WANDER;
	alien_defs[CD_ASTEROID].speed = 1;
	alien_defs[CD_ASTEROID].maxShield = 50;
	alien_defs[CD_ASTEROID].shield = 50;
	alien_defs[CD_ASTEROID].imageIndex[0] = 38;
	alien_defs[CD_ASTEROID].imageIndex[1] = 38;
	alien_defs[CD_ASTEROID].weaponType[0] = W_SPREADSHOT;
	alien_defs[CD_ASTEROID].weaponType[1] = W_HOMING_MISSILE;
	alien_defs[CD_ASTEROID].chance[0] = 0;
	alien_defs[CD_ASTEROID].chance[1] = 0;
	alien_defs[CD_ASTEROID].score = 0;
	alien_defs[CD_ASTEROID].collectChance = 25;
	alien_defs[CD_ASTEROID].collectType = P_ORE;
	alien_defs[CD_ASTEROID].collectValue = 1;
	alien_defs[CD_ASTEROID].flags = FL_WEAPCO;

	alien_defs[CD_ASTEROID2].classDef = CD_ASTEROID2;
	alien_defs[CD_ASTEROID2].AIType = AI_WANDER;
	alien_defs[CD_ASTEROID2].speed = 1;
	alien_defs[CD_ASTEROID2].maxShield = 10;
	alien_defs[CD_ASTEROID2].shield = 10;
	alien_defs[CD_ASTEROID2].imageIndex[0] = 39;
	alien_defs[CD_ASTEROID2].imageIndex[1] = 40;
	alien_defs[CD_ASTEROID2].weaponType[0] = W_SPREADSHOT;
	alien_defs[CD_ASTEROID2].weaponType[1] = W_HOMING_MISSILE;
	alien_defs[CD_ASTEROID2].chance[0] = 0;
	alien_defs[CD_ASTEROID2].chance[1] = 0;
	alien_defs[CD_ASTEROID2].score = 0;
	alien_defs[CD_ASTEROID2].collectChance = 25;
	alien_defs[CD_ASTEROID2].collectType = P_ORE;
	alien_defs[CD_ASTEROID2].collectValue = 1;
	alien_defs[CD_ASTEROID2].flags = FL_WEAPCO;

	// Escort
	alien_defs[CD_ESCORT].classDef = CD_ESCORT;
	alien_defs[CD_ESCORT].AIType = AI_NORMAL;
	alien_defs[CD_ESCORT].speed = 3;
	alien_defs[CD_ESCORT].maxShield = 200;
	alien_defs[CD_ESCORT].shield = 200;
	alien_defs[CD_ESCORT].imageIndex[0] = 30;
	alien_defs[CD_ESCORT].imageIndex[1] = 31;
	alien_defs[CD_ESCORT].weaponType[0] = W_LASER;
	alien_defs[CD_ESCORT].weaponType[1] = W_LASER;
	alien_defs[CD_ESCORT].chance[0] = 25;
	alien_defs[CD_ESCORT].chance[1] = 25;
	alien_defs[CD_ESCORT].score = 450;
	alien_defs[CD_ESCORT].collectChance = 100;
	alien_defs[CD_ESCORT].collectType = P_ANYTHING;
	alien_defs[CD_ESCORT].collectValue = 100;
	alien_defs[CD_ESCORT].flags = FL_WEAPCO;

	// Mobile Ray Cannon
	alien_defs[CD_MOBILE_RAY].classDef = CD_MOBILE_RAY;
	alien_defs[CD_MOBILE_RAY].AIType = AI_OFFENSIVE;
	alien_defs[CD_MOBILE_RAY].speed = 5;
	alien_defs[CD_MOBILE_RAY].maxShield = 250;
	alien_defs[CD_MOBILE_RAY].shield = 250;
	alien_defs[CD_MOBILE_RAY].imageIndex[0] = 10;
	alien_defs[CD_MOBILE_RAY].imageIndex[1] = 11;
	alien_defs[CD_MOBILE_RAY].weaponType[0] = W_ENERGYRAY;
	alien_defs[CD_MOBILE_RAY].weaponType[1] = W_ENERGYRAY;
	alien_defs[CD_MOBILE_RAY].chance[0] = 50;
	alien_defs[CD_MOBILE_RAY].chance[1] = 50;
	alien_defs[CD_MOBILE_RAY].score = 1000;
	alien_defs[CD_MOBILE_RAY].collectChance = 75;
	alien_defs[CD_MOBILE_RAY].collectType = P_SHIELD;
	alien_defs[CD_MOBILE_RAY].collectValue = 2;
	alien_defs[CD_MOBILE_RAY].flags = FL_WEAPCO;

	// Rebel Carrier
	alien_defs[CD_REBELCARRIER].classDef = CD_REBELCARRIER;
	alien_defs[CD_REBELCARRIER].AIType = AI_OFFENSIVE;
	alien_defs[CD_REBELCARRIER].speed = 2;
	alien_defs[CD_REBELCARRIER].maxShield = 100;
	alien_defs[CD_REBELCARRIER].shield = 100;
	alien_defs[CD_REBELCARRIER].imageIndex[0] = 32;
	alien_defs[CD_REBELCARRIER].imageIndex[1] = 33;
	alien_defs[CD_REBELCARRIER].weaponType[0] = W_DOUBLE_ROCKETS;
	alien_defs[CD_REBELCARRIER].weaponType[1] = W_MICRO_ROCKETS;
	alien_defs[CD_REBELCARRIER].chance[0] = 50;
	alien_defs[CD_REBELCARRIER].chance[1] = 2;
	alien_defs[CD_REBELCARRIER].score = 0;
	alien_defs[CD_REBELCARRIER].collectChance = 0;
	alien_defs[CD_REBELCARRIER].collectType = P_SHIELD;
	alien_defs[CD_REBELCARRIER].collectValue = 0;
	alien_defs[CD_REBELCARRIER].flags = FL_FRIEND;

	// Pluto Boss
	alien_defs[CD_PLUTOBOSS].classDef = CD_PLUTOBOSS;
	alien_defs[CD_PLUTOBOSS].AIType = AI_OFFENSIVE;
	alien_defs[CD_PLUTOBOSS].speed = 4;
	alien_defs[CD_PLUTOBOSS].maxShield = 500;
	alien_defs[CD_PLUTOBOSS].shield = 500;
	alien_defs[CD_PLUTOBOSS].imageIndex[0] = 12;
	alien_defs[CD_PLUTOBOSS].imageIndex[1] = 13;
	alien_defs[CD_PLUTOBOSS].weaponType[0] = W_DOUBLE_ROCKETS;
	alien_defs[CD_PLUTOBOSS].weaponType[1] = W_MICRO_ROCKETS;
	alien_defs[CD_PLUTOBOSS].chance[0] = 50;
	alien_defs[CD_PLUTOBOSS].chance[1] = 2;
	alien_defs[CD_PLUTOBOSS].score = 1000;
	alien_defs[CD_PLUTOBOSS].collectChance = 0;
	alien_defs[CD_PLUTOBOSS].collectType = P_SHIELD;
	alien_defs[CD_PLUTOBOSS].collectValue = 0;
	alien_defs[CD_PLUTOBOSS].flags = FL_WEAPCO;

	// Pluto Boss Barrier
	alien_defs[CD_BARRIER].classDef = CD_BARRIER;
	alien_defs[CD_BARRIER].AIType = AI_OFFENSIVE;
	alien_defs[CD_BARRIER].speed = 1;
	alien_defs[CD_BARRIER].maxShield = 250;
	alien_defs[CD_BARRIER].shield = 250;
	alien_defs[CD_BARRIER].imageIndex[0] = 32;
	alien_defs[CD_BARRIER].imageIndex[1] = 33;
	alien_defs[CD_BARRIER].weaponType[0] = W_DOUBLE_SHOT;
	alien_defs[CD_BARRIER].weaponType[1] = W_MICRO_ROCKETS;
	alien_defs[CD_BARRIER].chance[0] = 0;
	alien_defs[CD_BARRIER].chance[1] = 0;
	alien_defs[CD_BARRIER].score = 1000;
	alien_defs[CD_BARRIER].collectChance = 100;
	alien_defs[CD_BARRIER].collectType = P_ANYTHING;
	alien_defs[CD_BARRIER].collectValue = 25;
	alien_defs[CD_BARRIER].flags = FL_WEAPCO | FL_NOFIRE;

	// Neptune Boss
	alien_defs[CD_NEPTUNEBOSS].classDef = CD_NEPTUNEBOSS;
	alien_defs[CD_NEPTUNEBOSS].AIType = AI_OFFENSIVE;
	alien_defs[CD_NEPTUNEBOSS].speed = 4;
	alien_defs[CD_NEPTUNEBOSS].maxShield = 800;
	alien_defs[CD_NEPTUNEBOSS].shield = 800;
	alien_defs[CD_NEPTUNEBOSS].imageIndex[0] = 12;
	alien_defs[CD_NEPTUNEBOSS].imageIndex[1] = 13;
	alien_defs[CD_NEPTUNEBOSS].weaponType[0] = W_DOUBLE_SHOT;
	alien_defs[CD_NEPTUNEBOSS].weaponType[1] = W_MICRO_ROCKETS;
	alien_defs[CD_NEPTUNEBOSS].chance[0] = 100;
	alien_defs[CD_NEPTUNEBOSS].chance[1] = 0;
	alien_defs[CD_NEPTUNEBOSS].score = 1000;
	alien_defs[CD_NEPTUNEBOSS].collectChance = 100;
	alien_defs[CD_NEPTUNEBOSS].collectType = P_ANYTHING;
	alien_defs[CD_NEPTUNEBOSS].collectValue = 25;
	alien_defs[CD_NEPTUNEBOSS].flags = FL_WEAPCO;

	// Mobile Shield
	alien_defs[CD_MOBILESHIELD].classDef = CD_MOBILESHIELD;
	alien_defs[CD_MOBILESHIELD].AIType = AI_EVASIVE;
	alien_defs[CD_MOBILESHIELD].speed = 6;
	alien_defs[CD_MOBILESHIELD].maxShield = 150;
	alien_defs[CD_MOBILESHIELD].shield = 150;
	alien_defs[CD_MOBILESHIELD].imageIndex[0] = 34;
	alien_defs[CD_MOBILESHIELD].imageIndex[1] = 35;
	alien_defs[CD_MOBILESHIELD].weaponType[0] = W_DOUBLE_SHOT;
	alien_defs[CD_MOBILESHIELD].weaponType[1] = W_MICRO_ROCKETS;
	alien_defs[CD_MOBILESHIELD].chance[0] = 0;
	alien_defs[CD_MOBILESHIELD].chance[1] = 0;
	alien_defs[CD_MOBILESHIELD].score = 250;
	alien_defs[CD_MOBILESHIELD].collectChance = 100;
	alien_defs[CD_MOBILESHIELD].collectType = P_ANYTHING;
	alien_defs[CD_MOBILESHIELD].collectValue = 25;
	alien_defs[CD_MOBILESHIELD].flags = FL_WEAPCO | FL_NOFIRE;

	// Firefly
	alien_defs[CD_FIREFLY].classDef = CD_FIREFLY;
	alien_defs[CD_FIREFLY].AIType = AI_OFFENSIVE;
	alien_defs[CD_FIREFLY].speed = 5;
	alien_defs[CD_FIREFLY].maxShield = 250;
	alien_defs[CD_FIREFLY].shield = 250;
	alien_defs[CD_FIREFLY].imageIndex[0] = 0;
	alien_defs[CD_FIREFLY].imageIndex[1] = 1;
	alien_defs[CD_FIREFLY].weaponType[0] = W_TRIPLE_SHOT;
	alien_defs[CD_FIREFLY].weaponType[1] = W_DOUBLE_ROCKETS;
	alien_defs[CD_FIREFLY].chance[0] = 100;
	alien_defs[CD_FIREFLY].chance[1] = 5;
	alien_defs[CD_FIREFLY].score = 500;
	alien_defs[CD_FIREFLY].collectChance = 100;
	alien_defs[CD_FIREFLY].collectType = P_ANYTHING;
	alien_defs[CD_FIREFLY].collectValue = 250;
	alien_defs[CD_FIREFLY].flags = FL_WEAPCO;

	// Uranus Boss
	alien_defs[CD_URANUSBOSS].classDef = CD_URANUSBOSS;
	alien_defs[CD_URANUSBOSS].AIType = AI_OFFENSIVE;
	alien_defs[CD_URANUSBOSS].speed = 4;
	alien_defs[CD_URANUSBOSS].maxShield = 750;
	alien_defs[CD_URANUSBOSS].shield = 750;
	alien_defs[CD_URANUSBOSS].imageIndex[0] = 41;
	alien_defs[CD_URANUSBOSS].imageIndex[1] = 42;
	alien_defs[CD_URANUSBOSS].weaponType[0] = W_SPREADSHOT;
	alien_defs[CD_URANUSBOSS].weaponType[1] = W_DOUBLE_ROCKETS;
	alien_defs[CD_URANUSBOSS].chance[0] = 100;
	alien_defs[CD_URANUSBOSS].chance[1] = 5;
	alien_defs[CD_URANUSBOSS].score = 500;
	alien_defs[CD_URANUSBOSS].collectChance = 100;
	alien_defs[CD_URANUSBOSS].collectType = P_ANYTHING;
	alien_defs[CD_URANUSBOSS].collectValue = 250;
	alien_defs[CD_URANUSBOSS].flags = FL_WEAPCO;

	// Uranus Boss Wing 1
	alien_defs[CD_URANUSBOSSWING1].classDef = CD_URANUSBOSSWING1;
	alien_defs[CD_URANUSBOSSWING1].AIType = AI_OFFENSIVE;
	alien_defs[CD_URANUSBOSSWING1].speed = 4;
	alien_defs[CD_URANUSBOSSWING1].maxShield = 250;
	alien_defs[CD_URANUSBOSSWING1].shield = 250;
	alien_defs[CD_URANUSBOSSWING1].imageIndex[0] = 43;
	alien_defs[CD_URANUSBOSSWING1].imageIndex[1] = 44;
	alien_defs[CD_URANUSBOSSWING1].weaponType[0] = W_DOUBLE_ROCKETS;
	alien_defs[CD_URANUSBOSSWING1].weaponType[1] = W_DOUBLE_ROCKETS;
	alien_defs[CD_URANUSBOSSWING1].chance[0] = 5;
	alien_defs[CD_URANUSBOSSWING1].chance[1] = 0;
	alien_defs[CD_URANUSBOSSWING1].score = 500;
	alien_defs[CD_URANUSBOSSWING1].collectChance = 100;
	alien_defs[CD_URANUSBOSSWING1].collectType = P_ANYTHING;
	alien_defs[CD_URANUSBOSSWING1].collectValue = 250;
	alien_defs[CD_URANUSBOSSWING1].flags = FL_WEAPCO | FL_IMMORTAL;

	// Uranus Boss Wing 2
	alien_defs[CD_URANUSBOSSWING2].classDef = CD_URANUSBOSSWING2;
	alien_defs[CD_URANUSBOSSWING2].AIType = AI_OFFENSIVE;
	alien_defs[CD_URANUSBOSSWING2].speed = 4;
	alien_defs[CD_URANUSBOSSWING2].maxShield = 250;
	alien_defs[CD_URANUSBOSSWING2].shield = 250;
	alien_defs[CD_URANUSBOSSWING2].imageIndex[0] = 45;
	alien_defs[CD_URANUSBOSSWING2].imageIndex[1] = 46;
	alien_defs[CD_URANUSBOSSWING2].weaponType[0] = W_DOUBLE_ROCKETS;
	alien_defs[CD_URANUSBOSSWING2].weaponType[1] = W_DOUBLE_ROCKETS;
	alien_defs[CD_URANUSBOSSWING2].chance[0] = 5;
	alien_defs[CD_URANUSBOSSWING2].chance[1] = 0;
	alien_defs[CD_URANUSBOSSWING2].score = 500;
	alien_defs[CD_URANUSBOSSWING2].collectChance = 100;
	alien_defs[CD_URANUSBOSSWING2].collectType = P_ANYTHING;
	alien_defs[CD_URANUSBOSSWING2].collectValue = 250;
	alien_defs[CD_URANUSBOSSWING2].flags = FL_WEAPCO | FL_IMMORTAL;

	setColorIndexes();

	showStory();

	// Determine which part of the game we will go to...
	section = 0;

	currentGame.difficulty = DIFFICULTY_NORMAL;
	newGame();

	while (true)
	{
		switch (section)
		{
			case 0:
				section = doTitle();
				break;

			case 1:
				section = galaxyMap();
				break;

			case 2:
				if (currentGame.stationedPlanet == -1)
					doCutscene(0);
				section = mainGameLoop();
				break;
		}
	}

	return(0);
}
