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

extern Graphics graphics;
extern Star star[200];

extern bool collision(float x0, float y0, int w0, int h0, float x2, float y2, int w1, int h1);
extern bool collision(object *object1, object *object2);
extern bool collision(collectables *object1, object *object2);

extern SDL_Surface *loadImage(const char *filename);
extern void doStarfield();
extern int isOnScreen(int x, int y, int w, int h);
