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
	defEnemy[CD_DUALFIGHTER].classDef = CD_DUALFIGHTER;
	defEnemy[CD_DUALFIGHTER].AIType = AI_NORMAL;
	defEnemy[CD_DUALFIGHTER].speed = 4;
	defEnemy[CD_DUALFIGHTER].maxShield = 5;
	defEnemy[CD_DUALFIGHTER].shield = 5;
	defEnemy[CD_DUALFIGHTER].imageIndex[0] = 2;
	defEnemy[CD_DUALFIGHTER].imageIndex[1] = 3;
	defEnemy[CD_DUALFIGHTER].weaponType[0] = W_DOUBLE_SHOT;
	defEnemy[CD_DUALFIGHTER].weaponType[1] = W_ROCKETS;
	defEnemy[CD_DUALFIGHTER].chance[0] = 100;
	defEnemy[CD_DUALFIGHTER].chance[1] = 1;
	defEnemy[CD_DUALFIGHTER].score = 25;
	defEnemy[CD_DUALFIGHTER].collectChance = 50;
	defEnemy[CD_DUALFIGHTER].collectType = P_ANYTHING;
	defEnemy[CD_DUALFIGHTER].collectValue = 50;
	defEnemy[CD_DUALFIGHTER].flags = FL_WEAPCO;

	// Missile Boat
	defEnemy[CD_MISSILEBOAT].classDef = CD_MISSILEBOAT;
	defEnemy[CD_MISSILEBOAT].AIType = AI_DEFENSIVE;
	defEnemy[CD_MISSILEBOAT].speed = 2;
	defEnemy[CD_MISSILEBOAT].maxShield = 50;
	defEnemy[CD_MISSILEBOAT].shield = 50;
	defEnemy[CD_MISSILEBOAT].imageIndex[0] = 4;
	defEnemy[CD_MISSILEBOAT].imageIndex[1] = 5;
	defEnemy[CD_MISSILEBOAT].weaponType[0] = W_ROCKETS;
	defEnemy[CD_MISSILEBOAT].weaponType[1] = W_DOUBLE_ROCKETS;
	defEnemy[CD_MISSILEBOAT].chance[0] = 25;
	defEnemy[CD_MISSILEBOAT].chance[1] = 4;
	defEnemy[CD_MISSILEBOAT].score = 250;
	defEnemy[CD_MISSILEBOAT].collectChance = 25;
	defEnemy[CD_MISSILEBOAT].collectType = P_ANYTHING;
	defEnemy[CD_MISSILEBOAT].collectValue = 75;
	defEnemy[CD_MISSILEBOAT].flags = FL_WEAPCO;

	//Prototype fighter
	defEnemy[CD_PROTOFIGHTER].classDef = CD_PROTOFIGHTER;
	defEnemy[CD_PROTOFIGHTER].AIType = AI_DEFENSIVE;
	defEnemy[CD_PROTOFIGHTER].speed = 5;
	defEnemy[CD_PROTOFIGHTER].maxShield = 15;
	defEnemy[CD_PROTOFIGHTER].shield = 15;
	defEnemy[CD_PROTOFIGHTER].imageIndex[0] = 6;
	defEnemy[CD_PROTOFIGHTER].imageIndex[1] = 7;
	defEnemy[CD_PROTOFIGHTER].weaponType[0] = W_TRIPLE_SHOT;
	defEnemy[CD_PROTOFIGHTER].weaponType[1] = P_ANYTHING;
	defEnemy[CD_PROTOFIGHTER].chance[0] = 100;
	defEnemy[CD_PROTOFIGHTER].chance[1] = 1;
	defEnemy[CD_PROTOFIGHTER].score = 50;
	defEnemy[CD_PROTOFIGHTER].collectChance = 50;
	defEnemy[CD_PROTOFIGHTER].collectType = P_ANYTHING;
	defEnemy[CD_PROTOFIGHTER].collectValue = 50;
	defEnemy[CD_PROTOFIGHTER].flags = FL_WEAPCO;

	// Phoebe and Ursula
	defEnemy[CD_FRIEND].classDef = CD_FRIEND;
	defEnemy[CD_FRIEND].AIType = AI_OFFENSIVE;
	defEnemy[CD_FRIEND].speed = 3;
	defEnemy[CD_FRIEND].maxShield = 50;
	defEnemy[CD_FRIEND].shield = 50;
	defEnemy[CD_FRIEND].imageIndex[0] = 20;
	defEnemy[CD_FRIEND].imageIndex[1] = 21;
	defEnemy[CD_FRIEND].weaponType[0] = W_DOUBLE_SHOT;
	defEnemy[CD_FRIEND].weaponType[1] = W_HOMING_MISSILE;
	defEnemy[CD_FRIEND].chance[0] = 100;
	defEnemy[CD_FRIEND].chance[1] = 5;
	defEnemy[CD_FRIEND].score = 0;
	defEnemy[CD_FRIEND].collectChance = 0;
	defEnemy[CD_FRIEND].collectType = P_CASH;
	defEnemy[CD_FRIEND].collectValue = 0;
	defEnemy[CD_FRIEND].flags = FL_FRIEND;

	// Boss 1
	defEnemy[CD_FRIGATE].classDef = CD_BOSS;
	defEnemy[CD_FRIGATE].AIType = AI_NORMAL;
	defEnemy[CD_FRIGATE].speed = 2;
	defEnemy[CD_FRIGATE].maxShield = 550;
	defEnemy[CD_FRIGATE].shield = 550;
	defEnemy[CD_FRIGATE].imageIndex[0] = 8;
	defEnemy[CD_FRIGATE].imageIndex[1] = 9;
	defEnemy[CD_FRIGATE].weaponType[0] = W_MICRO_ROCKETS;
	defEnemy[CD_FRIGATE].weaponType[1] = W_ENERGYRAY;
	defEnemy[CD_FRIGATE].chance[0] = 100;
	defEnemy[CD_FRIGATE].chance[1] = 85;
	defEnemy[CD_FRIGATE].score = 500;
	defEnemy[CD_FRIGATE].collectChance = 100;
	defEnemy[CD_FRIGATE].collectType = P_CASH;
	defEnemy[CD_FRIGATE].collectValue = 250;
	defEnemy[CD_FRIGATE].flags = FL_WEAPCO;

	defEnemy[CD_FRIGATE_WING1].classDef = CD_FRIGATE_WING1;
	defEnemy[CD_FRIGATE_WING1].AIType = AI_NORMAL;
	defEnemy[CD_FRIGATE_WING1].speed = 2;
	defEnemy[CD_FRIGATE_WING1].maxShield = 100;
	defEnemy[CD_FRIGATE_WING1].shield = 100;
	defEnemy[CD_FRIGATE_WING1].imageIndex[0] = 10;
	defEnemy[CD_FRIGATE_WING1].imageIndex[1] = 11;
	defEnemy[CD_FRIGATE_WING1].weaponType[0] = W_TRIPLE_SHOT;
	defEnemy[CD_FRIGATE_WING1].weaponType[1] = W_ROCKETS;
	defEnemy[CD_FRIGATE_WING1].chance[0] = 100;
	defEnemy[CD_FRIGATE_WING1].chance[1] = 10;
	defEnemy[CD_FRIGATE_WING1].score = 500;
	defEnemy[CD_FRIGATE_WING1].collectChance = 100;
	defEnemy[CD_FRIGATE_WING1].collectType = P_ANYTHING;
	defEnemy[CD_FRIGATE_WING1].collectValue = 250;
	defEnemy[CD_FRIGATE_WING1].flags = FL_WEAPCO | FL_DAMAGEOWNER;

	defEnemy[CD_FRIGATE_WING2].classDef = CD_FRIGATE_WING2;
	defEnemy[CD_FRIGATE_WING2].AIType = AI_NORMAL;
	defEnemy[CD_FRIGATE_WING2].speed = 2;
	defEnemy[CD_FRIGATE_WING2].maxShield = 100;
	defEnemy[CD_FRIGATE_WING2].shield = 100;
	defEnemy[CD_FRIGATE_WING2].imageIndex[0] = 12;
	defEnemy[CD_FRIGATE_WING2].imageIndex[1] = 13;
	defEnemy[CD_FRIGATE_WING2].weaponType[0] = W_TRIPLE_SHOT;
	defEnemy[CD_FRIGATE_WING2].weaponType[1] = W_ROCKETS;
	defEnemy[CD_FRIGATE_WING2].chance[0] = 100;
	defEnemy[CD_FRIGATE_WING2].chance[1] = 10;
	defEnemy[CD_FRIGATE_WING2].score = 500;
	defEnemy[CD_FRIGATE_WING2].collectChance = 100;
	defEnemy[CD_FRIGATE_WING2].collectType = P_ANYTHING;
	defEnemy[CD_FRIGATE_WING2].collectValue = 250;
	defEnemy[CD_FRIGATE_WING2].flags = FL_WEAPCO | FL_DAMAGEOWNER;

	// Transport ship
	defEnemy[CD_TRANSPORTSHIP].classDef = CD_TRANSPORTSHIP;
	defEnemy[CD_TRANSPORTSHIP].AIType = AI_EVASIVE;
	defEnemy[CD_TRANSPORTSHIP].speed = 4;
	defEnemy[CD_TRANSPORTSHIP].maxShield = 10;
	defEnemy[CD_TRANSPORTSHIP].shield = 10;
	defEnemy[CD_TRANSPORTSHIP].imageIndex[0] = 14;
	defEnemy[CD_TRANSPORTSHIP].imageIndex[1] = 15;
	defEnemy[CD_TRANSPORTSHIP].weaponType[0] = W_DOUBLE_SHOT;
	defEnemy[CD_TRANSPORTSHIP].weaponType[1] = W_DOUBLE_SHOT;
	defEnemy[CD_TRANSPORTSHIP].chance[0] = 0;
	defEnemy[CD_TRANSPORTSHIP].chance[1] = 0;
	defEnemy[CD_TRANSPORTSHIP].score = 25;
	defEnemy[CD_TRANSPORTSHIP].collectChance = 100;
	defEnemy[CD_TRANSPORTSHIP].collectType = P_WEAPONS;
	defEnemy[CD_TRANSPORTSHIP].collectValue = 30;
	defEnemy[CD_TRANSPORTSHIP].flags = FL_WEAPCO | FL_NOFIRE;

	// Cargo ship
	defEnemy[CD_CARGOSHIP].classDef = CD_CARGOSHIP;
	defEnemy[CD_CARGOSHIP].AIType = AI_EVASIVE;
	defEnemy[CD_CARGOSHIP].speed = 4;
	defEnemy[CD_CARGOSHIP].maxShield = 10;
	defEnemy[CD_CARGOSHIP].shield = 10;
	defEnemy[CD_CARGOSHIP].imageIndex[0] = 22;
	defEnemy[CD_CARGOSHIP].imageIndex[1] = 23;
	defEnemy[CD_CARGOSHIP].weaponType[0] = W_DOUBLE_SHOT;
	defEnemy[CD_CARGOSHIP].weaponType[1] = W_DOUBLE_SHOT;
	defEnemy[CD_CARGOSHIP].chance[0] = 0;
	defEnemy[CD_CARGOSHIP].chance[1] = 0;
	defEnemy[CD_CARGOSHIP].score = 25;
	defEnemy[CD_CARGOSHIP].collectChance = 50;
	defEnemy[CD_CARGOSHIP].collectType = P_ANYTHING;
	defEnemy[CD_CARGOSHIP].collectValue = 100;
	defEnemy[CD_CARGOSHIP].flags = FL_WEAPCO | FL_NOFIRE;

	// Weapco Miner
	defEnemy[CD_MINER].classDef = CD_MINER;
	defEnemy[CD_MINER].AIType = AI_EVASIVE;
	defEnemy[CD_MINER].speed = 4;
	defEnemy[CD_MINER].maxShield = 25;
	defEnemy[CD_MINER].shield = 25;
	defEnemy[CD_MINER].imageIndex[0] = 16;
	defEnemy[CD_MINER].imageIndex[1] = 17;
	defEnemy[CD_MINER].weaponType[0] = W_DOUBLE_SHOT;
	defEnemy[CD_MINER].weaponType[1] = W_DOUBLE_SHOT;
	defEnemy[CD_MINER].chance[0] = 0;
	defEnemy[CD_MINER].chance[1] = 0;
	defEnemy[CD_MINER].score = 100;
	defEnemy[CD_MINER].collectChance = 100;
	defEnemy[CD_MINER].collectType = P_ANYTHING;
	defEnemy[CD_MINER].collectValue = 30;
	defEnemy[CD_MINER].flags = FL_WEAPCO | FL_NOFIRE | FL_DROPMINES;

	// Kline
	defEnemy[CD_KLINE].classDef = CD_KLINE;
	defEnemy[CD_KLINE].AIType = AI_OFFENSIVE;
	defEnemy[CD_KLINE].speed = 5;
	defEnemy[CD_KLINE].maxShield = 750;
	defEnemy[CD_KLINE].shield = 750;
	defEnemy[CD_KLINE].imageIndex[0] = 18;
	defEnemy[CD_KLINE].imageIndex[1] = 19;
	defEnemy[CD_KLINE].weaponType[0] = W_TRIPLE_SHOT;
	defEnemy[CD_KLINE].weaponType[1] = W_MICRO_ROCKETS;
	defEnemy[CD_KLINE].chance[0] = 100;
	defEnemy[CD_KLINE].chance[1] = 2;
	defEnemy[CD_KLINE].score = 0;
	defEnemy[CD_KLINE].collectChance = 0;
	defEnemy[CD_KLINE].collectType = P_ANYTHING;
	defEnemy[CD_KLINE].collectValue = 0;
	defEnemy[CD_KLINE].flags = FL_WEAPCO | FL_CANNOTDIE | FL_ALWAYSFACE | FL_CIRCLES;

	// Aim Fighter
	defEnemy[CD_AIMFIGHTER].classDef = CD_AIMFIGHTER;
	defEnemy[CD_AIMFIGHTER].AIType = AI_NORMAL;
	defEnemy[CD_AIMFIGHTER].speed = 3;
	defEnemy[CD_AIMFIGHTER].maxShield = 15;
	defEnemy[CD_AIMFIGHTER].shield = 15;
	defEnemy[CD_AIMFIGHTER].imageIndex[0] = 8;
	defEnemy[CD_AIMFIGHTER].imageIndex[1] = 9;
	defEnemy[CD_AIMFIGHTER].weaponType[0] = W_AIMED_SHOT;
	defEnemy[CD_AIMFIGHTER].weaponType[1] = W_AIMED_SHOT;
	defEnemy[CD_AIMFIGHTER].chance[0] = 7;
	defEnemy[CD_AIMFIGHTER].chance[1] = 1;
	defEnemy[CD_AIMFIGHTER].score = 50;
	defEnemy[CD_AIMFIGHTER].collectChance = 75;
	defEnemy[CD_AIMFIGHTER].collectType = P_ANYTHING;
	defEnemy[CD_AIMFIGHTER].collectValue = 100;
	defEnemy[CD_AIMFIGHTER].flags = FL_WEAPCO | FL_AIMS;

	// Slave ship
	defEnemy[CD_SLAVETRANSPORT].classDef = CD_SLAVETRANSPORT;
	defEnemy[CD_SLAVETRANSPORT].AIType = AI_EVASIVE;
	defEnemy[CD_SLAVETRANSPORT].speed = 2;
	defEnemy[CD_SLAVETRANSPORT].maxShield = 10;
	defEnemy[CD_SLAVETRANSPORT].shield = 20;
	defEnemy[CD_SLAVETRANSPORT].imageIndex[0] = 10;
	defEnemy[CD_SLAVETRANSPORT].imageIndex[1] = 11;
	defEnemy[CD_SLAVETRANSPORT].weaponType[0] = W_DOUBLE_SHOT;
	defEnemy[CD_SLAVETRANSPORT].weaponType[1] = W_DOUBLE_SHOT;
	defEnemy[CD_SLAVETRANSPORT].chance[0] = 0;
	defEnemy[CD_SLAVETRANSPORT].chance[1] = 0;
	defEnemy[CD_SLAVETRANSPORT].score = 25;
	defEnemy[CD_SLAVETRANSPORT].collectChance = 100;
	defEnemy[CD_SLAVETRANSPORT].collectType = P_SLAVES;
	defEnemy[CD_SLAVETRANSPORT].collectValue = 25;
	defEnemy[CD_SLAVETRANSPORT].flags = FL_WEAPCO | FL_NOFIRE;

	// Good Transport
	defEnemy[CD_GOODTRANSPORT].classDef = CD_GOODTRANSPORT;
	defEnemy[CD_GOODTRANSPORT].AIType = AI_EVASIVE;
	defEnemy[CD_GOODTRANSPORT].speed = 3;
	defEnemy[CD_GOODTRANSPORT].maxShield = 75;
	defEnemy[CD_GOODTRANSPORT].shield = 75;
	defEnemy[CD_GOODTRANSPORT].imageIndex[0] = 12;
	defEnemy[CD_GOODTRANSPORT].imageIndex[1] = 13;
	defEnemy[CD_GOODTRANSPORT].weaponType[0] = W_AIMED_SHOT;
	defEnemy[CD_GOODTRANSPORT].weaponType[1] = W_AIMED_SHOT;
	defEnemy[CD_GOODTRANSPORT].chance[0] = 100;
	defEnemy[CD_GOODTRANSPORT].chance[1] = 100;
	defEnemy[CD_GOODTRANSPORT].score = 0;
	defEnemy[CD_GOODTRANSPORT].collectChance = 0;
	defEnemy[CD_GOODTRANSPORT].collectType = P_ANYTHING;
	defEnemy[CD_GOODTRANSPORT].collectValue = 0;
	defEnemy[CD_GOODTRANSPORT].flags = FL_FRIEND | FL_NOFIRE | FL_AIMS;

	// Sid Wilson
	defEnemy[CD_SID].classDef = CD_SID;
	defEnemy[CD_SID].AIType = AI_NORMAL;
	defEnemy[CD_SID].speed = 3;
	defEnemy[CD_SID].maxShield = 50;
	defEnemy[CD_SID].shield = 50;
	defEnemy[CD_SID].imageIndex[0] = 24;
	defEnemy[CD_SID].imageIndex[1] = 25;
	defEnemy[CD_SID].weaponType[0] = W_IONCANNON;
	defEnemy[CD_SID].weaponType[1] = W_IONCANNON;
	defEnemy[CD_SID].chance[0] = 100;
	defEnemy[CD_SID].chance[1] = 0;
	defEnemy[CD_SID].score = 0;
	defEnemy[CD_SID].collectChance = 0;
	defEnemy[CD_SID].collectType = P_ANYTHING;
	defEnemy[CD_SID].collectValue = 0;
	defEnemy[CD_SID].flags = FL_FRIEND | FL_AIMS;

	// Mining Vessel Boss
	defEnemy[CD_MINEBOSS].classDef = CD_BOSS;
	defEnemy[CD_MINEBOSS].AIType = AI_NORMAL;
	defEnemy[CD_MINEBOSS].speed = 3;
	defEnemy[CD_MINEBOSS].maxShield = 1000;
	defEnemy[CD_MINEBOSS].shield = 1000;
	defEnemy[CD_MINEBOSS].imageIndex[0] = 26;
	defEnemy[CD_MINEBOSS].imageIndex[1] = 27;
	defEnemy[CD_MINEBOSS].weaponType[0] = W_TRIPLE_SHOT;
	defEnemy[CD_MINEBOSS].weaponType[1] = W_SPREADSHOT;
	defEnemy[CD_MINEBOSS].chance[0] = 0;
	defEnemy[CD_MINEBOSS].chance[1] = 0;
	defEnemy[CD_MINEBOSS].score = 1000;
	defEnemy[CD_MINEBOSS].collectChance = 100;
	defEnemy[CD_MINEBOSS].collectType = P_ANYTHING;
	defEnemy[CD_MINEBOSS].collectValue = 255;
	defEnemy[CD_MINEBOSS].flags = FL_WEAPCO | FL_IMMORTAL;

	defEnemy[CD_BOSS2_WING1].classDef = CD_BOSS2_WING1;
	defEnemy[CD_BOSS2_WING1].AIType = AI_NORMAL;
	defEnemy[CD_BOSS2_WING1].speed = 1;
	defEnemy[CD_BOSS2_WING1].maxShield = 250;
	defEnemy[CD_BOSS2_WING1].shield = 250;
	defEnemy[CD_BOSS2_WING1].imageIndex[0] = 28;
	defEnemy[CD_BOSS2_WING1].imageIndex[1] = 29;
	defEnemy[CD_BOSS2_WING1].weaponType[0] = W_TRIPLE_SHOT;
	defEnemy[CD_BOSS2_WING1].weaponType[1] = W_SPREADSHOT;
	defEnemy[CD_BOSS2_WING1].chance[0] = 0;
	defEnemy[CD_BOSS2_WING1].chance[1] = 0;
	defEnemy[CD_BOSS2_WING1].score = 1000;
	defEnemy[CD_BOSS2_WING1].collectChance = 100;
	defEnemy[CD_BOSS2_WING1].collectType = P_ANYTHING;
	defEnemy[CD_BOSS2_WING1].collectValue = 255;
	defEnemy[CD_BOSS2_WING1].flags = FL_WEAPCO | FL_DAMAGEOWNER;

	defEnemy[CD_BOSS2_WING2].classDef = CD_BOSS2_WING2;
	defEnemy[CD_BOSS2_WING2].AIType = AI_NORMAL;
	defEnemy[CD_BOSS2_WING2].speed = 1;
	defEnemy[CD_BOSS2_WING2].maxShield = 500;
	defEnemy[CD_BOSS2_WING2].shield = 500;
	defEnemy[CD_BOSS2_WING2].imageIndex[0] = 30;
	defEnemy[CD_BOSS2_WING2].imageIndex[1] = 31;
	defEnemy[CD_BOSS2_WING2].weaponType[0] = W_TRIPLE_SHOT;
	defEnemy[CD_BOSS2_WING2].weaponType[1] = W_SPREADSHOT;
	defEnemy[CD_BOSS2_WING2].chance[0] = 0;
	defEnemy[CD_BOSS2_WING2].chance[1] = 0;
	defEnemy[CD_BOSS2_WING2].score = 1000;
	defEnemy[CD_BOSS2_WING2].collectChance = 100;
	defEnemy[CD_BOSS2_WING2].collectType = P_ANYTHING;
	defEnemy[CD_BOSS2_WING2].collectValue = 255;
	defEnemy[CD_BOSS2_WING2].flags = FL_WEAPCO | FL_DEPLOYDRONES | FL_DAMAGEOWNER;

	defEnemy[CD_BOSS2_WING3].classDef = CD_BOSS2_WING3;
	defEnemy[CD_BOSS2_WING3].AIType = AI_NORMAL;
	defEnemy[CD_BOSS2_WING3].speed = 1;
	defEnemy[CD_BOSS2_WING3].maxShield = 500;
	defEnemy[CD_BOSS2_WING3].shield = 500;
	defEnemy[CD_BOSS2_WING3].imageIndex[0] = 32;
	defEnemy[CD_BOSS2_WING3].imageIndex[1] = 33;
	defEnemy[CD_BOSS2_WING3].weaponType[0] = W_TRIPLE_SHOT;
	defEnemy[CD_BOSS2_WING3].weaponType[1] = W_SPREADSHOT;
	defEnemy[CD_BOSS2_WING3].chance[0] = 0;
	defEnemy[CD_BOSS2_WING3].chance[1] = 0;
	defEnemy[CD_BOSS2_WING3].score = 1000;
	defEnemy[CD_BOSS2_WING3].collectChance = 100;
	defEnemy[CD_BOSS2_WING3].collectType = P_ANYTHING;
	defEnemy[CD_BOSS2_WING3].collectValue = 255;
	defEnemy[CD_BOSS2_WING3].flags = FL_WEAPCO | FL_DEPLOYDRONES | FL_DAMAGEOWNER;

	defEnemy[CD_BOSS2_WING4].classDef = CD_BOSS2_WING4;
	defEnemy[CD_BOSS2_WING4].AIType = AI_NORMAL;
	defEnemy[CD_BOSS2_WING4].speed = 1;
	defEnemy[CD_BOSS2_WING4].maxShield = 250;
	defEnemy[CD_BOSS2_WING4].shield = 250;
	defEnemy[CD_BOSS2_WING4].imageIndex[0] = 34;
	defEnemy[CD_BOSS2_WING4].imageIndex[1] = 35;
	defEnemy[CD_BOSS2_WING4].weaponType[0] = W_TRIPLE_SHOT;
	defEnemy[CD_BOSS2_WING4].weaponType[1] = W_SPREADSHOT;
	defEnemy[CD_BOSS2_WING4].chance[0] = 0;
	defEnemy[CD_BOSS2_WING4].chance[1] = 0;
	defEnemy[CD_BOSS2_WING4].score = 1000;
	defEnemy[CD_BOSS2_WING4].collectChance = 100;
	defEnemy[CD_BOSS2_WING4].collectType = P_ANYTHING;
	defEnemy[CD_BOSS2_WING4].collectValue = 255;
	defEnemy[CD_BOSS2_WING4].flags = FL_WEAPCO | FL_DAMAGEOWNER;

	// Drone
	defEnemy[CD_DRONE].classDef = CD_DRONE;
	defEnemy[CD_DRONE].AIType = AI_OFFENSIVE;
	defEnemy[CD_DRONE].speed = 8;
	defEnemy[CD_DRONE].maxShield = 5;
	defEnemy[CD_DRONE].shield = 5;
	defEnemy[CD_DRONE].imageIndex[0] = 36;
	defEnemy[CD_DRONE].imageIndex[1] = 37;
	defEnemy[CD_DRONE].weaponType[0] = W_DOUBLE_SHOT;
	defEnemy[CD_DRONE].weaponType[1] = W_LASER;
	defEnemy[CD_DRONE].chance[0] = 100;
	defEnemy[CD_DRONE].chance[1] = 0;
	defEnemy[CD_DRONE].score = 0;
	defEnemy[CD_DRONE].collectChance = 10;
	defEnemy[CD_DRONE].collectType = P_SHIELD;
	defEnemy[CD_DRONE].collectValue = 1;
	defEnemy[CD_DRONE].flags = FL_WEAPCO;

	// Experimental Fighter
	defEnemy[CD_CLOAKFIGHTER].classDef = CD_CLOAKFIGHTER;
	defEnemy[CD_CLOAKFIGHTER].AIType = AI_OFFENSIVE;
	defEnemy[CD_CLOAKFIGHTER].speed = 6;
	defEnemy[CD_CLOAKFIGHTER].maxShield = 1000;
	defEnemy[CD_CLOAKFIGHTER].shield = 1000;
	defEnemy[CD_CLOAKFIGHTER].imageIndex[0] = 10;
	defEnemy[CD_CLOAKFIGHTER].imageIndex[1] = 11;
	defEnemy[CD_CLOAKFIGHTER].weaponType[0] = W_SPREADSHOT;
	defEnemy[CD_CLOAKFIGHTER].weaponType[1] = W_DOUBLE_ROCKETS;
	defEnemy[CD_CLOAKFIGHTER].chance[0] = 100;
	defEnemy[CD_CLOAKFIGHTER].chance[1] = 5;
	defEnemy[CD_CLOAKFIGHTER].score = 550;
	defEnemy[CD_CLOAKFIGHTER].collectChance = 100;
	defEnemy[CD_CLOAKFIGHTER].collectType = P_CASH;
	defEnemy[CD_CLOAKFIGHTER].collectValue = 255;
	defEnemy[CD_CLOAKFIGHTER].flags = FL_WEAPCO | FL_CANCLOAK | FL_RUNSAWAY;

	// Evil Ursula
	defEnemy[CD_EVILURSULA].classDef = CD_EVILURSULA;
	defEnemy[CD_EVILURSULA].AIType = AI_OFFENSIVE;
	defEnemy[CD_EVILURSULA].speed = 5;
	defEnemy[CD_EVILURSULA].maxShield = 500;
	defEnemy[CD_EVILURSULA].shield = 500;
	defEnemy[CD_EVILURSULA].imageIndex[0] = 12;
	defEnemy[CD_EVILURSULA].imageIndex[1] = 13;
	defEnemy[CD_EVILURSULA].weaponType[0] = W_TRIPLE_SHOT;
	defEnemy[CD_EVILURSULA].weaponType[1] = W_HOMING_MISSILE;
	defEnemy[CD_EVILURSULA].chance[0] = 100;
	defEnemy[CD_EVILURSULA].chance[1] = 100;
	defEnemy[CD_EVILURSULA].score = 500;
	defEnemy[CD_EVILURSULA].collectChance = 100;
	defEnemy[CD_EVILURSULA].collectType = P_ESCAPEPOD;
	defEnemy[CD_EVILURSULA].collectValue = 1;
	defEnemy[CD_EVILURSULA].flags = FL_WEAPCO;

	// Mercenary
	defEnemy[CD_KRASS].classDef = CD_KRASS;
	defEnemy[CD_KRASS].AIType = AI_OFFENSIVE;
	defEnemy[CD_KRASS].speed = 5;
	defEnemy[CD_KRASS].maxShield = 1000;
	defEnemy[CD_KRASS].shield = 1000;
	defEnemy[CD_KRASS].imageIndex[0] = 26;
	defEnemy[CD_KRASS].imageIndex[1] = 27;
	defEnemy[CD_KRASS].weaponType[0] = W_SPREADSHOT;
	defEnemy[CD_KRASS].weaponType[1] = W_CHARGER;
	defEnemy[CD_KRASS].chance[0] = 100;
	defEnemy[CD_KRASS].chance[1] = 0;
	defEnemy[CD_KRASS].score = 2000;
	defEnemy[CD_KRASS].collectChance = 100;
	defEnemy[CD_KRASS].collectType = P_ANYTHING;
	defEnemy[CD_KRASS].collectValue = 255;
	defEnemy[CD_KRASS].flags = FL_FRIEND | FL_IMMORTAL;

	// Executive Transport
	defEnemy[CD_EXEC].classDef = CD_BOSS;
	defEnemy[CD_EXEC].AIType = AI_NORMAL;
	defEnemy[CD_EXEC].speed = 5;
	defEnemy[CD_EXEC].maxShield = 1000;
	defEnemy[CD_EXEC].shield = 1000;
	defEnemy[CD_EXEC].imageIndex[0] = 28;
	defEnemy[CD_EXEC].imageIndex[1] = 28;
	defEnemy[CD_EXEC].weaponType[0] = W_SPREADSHOT;
	defEnemy[CD_EXEC].weaponType[1] = W_HOMING_MISSILE;
	defEnemy[CD_EXEC].chance[0] = 0;
	defEnemy[CD_EXEC].chance[1] = 0;
	defEnemy[CD_EXEC].score = 2000;
	defEnemy[CD_EXEC].collectChance = 0;
	defEnemy[CD_EXEC].collectType = P_ANYTHING;
	defEnemy[CD_EXEC].collectValue = 0;
	defEnemy[CD_EXEC].flags = FL_WEAPCO | FL_NOFIRE;

	// Asteroid
	defEnemy[CD_ASTEROID].classDef = CD_ASTEROID;
	defEnemy[CD_ASTEROID].AIType = AI_WANDER;
	defEnemy[CD_ASTEROID].speed = 1;
	defEnemy[CD_ASTEROID].maxShield = 50;
	defEnemy[CD_ASTEROID].shield = 50;
	defEnemy[CD_ASTEROID].imageIndex[0] = 38;
	defEnemy[CD_ASTEROID].imageIndex[1] = 38;
	defEnemy[CD_ASTEROID].weaponType[0] = W_SPREADSHOT;
	defEnemy[CD_ASTEROID].weaponType[1] = W_HOMING_MISSILE;
	defEnemy[CD_ASTEROID].chance[0] = 0;
	defEnemy[CD_ASTEROID].chance[1] = 0;
	defEnemy[CD_ASTEROID].score = 0;
	defEnemy[CD_ASTEROID].collectChance = 25;
	defEnemy[CD_ASTEROID].collectType = P_ORE;
	defEnemy[CD_ASTEROID].collectValue = 1;
	defEnemy[CD_ASTEROID].flags = FL_WEAPCO;

	defEnemy[CD_ASTEROID2].classDef = CD_ASTEROID2;
	defEnemy[CD_ASTEROID2].AIType = AI_WANDER;
	defEnemy[CD_ASTEROID2].speed = 1;
	defEnemy[CD_ASTEROID2].maxShield = 10;
	defEnemy[CD_ASTEROID2].shield = 10;
	defEnemy[CD_ASTEROID2].imageIndex[0] = 39;
	defEnemy[CD_ASTEROID2].imageIndex[1] = 40;
	defEnemy[CD_ASTEROID2].weaponType[0] = W_SPREADSHOT;
	defEnemy[CD_ASTEROID2].weaponType[1] = W_HOMING_MISSILE;
	defEnemy[CD_ASTEROID2].chance[0] = 0;
	defEnemy[CD_ASTEROID2].chance[1] = 0;
	defEnemy[CD_ASTEROID2].score = 0;
	defEnemy[CD_ASTEROID2].collectChance = 25;
	defEnemy[CD_ASTEROID2].collectType = P_ORE;
	defEnemy[CD_ASTEROID2].collectValue = 1;
	defEnemy[CD_ASTEROID2].flags = FL_WEAPCO;

	// Escort
	defEnemy[CD_ESCORT].classDef = CD_ESCORT;
	defEnemy[CD_ESCORT].AIType = AI_NORMAL;
	defEnemy[CD_ESCORT].speed = 3;
	defEnemy[CD_ESCORT].maxShield = 200;
	defEnemy[CD_ESCORT].shield = 200;
	defEnemy[CD_ESCORT].imageIndex[0] = 30;
	defEnemy[CD_ESCORT].imageIndex[1] = 31;
	defEnemy[CD_ESCORT].weaponType[0] = W_LASER;
	defEnemy[CD_ESCORT].weaponType[1] = W_LASER;
	defEnemy[CD_ESCORT].chance[0] = 25;
	defEnemy[CD_ESCORT].chance[1] = 25;
	defEnemy[CD_ESCORT].score = 450;
	defEnemy[CD_ESCORT].collectChance = 100;
	defEnemy[CD_ESCORT].collectType = P_ANYTHING;
	defEnemy[CD_ESCORT].collectValue = 100;
	defEnemy[CD_ESCORT].flags = FL_WEAPCO;

	// Mobile Ray Cannon
	defEnemy[CD_MOBILE_RAY].classDef = CD_MOBILE_RAY;
	defEnemy[CD_MOBILE_RAY].AIType = AI_OFFENSIVE;
	defEnemy[CD_MOBILE_RAY].speed = 5;
	defEnemy[CD_MOBILE_RAY].maxShield = 250;
	defEnemy[CD_MOBILE_RAY].shield = 250;
	defEnemy[CD_MOBILE_RAY].imageIndex[0] = 10;
	defEnemy[CD_MOBILE_RAY].imageIndex[1] = 11;
	defEnemy[CD_MOBILE_RAY].weaponType[0] = W_ENERGYRAY;
	defEnemy[CD_MOBILE_RAY].weaponType[1] = W_ENERGYRAY;
	defEnemy[CD_MOBILE_RAY].chance[0] = 50;
	defEnemy[CD_MOBILE_RAY].chance[1] = 50;
	defEnemy[CD_MOBILE_RAY].score = 1000;
	defEnemy[CD_MOBILE_RAY].collectChance = 75;
	defEnemy[CD_MOBILE_RAY].collectType = P_SHIELD;
	defEnemy[CD_MOBILE_RAY].collectValue = 2;
	defEnemy[CD_MOBILE_RAY].flags = FL_WEAPCO;

	// Rebel Carrier
	defEnemy[CD_REBELCARRIER].classDef = CD_REBELCARRIER;
	defEnemy[CD_REBELCARRIER].AIType = AI_OFFENSIVE;
	defEnemy[CD_REBELCARRIER].speed = 2;
	defEnemy[CD_REBELCARRIER].maxShield = 100;
	defEnemy[CD_REBELCARRIER].shield = 100;
	defEnemy[CD_REBELCARRIER].imageIndex[0] = 32;
	defEnemy[CD_REBELCARRIER].imageIndex[1] = 33;
	defEnemy[CD_REBELCARRIER].weaponType[0] = W_DOUBLE_ROCKETS;
	defEnemy[CD_REBELCARRIER].weaponType[1] = W_MICRO_ROCKETS;
	defEnemy[CD_REBELCARRIER].chance[0] = 50;
	defEnemy[CD_REBELCARRIER].chance[1] = 2;
	defEnemy[CD_REBELCARRIER].score = 0;
	defEnemy[CD_REBELCARRIER].collectChance = 0;
	defEnemy[CD_REBELCARRIER].collectType = P_SHIELD;
	defEnemy[CD_REBELCARRIER].collectValue = 0;
	defEnemy[CD_REBELCARRIER].flags = FL_FRIEND;

	// Pluto Boss
	defEnemy[CD_PLUTOBOSS].classDef = CD_PLUTOBOSS;
	defEnemy[CD_PLUTOBOSS].AIType = AI_OFFENSIVE;
	defEnemy[CD_PLUTOBOSS].speed = 4;
	defEnemy[CD_PLUTOBOSS].maxShield = 500;
	defEnemy[CD_PLUTOBOSS].shield = 500;
	defEnemy[CD_PLUTOBOSS].imageIndex[0] = 12;
	defEnemy[CD_PLUTOBOSS].imageIndex[1] = 13;
	defEnemy[CD_PLUTOBOSS].weaponType[0] = W_DOUBLE_ROCKETS;
	defEnemy[CD_PLUTOBOSS].weaponType[1] = W_MICRO_ROCKETS;
	defEnemy[CD_PLUTOBOSS].chance[0] = 50;
	defEnemy[CD_PLUTOBOSS].chance[1] = 2;
	defEnemy[CD_PLUTOBOSS].score = 1000;
	defEnemy[CD_PLUTOBOSS].collectChance = 0;
	defEnemy[CD_PLUTOBOSS].collectType = P_SHIELD;
	defEnemy[CD_PLUTOBOSS].collectValue = 0;
	defEnemy[CD_PLUTOBOSS].flags = FL_WEAPCO;

	// Pluto Boss Barrier
	defEnemy[CD_BARRIER].classDef = CD_BARRIER;
	defEnemy[CD_BARRIER].AIType = AI_OFFENSIVE;
	defEnemy[CD_BARRIER].speed = 1;
	defEnemy[CD_BARRIER].maxShield = 250;
	defEnemy[CD_BARRIER].shield = 250;
	defEnemy[CD_BARRIER].imageIndex[0] = 32;
	defEnemy[CD_BARRIER].imageIndex[1] = 33;
	defEnemy[CD_BARRIER].weaponType[0] = W_DOUBLE_SHOT;
	defEnemy[CD_BARRIER].weaponType[1] = W_MICRO_ROCKETS;
	defEnemy[CD_BARRIER].chance[0] = 0;
	defEnemy[CD_BARRIER].chance[1] = 0;
	defEnemy[CD_BARRIER].score = 1000;
	defEnemy[CD_BARRIER].collectChance = 100;
	defEnemy[CD_BARRIER].collectType = P_ANYTHING;
	defEnemy[CD_BARRIER].collectValue = 25;
	defEnemy[CD_BARRIER].flags = FL_WEAPCO | FL_NOFIRE;

	// Neptune Boss
	defEnemy[CD_NEPTUNEBOSS].classDef = CD_NEPTUNEBOSS;
	defEnemy[CD_NEPTUNEBOSS].AIType = AI_OFFENSIVE;
	defEnemy[CD_NEPTUNEBOSS].speed = 4;
	defEnemy[CD_NEPTUNEBOSS].maxShield = 800;
	defEnemy[CD_NEPTUNEBOSS].shield = 800;
	defEnemy[CD_NEPTUNEBOSS].imageIndex[0] = 12;
	defEnemy[CD_NEPTUNEBOSS].imageIndex[1] = 13;
	defEnemy[CD_NEPTUNEBOSS].weaponType[0] = W_DOUBLE_SHOT;
	defEnemy[CD_NEPTUNEBOSS].weaponType[1] = W_MICRO_ROCKETS;
	defEnemy[CD_NEPTUNEBOSS].chance[0] = 100;
	defEnemy[CD_NEPTUNEBOSS].chance[1] = 0;
	defEnemy[CD_NEPTUNEBOSS].score = 1000;
	defEnemy[CD_NEPTUNEBOSS].collectChance = 100;
	defEnemy[CD_NEPTUNEBOSS].collectType = P_ANYTHING;
	defEnemy[CD_NEPTUNEBOSS].collectValue = 25;
	defEnemy[CD_NEPTUNEBOSS].flags = FL_WEAPCO;

	// Mobile Shield
	defEnemy[CD_MOBILESHIELD].classDef = CD_MOBILESHIELD;
	defEnemy[CD_MOBILESHIELD].AIType = AI_EVASIVE;
	defEnemy[CD_MOBILESHIELD].speed = 6;
	defEnemy[CD_MOBILESHIELD].maxShield = 150;
	defEnemy[CD_MOBILESHIELD].shield = 150;
	defEnemy[CD_MOBILESHIELD].imageIndex[0] = 34;
	defEnemy[CD_MOBILESHIELD].imageIndex[1] = 35;
	defEnemy[CD_MOBILESHIELD].weaponType[0] = W_DOUBLE_SHOT;
	defEnemy[CD_MOBILESHIELD].weaponType[1] = W_MICRO_ROCKETS;
	defEnemy[CD_MOBILESHIELD].chance[0] = 0;
	defEnemy[CD_MOBILESHIELD].chance[1] = 0;
	defEnemy[CD_MOBILESHIELD].score = 250;
	defEnemy[CD_MOBILESHIELD].collectChance = 100;
	defEnemy[CD_MOBILESHIELD].collectType = P_ANYTHING;
	defEnemy[CD_MOBILESHIELD].collectValue = 25;
	defEnemy[CD_MOBILESHIELD].flags = FL_WEAPCO | FL_NOFIRE;

	// Firefly
	defEnemy[CD_FIREFLY].classDef = CD_FIREFLY;
	defEnemy[CD_FIREFLY].AIType = AI_OFFENSIVE;
	defEnemy[CD_FIREFLY].speed = 5;
	defEnemy[CD_FIREFLY].maxShield = 250;
	defEnemy[CD_FIREFLY].shield = 250;
	defEnemy[CD_FIREFLY].imageIndex[0] = 0;
	defEnemy[CD_FIREFLY].imageIndex[1] = 1;
	defEnemy[CD_FIREFLY].weaponType[0] = W_TRIPLE_SHOT;
	defEnemy[CD_FIREFLY].weaponType[1] = W_DOUBLE_ROCKETS;
	defEnemy[CD_FIREFLY].chance[0] = 100;
	defEnemy[CD_FIREFLY].chance[1] = 5;
	defEnemy[CD_FIREFLY].score = 500;
	defEnemy[CD_FIREFLY].collectChance = 100;
	defEnemy[CD_FIREFLY].collectType = P_ANYTHING;
	defEnemy[CD_FIREFLY].collectValue = 250;
	defEnemy[CD_FIREFLY].flags = FL_WEAPCO;

	// Uranus Boss
	defEnemy[CD_URANUSBOSS].classDef = CD_URANUSBOSS;
	defEnemy[CD_URANUSBOSS].AIType = AI_OFFENSIVE;
	defEnemy[CD_URANUSBOSS].speed = 4;
	defEnemy[CD_URANUSBOSS].maxShield = 750;
	defEnemy[CD_URANUSBOSS].shield = 750;
	defEnemy[CD_URANUSBOSS].imageIndex[0] = 41;
	defEnemy[CD_URANUSBOSS].imageIndex[1] = 42;
	defEnemy[CD_URANUSBOSS].weaponType[0] = W_SPREADSHOT;
	defEnemy[CD_URANUSBOSS].weaponType[1] = W_DOUBLE_ROCKETS;
	defEnemy[CD_URANUSBOSS].chance[0] = 100;
	defEnemy[CD_URANUSBOSS].chance[1] = 5;
	defEnemy[CD_URANUSBOSS].score = 500;
	defEnemy[CD_URANUSBOSS].collectChance = 100;
	defEnemy[CD_URANUSBOSS].collectType = P_ANYTHING;
	defEnemy[CD_URANUSBOSS].collectValue = 250;
	defEnemy[CD_URANUSBOSS].flags = FL_WEAPCO;

	// Uranus Boss Wing 1
	defEnemy[CD_URANUSBOSSWING1].classDef = CD_URANUSBOSSWING1;
	defEnemy[CD_URANUSBOSSWING1].AIType = AI_OFFENSIVE;
	defEnemy[CD_URANUSBOSSWING1].speed = 4;
	defEnemy[CD_URANUSBOSSWING1].maxShield = 250;
	defEnemy[CD_URANUSBOSSWING1].shield = 250;
	defEnemy[CD_URANUSBOSSWING1].imageIndex[0] = 43;
	defEnemy[CD_URANUSBOSSWING1].imageIndex[1] = 44;
	defEnemy[CD_URANUSBOSSWING1].weaponType[0] = W_DOUBLE_ROCKETS;
	defEnemy[CD_URANUSBOSSWING1].weaponType[1] = W_DOUBLE_ROCKETS;
	defEnemy[CD_URANUSBOSSWING1].chance[0] = 5;
	defEnemy[CD_URANUSBOSSWING1].chance[1] = 0;
	defEnemy[CD_URANUSBOSSWING1].score = 500;
	defEnemy[CD_URANUSBOSSWING1].collectChance = 100;
	defEnemy[CD_URANUSBOSSWING1].collectType = P_ANYTHING;
	defEnemy[CD_URANUSBOSSWING1].collectValue = 250;
	defEnemy[CD_URANUSBOSSWING1].flags = FL_WEAPCO | FL_IMMORTAL;

	// Uranus Boss Wing 2
	defEnemy[CD_URANUSBOSSWING2].classDef = CD_URANUSBOSSWING2;
	defEnemy[CD_URANUSBOSSWING2].AIType = AI_OFFENSIVE;
	defEnemy[CD_URANUSBOSSWING2].speed = 4;
	defEnemy[CD_URANUSBOSSWING2].maxShield = 250;
	defEnemy[CD_URANUSBOSSWING2].shield = 250;
	defEnemy[CD_URANUSBOSSWING2].imageIndex[0] = 45;
	defEnemy[CD_URANUSBOSSWING2].imageIndex[1] = 46;
	defEnemy[CD_URANUSBOSSWING2].weaponType[0] = W_DOUBLE_ROCKETS;
	defEnemy[CD_URANUSBOSSWING2].weaponType[1] = W_DOUBLE_ROCKETS;
	defEnemy[CD_URANUSBOSSWING2].chance[0] = 5;
	defEnemy[CD_URANUSBOSSWING2].chance[1] = 0;
	defEnemy[CD_URANUSBOSSWING2].score = 500;
	defEnemy[CD_URANUSBOSSWING2].collectChance = 100;
	defEnemy[CD_URANUSBOSSWING2].collectType = P_ANYTHING;
	defEnemy[CD_URANUSBOSSWING2].collectValue = 250;
	defEnemy[CD_URANUSBOSSWING2].flags = FL_WEAPCO | FL_IMMORTAL;

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
