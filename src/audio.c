/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012 Guus Sliepen
Copyright (C) 2015-2020 The Diligent Circle <diligentcircle@riseup.net>

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

#include <math.h>

#ifndef NOSOUND
#include "SDL_mixer.h"
#endif

#include "defs.h"
#include "structs.h"

#include "alien.h"
#include "game.h"
#include "engine.h"
#include "screen.h"

#ifndef NOSOUND
static Mix_Chunk *sound[SFX_MAX];
static Mix_Music *music = NULL;
#endif

void audio_loadSounds()
{
#ifndef NOSOUND
	sound[SFX_EXPLOSION] = Mix_LoadWAV("sound/explode.ogg");
	sound[SFX_HIT] = Mix_LoadWAV("sound/explode2.ogg");
	sound[SFX_DEATH] = Mix_LoadWAV("sound/maledeath.ogg");
	sound[SFX_MISSILE] = Mix_LoadWAV("sound/missile.ogg");
	sound[SFX_PLASMA] = Mix_LoadWAV("sound/plasma.ogg");
	sound[SFX_CLOCK] = Mix_LoadWAV("sound/clock.ogg");
	sound[SFX_FLY] = Mix_LoadWAV("sound/flyby.ogg");
	sound[SFX_ENERGYRAY] = Mix_LoadWAV("sound/beamLaser.ogg");
	sound[SFX_PICKUP] = Mix_LoadWAV("sound/item.ogg");
	sound[SFX_SHIELDUP] = Mix_LoadWAV("sound/shield.ogg");
	sound[SFX_CLOAK] = Mix_LoadWAV("sound/cloak.ogg");
	sound[SFX_DEBRIS] = Mix_LoadWAV("sound/explode3.ogg");
	sound[SFX_DEBRIS2] = Mix_LoadWAV("sound/explode4.ogg");
	sound[SFX_LASER] = Mix_LoadWAV("sound/laser.ogg");
	sound[SFX_PLASMA2] = Mix_LoadWAV("sound/plasma2.ogg");
	sound[SFX_PLASMA3] = Mix_LoadWAV("sound/plasma3.ogg");
#endif
}

void audio_playSound(int sid, float x, float y)
{
#ifndef NOSOUND
	int channel = -1;
	static int freechannel = 4;
	static int channelVolume[4] = {0, 0, 0, 0};
	int angle = atanf((x - (screen->w / 2)) / (screen->w / 2)) * 180 / M_PI;
	int attenuation = fabsf(x - (screen->w / 2)) / (screen->w / 20);
	float distance = sqrtf(powf(fabsf(x - (screen->w / 2)), 2) + powf(fabsf(y - (screen->h / 2)), 2));
	const int max_volume = MIX_MAX_VOLUME / 4;
	int volume = max_volume - (max_volume * distance / (3 * screen->w));

	if ((!engine.useSound) || (!engine.useAudio) || (volume <= 0))
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

	if (channel == -1)
	{
		channel = freechannel++;
		if (freechannel >= 8)
			freechannel = 4;
	}
	else
	{
		if (Mix_Playing(channel) && (volume <= max_volume / 4)
				&& (channelVolume[channel] >= max_volume * 3 / 4))
			return;
		else
			channelVolume[channel] = volume;
	}

	angle %= 360;
	if (angle < 0)
		angle += 360;

	if (attenuation > 255)
		attenuation = 255;

	Mix_SetPosition(channel, angle, attenuation);
	Mix_Volume(channel, volume);
	Mix_PlayChannel(channel, sound[sid], 0);
#endif
}

void audio_haltMusic()
{
#ifndef NOSOUND
	if (Mix_PlayingMusic())
	{
		Mix_ResumeMusic();
		Mix_HaltMusic();
	}

	if (music != NULL)
	{
		Mix_FreeMusic(music);
		music = NULL;
	}
#endif
}

void audio_pauseMusic()
{
#ifndef NOSOUND
	if (Mix_PlayingMusic() && !Mix_PausedMusic())
		Mix_PauseMusic();
#endif
}

void audio_resumeMusic()
{
#ifndef NOSOUND
	Mix_ResumeMusic();
#endif
}

void audio_setMusicVolume(int volume)
{
#ifndef NOSOUND
	if (engine.useMusic && engine.useAudio)
		Mix_VolumeMusic(volume);
#endif
}

void audio_setMusicPosition(double position)
{
#ifndef NOSOUND
	if (engine.useMusic && engine.useAudio)
		Mix_SetMusicPosition(position);
#endif
}

void audio_playMusic(const char *filename, int loops, int amplified)
{
#ifndef NOSOUND
	if (engine.useMusic && engine.useAudio)
	{
		audio_haltMusic();
		music = Mix_LoadMUS(filename);
		audio_setMusicVolume(amplified ? MIX_MAX_VOLUME : MIX_MAX_VOLUME / 4);
		Mix_PlayMusic(music, loops);
	}
#endif
}

void audio_playRandomTrack()
{
#ifndef NOSOUND
	if ((!engine.useMusic) || (!engine.useAudio))
		return;

#ifdef OLD_MUSIC
	int tracks = 0;
	char track[][PATH_MAX] = {
		"music/Frantic.mod", "music/Artificial.mod", "music/Lunatic.mod",
		"music/ToxicFriend.mod", "music/DigitalInferno.mod",
		"music/TempoTrance.mod", "music/IntoTheMachine.mod"
	};

	switch (game.system)
	{
		case SYSTEM_SPIRIT:
			tracks = 3;
			break;
		case SYSTEM_EYANANTH:
			tracks = 5;
			break;
		case SYSTEM_MORDOR:
		case SYSTEM_SOL:
			tracks = 7;
			break;
		default:
			tracks = 3;
	}
#else
	int tracks = 4;
	char track[][PATH_MAX] = {
		"music/railjet_short.ogg", "music/space_dimensions.ogg",
		"music/frozen_jam.ogg", "music/sound_and_silence.ogg"
	};
#endif

	switch (game.area)
	{
#ifndef OLD_MUSIC
		case MISN_START:
		case MISN_INTERCEPTION:
			if ((aliens[ALIEN_KLINE].classDef == CD_KLINE)
					&& aliens[ALIEN_KLINE].active)
				audio_playMusic("music/last_cyber_dance.ogg", -1, 0);
			else if ((game.system == SYSTEM_MORDOR)
					&& (aliens[ALIEN_BOSS].classDef == CD_CLOAKFIGHTER)
					&& aliens[ALIEN_BOSS].active)
				audio_playMusic("music/space_dimensions.ogg", -1, 0);
			else
				audio_playMusic("music/railjet_short.ogg", -1, 0);
			break;
		case MISN_HAIL:
		case MISN_JOLDAR:
		case MISN_DORIM:
		case MISN_SIVEDI:
		case MISN_NEPTUNE:
		case MISN_SATURN:
			audio_playMusic("music/frozen_jam.ogg", -1, 0);
			break;
		case MISN_CERADSE:
		case MISN_ALLEZ:
		case MISN_URUSOR:
		case MISN_FELLON:
		case MISN_POSWIC:
		case MISN_MARS:
			audio_playMusic("music/sound_and_silence.ogg", -1, 0);
			break;
		case MISN_HINSTAG:
		case MISN_NEROD:
		case MISN_ODEON:
		case MISN_ALMARTHA:
		case MISN_PLUTO:
		case MISN_URANUS:
		case MISN_JUPITER:
			audio_playMusic("music/space_dimensions.ogg", -1, 0);
			break;
#endif
		case MISN_MOEBO:
		case MISN_ELAMALE:
		case MISN_ELLESH:
		case MISN_EARTH:
#ifdef OLD_MUSIC
			audio_playMusic("music/HardTranceDub.mod", -1, 0);
#else
			audio_playMusic("music/orbital_colossus.ogg", -1, 0);
#endif
			break;
		case MISN_VENUS:
#ifdef OLD_MUSIC
			audio_playMusic("music/LoopsAndTings.mod", -1, 0);
#else
			audio_playMusic("music/androids_last_dance.ogg", -1, 0);
#endif
			break;
		default:
			audio_playMusic(track[rand() % tracks], -1, 0);
	}
#endif
}

void audio_free()
{
#ifndef NOSOUND
	for (int i = 0 ; i < SFX_MAX ; i++)
	{
		if (sound[i] != NULL)
		{
			Mix_FreeChunk(sound[i]);
			sound[i] = NULL;
		}
	}

	if (music != NULL)
	{
		Mix_FreeMusic(music);
		music = NULL;
	}
#endif
}
