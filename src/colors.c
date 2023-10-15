/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012, 2013 Guus Sliepen
Copyright (C) 2015-2023 Diligent Circle <diligentcircle@riseup.net>

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

#include "SDL.h"

#include "defs.h"
#include "structs.h"

#include "screen.h"

Uint32 red;
Uint32 darkRed;
Uint32 yellow;
Uint32 lightYellow;
Uint32 darkYellow;
Uint32 green;
Uint32 lightGreen;
Uint32 darkGreen;
Uint32 blue;
Uint32 lightBlue;
Uint32 lighterBlue;
Uint32 darkBlue;
Uint32 darkerBlue;
Uint32 black;
Uint32 white;
Uint32 lightGrey;
Uint32 darkGrey;

/*
Finds the location of the requested color within the palette and returns
it's number. This colors are used for drawing rectangles, circle, etc in
the correct colors.
*/
void colors_init()
{
	red = SDL_MapRGB(screen->format, 0xff, 0x00, 0x00);
	darkRed = SDL_MapRGB(screen->format, 0x66, 0x00, 0x00);

	yellow = SDL_MapRGB(screen->format, 0xff, 0xff, 0x00);
	lightYellow = SDL_MapRGB(screen->format, 0xff, 0xff, 0x80);
	darkYellow = SDL_MapRGB(screen->format, 0x66, 0x66, 0x00);

	green = SDL_MapRGB(screen->format, 0x00, 0xff, 0x00);
	lightGreen = SDL_MapRGB(screen->format, 0x80, 0xff, 0x80);
	darkGreen = SDL_MapRGB(screen->format, 0x00, 0x66, 0x00);

	blue = SDL_MapRGB(screen->format, 0x00, 0x00, 0xff);
	lightBlue = SDL_MapRGB(screen->format, 0x99, 0x99, 0xff);
	lighterBlue = SDL_MapRGB(screen->format, 0xbb, 0xbb, 0xff);
	darkBlue = SDL_MapRGB(screen->format, 0x00, 0x00, 0xcc);
	darkerBlue = SDL_MapRGB(screen->format, 0x00, 0x00, 0x44);

	black = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	white = SDL_MapRGB(screen->format, 0xff, 0xff, 0xff);
	lightGrey = SDL_MapRGB(screen->format, 0xcc, 0xcc, 0xcc);
	darkGrey = SDL_MapRGB(screen->format, 0x99, 0x99, 0x99);
}
