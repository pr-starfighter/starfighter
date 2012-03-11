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

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

// ALL
#define NONE 0

//AI Types
enum {

	AI_NORMAL = 1,
	AI_DEFENSIVE,
	AI_OFFENSIVE,
	AI_EVASIVE,
	AI_WANDER
};

// Object Flags
#define FL_WEAPCO 1
#define FL_FRIEND 2
#define FL_IMMORTAL 4
#define FL_NOMOVE 8
#define FL_NOFIRE 16
#define FL_FIRERAY 32
#define FL_DAMAGEOWNER 64
#define FL_LEAVESECTOR 128
#define FL_ESCAPED 256
#define FL_DROPMINES 512
#define FL_AIMS 1024
#define FL_DISABLED 2048
#define FL_CANNOTDIE 4096 // This will only apply to Kline before the final mission
#define FL_RUNSAWAY 8192
#define FL_ALWAYSFACE 16384 // Kline doesn't turn his back on you! ;)
#define FL_CIRCLES 32768 // Kline can circle around
#define FL_CONTINUOUS_FIRE 65536 // Go absolutely nutts(!)
#define FL_DEPLOYDRONES 131072 // Deploys small drone - Used by Boss 2
#define FL_CANCLOAK 262144
#define FL_ISCLOAKED 524288
#define FL_ACTIVATE 1048576
#define FL_HASMINIMUMSPEED 2097152
#define FL_FIRELASER 4194304

// These are for Alien *indexes* NOT classdefs!!
enum {

	WC_BOSS = 14,
	WC_KLINE,
	FR_PHOEBE,
	FR_URSULA,
	FR_SID
};

// Droppables
enum {

	P_ANYTHING = 1,
	P_WEAPONS,
	P_CASH,
	P_ROCKET,
	P_SHIELD,
	P_CARGO,
	P_PLASMA_AMMO,
	P_PLASMA_RATE,
	P_PLASMA_SHOT,
	P_PLASMA_DAMAGE,
	P_MINE,				// mines detonate when you "pick them up!"
	P_PHOEBE,			// only used as an attachment(!)
	P_SLAVES,
	P_ESCAPEPOD,
	P_ORE,
	P_SUPER
};

// Jobs
enum {

	WT_PLASMA = 1,
	WT_ROCKET,
	WT_ENERGYRAY,
	WT_LASER,
	WT_MICROROCKET,
	WT_CHARGER,
	WT_DIRECTIONAL,
	WT_SPREAD
};

// Explosions
#define E_SMALL_EXPLOSION 4
#define E_BIG_EXPLOSION 8
#define E_SMOKE 12
#define E_TINY_EXPLOSION 16
#define E_ELECTRICAL 20

// Weapons
enum {

	W_NONE = -1,
	W_PLAYER_WEAPON,
	W_PLAYER_WEAPON2,
	W_SINGLE_SHOT,
	W_DOUBLE_SHOT,
	W_TRIPLE_SHOT,
	W_ROCKETS,
	W_DOUBLE_ROCKETS,
	W_MICRO_ROCKETS,
	W_ENERGYRAY,
	W_LASER,
	W_CHARGER,
	W_HOMING_MISSILE,
	W_DOUBLE_HOMING_MISSILES,
	W_MICRO_HOMING_MISSILES,
	W_AIMED_SHOT,
	W_SPREADSHOT,
	W_IONCANNON,
	W_DIRSHOCKMISSILE
};

// Weapon flags
#define WF_SPREAD 4
#define WF_SCATTER 8
#define WF_VARIABLE_SPEED 16
#define WF_HOMING 32
#define WF_SHOCKWAVE 64
#define WF_WEAPCO 128
#define WF_FRIEND 256
#define WF_AIMED 512
#define WF_DISABLE 1024
#define WF_TIMEDEXPLOSION 2048

// Missions
enum {

	M_DESTROY_ALL_TARGETS = 1,
	M_DESTROY_TARGET_TYPE,
	M_COLLECT,
	M_PROTECT_PICKUP,
	M_PROTECT_TARGET,
	M_DISABLE_TARGET,
	M_ESCAPE_TARGET
};

enum {

	OB_JUST_FAILED = -2,
	OB_FAILED,
	OB_INCOMPLETE,
	OB_COMPLETED,
	OB_JUST_COMPLETED,
	OB_CONDITION,
	OB_HIDDEN
};

// Class Defs - Some of these are just place holders
enum {

	CD_DUALFIGHTER, 	// 0
	CD_MISSILEBOAT,
	CD_PROTOFIGHTER,
	CD_FRIEND,
	CD_FRIGATE,
	CD_FRIGATE_WING1,
	CD_FRIGATE_WING2,
	CD_TRANSPORTSHIP,
	CD_CARGOSHIP,
 	CD_MINER,
	CD_KLINE, 			// 10
	CD_AIMFIGHTER,
	CD_SLAVETRANSPORT,
	CD_GOODTRANSPORT,
	CD_SID,
	CD_MINEBOSS,
	CD_BOSS2_WING1,
	CD_BOSS2_WING2,
	CD_BOSS2_WING3,
	CD_BOSS2_WING4,
	CD_DRONE,			// 20
	CD_CLOAKFIGHTER,
	CD_EVILURSULA,
	CD_KRASS,
	CD_EXEC,
	CD_ASTEROID,
	CD_ASTEROID2,
	CD_ESCORT,
	CD_MOBILE_RAY,
	CD_REBELCARRIER,
	CD_PLUTOBOSS, 		// 30
	CD_BARRIER,
	CD_NEPTUNEBOSS,
	CD_MOBILESHIELD,
	CD_PIRATE,
	CD_FIREFLY,
	CD_URANUSBOSS,
	CD_URANUSBOSSWING1,
	CD_URANUSBOSSWING2,

	// Some special ones
	CD_ANY = 100,
	CD_BOSS,
	CD_PHOEBE,
	CD_URSULA
};

// Font Colors
enum {

	FONT_WHITE,
	FONT_RED,
	FONT_YELLOW,
	FONT_GREEN,
	FONT_CYAN,
	FONT_OUTLINE // a dark blue color
};

// Sounds
enum {

	SFX_EXPLOSION,
	SFX_HIT,
	SFX_DEATH,
	SFX_MISSILE,
	SFX_PLASMA,
	SFX_CLOCK,
	SFX_FLY,
	SFX_ENERGYRAY,
	SFX_PICKUP,
	SFX_SHIELDUP,
	SFX_CLOAK,
	SFX_DEBRIS,
	SFX_DEBRIS2,
	SFX_LASER,
	SFX_PLASMA2,
	SFX_PLASMA3
};

enum {

	SECTION_TITLE,
	SECTION_INTERMISSION,
	SECTION_GAME
};

enum {

	FACE_CHRIS = 90,
	FACE_SID,
	FACE_KRASS,
	FACE_KLINE,
	FACE_PHOEBE,
	FACE_URSULA,
	FACE_CREW
};

#define MAX_WEAPONS 20
#define MAX_SHAPES 100
#define MAX_SHIPSHAPES 120
#define MAX_SOUNDS 17
#define MAX_ALIENS 25
#define MAX_TEXTSHAPES 70
#define MAX_FONTSHAPES 6
#define MAX_SHOPSHAPES 6
#define MAX_DEFALIENS 40
#define MAX_MISSIONS 28
#define MAX_CARGO 20
#define MAX_SHOPITEMS 17

#define SHIP_HIT_INDEX 60

#ifndef PACKLOCATION
#define PACKLOCATION "starfighter.pak"
#endif
#ifndef PATH_MAX
#define PATH_MAX	4096
#endif

enum {

	PAK_IMG,
	PAK_WAV,
	PAK_MOD,
	PAK_FONT,
	PAK_S3M
};

extern const char *systemNames[];

extern const char *systemBackground[];

static const signed char rate2reload[6] = {15, 15, 13, 11, 9, 7};
