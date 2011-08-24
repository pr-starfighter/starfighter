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

#include "events.h"

/*
Checked during the main game loop. When the game is paused
it goes into a constant loop checking this routine. If escape is
pressed, the game automatically ends and goes back to the title screen
*/
signed char checkPauseRequest()
{
	getPlayerInput();
		
	if (engine.keyState[SDLK_ESCAPE])
	{
		engine.paused = 0;
		engine.done = 1;
		player.shield = 0;
		return 1;
	}
	
	if (engine.keyState[SDLK_p])
	{
		engine.paused = 0;
		engine.keyState[SDLK_p] = 0;
	}

	return 0;
}

void compareLastKeyInputs()
{
	if (strstr(lastKeyEvents, "humansdoitbetter") != NULL)
		{engine.cheat = 1; memset(lastKeyEvents, ' ', 25);}

	if (strstr(lastKeyEvents, "credits") != NULL)
		{engine.cheatCredits = 1; memset(lastKeyEvents, ' ', 25);}
}

void addKeyEvent(const char *keyName)
{
	if (strlen(keyName) > 1)
		return;

	int index = -1;

	for (int i = 0 ; i < 25 ; i++)
	{
		if (lastKeyEvents[i] == ' ')
		{
			index = i;
			break;
		}
	}
	
	if (index == -1)
	{
		for (int i = 0 ; i < 25 ; i++)
		{	
			lastKeyEvents[i] = lastKeyEvents[i + 1];
		}
		
		index = 24;
	}
	
	lastKeyEvents[index] = keyName[0];
	
	compareLastKeyInputs();
}
