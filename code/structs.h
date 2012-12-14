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

struct object {

	bool active;
	signed char classDef; // Used by aliens to determine what they are
	signed char AIType; // Type of articifial intelligence

	signed char id; // The "job" of the object
	object *target; //  index target in enemy array

	signed char reload[2];

	int systemPower; // computer systems for craft
	int shield;   // current shield
	int maxShield; // max shield (for recharging)
	int deathCounter; // how long to explode for

	signed char speed;
	unsigned char damage; // Contact damage for bullets
	int score; // How much a kill of this is worth
	unsigned char ammo[2]; // Ammo for 2nd weapon. Max of 100 (except laser)

	signed char face; // Either 0 or 1

	object *owner; // Who owns this object

	int chance[2]; // Chance of using the weapons (out of 1000)

	SDL_Surface *image[2]; // For facing left and right
	unsigned char imageIndex[2]; // used for loading
	signed char hit; // used to make a craft "flash" if it is struck by a shot

	int engineX; // The place for the engine on the other side of the craft
	int engineY; // The middle of the engine on the craft

	int thinktime; // When the object will next react
	signed char weaponType[2]; // Weapon types

	signed char collectChance; // Chance of dropping the object
	signed char collectType; // What the object is carrying
	unsigned char collectValue; // What it is worth

	int flags; // Various flags for an object

	float x, y, dx, dy;

	object *next;

};

struct mission {

	char primaryObjective[3][50]; // Description
	signed char primaryType[3]; // The type of mission this is
	signed char target1[3]; // index of target in enemy array
	int targetValue1[3]; // Number of things to collect (slaves, cash, etc)
	signed char timeLimit1[3]; // In minutes
	int completed1[3];

	char secondaryObjective[3][50]; // Description
	signed char secondaryType[3]; // The type of mission this is
	signed char target2[3]; // index of target in enemy array
	int targetValue2[3]; // Number of things to collect (slaves, cash, etc)
	signed char timeLimit2[3]; // In minutes
	int completed2[3];

	signed char remainingObjectives1;
	signed char remainingObjectives2;
	int addAliens; // How often to add new enemies

};

struct Star {

	float x, y, dx, dy;
	signed char speed; // How fast the star moves

};

struct collectables {

	bool active;
	float x, y, dx, dy;
	SDL_Surface *image;
	signed char type; // What kind of collectable is it?
	unsigned char value; // How much is it worth?
	int life; // How long it will stay around for

	collectables *next;

};

struct textObject {

	SDL_Surface *image;
	unsigned char life;
	float x, y;
	signed char fontColor;
	char text[255];

};

struct Game {
	object thePlayer;
	object playerWeapon;

	unsigned char system;
	unsigned char area;
	unsigned char musicVolume;
	unsigned char sfxVolume;

	signed char saveFormat;
	signed char gamePlay;
	signed char difficulty;

	signed char autoSaveSlot;

	unsigned int cash;
	unsigned int cashEarned;
	unsigned int shots;
	unsigned int hits;
	unsigned char accuracy;
	unsigned char hasWingMate1, hasWingMate2;
	unsigned int totalKills, wingMate1Kills, wingMate2Kills;
	unsigned char wingMate1Ejects, wingMate2Ejects;
	unsigned int totalOtherKills;
	unsigned char secondaryMissions, secondaryMissionsCompleted;
	unsigned int shieldPickups, rocketPickups, cellPickups, powerups, minesKilled, cargoPickups;

	// slaves for Eyananth
	unsigned int slavesRescued;

	// remaining shield for experimental fighter
	unsigned int experimentalShield;

	unsigned int timeTaken; // In seconds
	unsigned char missionCompleted[10];

	signed char stationedPlanet;
	signed char destinationPlanet;

	char stationedName[20];
	char destinationName[20];
	int distanceCovered;

	unsigned char minPlasmaRate;
	unsigned char minPlasmaDamage;
	unsigned char minPlasmaOutput;
	unsigned char maxPlasmaRate;
	unsigned char maxPlasmaDamage;
	unsigned char maxPlasmaOutput;
	unsigned char maxPlasmaAmmo;
	unsigned char maxRocketAmmo;
	unsigned char shieldUnits;


};

struct ShopItem {

	int x, y;
	unsigned int price;
	char name[50];
	char description[255];
	unsigned char image;
};

struct bRect {

	int x, y, w, h;
	bRect *next;

};

struct Planet {

	int y;
	char name[50];
	SDL_Surface *image;

	signed char missionNumber; // associated mission number
	signed char missionCompleted; // whether it has been completed

	signed char messageMission;
	signed char messageSlot;
	signed char faceImage;
	char from[50];
	char subject[100];
};


struct globalEngineVariables {

	SDL_Event event;
	signed char done;

	SDL_RWops *sdlrw;

	float musicVolume;

	signed char maxAliens;

	float ssx;
	float ssy;

	Mix_Music *music;

	object *bulletHead;
	object *bulletTail;
	object *explosionHead;
	object *explosionTail;
	collectables *collectableHead;
	collectables *collectableTail;
	object *debrisHead;
	object *debrisTail;

	int cursor_x, cursor_y;

	signed char commsSection;

	signed char eventTimer;

	signed char lowShield;
	signed char averageShield;

	float targetShield;
	signed char targetIndex;
	int targetArrowTimer;

	// Mission completion timer (allows for 4 seconds before leaving sector)
	unsigned long missionCompleteTimer;

	// Times the mission normally
	unsigned int counter2;
	int timeTaken; // In seconds

	// For missions with a time limit
	signed char timeMission;
	unsigned int counter;
	signed char seconds;
	signed char minutes;

	// Mission Related stuff
	signed char allAliensDead;
	int addAliens;

	bool paused;
	signed char gameSection;

	bool useAudio;
	bool useSound;
	bool useMusic;
	bool fullScreen;

	// This really only applies to Linux users.
	char userHomeDirectory[1024];
	
	char keyState[350];

	bool cheat; // overall cheat
	bool cheatShield;
	bool cheatCash;
	bool cheatAmmo;
	bool cheatTime;
	bool cheatCredits;
};

struct event {

	int time;
	char message[255];
	signed char face;
	signed char entity;
	int flag;
};

struct cutMsg {

	int face;
	char message[255];

};

struct devVariables {

	signed char moveAliens;
	signed char fireAliens;
	
};



