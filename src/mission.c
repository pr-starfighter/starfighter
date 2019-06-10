/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012, 2013 Guus Sliepen
Copyright (C) 2012, 2015-2019 Julie Marchant <onpon4@riseup.net>

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
#include <stdlib.h>
#include <string.h>

#include "SDL.h"

#include "colors.h"
#include "defs.h"
#include "structs.h"

#include "alien.h"
#include "audio.h"
#include "engine.h"
#include "event.h"
#include "game.h"
#include "gfx.h"
#include "info.h"
#include "intermission.h"
#include "mission.h"
#include "player.h"
#include "radio.h"
#include "renderer.h"
#include "screen.h"

Mission mission;

static Mission missions[MISN_MAX];

/*
Timer Variable:
1+ : Time in minutes
-1 : Time up
-2 : No timer
*/
static void mission_clearAll()
{
	for (int m = 0 ; m < MISN_MAX ; m++)
	{
		for (int i = 0 ; i < 3 ; i++)
		{
			strcpy(missions[m].primaryObjective[i], "");
			missions[m].primaryType[i] = M_NONE;
			missions[m].target1[i] = -1;
			missions[m].targetValue1[i] = -1;
			missions[m].timeLimit1[i] = MT_NONE;
			missions[m].completed1[i] = OB_COMPLETED;
		}

		for (int i = 0 ; i < 3 ; i++)
		{
			strcpy(missions[m].secondaryObjective[i], "");
			missions[m].secondaryType[i] = M_NONE;
			missions[m].target2[i] = -1;
			missions[m].targetValue2[i] = -1;
			missions[m].timeLimit2[i] = MT_NONE;
			missions[m].completed2[i] = OB_COMPLETED;
		}

		missions[m].addAliens = -1;
	}
}

/*
This is where all the missions are defined.
*/
void mission_init()
{
	// Seconds to wait between attempting to add an enemy
	static const int SOMETIMES    = 40 * 60;
	static const int NORMAL       = 15 * 60;
	static const int FREQUENT     = 5 * 60;
	static const int ALWAYS       = 1 * 60;
	static const int NEVER        = -1;

	/// Mission objective
	const char *ob_destroyAll = _("Destroy all remaining WEAPCO fighters");

	mission_clearAll();

	/// Mission objective (start)
	strcpy(missions[MISN_START].primaryObjective[0], _("Escape from WEAPCO Persuit"));
	missions[MISN_START].primaryType[0] = M_DESTROY_ALL_TARGETS;
	missions[MISN_START].completed1[0] = OB_INCOMPLETE;


	/// Mission objective (Hail)
	strcpy(missions[MISN_HAIL].primaryObjective[0], _("Collect $500 to pay Mercenary for FIREFLY"));
	missions[MISN_HAIL].primaryType[0] = M_COLLECT;
	missions[MISN_HAIL].target1[0] = P_CASH;
	missions[MISN_HAIL].targetValue1[0] = 500;
	missions[MISN_HAIL].completed1[0] = OB_INCOMPLETE;

	strcpy(missions[MISN_HAIL].primaryObjective[1], ob_destroyAll);
	missions[MISN_HAIL].primaryType[1] = M_DESTROY_ALL_TARGETS;
	missions[MISN_HAIL].completed1[1] = OB_INCOMPLETE;

	missions[MISN_HAIL].addAliens = FREQUENT;


	/// Mission objective (Ceradse)
	strcpy(missions[MISN_CERADSE].primaryObjective[0], _("Collect 6 Cargo Pods"));
	missions[MISN_CERADSE].primaryType[0] = M_COLLECT;
	missions[MISN_CERADSE].target1[0] = P_CARGO;
	missions[MISN_CERADSE].targetValue1[0] = 6;
	missions[MISN_CERADSE].completed1[0] = OB_INCOMPLETE;

	/// Mission objective (Ceradse)
	strcpy(missions[MISN_CERADSE].primaryObjective[1], _("Do not destroy *ANY* Cargo Pods"));
	missions[MISN_CERADSE].primaryType[1] = M_PROTECT_PICKUP;
	missions[MISN_CERADSE].target1[1] = P_CARGO;
	missions[MISN_CERADSE].targetValue1[1] = 0;
	missions[MISN_CERADSE].completed1[1] = OB_CONDITION;

	strcpy(missions[MISN_CERADSE].secondaryObjective[0], ob_destroyAll);
	missions[MISN_CERADSE].secondaryType[0] = M_DESTROY_ALL_TARGETS;
	missions[MISN_CERADSE].completed2[0] = OB_INCOMPLETE;

	missions[MISN_CERADSE].addAliens = FREQUENT;


	/// Mission objective (Hinstag)
	strcpy(missions[MISN_HINSTAG].primaryObjective[0], _("Destroy 5 WEAPCO Missile Boats"));
	missions[MISN_HINSTAG].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[MISN_HINSTAG].target1[0] = CD_MISSILEBOAT;
	missions[MISN_HINSTAG].targetValue1[0] = 5;
	missions[MISN_HINSTAG].completed1[0] = OB_INCOMPLETE;

	strcpy(missions[MISN_HINSTAG].secondaryObjective[0], ob_destroyAll);
	missions[MISN_HINSTAG].secondaryType[0] = M_DESTROY_ALL_TARGETS;
	missions[MISN_HINSTAG].completed2[0] = OB_INCOMPLETE;

	missions[MISN_HINSTAG].addAliens = NORMAL;


	/// Mission objective (Joldar)
	strcpy(missions[MISN_JOLDAR].primaryObjective[0], _("Destroy 9 WEAPCO Mine-droppers"));
	missions[MISN_JOLDAR].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[MISN_JOLDAR].target1[0] = CD_MINER;
	missions[MISN_JOLDAR].targetValue1[0] = 9;
	missions[MISN_JOLDAR].completed1[0] = OB_INCOMPLETE;

	strcpy(missions[MISN_JOLDAR].secondaryObjective[0], ob_destroyAll);
	missions[MISN_JOLDAR].secondaryType[0] = M_DESTROY_ALL_TARGETS;
	missions[MISN_JOLDAR].completed2[0] = OB_INCOMPLETE;

	missions[MISN_JOLDAR].addAliens = NORMAL;


	/// Mission objective (Moebo)
	strcpy(missions[MISN_MOEBO].primaryObjective[0], _("Destroy WEAPCO Frigate"));
	missions[MISN_MOEBO].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[MISN_MOEBO].target1[0] = CD_BOSS;
	missions[MISN_MOEBO].targetValue1[0] = 1;
	missions[MISN_MOEBO].completed1[0] = OB_INCOMPLETE;

	missions[MISN_MOEBO].timeLimit1[0] = 3;

	missions[MISN_MOEBO].addAliens = SOMETIMES;


	/// Mission objective (Nerod)
	strcpy(missions[MISN_NEROD].primaryObjective[0], _("Rescue Phoebe Lexx"));
	missions[MISN_NEROD].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[MISN_NEROD].target1[0] = CD_CARGOSHIP;
	missions[MISN_NEROD].targetValue1[0] = 1;
	missions[MISN_NEROD].completed1[0] = OB_INCOMPLETE;

	/// Mission objective (Nerod)
	strcpy(missions[MISN_NEROD].primaryObjective[1], _("Do not allow Phoebe to be killed"));
	missions[MISN_NEROD].primaryType[1] = M_PROTECT_TARGET;
	missions[MISN_NEROD].target1[1] = CD_PHOEBE;
	missions[MISN_NEROD].targetValue1[1] = 0;
	missions[MISN_NEROD].completed1[1] = OB_CONDITION;

	strcpy(missions[MISN_NEROD].primaryObjective[2], ob_destroyAll);
	missions[MISN_NEROD].primaryType[2] = M_DESTROY_TARGET_TYPE;
	missions[MISN_NEROD].target1[2] = CD_ANY;
	missions[MISN_NEROD].targetValue1[2] = 35;
	missions[MISN_NEROD].completed1[2] = OB_INCOMPLETE;

	missions[MISN_NEROD].addAliens = ALWAYS;


	/// Mission objective (Allez)
	strcpy(missions[MISN_ALLEZ].primaryObjective[0], _("Assist medical supply craft"));
	missions[MISN_ALLEZ].primaryType[0] = M_ESCAPE_TARGET;
	missions[MISN_ALLEZ].target1[0] = CD_GOODTRANSPORT;
	missions[MISN_ALLEZ].targetValue1[0] = 0;
	missions[MISN_ALLEZ].completed1[0] = OB_INCOMPLETE;

	/// Mission objective (Allez)
	strcpy(missions[MISN_ALLEZ].primaryObjective[1], _("Do not allow supply craft to be destroyed"));
	missions[MISN_ALLEZ].primaryType[1] = M_PROTECT_TARGET;
	missions[MISN_ALLEZ].target1[1] = CD_GOODTRANSPORT;
	missions[MISN_ALLEZ].targetValue1[1] = 0;
	missions[MISN_ALLEZ].completed1[1] = OB_CONDITION;

	strcpy(missions[MISN_ALLEZ].secondaryObjective[0], ob_destroyAll);
	missions[MISN_ALLEZ].secondaryType[0] = M_DESTROY_ALL_TARGETS;
	missions[MISN_ALLEZ].completed2[0] = OB_INCOMPLETE;

	missions[MISN_ALLEZ].addAliens = FREQUENT;


	/// Mission objective (Urusor)
	strcpy(missions[MISN_URUSOR].primaryObjective[0], _("Disable five WEAPCO supply craft"));
	missions[MISN_URUSOR].primaryType[0] = M_DISABLE_TARGET;
	missions[MISN_URUSOR].target1[0] = CD_CARGOSHIP;
	missions[MISN_URUSOR].targetValue1[0] = 5;
	missions[MISN_URUSOR].completed1[0] = OB_INCOMPLETE;

	strcpy(missions[MISN_URUSOR].primaryObjective[1], ob_destroyAll);
	missions[MISN_URUSOR].primaryType[1] = M_DESTROY_ALL_TARGETS;
	missions[MISN_URUSOR].completed1[1] = OB_INCOMPLETE;

	/// Mission objective (Urusor)
	strcpy(missions[MISN_URUSOR].primaryObjective[2], _("Protect supply craft AND Sid Wilson"));
	missions[MISN_URUSOR].primaryType[2] = M_PROTECT_TARGET;
	missions[MISN_URUSOR].target1[2] = CD_CARGOSHIP;
	missions[MISN_URUSOR].targetValue1[2] = 0;
	missions[MISN_URUSOR].completed1[2] = OB_CONDITION;

	missions[MISN_URUSOR].addAliens = FREQUENT;


	/// Mission objective (Dorim)
	strcpy(missions[MISN_DORIM].primaryObjective[0], _("Locate doctor's escape pod"));
	missions[MISN_DORIM].primaryType[0] = M_COLLECT;
	missions[MISN_DORIM].target1[0] = P_ESCAPEPOD;
	missions[MISN_DORIM].targetValue1[0] = 1;
	missions[MISN_DORIM].completed1[0] = OB_INCOMPLETE;

	/// Mission objective (Dorim)
	strcpy(missions[MISN_DORIM].primaryObjective[1], _("Do not destroy doctor's escape pod"));
	missions[MISN_DORIM].primaryType[1] = M_PROTECT_PICKUP;
	missions[MISN_DORIM].target1[1] = P_ESCAPEPOD;
	missions[MISN_DORIM].targetValue1[1] = 1; // DONE ON PURPOSE!! DO NOT CHANGE THIS!!!!
	missions[MISN_DORIM].completed1[1] = OB_CONDITION;

	/// Mission objective (Dorim)
	strcpy(missions[MISN_DORIM].secondaryObjective[0], _("Collect 10 pieces of Ore"));
	missions[MISN_DORIM].secondaryType[0] = M_COLLECT;
	missions[MISN_DORIM].target2[0] = P_ORE;
	missions[MISN_DORIM].targetValue2[0] = 10;
	missions[MISN_DORIM].completed2[0] = OB_INCOMPLETE;

	missions[MISN_DORIM].addAliens = ALWAYS;

	missions[MISN_DORIM].timeLimit1[0] = 3;
	missions[MISN_DORIM].timeLimit2[0] = 3;


	/// Mission objective (Elamale)
	strcpy(missions[MISN_ELAMALE].primaryObjective[0], _("Destroy WEAPCO ore mining craft"));
	missions[MISN_ELAMALE].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[MISN_ELAMALE].target1[0] = CD_BOSS;
	missions[MISN_ELAMALE].targetValue1[0] = 1;
	missions[MISN_ELAMALE].completed1[0] = OB_INCOMPLETE;

	/// Mission objective (Elamale)
	strcpy(missions[MISN_ELAMALE].secondaryObjective[0], _("Save present slaves"));
	missions[MISN_ELAMALE].secondaryType[0] = M_PROTECT_PICKUP;
	missions[MISN_ELAMALE].target2[0] = P_SLAVES;
	missions[MISN_ELAMALE].completed2[0] = OB_CONDITION;

	/// Mission objective (Elamale)
	strcpy(missions[MISN_ELAMALE].primaryObjective[1], _("Battle Kline"));
	missions[MISN_ELAMALE].primaryType[1] = M_ESCAPE_TARGET;
	missions[MISN_ELAMALE].target1[1] = CD_KLINE;
	missions[MISN_ELAMALE].targetValue1[1] = 1;
	missions[MISN_ELAMALE].completed1[1] = OB_HIDDEN;

	missions[MISN_ELAMALE].addAliens = NEVER;


	/// Mission objective (Odeon)
	strcpy(missions[MISN_ODEON].primaryObjective[0], _("Destroy Ursula's ship"));
	missions[MISN_ODEON].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[MISN_ODEON].target1[0] = CD_EVILURSULA;
	missions[MISN_ODEON].targetValue1[0] = 0;
	missions[MISN_ODEON].completed1[0] = OB_INCOMPLETE;

	/// Mission objective (Odeon)
	strcpy(missions[MISN_ODEON].primaryObjective[1], _("Capture Ursula's escape pod"));
	missions[MISN_ODEON].primaryType[1] = M_COLLECT;
	missions[MISN_ODEON].target1[1] = P_ESCAPEPOD;
	missions[MISN_ODEON].targetValue1[1] = 1;
	missions[MISN_ODEON].completed1[1] = OB_INCOMPLETE;

	/// Mission objective (Odeon)
	strcpy(missions[MISN_ODEON].primaryObjective[2], _("Do not kill Ursula"));
	missions[MISN_ODEON].primaryType[2] = M_PROTECT_PICKUP;
	missions[MISN_ODEON].target1[2] = P_ESCAPEPOD;
	missions[MISN_ODEON].targetValue1[2] = 0;
	missions[MISN_ODEON].completed1[2] = OB_CONDITION;

	strcpy(missions[MISN_ODEON].secondaryObjective[0], ob_destroyAll);
	missions[MISN_ODEON].secondaryType[0] = M_DESTROY_ALL_TARGETS;
	missions[MISN_ODEON].completed2[0] = OB_INCOMPLETE;

	missions[MISN_ODEON].addAliens = FREQUENT;


	/// Mission objective (Fellon)
	strcpy(missions[MISN_FELLON].primaryObjective[0], _("Assist attack on WEAPCO ore mining craft"));
	missions[MISN_FELLON].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[MISN_FELLON].target1[0] = CD_BOSS;
	missions[MISN_FELLON].targetValue1[0] = 1;
	missions[MISN_FELLON].completed1[0] = OB_INCOMPLETE;

	/// Mission objective (Fellon)
	strcpy(missions[MISN_FELLON].primaryObjective[1], _("At least 1 rebel craft must survive"));
	missions[MISN_FELLON].primaryType[1] = M_PROTECT_TARGET;
	missions[MISN_FELLON].target1[1] = CD_REBELCARRIER;
	missions[MISN_FELLON].targetValue1[1] = 2;
	missions[MISN_FELLON].completed1[1] = OB_CONDITION;

	strcpy(missions[MISN_FELLON].primaryObjective[2], ob_destroyAll);
	missions[MISN_FELLON].primaryType[2] = M_DESTROY_ALL_TARGETS;
	missions[MISN_FELLON].completed1[2] = OB_INCOMPLETE;

	missions[MISN_FELLON].addAliens = ALWAYS;


	/// Mission objective (Sivedi)
	strcpy(missions[MISN_SIVEDI].primaryObjective[0], _("Collect 25 pieces of Ore"));
	missions[MISN_SIVEDI].primaryType[0] = M_COLLECT;
	missions[MISN_SIVEDI].target1[0] = P_ORE;
	missions[MISN_SIVEDI].targetValue1[0] = 25;
	missions[MISN_SIVEDI].completed1[0] = OB_INCOMPLETE;

	/// Mission objective (Sivedi)
	strcpy(missions[MISN_SIVEDI].secondaryObjective[0], _("Collect 50 pieces of Ore"));
	missions[MISN_SIVEDI].secondaryType[0] = M_COLLECT;
	missions[MISN_SIVEDI].target2[0] = P_ORE;
	missions[MISN_SIVEDI].targetValue2[0] = 25; // 25 + 25 = 50 (the overall total)
	missions[MISN_SIVEDI].completed2[0] = OB_INCOMPLETE;

	missions[MISN_SIVEDI].addAliens = ALWAYS;


	/// Mission objective (Almartha)
	strcpy(missions[MISN_ALMARTHA].primaryObjective[0], _("Collect $2000 to pay mercenary"));
	missions[MISN_ALMARTHA].primaryType[0] = M_COLLECT;
	missions[MISN_ALMARTHA].target1[0] = P_CASH;
	missions[MISN_ALMARTHA].targetValue1[0] = 2000;
	missions[MISN_ALMARTHA].completed1[0] = OB_INCOMPLETE;

	strcpy(missions[MISN_ALMARTHA].primaryObjective[1], ob_destroyAll);
	missions[MISN_ALMARTHA].primaryType[1] = M_DESTROY_ALL_TARGETS;
	missions[MISN_ALMARTHA].completed1[1] = OB_INCOMPLETE;

	missions[MISN_ALMARTHA].addAliens = ALWAYS;


	/// Mission objective (Poswic)
	strcpy(missions[MISN_POSWIC].primaryObjective[0], _("Destroy escorts"));
	missions[MISN_POSWIC].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[MISN_POSWIC].target1[0] = CD_ESCORT;
	missions[MISN_POSWIC].targetValue1[0] = 5;
	missions[MISN_POSWIC].completed1[0] = OB_INCOMPLETE;

	/// Mission objective (Poswic)
	strcpy(missions[MISN_POSWIC].primaryObjective[1], _("Disable executive transport"));
	missions[MISN_POSWIC].primaryType[1] = M_ESCAPE_TARGET;
	missions[MISN_POSWIC].target1[1] = CD_BOSS;
	missions[MISN_POSWIC].targetValue1[1] = 1;
	missions[MISN_POSWIC].completed1[1] = OB_INCOMPLETE;

	strcpy(missions[MISN_POSWIC].primaryObjective[2], ob_destroyAll);
	missions[MISN_POSWIC].primaryType[2] = M_DESTROY_ALL_TARGETS;
	missions[MISN_POSWIC].completed1[2] = OB_INCOMPLETE;

	missions[MISN_POSWIC].addAliens = NORMAL;


	/// Mission objective (Ellesh)
	strcpy(missions[MISN_ELLESH].primaryObjective[0], _("Destroy executive transport"));
	missions[MISN_ELLESH].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[MISN_ELLESH].target1[0] = CD_BOSS;
	missions[MISN_ELLESH].targetValue1[0] = 1;
	missions[MISN_ELLESH].completed1[0] = OB_INCOMPLETE;

	missions[MISN_ELLESH].addAliens = ALWAYS;


	/// Mission objective (Pluto, Neptune, Uranus)
	strcpy(missions[MISN_PLUTO].primaryObjective[0], _("Destroy planetary guardian"));
	missions[MISN_PLUTO].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[MISN_PLUTO].target1[0] = CD_PLUTOBOSS;
	missions[MISN_PLUTO].targetValue1[0] = 1;
	missions[MISN_PLUTO].completed1[0] = OB_INCOMPLETE;

	strcpy(missions[MISN_PLUTO].primaryObjective[1], ob_destroyAll);
	missions[MISN_PLUTO].primaryType[1] = M_DESTROY_ALL_TARGETS;
	missions[MISN_PLUTO].completed1[1] = OB_INCOMPLETE;

	missions[MISN_PLUTO].timeLimit1[0] = 5;
	missions[MISN_PLUTO].timeLimit1[1] = 5;

	missions[MISN_PLUTO].addAliens = ALWAYS;


	strcpy(missions[MISN_NEPTUNE].primaryObjective[0], missions[MISN_PLUTO].primaryObjective[0]);
	missions[MISN_NEPTUNE].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[MISN_NEPTUNE].target1[0] = CD_NEPTUNEBOSS;
	missions[MISN_NEPTUNE].targetValue1[0] = 1;
	missions[MISN_NEPTUNE].completed1[0] = OB_INCOMPLETE;

	strcpy(missions[MISN_NEPTUNE].primaryObjective[1], ob_destroyAll);
	missions[MISN_NEPTUNE].primaryType[1] = M_DESTROY_ALL_TARGETS;
	missions[MISN_NEPTUNE].completed1[1] = OB_INCOMPLETE;

	missions[MISN_NEPTUNE].timeLimit1[0] = 5;
	missions[MISN_NEPTUNE].timeLimit1[1] = 5;

	missions[MISN_NEPTUNE].addAliens = ALWAYS;


	strcpy(missions[MISN_URANUS].primaryObjective[0], missions[MISN_PLUTO].primaryObjective[0]);
	missions[MISN_URANUS].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[MISN_URANUS].target1[0] = CD_URANUSBOSS;
	missions[MISN_URANUS].targetValue1[0] = 1;
	missions[MISN_URANUS].completed1[0] = OB_INCOMPLETE;

	strcpy(missions[MISN_URANUS].primaryObjective[1], ob_destroyAll);
	missions[MISN_URANUS].primaryType[1] = M_DESTROY_ALL_TARGETS;
	missions[MISN_URANUS].completed1[1] = OB_INCOMPLETE;

	missions[MISN_URANUS].timeLimit1[0] = 5;
	missions[MISN_URANUS].timeLimit1[1] = 5;

	missions[MISN_URANUS].addAliens = ALWAYS;


	/// Mission objective (Saturn)
	strcpy(missions[MISN_SATURN].primaryObjective[0], _("Destroy outer defence systems"));
	missions[MISN_SATURN].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[MISN_SATURN].target1[0] = CD_MOBILE_RAY;
	missions[MISN_SATURN].targetValue1[0] = 6;
	missions[MISN_SATURN].completed1[0] = OB_INCOMPLETE;

	strcpy(missions[MISN_SATURN].primaryObjective[1], ob_destroyAll);
	missions[MISN_SATURN].primaryType[1] = M_DESTROY_ALL_TARGETS;
	missions[MISN_SATURN].completed1[1] = OB_INCOMPLETE;

	missions[MISN_SATURN].addAliens = NORMAL;


	/// Mission objective (Jupiter)
	strcpy(missions[MISN_JUPITER].primaryObjective[0], _("Investigate distress call"));
	missions[MISN_JUPITER].primaryType[0] = M_DESTROY_ALL_TARGETS;
	missions[MISN_JUPITER].completed1[0] = OB_CONDITION;

	/// Mission objective (Jupiter)
	strcpy(missions[MISN_JUPITER].primaryObjective[1], _("Defeat Krass Tyler"));
	missions[MISN_JUPITER].primaryType[1] = M_DESTROY_TARGET_TYPE;
	missions[MISN_JUPITER].target1[1] = CD_KRASS;
	missions[MISN_JUPITER].targetValue1[1] = 1;
	missions[MISN_JUPITER].completed1[1] = OB_HIDDEN;

	/// Mission objective (Jupiter)
	strcpy(missions[MISN_JUPITER].primaryObjective[2], _("Destroy Krass' support group"));
	missions[MISN_JUPITER].primaryType[2] = M_DESTROY_ALL_TARGETS;
	missions[MISN_JUPITER].target1[1] = CD_FIREFLY;
	missions[MISN_JUPITER].targetValue1[1] = 4;
	missions[MISN_JUPITER].completed1[2] = OB_HIDDEN;

	missions[MISN_JUPITER].addAliens = ALWAYS;


	/// Mission objective (Mars)
	strcpy(missions[MISN_MARS].primaryObjective[0], _("Navigate asteroid belt"));
	missions[MISN_MARS].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[MISN_MARS].target1[0] = CD_BOSS;
	missions[MISN_MARS].targetValue1[0] = 1;
	missions[MISN_MARS].completed1[0] = OB_INCOMPLETE;

	missions[MISN_MARS].timeLimit1[0] = 2;

	missions[MISN_MARS].addAliens = ALWAYS;


	/// Mission objective (Earth)
	strcpy(missions[MISN_EARTH].primaryObjective[0], _("Destroy WEAPCO frontline forces"));
	missions[MISN_EARTH].primaryType[0] = M_DESTROY_TARGET_TYPE;
	missions[MISN_EARTH].target1[0] = CD_ANY;
	missions[MISN_EARTH].targetValue1[0] = 100;
	missions[MISN_EARTH].completed1[0] = OB_INCOMPLETE;

	missions[MISN_EARTH].addAliens = ALWAYS;


	/// Mission objective (Venus)
	strcpy(missions[MISN_VENUS].primaryObjective[0], _("Defeat Kline"));
	missions[MISN_VENUS].primaryType[0] = M_DESTROY_ALL_TARGETS;
	missions[MISN_VENUS].completed1[0] = OB_INCOMPLETE;


	strcpy(missions[MISN_INTERCEPTION].primaryObjective[0], "");
	missions[MISN_INTERCEPTION].primaryType[0] = M_DESTROY_ALL_TARGETS;
	missions[MISN_INTERCEPTION].completed1[0] = OB_INCOMPLETE;
}

/*
Sets the mission Object to the mission number the player
is currently on. Timing is assigned if it is required. The rate
at which to add enemies in this mission is also set.
*/
void mission_set(int misn)
{
	mission = missions[misn];
	engine.minutes = mission.timeLimit1[0];

	for (int i = 0 ; i < 3 ; i++)
	{
		if (mission.timeLimit1[i] > engine.minutes)
			engine.minutes = mission.timeLimit1[i];
		if (mission.timeLimit2[i] > engine.minutes)
			engine.minutes = mission.timeLimit2[i];

		if (mission.completed1[i] == OB_INCOMPLETE)
			mission.remainingObjectives1++;
		if (mission.completed2[i] == OB_INCOMPLETE)
			mission.remainingObjectives1++;
	}

	engine.addAliens = mission.addAliens;

	if (engine.minutes > -1)
	{
		engine.timeMission = 1;
		engine.seconds = 0;
	}

	engine.counter2 = 0;
	engine.timeTaken = 0;
}

/*
Call this whenever a mission requires all the remaining aliens to
automatically die
*/
static void mission_killAllEnemies()
{
	for (int i = 0 ; i < ALIEN_MAX ; i++)
	{
		if ((aliens[i].flags & FL_WEAPCO) && (aliens[i].active) &&
				(aliens[i].shield > 0))
			aliens[i].shield = 0;
	}
}

void mission_checkTimer()
{
	for (int i = 0 ; i < 3 ; i++)
	{
		if ((mission.timeLimit1[i] == -1) && (mission.completed1[i] == OB_INCOMPLETE))
			mission.completed1[i] = OB_JUST_FAILED;
	}

	for (int i = 0 ; i < 3 ; i++)
	{
		if ((mission.timeLimit2[i] == -1) && (mission.completed2[i] == OB_INCOMPLETE))
			mission.completed2[i] = OB_JUST_FAILED;
	}

	// Find out if there are any uncompleted missions that require the timer
	engine.timeMission = 0;
	for (int i = 0 ; i < 3 ; i++)
	{
		if ((mission.timeLimit1[i] > -1) && (mission.completed1[i] == OB_INCOMPLETE))
			engine.timeMission = 1;
		if ((mission.timeLimit2[i] > -1) && (mission.completed2[i] == OB_INCOMPLETE))
			engine.timeMission = 1;
	}

	// specific to Spirit Boss
	if ((game.area == MISN_MOEBO) &&
			(mission.completed1[0] < OB_INCOMPLETE))
		engine.timeMission = 1;

	// specific to the Asteroid belt
	if ((game.area == MISN_MARS) &&
		(mission.completed1[0] < OB_INCOMPLETE))
	{
		mission.completed1[0] = OB_COMPLETED;
		mission_killAllEnemies();
		engine.addAliens = -1;
		info_setLine(_("*** All Primary Objectives Completed ***"), FONT_GREEN);
	}
}

static void mission_evaluate(int type, int id, int *completed, int *targetValue, int fontColor)
{
	char message[STRMAX_SHORT];
	char fmt[STRMAX_SHORT];

	if ((*targetValue <= 0) && (type != M_PROTECT_TARGET) &&
			(type != M_PROTECT_PICKUP))
	{
		*completed = OB_JUST_COMPLETED;
		mission_checkTimer();
		if ((game.area == MISN_URUSOR) && (type == M_DISABLE_TARGET))
		{
			/// Dialog (Sid Wilson)
			/// Used when Sid disables the last target in the Urusor mission.
			radio_setMessage(FS_SID, _("All vessels disabled!"), 1);
		}
	}
	else
	{
		strcpy(message, "");
		switch(type)
		{
			case M_COLLECT:
				switch(id)
				{
					case P_CASH:
						radio_getRandomMessage(fmt, ngettext(
							/// Info line messages for remaining cash to collect (singular)
							/// This is a "\n"-separated list of possible choices to make.  Please feel free
							/// to add as many as you like. Each entry must have one instance of "%d", which
							/// is replaced with the number remaining.
							"Collect $%d more...\n"
							"$%d more to collect...\n"
							"Just $%d more needed...\n"
							"Collect just $%d more...",
							/// Info line messages for remaining cash to collect (plural)
							/// This is a "\n"-separated list of possible choices to make.  Please feel free
							/// to add as many as you like. Each entry must have one instance of "%d", which
							/// is replaced with the number remaining.
							"Collect $%d more...\n"
							"$%d more to collect...\n"
							"$%d more needed...",
							*targetValue));
						sprintf(message, fmt, *targetValue);
						break;
					case P_CARGO:
						radio_getRandomMessage(fmt, ngettext(
							/// Info line messages for remaining cargo pods to collect (singular)
							/// This is a "\n"-separated list of possible choices to make.  Please feel free
							/// to add as many as you like. Each entry must have one instance of "%d", which
							/// is replaced with the number remaining.
							"Collect %d more cargo pod...\n"
							"%d more cargo pod to collect...\n"
							"%d more cargo pod needed...\n"
							"Collect just %d more cargo pod...\n"
							"Only %d cargo pod left to collect...",
							/// Info line messages for remaining cargo pods to collect (plural)
							/// This is a "\n"-separated list of possible choices to make.  Please feel free
							/// to add as many as you like. Each entry must have one instance of "%d", which
							/// is replaced with the number remaining.
							"Collect %d more cargo pods...\n"
							"%d more cargo pods to collect...\n"
							"%d more cargo pods needed...\n"
							"Collect %d remaining cargo pods...",
							*targetValue));
						sprintf(message, fmt, *targetValue);
						break;
					case P_ORE:
						radio_getRandomMessage(fmt,ngettext(
							/// Info line messages for remaining ore to collect (singular)
							/// This is a "\n"-separated list of possible choices to make.  Please feel free
							/// to add as many as you like. Each entry must have one instance of "%d", which
							/// is replaced with the number remaining.
							"Collect %d more piece of ore...\n"
							"%d more piece of ore to collect...\n"
							"%d more piece of ore needed...\n"
							"Collect just %d more piece...",
							/// Info line messages for remaining ore to collect (plural)
							/// This is a "\n"-separated list of possible choices to make.  Please feel free
							/// to add as many as you like. Each entry must have one instance of "%d", which
							/// is replaced with the number remaining.
							"Collect %d more pieces of ore...\n"
							"%d more pieces of ore to collect...\n"
							"%d more pieces of ore needed...",
							*targetValue));
						sprintf(message, fmt, *targetValue);
						break;
				}
				break;
			case M_PROTECT_PICKUP:
				*completed = OB_JUST_FAILED;
				switch(id)
				{
					case P_CARGO:
						sprintf(message, _("Cargo pod destroyed!"));
						if (game.area == MISN_CERADSE) // Get lectured by Sid
							/// Dialog (Sid Wilson)
							/// Used when a cargo pod is destroyed in the Ceradse mission.
							radio_setMessage(FS_SID, _("Chris, we needed that pod! I told you that we couldn't afford to lose a single one!"), 1);
						break;
					case P_ESCAPEPOD:
						sprintf(message, _("Escape Pod lost!"));
						if (game.area == MISN_ODEON) // Get lectured by Phoebe
						{
							/// Dialog (Phoebe Lexx)
							/// Used when Ursula is lost in the Odeon mission.
							radio_setMessage(FS_PHOEBE, _("No... Ursula..."), 1);
						}
						break;
				}
				break;
			case M_PROTECT_TARGET:
				if (*targetValue <= 0)
				{
					*completed = OB_JUST_FAILED;
					switch (game.area)
					{
						case MISN_NEROD:
							/// Dialog (Sid Wilson)
							/// Used when Phoebe is killed in the Nerod mission.
							radio_setMessage(FS_SID, _("Dammit, Chris! We just lost her!"), 1);
							break;
						case MISN_ALLEZ:
							/// Dialog (friendly transport from Eyananth, Allez mission)
							/// Used when the friendly transport in the Allez mission is destroyed.
							radio_setMessage(FS_CREW, _("No no no no no! Hull bre-..."), 1);
							break;
						case MISN_URUSOR:
							/// Dialog (Sid Wilson)
							/// Used when a target transport is destroyed in the Urusor mission.
							radio_setMessage(FS_SID, _("Chris, we've got to disable them, not destroy them!!"), 1);
							break;
					}
				}
				break;
			case M_DESTROY_TARGET_TYPE:
				if ((*targetValue <= 10) || (*targetValue % 10 == 0))
				{
					radio_getRandomMessage(fmt, ngettext(
						/// Info line messages for remaining enemies to destroy (singular)
						/// This is a "\n"-separated list of possible choices to make.  Please feel free
						/// to add as many as you like. Each entry must have one instance of "%d", which
						/// is replaced with the number remaining.
						"Destroy %d more target...\n"
						"%d more target to destroy...\n"
						"%d target remains...\n"
						"Destroy just %d more...\n"
						"Only %d target left...\n"
						"Destroy %d last target...",
						/// Info line messages for remaining enemies to destroy (plural)
						/// This is a "\n"-separated list of possible choices to make.  Please feel free
						/// to add as many as you like. Each entry must have one instance of "%d", which
						/// is replaced with the number remaining.
						"Destroy %d more targets...\n"
						"%d more targets to destroy...\n"
						"%d targets remain...\n"
						"Destroy %d remaining targets...",
						*targetValue));
					sprintf(message, fmt, *targetValue);
				}
				break;
			case M_DISABLE_TARGET:
				radio_getRandomMessage(fmt, ngettext(
					/// Info line messages for remaining enemies to disable (singular)
					/// This is a "\n"-separated list of possible choices to make.  Please feel free
					/// to add as many as you like. Each entry must have one instance of "%d", which
					/// is replaced with the number remaining.
					"Disable %d more target...\n"
					"%d more target to disable...\n"
					"Disable just %d more...\n"
					"Disable %d last target...",
					/// Info line messages for remaining enemies to disable (plural)
					/// This is a "\n"-separated list of possible choices to make.  Please feel free
					/// to add as many as you like. Each entry must have one instance of "%d", which
					/// is replaced with the number remaining.
					"Disable %d more targets...\n"
					"%d more targets to disable...\n"
					"Disable %d remaining targets...",
					*targetValue));
				sprintf(message, fmt, *targetValue);
				break;
		}

		if (strcmp(message, "") != 0)
			info_setLine(message, fontColor);
	}
}

void mission_updateRequirements(int type, int id, int value)
{
	char message[STRMAX_SHORT];
	char fmt[STRMAX_SHORT];
	int matched = 0;
	int slavesNeeded;

	// Can't complete missions if you're dead!
	if (player.shield <= 0)
		return;

	// We don't need to worry here since if Sid dies,
	// you will automatically fail the mission(!)
	if ((type == M_DESTROY_TARGET_TYPE) && (id == CD_SID))
	{
		info_setLine(_("Sid has been killed!"), FONT_RED);
		mission.completed1[0] = OB_JUST_FAILED;
		/// Dialog (Sid Wilson)
		/// Used when Sid is killed.
		/// Translation note: the end is Sid trying to say "FUCK" and getting cut
		/// out because of the failure of his communication system, so please
		/// translate that to the start of a similar curse word in the target
		/// language being similarly cut out.
		radio_setMessage(FS_SID, _("HULL BREACHED! SYSTEMS FAILING! F-..."), 1);
	}

	for (int i = 0 ; i < 3 ; i++)
	{
		if ((mission.completed1[i] == OB_INCOMPLETE) || (mission.completed1[i] == OB_CONDITION))
		{
			if ((mission.primaryType[i] == type) &&
					((mission.target1[i] == id) ||
						(mission.target1[i] == CD_ANY)))
			{
				matched = 1;
				mission.targetValue1[i] -= value;
				mission_evaluate(type, id, &mission.completed1[i], &mission.targetValue1[i], FONT_CYAN);
			}
		}
	}

	// Don't evaluate secondary Objectives at the same time!
	if (matched)
		return;

	for (int i = 0 ; i < 3 ; i++)
	{
		if ((mission.completed2[i] == OB_INCOMPLETE) || (mission.completed2[i] == OB_CONDITION))
		{
			if ((mission.secondaryType[i] == type) &&
					((mission.target2[i] == id) ||
						(mission.target2[i] == CD_ANY)))
			{
				mission.targetValue2[i] -= value;
				mission_evaluate(type, id, &mission.completed2[i], &mission.targetValue2[i], FONT_YELLOW);
				return;
			}
		}
	}

	// Special Case - Interceptions
	if (game.area == MISN_INTERCEPTION)
	{
		if ((type == M_COLLECT) && (id == P_SLAVES))
		{
			if (intermission_planets[PLANET_RESCUESLAVES].missionCompleted == 0)
			{
				if (game.slavesRescued >= SLAVE_RESCUE_TARGET)
				{
					info_setLine(_("*** Slaves Rescued - Mission Completed ***"), FONT_GREEN);
					intermission_planets[PLANET_RESCUESLAVES].missionCompleted = 1;
					/// Dialog (Chris Bainfield)
					/// Used when you rescue enough slaves for the Eyananth slave rescue mission.
					radio_setMessage(FS_CHRIS, _("Alright, I think I've rescued enough slaves to stir up some trouble for WEAPCO!"), 1);
				}
				else
				{
					slavesNeeded = SLAVE_RESCUE_TARGET - game.slavesRescued;
					radio_getRandomMessage(fmt, ngettext(
						/// Info line messages for remaining slaves to rescue (singular)
						/// This is a "\n"-separated list of possible choices to make.  Please feel free
						/// to add as many as you like. Each entry must have one instance of "%d", which
						/// is replaced with the number remaining.
						"Rescue %d more slave...\n"
						"Rescue at least %d more slave...\n"
						"At least %d more slave to rescue...\n"
						"At least %d more rescued slave needed...",
						/// Info line messages for remaining slaves to rescue (plural)
						/// This is a "\n"-separated list of possible choices to make.  Please feel free
						/// to add as many as you like. Each entry must have one instance of "%d", which
						/// is replaced with the number remaining.
						"Rescue %d more slaves...\n"
						"Rescue at least %d more slaves...\n"
						"At least %d more slaves to rescue...\n"
						"At least %d more rescued slaves needed...",
						slavesNeeded));
					sprintf(message, fmt, slavesNeeded);
					info_setLine(message, FONT_CYAN);
				}
			}
		}

		if ((type == M_DESTROY_TARGET_TYPE) && (id == CD_CLOAKFIGHTER))
		{
			info_setLine(_("*** Experimental Fighter Destroyed - Mission Completed ***"), FONT_GREEN);
			intermission_planets[PLANET_CLOAKFIGHTER].missionCompleted = 1;
			/// Dialog (Chris Bainfield)
			/// Used when the Mordor cloak ship is destroyed.
			radio_setMessage(FS_CHRIS, _("That's one less suprise that WEAPCO can spring on us!"), 1);
			game.experimentalShield = 0;
		}
	}
}

/*
This is only used a few times in the game.
Missions 11 and 23 to be exact!
*/
static int mission_revealObjectives()
{
	int allDone = 1;

	for (int i = 0 ; i < 3 ; i++)
	{
		if (mission.completed1[i] == OB_HIDDEN)
		{
			mission.completed1[i] = OB_INCOMPLETE;
			allDone = 0;
		}
	}

	if (!allDone)
	{
		// Activate Kline!! :)
		if (game.area == MISN_ELAMALE)
		{
			mission_killAllEnemies();
			events_sync();
			aliens[ALIEN_KLINE].active = 1;
			aliens[ALIEN_KLINE].x = player.x + 1000;
			aliens[ALIEN_KLINE].y = player.y;
			aliens[ALIEN_KLINE].flags |= FL_IMMORTAL | FL_NOFIRE;
			player_setTarget(ALIEN_KLINE);
			audio_playMusic("music/last_cyber_dance.ogg", -1);
		}
	}

	return allDone;
}

int mission_checkCompleted()
{
	for (int i = 0 ; i < 3 ; i++)
	{
		if (mission.completed1[i] == OB_INCOMPLETE)
		{
			if ((mission.primaryType[i] == M_DESTROY_ALL_TARGETS) && (engine.allAliensDead) && (mission.remainingObjectives1 + mission.remainingObjectives2 == 1))
			{
				mission.completed1[i] = OB_JUST_COMPLETED;
				mission_checkTimer();
			}
		}
	}

	for (int i = 0 ; i < 3 ; i++)
	{
		if (mission.completed2[i] == OB_INCOMPLETE)
		{
			if ((mission.secondaryType[i] == M_DESTROY_ALL_TARGETS) && (engine.allAliensDead) && (mission.remainingObjectives1 + mission.remainingObjectives2 == 1))
			{
				mission.completed2[i] = OB_JUST_COMPLETED;
				mission_checkTimer();
			}
		}
	}

	for (int i = 0 ; i < 3 ; i++)
	{
		if (mission.completed1[i] == OB_JUST_COMPLETED)
		{
			if (mission.remainingObjectives1 > 1)
			{
				if ((game.area != MISN_POSWIC) || (i != 1))
					info_setLine(_("*** Primary Objective Completed ***"), FONT_GREEN);
				else
					info_setLine(_(">>> Primary Objective Failed <<<"), FONT_RED);
				mission.completed1[i] = OB_COMPLETED;
			}
			else
			{
				if (mission.remainingObjectives2 > 0)
				{
					info_setLine(_("Emergency warp drive activated. Press button to engage."), FONT_CYAN);
				}

				if (game.area != MISN_INTERCEPTION)
					info_setLine(_("*** All Primary Objectives Completed ***"), FONT_GREEN);
				else
					info_setLine(_("*** Interception Destroyed ***"), FONT_GREEN);
				mission.completed1[i] = OB_COMPLETED;

				// do some area specific things
				if ((game.area == MISN_MOEBO) ||
					(game.area == MISN_DORIM) ||
					(game.area == MISN_ELLESH) ||
					(game.area == MISN_MARS))
				{
					if (mission.remainingObjectives2 == 0)
					{
						mission_killAllEnemies();
						engine.addAliens = -1;
					}
				}

				if (game.area == MISN_EARTH)
				{
					/// Dialog (Chris Bainfield)
					/// Used when the Earth mission is completed.
					radio_setMessage(FS_CHRIS, _("You guys stay here and keep things under control. I'm going after Kethlan!"), 1);
				}
			}
		}

		if (mission.completed2[i] == OB_JUST_COMPLETED)
		{
			if (mission.remainingObjectives2 > 1)
			{
				info_setLine(_("*** Secondary Objective Completed ***"), FONT_GREEN);
				mission.completed2[i] = OB_COMPLETED;
			}
			else
			{
				info_setLine(_("*** All Secondary Objectives Completed ***"), FONT_GREEN);
				mission.completed2[i] = OB_COMPLETED;

				// do some area specific things
				if ((game.area == MISN_DORIM) &&
					(mission.remainingObjectives1 == 0))
				{
					mission_killAllEnemies();
					engine.addAliens = -1;
				}
			}
		}

		if (mission.completed1[i] == OB_JUST_FAILED)
		{
			info_setLine(_(">>> MISSION FAILED <<<"), FONT_RED);
			mission.completed1[i] = OB_FAILED;
		}

		if (mission.completed2[i] == OB_JUST_FAILED)
		{
			info_setLine(_(">>> Secondary Objective Failed <<<"), FONT_RED);
			mission.completed2[i] = OB_FAILED;
		}
	}

	int remaining;
	int add = 0;
	int allDone = 1;

	// Zero Objective list for a recount
	mission.remainingObjectives1 = mission.remainingObjectives2 = 0;

	for (int i = 0 ; i < 3 ; i++)
	{
		if (mission.primaryType[i] != M_NONE)
		{
			if (mission.completed1[i] == OB_INCOMPLETE)
			{
				mission.remainingObjectives1++;
				if (mission.primaryType[i] == M_DESTROY_ALL_TARGETS)
					add = 1;
				allDone = 0;
			}

			if (mission.completed1[i] < OB_INCOMPLETE)
				return 0;
		}
		if (mission.secondaryType[i] != M_NONE)
		{
			if (mission.completed2[i] == OB_INCOMPLETE)
			{
				mission.remainingObjectives2++;
				if (mission.secondaryType[i] == M_DESTROY_ALL_TARGETS)
					add = 1;
				allDone = 0;
			}
		}
	}

	if (allDone)
		allDone = mission_revealObjectives();

	remaining = mission.remainingObjectives1 + mission.remainingObjectives2;

	// We've only got one Objective left and it's destroy all targets,
	// so stop adding aliens (otherwise it might be impossible to finish!)
	if ((add) && (remaining == 1))
		engine.addAliens = -1;

	return allDone;
}

int mission_checkFailed()
{
	for (int i = 0 ; i < 3 ; i++)
	{
		if (mission.completed1[i] < OB_INCOMPLETE)
		{
			return 1;
		}
	}

	return 0;
}

static void mission_drawScreen()
{
	SDL_Rect r = {0, 0, (int)screen->w, 2};

	for (int i = 0 ; i < (int)(screen->h / 4) - 30 ; i++)
	{
		r.y = (i * 2) + 62; // Not a typo; a black gap is left in the middle if it's 60.
		SDL_FillRect(screen, &r, SDL_MapRGB(screen->format, 0, i, 0));
		r.y = (screen->h - (i * 2) - 60);
		SDL_FillRect(screen, &r, SDL_MapRGB(screen->format, 0, i, 0));
	}

	screen_drawRect(screen->w / 2 - 260, screen->h / 2 - 235, 500, 20, 0x00, 0x77, 0x00);
	screen_drawRect(screen->w / 2 - 260, screen->h / 2 - 215, 500, 130, 0x00, 0x33, 0x00);
	screen_renderUnicode(_("Primary Objectives"), screen->w / 2 - 250, screen->h / 2 - 232, FONT_WHITE);

	for (int i = 0 ; i < 3 ; i++)
	{
		if ((mission.primaryType[i] != M_NONE) && (mission.completed1[i] != OB_HIDDEN))
		{
			screen_renderUnicode(mission.primaryObjective[i], screen->w / 2 - 240, screen->h / 2 + (i * 30) - 191, FONT_WHITE);
		}
	}

	if (mission.secondaryType[0] != M_NONE)
	{
		screen_drawRect(screen->w / 2 - 260, screen->h / 2 - 75, 500, 20, 0x00, 0x77, 0x77);
		screen_drawRect(screen->w / 2 - 260, screen->h / 2 - 55, 500, 130, 0x00, 0x33, 0x33);
		screen_renderUnicode(_("Secondary Objectives"), screen->w / 2 - 250, screen->h / 2 - 72, FONT_WHITE);

		for (int i = 0 ; i < 3 ; i++)
		{
			if (mission.secondaryType[i] != M_NONE)
			{
				screen_renderUnicode(mission.secondaryObjective[i], screen->w / 2 - 240, screen->h / 2 + (i * 30) - 31, FONT_WHITE);
				game.secondaryMissions++;
			}
		}
	}

	screen_drawRect(screen->w / 2 - 260, screen->h / 2 + 85, 500, 20, 0x77, 0x77, 0x00);
	screen_drawRect(screen->w / 2 - 260, screen->h / 2 + 105, 500, 130, 0x33, 0x33, 0x00);
	screen_renderUnicode(_("Additional Information"), screen->w / 2 - 250, screen->h / 2 + 88, FONT_WHITE);
}

/*
Simply displays a screen with all the mission information on it, pulled
back from the data stored in the mission Object. The music for the
mission begins playing here.
*/
void mission_showStartScreen()
{
	// TODO: Replace all "TS_*_T" objs with string formatting, plus adjust
	// spacing for things like TS_SHIELD
	screen_clear(black);
	renderer_update();

	gfx_loadSprites();
	/// Used to indicate the player's shield meter in the HUD.
	gfx_createTextObject(TS_SHIELD, _("Shield"), 0, 0, FONT_WHITE);

	gfx_createTextObject(TS_TARGET, _("Target"), 0, 0, FONT_WHITE);
	gfx_createTextObject(TS_TARGET_SID, _("Sid"), 0, 0, FONT_WHITE);
	gfx_createTextObject(TS_TARGET_PHOEBE, _("Phoebe"), 0, 0, FONT_WHITE);
	gfx_createTextObject(TS_TARGET_KLINE, _("Kline"), 0, 0, FONT_WHITE);
	gfx_createTextObject(TS_TIME_T, "Time Remaining - ", 0, 0, FONT_WHITE);
	gfx_createTextObject(TS_POWER, "Power", 0, 0, FONT_WHITE);
	gfx_createTextObject(TS_OUTPUT, "Output", 0, 0, FONT_WHITE);
	gfx_createTextObject(TS_COOLER, "Cooler", 0, 0, FONT_WHITE);
	audio_playRandomTrack();

	if (game.area != MISN_INTERCEPTION)
	{
		renderer_update();

		player_flushInput();
		engine.done = 0;
		engine.keyState[KEY_FIRE] = 0;
		engine.keyState[KEY_ALTFIRE] = 0;
		engine.keyState[KEY_ESCAPE] = 0;

		while (1)
		{
			screen_clear(black);
			mission_drawScreen();

			if (mission.timeLimit1[0] > 0)
			{
				char temp[50];
				if (game.area != MISN_MARS)
				{
					/// "%d" must be retained.  It is replaced with the mission time
					/// limit in minutes.
					sprintf(temp, _("TIME LIMIT: %d minutes"), mission.timeLimit1[0]);
				}
				else
				{
					/// "%d" must be retained.  It is replaced with the mission required
					/// survival time in minutes.
					sprintf(temp, _("SURVIVAL FOR %d minutes"), mission.timeLimit1[0]);
				}
				screen_renderUnicode(temp, -1, screen->h / 2 + 195, FONT_RED);
			}

			switch (game.area)
			{
				case MISN_URUSOR:
				case MISN_DORIM:
				case MISN_SIVEDI:
				case MISN_ALMARTHA:
				case MISN_ELLESH:
				case MISN_MARS:
				case MISN_VENUS:
					screen_renderUnicode(_("Phoebe Lexx will not be present"), screen->w / 2 - 240, screen->h / 2 + 115, FONT_WHITE);
					if (game.hasWingMate2)
						screen_renderUnicode(_("Ursula Lexx will not be present"), screen->w / 2 - 240, screen->h / 2 + 145, FONT_WHITE);
					break;
			}

			if ((game.area == MISN_URUSOR) ||
					(game.area == MISN_POSWIC) ||
					(game.area == MISN_EARTH))
				screen_renderUnicode(_("Sid Wilson will join you on this mission"), screen->w / 2 - 240, screen->h / 2 + 175, FONT_WHITE);

			renderer_update();

			game_delayFrame();
			player_getInput();
			if ((engine.keyState[KEY_FIRE]) || (engine.keyState[KEY_ALTFIRE]) ||
					(engine.keyState[KEY_ESCAPE]))
				break;
		}

		screen_clear(black);
		renderer_update();
		screen_clear(black);
	}

	engine.gameSection = SECTION_GAME;
}

/*
Display a screen showing all the information from the mission
the player has just done. This includes Objectives that have been
completed and failed. A mission timer is also displayed at the bottom
of the screen.
*/
void mission_showFinishedScreen()
{
	int shield_bonus;
	char temp[100];

	if (game.area != MISN_INTERCEPTION)
	{
		screen_clear(black);
		renderer_update();

		if (game.shots > 0)
			game.accuracy = (game.hits * 100) / game.shots;

		screen_clear(black);
		mission_drawScreen();

		for (int i = 0 ; i < 3 ; i++)
		{
			if (mission.primaryType[i] != M_NONE)
			{
				if ((game.area != MISN_POSWIC) || (i != 1))
					screen_renderUnicode(_("COMPLETED"), screen->w / 2 + 150, screen->h / 2 + (i * 30) - 191, FONT_GREEN);
				else
					screen_renderUnicode(_("FAILED"), screen->w / 2 + 150, screen->h / 2 + (i * 30) - 191, FONT_RED);
			}
		}

		if (mission.secondaryType[0] != M_NONE)
		{
			for (int i = 0 ; i < 3 ; i++)
			{
				if (mission.secondaryType[i] != M_NONE)
				{
					strcpy(temp, mission.secondaryObjective[i]);
					if (mission.completed2[i] >= OB_COMPLETED)
					{
						screen_renderUnicode(_("COMPLETED"), screen->w / 2 + 150, screen->h / 2 + (i * 30) - 31, FONT_GREEN);
						game.secondaryMissionsCompleted++;
					}
					else
					{
						screen_renderUnicode(_("FAILED"), screen->w / 2 + 150, screen->h / 2 + (i * 30) - 31, FONT_RED);
					}
				}
			}
		}

		if (mission.remainingObjectives1 + mission.remainingObjectives2 == 0)
		{
			if (game.difficulty == DIFFICULTY_NIGHTMARE)
				shield_bonus = 100;
			else
				shield_bonus = player.shield * 10;
			/// "%d" must be retained.  It is replaced with the money earned
			/// from the shield bonus.  Please keep "$" as-is as well for the
			/// sake of continuity with untranslateable parts of the interface.
			sprintf(temp, _("Shield Bonus: $%d"), shield_bonus);
			screen_renderUnicode(temp, -1, screen->h / 2 + 130, FONT_WHITE);
			game.cash += shield_bonus;
			game.cashEarned += shield_bonus;
		}

		game.timeTaken += engine.timeTaken;

		/// "%02ld" sequences (which represent minutes and seconds,
		/// respectively) must remain and stay in the same order relative
		/// to each other.  The ":"s between them can be changed to other
		/// characters if desired, e.g. this would be acceptable:
		///
		///     "Mission time: %02ldm %02lds"
		///
		/// If you are familiar with printf formatting, you may also change
		/// the formatting as long as the "ld" type remains. For example,
		/// the "%02ld" sequences may be changed to "%ld" if you wish to
		/// not force two digits to be filled in (e.g. to render the number
		/// 3 as "3" instead of "03").
		snprintf(temp, sizeof temp, _("Mission Time: %02ld:%02ld"), engine.timeTaken / 60, engine.timeTaken % 60);

		screen_renderUnicode(temp, -1, 500, FONT_WHITE);

		// Do some mission specific stuff here...
		if (game.area == MISN_HAIL)
			game.cash -= 500;
		else if (game.area == MISN_ODEON)
			game.hasWingMate2 = 1;
		else if (game.area == MISN_ALMARTHA)
			game.cash -= 2000;

		intermission_unlockPlanets();

		renderer_update();

		player_flushInput();
		engine.done = 0;
		engine.keyState[KEY_FIRE] = 0;

		while (1)
		{
			game_delayFrame();
			player_getInput();
			if ((engine.keyState[KEY_FIRE]))
				break;
		}
	}

	audio_haltMusic();
}
