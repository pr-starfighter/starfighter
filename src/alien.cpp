/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012 Guus Sliepen
Copyright (C) 2012, 2015, 2016 onpon4 <onpon4@riseup.net>

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

object alien_defs[CD_MAX];
object aliens[ALIEN_MAX];

/*
This simply pulls back an alien from the array that is
"dead" (no shield) and returns the index number so we can have
a new one.
*/
static int alien_getFreeIndex()
{
	for (int i = 0 ; i < engine.maxAliens ; i++)
	{
		if (!aliens[i].active)
		{
			return i;
		}
	}

	return -1;
}

void alien_defs_init()
{
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
	alien_defs[CD_FRIGATE].collectChance = 100;
	alien_defs[CD_FRIGATE].collectType = P_CASH;
	alien_defs[CD_FRIGATE].collectValue = 500;
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
	alien_defs[CD_TRANSPORTSHIP].collectChance = 100;
	alien_defs[CD_TRANSPORTSHIP].collectType = P_WEAPONS;
	alien_defs[CD_TRANSPORTSHIP].collectValue = 40;
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
	alien_defs[CD_MINER].collectChance = 100;
	alien_defs[CD_MINER].collectType = P_ANYTHING;
	alien_defs[CD_MINER].collectValue = 30;
	alien_defs[CD_MINER].flags = FL_WEAPCO | FL_NOFIRE | FL_DROPMINES;

	// Kline
	alien_defs[CD_KLINE].classDef = CD_KLINE;
	alien_defs[CD_KLINE].AIType = AI_OFFENSIVE;
	alien_defs[CD_KLINE].speed = 5;
	alien_defs[CD_KLINE].maxShield = 2000;
	alien_defs[CD_KLINE].shield = 2000;
	alien_defs[CD_KLINE].imageIndex[0] = 18;
	alien_defs[CD_KLINE].imageIndex[1] = 19;
	alien_defs[CD_KLINE].weaponType[0] = W_TRIPLE_SHOT;
	alien_defs[CD_KLINE].weaponType[1] = W_MICRO_ROCKETS;
	alien_defs[CD_KLINE].chance[0] = 100;
	alien_defs[CD_KLINE].chance[1] = 2;
	alien_defs[CD_KLINE].collectChance = 0;
	alien_defs[CD_KLINE].collectType = P_ANYTHING;
	alien_defs[CD_KLINE].collectValue = 0;
	alien_defs[CD_KLINE].flags = FL_WEAPCO | FL_ALWAYSFACE | FL_CIRCLES;

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
	alien_defs[CD_MINEBOSS].collectChance = 100;
	alien_defs[CD_MINEBOSS].collectType = P_ANYTHING;
	alien_defs[CD_MINEBOSS].collectValue = 1000;
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
	alien_defs[CD_BOSS2_WING1].collectChance = 100;
	alien_defs[CD_BOSS2_WING1].collectType = P_ANYTHING;
	alien_defs[CD_BOSS2_WING1].collectValue = 250;
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
	alien_defs[CD_BOSS2_WING2].collectChance = 100;
	alien_defs[CD_BOSS2_WING2].collectType = P_ANYTHING;
	alien_defs[CD_BOSS2_WING2].collectValue = 250;
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
	alien_defs[CD_BOSS2_WING3].collectChance = 100;
	alien_defs[CD_BOSS2_WING3].collectType = P_ANYTHING;
	alien_defs[CD_BOSS2_WING3].collectValue = 250;
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
	alien_defs[CD_BOSS2_WING4].collectChance = 100;
	alien_defs[CD_BOSS2_WING4].collectType = P_ANYTHING;
	alien_defs[CD_BOSS2_WING4].collectValue = 250;
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
	alien_defs[CD_CLOAKFIGHTER].collectChance = 100;
	alien_defs[CD_CLOAKFIGHTER].collectType = P_CASH;
	alien_defs[CD_CLOAKFIGHTER].collectValue = 250;
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
	alien_defs[CD_KRASS].collectChance = 100;
	alien_defs[CD_KRASS].collectType = P_ANYTHING;
	alien_defs[CD_KRASS].collectValue = 2250;
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
	alien_defs[CD_EXEC].collectChance = 0;
	alien_defs[CD_EXEC].collectType = P_ANYTHING;
	alien_defs[CD_EXEC].collectValue = 2000;
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
	alien_defs[CD_MOBILE_RAY].collectChance = 75;
	alien_defs[CD_MOBILE_RAY].collectType = P_ANYTHING;
	alien_defs[CD_MOBILE_RAY].collectValue = 100;
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
	alien_defs[CD_PLUTOBOSS].collectChance = 0;
	alien_defs[CD_PLUTOBOSS].collectType = P_ANYTHING;
	alien_defs[CD_PLUTOBOSS].collectValue = 1000;
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
	alien_defs[CD_NEPTUNEBOSS].collectChance = 100;
	alien_defs[CD_NEPTUNEBOSS].collectType = P_ANYTHING;
	alien_defs[CD_NEPTUNEBOSS].collectValue = 1000;
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
	alien_defs[CD_URANUSBOSS].collectChance = 100;
	alien_defs[CD_URANUSBOSS].collectType = P_ANYTHING;
	alien_defs[CD_URANUSBOSS].collectValue = 500;
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
	alien_defs[CD_URANUSBOSSWING1].collectChance = 100;
	alien_defs[CD_URANUSBOSSWING1].collectType = P_ANYTHING;
	alien_defs[CD_URANUSBOSSWING1].collectValue = 250;
	alien_defs[CD_URANUSBOSSWING1].flags = FL_WEAPCO | FL_IMMORTAL | FL_DAMAGEOWNER;

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
	alien_defs[CD_URANUSBOSSWING2].collectChance = 100;
	alien_defs[CD_URANUSBOSSWING2].collectType = P_ANYTHING;
	alien_defs[CD_URANUSBOSSWING2].collectValue = 250;
	alien_defs[CD_URANUSBOSSWING2].flags = FL_WEAPCO | FL_IMMORTAL | FL_DAMAGEOWNER;
}

void aliens_init()
{
	int placeAttempt;
	int barrierSpeed;

	engine.targetIndex = -1;
	barrierSpeed = 1;

	for (int i = 0 ; i < ALIEN_MAX ; i++)
	{
		aliens[i].active = false;
		aliens[i].shield = -1;
		aliens[i].flags = 0;
	}

	switch (game.area)
	{
		case MISN_START:
			aliens[0] = alien_defs[CD_TRANSPORTSHIP];
			break;

		case MISN_CERADSE:
			aliens[0] = alien_defs[CD_CARGOSHIP];
			aliens[1] = alien_defs[CD_CARGOSHIP];
			aliens[2] = alien_defs[CD_CARGOSHIP];
			aliens[3] = alien_defs[CD_CARGOSHIP];
			aliens[4] = alien_defs[CD_CARGOSHIP];
			aliens[5] = alien_defs[CD_CARGOSHIP];
			break;

		case MISN_HINSTAG:
			aliens[0] = alien_defs[CD_MISSILEBOAT];
			aliens[1] = alien_defs[CD_MISSILEBOAT];
			aliens[2] = alien_defs[CD_MISSILEBOAT];
			aliens[3] = alien_defs[CD_MISSILEBOAT];
			aliens[4] = alien_defs[CD_MISSILEBOAT];
			break;

		case MISN_JOLDAR:
			aliens[0] = alien_defs[CD_MINER];
			aliens[1] = alien_defs[CD_MINER];
			aliens[2] = alien_defs[CD_MINER];
			aliens[3] = alien_defs[CD_MINER];
			aliens[4] = alien_defs[CD_MINER];
			aliens[5] = alien_defs[CD_MINER];
			aliens[6] = alien_defs[CD_MINER];
			aliens[7] = alien_defs[CD_MINER];
			aliens[8] = alien_defs[CD_MINER];
			break;

		case MISN_MOEBO:
			aliens[ALIEN_BOSS] = alien_defs[CD_FRIGATE];
			aliens[ALIEN_BOSS_PART1] = alien_defs[CD_FRIGATE_WING1];
			aliens[ALIEN_BOSS_PART2] = alien_defs[CD_FRIGATE_WING2];
			break;

		case MISN_NEROD:
			aliens[0] = alien_defs[CD_CARGOSHIP];
			break;

		case MISN_ALLEZ:
			aliens[ALIEN_FRIEND1] = alien_defs[CD_GOODTRANSPORT];
			break;

		case MISN_URUSOR:
			aliens[ALIEN_BOSS_PART1] = alien_defs[CD_CARGOSHIP];
			aliens[ALIEN_BOSS_PART2] = alien_defs[CD_CARGOSHIP];
			aliens[ALIEN_BOSS_PART3] = alien_defs[CD_CARGOSHIP];
			aliens[ALIEN_BOSS_PART4] = alien_defs[CD_CARGOSHIP];
			aliens[ALIEN_BOSS_PART5] = alien_defs[CD_CARGOSHIP];
			break;

		case MISN_ELAMALE:
			aliens[ALIEN_BOSS] = alien_defs[CD_MINEBOSS];
			aliens[ALIEN_BOSS_PART1] = alien_defs[CD_BOSS2_WING1];
			aliens[ALIEN_BOSS_PART2] = alien_defs[CD_BOSS2_WING2];
			aliens[ALIEN_BOSS_PART3] = alien_defs[CD_BOSS2_WING3];
			aliens[ALIEN_BOSS_PART4] = alien_defs[CD_BOSS2_WING4];
			aliens[ALIEN_KLINE] = alien_defs[CD_KLINE];
			aliens[ALIEN_BOSS_PART5] = alien_defs[CD_SLAVETRANSPORT];
			aliens[ALIEN_BOSS_PART6] = alien_defs[CD_SLAVETRANSPORT];
			break;

		case MISN_ODEON:
			aliens[ALIEN_BOSS] = alien_defs[CD_EVILURSULA];
			break;

		case MISN_FELLON:
			aliens[ALIEN_BOSS] = alien_defs[CD_MINEBOSS];
			aliens[ALIEN_BOSS_PART1] = alien_defs[CD_BOSS2_WING1];
			aliens[ALIEN_BOSS_PART2] = alien_defs[CD_BOSS2_WING2];
			aliens[ALIEN_BOSS_PART3] = alien_defs[CD_BOSS2_WING3];
			aliens[ALIEN_BOSS_PART4] = alien_defs[CD_BOSS2_WING4];
			aliens[ALIEN_FRIEND1] = alien_defs[CD_REBELCARRIER];
			aliens[ALIEN_FRIEND2] = alien_defs[CD_REBELCARRIER];
			break;

		case MISN_ALMARTHA:
			aliens[ALIEN_BOSS_PART1] = alien_defs[CD_CLOAKFIGHTER];
			aliens[ALIEN_BOSS_PART2] = alien_defs[CD_CLOAKFIGHTER];
			aliens[ALIEN_BOSS_PART3] = alien_defs[CD_CLOAKFIGHTER];
			aliens[ALIEN_FRIEND1] = alien_defs[CD_KRASS];
			break;

		case MISN_POSWIC:
			aliens[0] = alien_defs[CD_ESCORT];
			aliens[1] = alien_defs[CD_ESCORT];
			aliens[2] = alien_defs[CD_ESCORT];
			aliens[3] = alien_defs[CD_ESCORT];
			aliens[4] = alien_defs[CD_ESCORT];
			aliens[ALIEN_BOSS] = alien_defs[CD_EXEC];
			break;

		case MISN_ELLESH:
			aliens[ALIEN_BOSS] = alien_defs[CD_EXEC];
			break;

		case MISN_PLUTO:
			aliens[ALIEN_BOSS] = alien_defs[CD_PLUTOBOSS];
			aliens[ALIEN_BOSS_PART1] = alien_defs[CD_BARRIER];
			aliens[ALIEN_BOSS_PART2] = alien_defs[CD_BARRIER];
			aliens[ALIEN_BOSS_PART3] = alien_defs[CD_BARRIER];
			aliens[ALIEN_BOSS_PART4] = alien_defs[CD_BARRIER];
			aliens[ALIEN_BOSS_PART5] = alien_defs[CD_BARRIER];
			aliens[ALIEN_BOSS_PART6] = alien_defs[CD_BARRIER];
			break;

		case MISN_NEPTUNE:
			aliens[ALIEN_BOSS] = alien_defs[CD_NEPTUNEBOSS];
			aliens[ALIEN_BOSS_PART1] = alien_defs[CD_MOBILESHIELD];
			aliens[ALIEN_BOSS_PART2] = alien_defs[CD_MOBILESHIELD];
			aliens[ALIEN_BOSS_PART3] = alien_defs[CD_MOBILESHIELD];
			aliens[ALIEN_BOSS_PART4] = alien_defs[CD_MOBILESHIELD];
			aliens[ALIEN_BOSS_PART5] = alien_defs[CD_MOBILESHIELD];
			break;

		case MISN_URANUS:
			aliens[ALIEN_BOSS] = alien_defs[CD_URANUSBOSS];
			aliens[ALIEN_BOSS_PART1] = alien_defs[CD_URANUSBOSSWING1];
			aliens[ALIEN_BOSS_PART2] = alien_defs[CD_URANUSBOSSWING2];
			break;

		case MISN_SATURN:
			aliens[ALIEN_BOSS_PART1] = alien_defs[CD_MOBILE_RAY];
			aliens[ALIEN_BOSS_PART2] = alien_defs[CD_MOBILE_RAY];
			aliens[ALIEN_BOSS_PART3] = alien_defs[CD_MOBILE_RAY];
			aliens[ALIEN_BOSS_PART4] = alien_defs[CD_MOBILE_RAY];
			aliens[ALIEN_BOSS_PART5] = alien_defs[CD_MOBILE_RAY];
			aliens[ALIEN_BOSS_PART6] = alien_defs[CD_MOBILE_RAY];
			break;

		case MISN_JUPITER:
			aliens[ALIEN_BOSS] = alien_defs[CD_KRASS];
			aliens[ALIEN_BOSS_PART1] = alien_defs[CD_FIREFLY];
			aliens[ALIEN_BOSS_PART2] = alien_defs[CD_FIREFLY];
			aliens[ALIEN_BOSS_PART3] = alien_defs[CD_FIREFLY];
			aliens[ALIEN_BOSS_PART4] = alien_defs[CD_FIREFLY];
			break;

		case MISN_EARTH:
		case MISN_VENUS:
			aliens[ALIEN_KLINE] = alien_defs[CD_KLINE];
			break;
	}

	for (int i = 0 ; i < ALIEN_MAX ; i++)
	{
		if (aliens[i].shield != -1)
		{
			placeAttempt = 0;

			aliens[i].owner = &aliens[i];
			aliens[i].target = &aliens[i];
			aliens[i].face = rand() % 2;
			aliens[i].active = true;

			/*
			we make 1000 attempts to place this enemy since it is required. If after
			1000 attempts we still haven't managed to place the alien, then it
			simply isn't going to happen and we will just exit the game. The chances
			of this happening are very very low!
			*/
			while (true)
			{
				placeAttempt++;

				if (alien_place(&aliens[i]))
					break;

				if (placeAttempt > 1000)
					engine_showError(2, "");
			}

			if (game.area == MISN_CERADSE)
				cargo_add(&aliens[i], P_CARGO);
			else if (game.area == MISN_NEROD)
				cargo_add(&aliens[i], P_PHOEBE);

			if (i == ALIEN_KLINE)
			{
				aliens[ALIEN_KLINE].target = &player;
			}

			if (aliens[i].classDef == CD_CLOAKFIGHTER)
			{
				aliens[i].active = false;
				aliens[i].maxShield = aliens[i].shield = 400;
				aliens[i].flags &= ~FL_RUNSAWAY;
				aliens[i].speed = 3;
			}

			if ((aliens[i].classDef == CD_MOBILE_RAY) && (i >= ALIEN_BOSS_PART3))
			{
				aliens[i].active = false;
			}

			if (aliens[i].classDef == CD_FIREFLY)
			{
				aliens[i].active = false;
			}

			if (aliens[i].classDef == CD_BARRIER)
			{
				aliens[i].owner = &aliens[ALIEN_BOSS];
				aliens[i].speed = barrierSpeed;
				barrierSpeed++;
			}

			if ((game.area == MISN_POSWIC) &&
				(aliens[i].classDef == CD_BOSS))
			{
				aliens[i].imageIndex[1] = 29;
				aliens[i].flags |= FL_IMMORTAL;
			}

			if (game.area == MISN_ELLESH)
				aliens[i].flags |= FL_HASMINIMUMSPEED;

			if (game.area == MISN_JUPITER)
			{
				aliens[i].flags = FL_WEAPCO;
				if (i == ALIEN_BOSS)
					aliens[i].chance[1] = 5;
			}
		}
	}

	if (game.area == MISN_MOEBO)
	{
		aliens[ALIEN_BOSS].target = &player;
		aliens[ALIEN_BOSS].x = -screen->w / 2;
		aliens[ALIEN_BOSS].y = screen->h / 2;

		aliens[ALIEN_BOSS_PART1].owner = &aliens[ALIEN_BOSS];
		aliens[ALIEN_BOSS_PART1].target = &player;
		aliens[ALIEN_BOSS_PART1].dx = -25;
		aliens[ALIEN_BOSS_PART1].dy = -21;

		aliens[ALIEN_BOSS_PART2].owner = &aliens[ALIEN_BOSS];
		aliens[ALIEN_BOSS_PART2].target = &player;
		aliens[ALIEN_BOSS_PART2].dx = -20;
		aliens[ALIEN_BOSS_PART2].dy = 37;
	}
	else if ((game.area == MISN_ELAMALE) ||
		(game.area == MISN_FELLON))
	{
		aliens[ALIEN_BOSS].target = &player;
		aliens[ALIEN_BOSS].x = -screen->w / 2;
		aliens[ALIEN_BOSS].y = screen->h / 2;

		aliens[ALIEN_BOSS_PART1].owner = &aliens[ALIEN_BOSS_PART2];
		aliens[ALIEN_BOSS_PART1].target = &player;
		aliens[ALIEN_BOSS_PART1].dx = -35;
		aliens[ALIEN_BOSS_PART1].dy = -12;

		aliens[ALIEN_BOSS_PART2].owner = &aliens[ALIEN_BOSS];
		aliens[ALIEN_BOSS_PART2].target = &player;
		aliens[ALIEN_BOSS_PART2].dx = 15;
		aliens[ALIEN_BOSS_PART2].dy = -22;

		aliens[ALIEN_BOSS_PART3].owner = &aliens[ALIEN_BOSS];
		aliens[ALIEN_BOSS_PART3].target = &player;
		aliens[ALIEN_BOSS_PART3].dx = 15;
		aliens[ALIEN_BOSS_PART3].dy = 22;

		aliens[ALIEN_BOSS_PART4].owner = &aliens[ALIEN_BOSS_PART3];
		aliens[ALIEN_BOSS_PART4].target = &player;
		aliens[ALIEN_BOSS_PART4].dx = -35;
		aliens[ALIEN_BOSS_PART4].dy = 20;

		if (game.area == MISN_FELLON)
		{
			aliens[ALIEN_BOSS].AIType = AI_EVASIVE;

			for (int i = 10 ; i < 15 ; i++)
			{
				aliens[i].imageIndex[0] += 15;
				aliens[i].imageIndex[1] += 15;

				aliens[i].image[0] = shipShape[aliens[i].imageIndex[0]];
				aliens[i].image[1] = shipShape[aliens[i].imageIndex[1]];
			}
		}
	}
	else if (game.area == MISN_URANUS)
	{
		aliens[ALIEN_BOSS].target = &player;
		aliens[ALIEN_BOSS].x = -screen->w / 2;
		aliens[ALIEN_BOSS].y = screen->h / 2;

		aliens[ALIEN_BOSS_PART1].owner = &aliens[ALIEN_BOSS];
		aliens[ALIEN_BOSS_PART1].dy = -16;

		aliens[ALIEN_BOSS_PART2].owner = &aliens[ALIEN_BOSS];
		aliens[ALIEN_BOSS_PART2].dy = 20;
	}
}

bool alien_add()
{
	int index = alien_getFreeIndex();

	if ((index == -1) || (game.area == MISN_JUPITER) ||
			(game.area == MISN_VENUS))
		return 0;

	signed char *alienArray;
	signed char numberOfAliens = 1;

	alienArray = new signed char[8];

	switch(game.area)
	{
		case MISN_START:
		case MISN_HINSTAG:
		case MISN_ELAMALE:
			numberOfAliens = 1;
			alienArray[0] = CD_DUALFIGHTER;
			break;
		case MISN_HAIL:
		case MISN_CERADSE:
		case MISN_JOLDAR:
		case MISN_MOEBO:
			numberOfAliens = 2;
			alienArray[0] = CD_DUALFIGHTER;
			alienArray[1] = CD_PROTOFIGHTER;
			break;
		case MISN_NEROD:
		case MISN_ALLEZ:
			numberOfAliens = 3;
			alienArray[0] = CD_DUALFIGHTER;
			alienArray[1] = CD_PROTOFIGHTER;
			alienArray[2] = CD_AIMFIGHTER;
			break;
		case MISN_URUSOR:
			// This is the mission where you need to disable cargo ships.
			// Missiles are extremely bad in this mission, not because
			// of the damage they do to you, but because they tend to
			// accidentally destroy the cargo ships. Therefore, ships
			// with missiles (dual fighters and missile boats) are
			// excluded from this mission.
			numberOfAliens = 2;
			alienArray[0] = CD_PROTOFIGHTER;
			alienArray[1] = CD_AIMFIGHTER;
			// ...That is, unless we're in Classic difficulty, because
			// the original game actually had these enemies.
			if (game.difficulty == DIFFICULTY_ORIGINAL)
			{
				numberOfAliens = 4;
				alienArray[2] = CD_DUALFIGHTER;
				alienArray[3] = CD_MISSILEBOAT;
			}
			break;
		case MISN_DORIM:
		case MISN_SIVEDI:
			numberOfAliens = 1;
			alienArray[0] = CD_ASTEROID;
			break;
		case MISN_ODEON:
		case MISN_FELLON:
		case MISN_ALMARTHA:
			numberOfAliens = 4;
			alienArray[0] = CD_DUALFIGHTER;
			alienArray[1] = CD_PROTOFIGHTER;
			alienArray[2] = CD_MISSILEBOAT;
			alienArray[3] = CD_AIMFIGHTER;
			break;
		case MISN_ELLESH:
			numberOfAliens = 2;
			alienArray[0] = CD_DUALFIGHTER;
			alienArray[1] = CD_MINER;
			break;
		case MISN_SATURN:
			numberOfAliens = 2;
			alienArray[0] = CD_AIMFIGHTER;
			alienArray[1] = CD_DUALFIGHTER;
			break;
		case MISN_MARS:
			numberOfAliens = 2;
			alienArray[0] = CD_ASTEROID;
			alienArray[1] = CD_ASTEROID2;
			break;
		case MISN_EARTH:
			numberOfAliens = 6;
			alienArray[0] = CD_DUALFIGHTER;
			alienArray[1] = CD_PROTOFIGHTER;
			alienArray[2] = CD_MISSILEBOAT;
			alienArray[3] = CD_AIMFIGHTER;
			alienArray[4] = CD_ESCORT;
			alienArray[5] = CD_MOBILE_RAY;
			break;
		case MISN_INTERCEPTION:
			numberOfAliens = 3;
			alienArray[0] = CD_DUALFIGHTER;
			alienArray[1] = CD_MISSILEBOAT;
			alienArray[2] = CD_AIMFIGHTER;
			if (game.system == 2)
			{
				numberOfAliens = 4;
				alienArray[3] = CD_PROTOFIGHTER;
			}
			break;
		default:
			numberOfAliens = 1;
			alienArray[0] = CD_DUALFIGHTER;
			break;
	}

	signed char randEnemy = alienArray[rand() % numberOfAliens];

	if ((game.area != MISN_DORIM) &&
		(game.area != MISN_SIVEDI) &&
		(game.area != MISN_MARS))
	{
		if ((game.system == 1) && (game.area == MISN_INTERCEPTION))
		{
			if ((rand() % 5) == 0)
				randEnemy = CD_SLAVETRANSPORT;
		}

		if ((rand() % 6) == 0)
			randEnemy = CD_TRANSPORTSHIP;
	}

	delete[] alienArray;

	aliens[index] = alien_defs[randEnemy];
	aliens[index].active = true;
	aliens[index].face = rand() % 2;
	aliens[index].owner = &aliens[index]; // Most enemies will own themselves
	aliens[index].target = &aliens[index];
	aliens[index].thinktime = (50 + rand() % 50);
	aliens[index].systemPower = aliens[index].maxShield;
	aliens[index].deathCounter = 0 - (aliens[index].maxShield * 3);
	aliens[index].hit = 0;

	LIMIT(aliens[index].deathCounter, -250, 0);

	// Attempts to place an alien. If it fails, the alien is deactivated.
	for (int i = 0 ; i < 100 ; i++)
	{
		if (alien_place(&aliens[index]))
			break;
		aliens[index].active = false;

		return false;
	}

	if (aliens[index].classDef == CD_CARGOSHIP)
		cargo_add(&aliens[index], P_CARGO);

	if (aliens[index].classDef == CD_MOBILE_RAY)
		aliens[index].shield = 25;

	if (aliens[index].classDef == CD_ESCORT)
		aliens[index].shield = 50;

	aliens[index].dx = RANDRANGE(-2, 2);
	aliens[index].dy = RANDRANGE(-2, 2);

	aliens[index].ammo[0] = 0;

	if (game.area == MISN_ELLESH)
		aliens[index].flags |= FL_HASMINIMUMSPEED;

	return true;
}

void alien_addDrone(object *hostAlien)
{
	int index = alien_getFreeIndex();

	if (index == -1)
		return;

	aliens[index] = alien_defs[CD_DRONE];
	aliens[index].active = true;
	aliens[index].face = rand() % 2;
	aliens[index].owner = &aliens[index]; // Most enemies will own themselves
	aliens[index].target = &aliens[index];
	aliens[index].thinktime = (50 + rand() % 50);
	aliens[index].systemPower = aliens[index].maxShield;
	aliens[index].deathCounter = 0 - (aliens[index].maxShield * 3);
	aliens[index].hit = 0;

	aliens[index].x = hostAlien->x + rand() % 50;
	aliens[index].y = hostAlien->y + rand() % 50;
}

void alien_addSmallAsteroid(object *hostAlien)
{
	if (engine.missionCompleteTimer != 0)
		return;

	int index = -1;
	int debris = RANDRANGE(1, 10);

	for (int i = 0 ; i < debris ; i++)
		bullet_add(&weapon[W_ROCKETS], hostAlien, 0, 0);

	for (int i = 10 ; i < 20 ; i++)
		if (!aliens[i].active)
			index = i;

	if (index == -1)
		return;

	if ((rand() % 10) > 3)
	{
		aliens[index] = alien_defs[CD_ASTEROID2];
		aliens[index].imageIndex[0] = aliens[index].imageIndex[1] = 39 + rand() % 2;
		aliens[index].image[0] = shipShape[aliens[index].imageIndex[0]];
		aliens[index].image[1] = shipShape[aliens[index].imageIndex[1]];
	}
	else
	{
		aliens[index] = alien_defs[CD_DRONE];
	}

	aliens[index].owner = &aliens[index]; // Most enemies will own themselves
	aliens[index].target = &aliens[index];
	aliens[index].thinktime = 1;
	aliens[index].systemPower = aliens[index].maxShield;
	aliens[index].deathCounter = 0 - (aliens[index].maxShield * 3);
	aliens[index].hit = 0;

	aliens[index].x = hostAlien->x;
	aliens[index].y = hostAlien->y;
	aliens[index].active = true;
}

void alien_addFriendly(int type)
{
	if (type != ALIEN_SID)
		aliens[type] = alien_defs[CD_FRIEND];
	else
		aliens[type] = alien_defs[CD_SID];

	aliens[type].owner = &aliens[type];
	aliens[type].target = &aliens[type];
	aliens[type].active = true;
	aliens[type].x = RANDRANGE((screen->w / 2) - 150, (screen->w / 2) + 150);
	aliens[type].y = RANDRANGE((screen->h / 2) - 150, (screen->h / 2) + 150);

	if (type == ALIEN_PHOEBE)
		aliens[type].classDef = CD_PHOEBE;

	if (type == ALIEN_URSULA)
		aliens[type].classDef = CD_URSULA;

	// For the sake of it being the final battle :)
	if (game.area == MISN_EARTH)
		aliens[type].flags |= FL_IMMORTAL;
}

bool alien_place(object *alien)
{
	if (rand() % 2 == 0)
		alien->x = RANDRANGE(screen->w, screen->w * 2);
	else
		alien->x = RANDRANGE(-screen->w, 0);

	if (rand() % 2 == 0)
		alien->y = RANDRANGE(screen->h, screen->h * 2);
	else
		alien->y = RANDRANGE(-screen->h, 0);

	if (game.area == MISN_MARS)
	{
		alien->x = screen->w + RANDRANGE(0, 400);
		alien->y = RANDRANGE(-screen->h / 3, (4 * screen->h) / 3);
	}

	for (int i = 0 ; i < ALIEN_MAX ; i++)
	{
		if ((aliens[i].owner != alien) && (aliens[i].shield > 0))
		{
			if (ship_collision(alien, &aliens[i]))
				return false;
		}
	}

	return true;
}

void alien_setAI(object *alien)
{
	// Make friendly craft generally concentrate on smaller fighters
	if ((alien->flags & FL_FRIEND) && (alien->target == &aliens[ALIEN_BOSS]))
	{
		if ((rand() % 5) == 0)
		{
			alien->target = alien;
			alien->thinktime = 0;
			return;
		}
	}

	int i = rand() % 10;
	float tx = alien->target->x;
	float ty = alien->target->y;

	int chase = 0; // Chance in 10 of chasing player
	int area = 0; // Chance in 10 of moving to an area around the player
	int stop = 0; // Chance in 10 of hanging back
	int point = 0; // Size of area alien will move into

	switch (alien->AIType)
	{
		case AI_NORMAL:
			chase = 3;
			point = 6;
			stop = 9;
			area = 250;
			break;
		case AI_OFFENSIVE:
			chase = 7;
			point = 8;
			stop = 9;
			area = 50;
			break;
		case AI_DEFENSIVE:
			chase = 2;
			point = 6;
			stop = 8;
			area = 300;
			break;
		case AI_EVASIVE:
			chase = 1;
			point = 8;
			stop = 9;
			area = 600;
			break;
		case AI_WANDER:
			chase = -1;
			point = 0;
			stop = 10;
			area = 1200;
			break;
	}

	if (i <= chase)
	{
		// Chase the target
		alien->dx = ((alien->x - tx) / ((300 / alien->speed) + rand() % 100));
		alien->dy = ((alien->y - ty) / ((300 / alien->speed) + rand() % 100));
		return;
	}
	else if ((i >= point) && (i <= stop))
	{
		// Fly to a random point around the target
		tx += (rand() % area - (rand() % area * 2));
		ty += (rand() % area - (rand() % area * 2));
		alien->dx = ((alien->x - tx) / ((300 / alien->speed) + rand() % 100));
		alien->dy = ((alien->y - ty) / ((300 / alien->speed) + rand() % 100));
		return;
	}
	else
	{
		// Hang back
		alien->dx = 0;
		alien->dy = 0;
		return;
	}
}

void alien_setKlineAttackMethod(object *alien)
{
	if (alien->shield <= 500)
	{
		setRadioMessage(FACE_KLINE, "ENOUGH!! THIS ENDS NOW!!!", 1);
		alien->weaponType[0] = W_AIMED_SHOT;
		alien->weaponType[1] = W_MICRO_HOMING_MISSILES;
		alien->flags |= FL_CANCLOAK;
		alien->chance[0] = 100;
		alien->chance[1] = 2;
		alien->flags &= ~FL_CANNOTDIE;
	}
	else if (alien->shield <= 1000)
	{
		setRadioMessage(FACE_KLINE, "Your ability to stay alive irritates me!! Try dodging some of these!!", 1);
		alien->weaponType[0] = W_DIRSHOCKMISSILE;
		alien->weaponType[1] = W_DIRSHOCKMISSILE;
		alien->chance[0] = 2;
		alien->chance[1] = 2;
		alien->flags |= FL_AIMS;
	}
	else
	{
		setRadioMessage(FACE_KLINE, "Very good, Bainfield. Now let's get a little more serious...", 1);
		alien->weaponType[0] = W_SPREADSHOT;
		alien->chance[1] = 40;
	}
}

/*
This AI is exclusively for Kline.
*/
void alien_setKlineAI(object *alien)
{
	// Weapon type change
	if ((rand() % 3) == 0)
	{
		if (game.area != MISN_VENUS)
		{
			alien->flags &= ~FL_AIMS;

			switch(rand() % 2)
			{
				case 0:
					alien->weaponType[0] = W_TRIPLE_SHOT;
					break;
				case 1:
					alien->weaponType[0] = W_AIMED_SHOT;
					alien->flags |= FL_AIMS;
					break;
			}
		}
	}

	alien->flags &= ~(FL_CIRCLES | FL_CONTINUOUS_FIRE | FL_DROPMINES);

	switch(rand() % 10)
	{
		case 0:
			if ((alien->weaponType[0] != W_DIRSHOCKMISSILE) &&
					(alien->weaponType[1] != W_MICRO_HOMING_MISSILES))
				alien->flags |= FL_CONTINUOUS_FIRE;
			alien->dx = ((alien->x - alien->target->x) /
				((300 / alien->speed)  + rand() % 100));
			alien->dy = ((alien->y - alien->target->y) /
				((300 / alien->speed)  + rand() % 100));
			break;
		case 1:
		case 2:
			// Kline only attacks then he is ready!
			if ((!(alien->flags & FL_NOFIRE)) &&
					(game.area == MISN_MOEBO))
				alien->flags |= FL_DROPMINES;
			break;
		case 3:
		case 4:
			alien->flags |= FL_CIRCLES;
			break;
		default:
			alien_setAI(alien);
			break;
	}
}

/*
"Looks" for an enemy by picking a randomly active enemy and using them
as a target. If the target is too far away, it will be ignored.
*/
void alien_searchForTarget(object *alien)
{
	int i;

	if (alien->flags & FL_WEAPCO)
	{
		i = (rand() % 10);

		if (i == 0)
		{
			alien->target = &player;
			return;
		}
	}

	i = rand() % ALIEN_MAX;

	object *targetEnemy = &aliens[i];

	// Tell Sid not to attack craft that are already disabled or can
	// return fire. This will save him from messing about (unless we're on the last mission)
	if ((alien->classDef == CD_SID) && (game.area != MISN_EARTH))
	{
		if ((targetEnemy->flags & FL_DISABLED) || (!(targetEnemy->flags & FL_NOFIRE)))
			return;
	}

	// Tell Phoebe and Ursula not to attack ships that cannot fire or are disabled (unless we're on the last mission)
	if (game.area != MISN_EARTH)
	{
		if ((alien->classDef == CD_PHOEBE) || (alien->classDef == CD_URSULA))
		{
			// Don't attack the boss or we could be here all day(!)
			if (targetEnemy->classDef == CD_BOSS)
				return;

			if ((targetEnemy->flags & FL_DISABLED) ||
					(targetEnemy->flags & FL_NOFIRE))
				return;
		}
	}

	if ((targetEnemy->shield < 1) || (!targetEnemy->active))
		return;

	if ((targetEnemy->flags & FL_WEAPCO) && (alien->flags & FL_WEAPCO))
		return;

	if ((targetEnemy->flags & FL_FRIEND) && (alien->flags & FL_FRIEND))
		return;

	if (abs((int)alien->x - (int)alien->target->x) > 550)
		return;

	if (abs((int)alien->y - (int)alien->target->y) > 400)
		return;

	alien->target = targetEnemy;
}

/*
Do various checks to see if the alien can fire at the target.
*/
int alien_checkTarget(object *alien)
{
	// No target
	if (alien->target == alien)
		return 0;

	// Whilst firing a Ray, no other weapons can be fired!
	if (alien->flags & FL_FIRERAY)
		return 0;

	// The target is on the same side as you!
	if ((alien->flags & FL_WEAPCO) && (alien->target->flags & FL_WEAPCO))
		return 0;
	if ((alien->flags & FL_FRIEND) && (alien->target->flags & FL_FRIEND))
		return 0;

	// You're facing the wrong way
	if ((alien->face == 0) && (alien->target->x < alien->x))
		return 0;
	if ((alien->face == 1) && (alien->target->x > alien->x))
		return 0;

	// Slightly more than half a screen away from you
	if (abs((int)alien->x - (int)alien->target->x) > 550)
		return 0;

	if ((alien->flags & FL_AIMS) || (alien->flags & FL_CONTINUOUS_FIRE))
		return 1;

	// Not at the correct vertical height
	if ((alien->y < alien->target->y - 15) ||
			(alien->y > alien->target->y + alien->target->image[0]->h + 15))
		return 0;

	return 1;
}

/*
Currently only used for the allies. Whilst flying around, the allies will fire on
any enemy craft that enter their line of sight.
*/
int alien_enemiesInFront(object *alien)
{
	for (int i = 0 ; i < ALIEN_MAX ; i++)
	{
		if ((alien != &aliens[i]) && (aliens[i].flags & FL_WEAPCO) &&
			(aliens[i].shield > 0))
		{
			if ((alien->y > aliens[i].y - 15) &&
				(alien->y < aliens[i].y + aliens[i].image[0]->h + 15))
			{
				if ((alien->face == 1) && (aliens[i].x < alien->x))
					return 1;
				if ((alien->face == 0) && (aliens[i].x > alien->x))
					return 1;
			}
		}
	}

	return 0;
}

void alien_move(object *alien)
{
	bool checkCollisions;

	if ((alien->flags & FL_LEAVESECTOR) || (alien->shield < 1))
		checkCollisions = false;
	else
		checkCollisions = true;

	if (alien->owner == alien)
	{
		if (alien->flags & FL_CIRCLES)
		{
			if (alien->face == 0)
			{
				alien->dx += 0.02;
				alien->dy += 0.02;
			}
			else
			{
				alien->dx -= 0.02;
				alien->dy -= 0.02;
			}

			alien->x -= (sinf(alien->dx) * 4);
			alien->y -= (cosf(alien->dy) * 4);
		}
		else
		{
			alien->x -= alien->dx;
			alien->y -= alien->dy;
		}
	}

	if (checkCollisions)
	{
		for (int i = 0 ; i < ALIEN_MAX ; i++)
		{
			if ((alien->flags & FL_LEAVESECTOR) ||
				(alien->classDef == CD_DRONE) ||
				(alien->classDef == CD_ASTEROID2) ||
				(alien->owner == aliens[i].owner) ||
				(alien->owner->owner == aliens[i].owner) ||
				(aliens[i].shield < 1))
			{
				continue;
			}

			if (ship_collision(alien, &aliens[i]))
			{
				if ((aliens[i].classDef == CD_BARRIER) &&
					(aliens[i].owner != alien))
				{
					alien->shield--;
					alien->hit = 3;
					alien->dx *= -1;
					alien->dy *= -1;
					audio_playSound(SFX_HIT, alien->x);
				}
			}
		}
	}

	// Handle a collision with the player
	if ((player.shield > 0) && (alien->shield > 0) && (checkCollisions))
	{
		if (ship_collision(alien, &player))
		{
			if (alien->classDef == CD_ASTEROID)
			{
				if (!engine.cheatShield)
					player.shield -= alien->shield;
				alien->shield = 0;
				audio_playSound(SFX_EXPLOSION, alien->x);
				player.hit = 5;
				audio_playSound(SFX_HIT, player.x);
			}

			if (alien->classDef == CD_ASTEROID2)
			{
				if (!engine.cheatShield)
					player.shield -= alien->shield;
				alien->shield = 0;
				audio_playSound(SFX_EXPLOSION, alien->x);
				player.hit = 5;
				audio_playSound(SFX_HIT, player.x);
			}

			if (alien->classDef == CD_BARRIER)
			{
				if (!engine.cheatShield)
					player.shield--;
				player.hit = 5;
				audio_playSound(SFX_HIT, player.x);
			}
		}
	}
}

/*
Fill in later...
*/
void alien_destroy(object *alien, object *attacker)
{
	audio_playSound(SFX_EXPLOSION, alien->x);

	if (alien->flags & FL_FRIEND)
	{
		if (alien->classDef == CD_PHOEBE)
			game.wingMate1Ejects++;
		else if (alien->classDef == CD_URSULA)
			game.wingMate2Ejects++;

		// Phoebe cannot eject on the rescue mission
		if (game.area != MISN_NEROD)
		{
			if ((alien->classDef == CD_PHOEBE) || (alien->classDef == CD_URSULA))
				setInfoLine(">> Ally has ejected! <<\n", FONT_RED);
			else
				setInfoLine(">> Friendly craft has been destroyed! <<\n", FONT_RED);
		}
	}

	if (attacker != NULL)
	{
		if (attacker == &player)
		{
			game.totalKills++;
		}
		else if (attacker->classDef == CD_PHOEBE)
		{
			game.wingMate1Kills++;
		}
		else if (attacker->classDef == CD_URSULA)
		{
			game.wingMate2Kills++;
		}
		else
		{
			game.totalOtherKills++;
		}

		if ((attacker->classDef == CD_PHOEBE) || (attacker->classDef == CD_URSULA))
		{
			if ((rand() % 8) == 0)
			{
				getKillMessage(attacker);
			}
		}
	}

	updateMissionRequirements(M_DESTROY_TARGET_TYPE, alien->classDef, 1);
	updateMissionRequirements(M_PROTECT_TARGET, alien->classDef, 1);

	if (rand() % 100 <= alien->collectChance)
	{
		unsigned char value;

		if ((rand() % 10) == 0)
			alien->collectValue *= 2;

		while (alien->collectValue > 0)
		{
			value = (10 + (rand() % alien->collectValue));
			if (value > alien->collectValue)
				value = alien->collectValue;
			collectable_add(alien->x, alien->y, alien->collectType, value, 600);
			alien->collectValue -= value;
		}
	}

	// Make it explode immediately
	if (alien->classDef == CD_ASTEROID)
	{
		alien->shield = -999;
	}

	if ((alien->classDef == CD_KRASS) && (attacker == &player))
		setRadioMessage(FACE_CHRIS, "The name's CHRIS, old-timer.", 1);

	if (alien->classDef == CD_KLINE)
	{
		setRadioMessage(FACE_KLINE, "It was an honor... to have fought you...", 1);
		alien->dx = alien->dy = 0;
		alien->shield = -150;
	}
}

void alien_hurt(object *alien, object *attacker, int damage, bool ion)
{
	if (ion)
		alien->systemPower -= damage;
	else
		alien->shield -= damage;

	// Chain reaction damage if needed
	if ((alien->owner != alien) && (alien->flags & FL_DAMAGEOWNER))
	{
		alien_hurt(alien->owner, attacker, damage, ion);
	}

	if (alien->classDef == CD_KLINE)
	{
		if (game.area == MISN_ELAMALE)
		{
			if ((alien->shield <= alien->maxShield - 500) &&
				!(alien->flags & FL_LEAVESECTOR))
			{
				alien->flags |= FL_LEAVESECTOR;
				alien->flags &= ~FL_CIRCLES;
				setRadioMessage(FACE_KLINE, "Seems I underestimated you, Bainfield. We'll meet again!", 1);
			}
		}
		else if (game.area == MISN_EARTH)
		{
			if ((alien->shield <= alien->maxShield - 750) &&
				!(alien->flags & FL_LEAVESECTOR))
			{
				alien->flags |= FL_LEAVESECTOR;
				alien->flags &= ~FL_CIRCLES;
				setRadioMessage(FACE_SID, "Chris, Kethlan is getting away!", 1);
			}
		}
		else if (game.area == MISN_VENUS)
		{
			if (alien->shield + damage > 1500 &&
					alien->shield <= 1500)
				alien_setKlineAttackMethod(alien);
			else if (alien->shield + damage > 1000 &&
					alien->shield <= 1000)
				alien_setKlineAttackMethod(alien);
			else if (alien->shield + damage > 500 &&
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

	audio_playSound(SFX_HIT, alien->x);
	if (alien->AIType == AI_EVASIVE)
		alien->thinktime = 0;
	
	if (alien->shield < 1)
		alien_destroy(alien, attacker);

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
}
