/*
Copyright (C) 2003 Parallel Realities

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

Mix_Chunk *sound[MAX_SOUNDS];

void playSound(int sid, float x)
{
	if ((!engine.useSound) || (!engine.useAudio))
		return;

	int channel = -1;
	static int freechannel = 4;

	switch(sid)
	{
		case SFX_DEATH:
		case SFX_CLOCK:
		case SFX_FLY:
		case SFX_SHIELDUP:
		case SFX_PICKUP:
		case SFX_CLOAK:
		case SFX_PLASMA2:
		case SFX_PLASMA3:
			channel = -1;	
			break;
		case SFX_PLASMA:
		case SFX_LASER:
			channel = 0;
			break;
		case SFX_ENERGYRAY:
		case SFX_MISSILE:
			channel = 1;
			break;
		case SFX_HIT:
			channel = 2;
			break;
		case SFX_EXPLOSION:
		case SFX_DEBRIS:
		case SFX_DEBRIS2:
			channel = 3;
			break;
	}

	if(channel == -1) {
		channel = freechannel++;
		if(freechannel >= 8)
			freechannel = 4;
	}

	int angle = atanf((x - 400) / 400) * 180 / M_PI;
	int attenuation = fabsf(x - 400) / 40;

	if(angle < 0)
		angle += 360;

	if(attenuation > 255)
		attenuation = 255;

	Mix_SetPosition(channel, angle, attenuation);
	Mix_PlayChannel(channel, sound[sid], 0);
}

Mix_Chunk *loadSound(const char *filename)
{
	Mix_Chunk *chunk;

	chunk = Mix_LoadWAV(filename);

	return chunk;
}

void loadMusic(const char *filename)
{
	if (Mix_PlayingMusic())
		Mix_HaltMusic();

	if (engine.music != NULL)
		Mix_FreeMusic(engine.music);

	engine.music = Mix_LoadMUS(filename);
}

void playRandomTrack()
{
	if ((!engine.useMusic) || (!engine.useAudio))
		return;

	int tracks = 0;

	char track[][64] = {
	"music/railjet_short.ogg", "music/grey_sector.ogg",
	"music/space_dimensions.ogg", "music/frozen_jam.ogg"
	};

	switch(currentGame.system)
	{
		case 0:
		case 1:
			tracks = 2;
			break;
		case 2:
		case 3:
			tracks = 4;
			break;
	}

	switch(currentGame.area)
	{
		case 5:
		case 11:
		case 18:
		case 25:
			loadMusic("music/orbital_colossus.ogg");
			break;
		case 26:
			loadMusic("music/RE.ogg");
			break;
		default:
			loadMusic(track[rand() % tracks]);
	}

	Mix_PlayMusic(engine.music, -1);
}
