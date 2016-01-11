/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012 Guus Sliepen
Copyright (C) 2015, 2016 onpon4 <onpon4@riseup.net>

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

static Mix_Chunk *sound[SFX_MAX];
static Mix_Music *music = NULL;

void audio_loadSounds()
{
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
}

void audio_playSound(int sid, float x, float y)
{
	int channel = -1;
	static int freechannel = 4;
	static int channelVolume[4] = {0, 0, 0, 0};
	int angle = atanf((x - (screen->w / 2)) / (screen->w / 2)) * 180 / M_PI;
	int attenuation = fabsf(x - (screen->w / 2)) / (screen->w / 20);
	float distance = sqrtf(powf(fabsf(x - (screen->w / 2)), 2) + powf(fabsf(y - (screen->h / 2)), 2));
	int volume = MIX_MAX_VOLUME - (MIX_MAX_VOLUME * distance / (3 * screen->w));

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
		if (Mix_Playing(channel) && (volume <= MIX_MAX_VOLUME / 4) &&
				(channelVolume[channel] >= MIX_MAX_VOLUME * 3 / 4))
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
}

void audio_haltMusic()
{
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
}

void audio_pauseMusic()
{
	if (Mix_PlayingMusic() && !Mix_PausedMusic())
		Mix_PauseMusic();
}

void audio_resumeMusic()
{
	Mix_ResumeMusic();
}

void audio_setMusicVolume(int volume)
{
	if (engine.useMusic && engine.useAudio)
		Mix_VolumeMusic(volume);
}

void audio_playMusic(const char *filename, int loops)
{
	if (engine.useMusic && engine.useAudio)
	{
		audio_haltMusic();
		music = Mix_LoadMUS(filename);
		audio_setMusicVolume(100);
		Mix_PlayMusic(music, loops);
	}
}

void audio_playRandomTrack()
{
	if ((!engine.useMusic) || (!engine.useAudio))
		return;

	int tracks = 4;
	char track[][64] = {
		"music/railjet_short.ogg", "music/space_dimensions.ogg",
		"music/frozen_jam.ogg", "music/sound_and_silence.ogg"
	};

	switch(game.area)
	{
		case MISN_START:
			audio_playMusic("music/railjet_short.ogg", -1);
			break;
		case MISN_MOEBO:
		case MISN_ELAMALE:
		case MISN_ELLESH:
		case MISN_EARTH:
			audio_playMusic("music/orbital_colossus.ogg", -1);
			break;
		case MISN_VENUS:
			audio_playMusic("music/RE.ogg", -1);
			break;
		default:
			audio_playMusic(track[rand() % tracks], -1);
	}
}

void audio_free()
{
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
}
