/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012, 2013 Guus Sliepen
Copyright (C) 2015, 2017 Julie Marchant <onpon4@riseup.net>

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

SDL_Renderer *renderer;
SDL_Texture *renderer_texture;

void renderer_update()
{
	// XXX: The SDL wiki says that SDL_UpdateTexture is slow and shouldn't
	// be used for textures that are updated often. This should be looked
	// into.
	SDL_UpdateTexture(renderer_texture, NULL, screen->pixels, screen->w * 4);
	SDL_RenderCopy(renderer, renderer_texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}
