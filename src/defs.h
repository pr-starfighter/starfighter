/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012 Guus Sliepen
Copyright (C) 2012, 2015 Julian Marchant

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

#ifndef DEFS_H
#define DEFS_H

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define LIMIT(x, a, b) x = ((x) < (b) ? ((x) > (a) ? (x) : (a)) : (b))
#define LIMIT_ADD(x, y, a, b) x = (((x) + (y)) < (b) ? \
	(((x) + (y)) > (a) ? \
		((x) + (y)) : (a)) : (b))
#define WRAP_ADD(x, y, a, b) x = (((x) + (y)) + \
	((x) + (y) < (a) ? ((b) - (a)) : 0) + \
	((x) + (y) > (b) ? ((a) - (b)) : 0))
#define RANDRANGE(x, y) (((x) < (y)) ? ((x) + (rand() % (1 + (y) - (x)))) : (x))

// ALL
#define NONE 0

// Compile-time options
#ifndef VERSION
#define VERSION "???"
#endif

#ifndef DATADIR
#define DATADIR "."
#endif 

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define STARS_NUM 200

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
#define FL_CIRCLES 32768L // Kline can circle around
#define FL_CONTINUOUS_FIRE 65536L // Go absolutely nutts(!)
#define FL_DEPLOYDRONES 131072L // Deploys small drone - Used by Boss 2
#define FL_CANCLOAK 262144L
#define FL_ISCLOAKED 524288L
#define FL_ACTIVATE 1048576L
#define FL_HASMINIMUMSPEED 2097152L
#define FL_FIRELASER 4194304L

// Explosions
#define E_SMALL_EXPLOSION 4
#define E_BIG_EXPLOSION 8
#define E_SMOKE 12
#define E_TINY_EXPLOSION 16
#define E_ELECTRICAL 20

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

#define MAX_WEAPONS 20
#define MAX_SHAPES 100
#define MAX_SHIPSHAPES 120
#define MAX_TEXTSHAPES 150
#define MAX_FONTSHAPES 6
#define MAX_SHOPSHAPES 6
#define MAX_CARGO 20

#define SHIP_HIT_INDEX 60

// AI Types
enum {

	AI_NORMAL = 1,
	AI_DEFENSIVE,
	AI_OFFENSIVE,
	AI_EVASIVE,
	AI_WANDER
};

// These are for Alien *indexes* NOT classdefs!!
enum {

	ALIEN_BOSS_PART4 = 10,
	ALIEN_BOSS_PART3 = 11,
	ALIEN_BOSS_PART2 = 12,
	ALIEN_BOSS_PART1 = 13,
	ALIEN_BOSS = 14,
	ALIEN_KLINE,
	ALIEN_PHOEBE,
	ALIEN_URSULA,
	ALIEN_SID,
	ALIEN_GOODTRANSPORT = 19,
	ALIEN_MAX = 25
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

// Mission types
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
	CD_MAX,

	// Some special ones
	CD_ANY = 100,
	CD_BOSS,
	CD_PHOEBE,
	CD_URSULA
};

// Text shapes
enum {

	TS_PRESENTS,
	TS_AN_SDL_GAME,
	TS_START_NEW_GAME,
	TS_LOAD_GAME,
	TS_CONTINUE_CURRENT_GAME,
	TS_OPTIONS,
	TS_CHEAT_OPTIONS,
	TS_QUIT,
	TS_SOUND,
	TS_MUSIC,
	TS_FULLSCREEN,
	TS_AUTOPAUSE,
	TS_BACK_TO_MAIN_MENU,
	TS_SAVESLOT_0,
	TS_SAVESLOT_1,
	TS_SAVESLOT_2,
	TS_SAVESLOT_3,
	TS_SAVESLOT_4,
	TS_SAVESLOT_5,
	TS_UNLIMITED_SHIELD,
	TS_UNLIMITED_AMMO,
	TS_UNLIMITED_CASH,
	TS_UNLIMITED_TIME,
	TS_START_GAME,
	TS_DIFFICULTY
};

// Menu types
enum {

	MENU_MAIN,
	MENU_DIFFICULTY,
	MENU_LOAD,
	MENU_OPTIONS,
	MENU_CHEAT
};

// Shop items
enum {

	SHOP_PLASMA_MAX_OUTPUT,
	SHOP_PLASMA_MAX_DAMAGE,
	SHOP_PLASMA_MAX_RATE,
	SHOP_PLASMA_MIN_OUTPUT,
	SHOP_PLASMA_MIN_DAMAGE,
	SHOP_PLASMA_MIN_RATE,
	SHOP_PLASMA_AMMO,
	SHOP_ROCKET_AMMO,
	SHOP_PLASMA_MAX_AMMO,
	SHOP_ROCKET_MAX_AMMO,
	SHOP_DOUBLE_ROCKETS,
	SHOP_MICRO_ROCKETS,
	SHOP_LASER,
	SHOP_HOMING_MISSILE,
	SHOP_CHARGER,
	SHOP_DOUBLE_HOMING_MISSILES,
	SHOP_MICRO_HOMING_MISSILES,
	MAX_SHOPITEMS
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
	SFX_PLASMA3,
	SFX_MAX
};

// Sections
enum {

	SECTION_TITLE,
	SECTION_INTERMISSION,
	SECTION_GAME
};

// Faces
enum {

	FACE_CHRIS = 90,
	FACE_SID,
	FACE_KRASS,
	FACE_KLINE,
	FACE_PHOEBE,
	FACE_URSULA,
	FACE_CREW
};

// Missions
enum {

	MISN_START,
	MISN_HAIL,
	MISN_CERADSE,
	MISN_HINSTAG,
	MISN_JOLDAR,
	MISN_MOEBO,
	MISN_RESCUESLAVES,
	MISN_NEROD,
	MISN_ALLEZ,
	MISN_URUSOR,
	MISN_DORIM,
	MISN_ELAMALE,
	MISN_CLOAKFIGHTER,
	MISN_ODEON,
	MISN_FELLON,
	MISN_SIVEDI,
	MISN_ALMARTHA,
	MISN_POSWIC,
	MISN_ELLESH,
	MISN_PLUTO,
	MISN_NEPTUNE,
	MISN_URANUS,
	MISN_SATURN,
	MISN_JUPITER,
	MISN_MARS,
	MISN_EARTH,
	MISN_VENUS,
	MISN_INTERCEPTION,
	MISN_MAX
};

// Difficulties
enum {
	DIFFICULTY_EASY = 0,
	DIFFICULTY_NORMAL,
	DIFFICULTY_HARD,
	DIFFICULTY_NIGHTMARE,
	DIFFICULTY_ORIGINAL,
	DIFFICULTY_MAX
};

const char * const systemNames[] = {"Spirit", "Eyananth", "Mordor", "Sol"};

const char * const systemBackground[] = {
	"gfx/spirit.jpg", "gfx/eyananth.jpg", "gfx/mordor.jpg", "gfx/sol.jpg"
};

const int rate2reload[6] = {15, 15, 13, 11, 9, 7};
const int screenWidth = 800;
const int screenHeight = 600;
const int xViewBorder = 100;
const int yViewBorder = 100;
const float cameraMaxSpeed = 3.;
const int maxHoming = 20;
const int maxDoubleHoming = 15;
const int maxMicroHoming = 10;

#endif
