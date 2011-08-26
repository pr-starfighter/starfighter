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

#include "Starfighter.h"

Mix_Chunk *sound[MAX_SOUNDS];

void playSound(int sid)
{
	if ((!engine.useSound) || (!engine.useAudio))
		return;

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
			Mix_PlayChannel(-1, sound[sid], 0);
			break;
		case SFX_PLASMA:
		case SFX_LASER:
			Mix_PlayChannel(0, sound[sid], 0);
			break;
		case SFX_ENERGYRAY:
		case SFX_MISSILE:
			Mix_PlayChannel(1, sound[sid], 0);
			break;
		case SFX_HIT:
			Mix_PlayChannel(4, sound[sid], 0);
			break;
		case SFX_EXPLOSION:
		case SFX_DEBRIS:
		case SFX_DEBRIS2:
			Mix_PlayChannel(3, sound[sid], 0);
			break;
	}
}

Mix_Chunk *loadSound(const char *filename)
{
	Mix_Chunk *chunk;

	#if USEPACK
	unpack(filename, PAK_WAV);
	chunk = Mix_LoadWAV_RW(engine.sdlrw, 1);
	#else
	chunk = Mix_LoadWAV(filename);
	#endif

	return chunk;
}

void loadMusic(const char *filename)
{
	if (Mix_PlayingMusic())
		Mix_HaltMusic();

	if (engine.music != NULL)
		Mix_FreeMusic(engine.music);

	#if USEPACK
	unpack(filename, PAK_MOD);

	char musicFilename[PATH_MAX];
	strcpy(musicFilename, "");
	sprintf(musicFilename, "%smusic.mod", engine.userHomeDirectory);

	engine.music = Mix_LoadMUS(musicFilename);
	#else
	engine.music = Mix_LoadMUS(filename);
	#endif
}

void playRandomTrack()
{
	if ((!engine.useMusic) || (!engine.useAudio))
		return;

	int tracks = 0;

	char track[][30] = {
	"music/Frantic.mod", "music/Artificial.mod", "music/Lunatic.mod", "music/ToxicFriend.mod",
	"music/DigitalInferno.mod", "music/TempoTrance.mod", "music/IntoTheMachine.mod"
	};

	switch(currentGame.system)
	{
		case 0:
			tracks = 3;
			break;
		case 1:
			tracks = 5;
			break;
		case 2:
		case 3:
			tracks = 7;
			break;
	}

	switch(currentGame.area)
	{
		case 5:
		case 11:
		case 18:
		case 25:
			loadMusic("music/HardTranceDub.mod");
			break;
		case 26:
			loadMusic("music/LoopsAndTings.mod");
			break;
		default:
			loadMusic(track[rand() % tracks]);
	}

	Mix_PlayMusic(engine.music, -1);
}
