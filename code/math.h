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

static inline void limitChar(signed char *in, int low, int high)
{
	if (*in < low)
		*in = low;
	if (*in > high)
		*in = high;
}

static inline void limitChar(unsigned char *in, int low, int high)
{
	if (*in < low)
		*in = low;
	if (*in > high)
		*in = high;
}

static inline void limitCharAdd(signed char *in, int add, int low, int high)
{
	int tmp = (int)*in + add;
	if (tmp < low)
		tmp = low;
	if (tmp > high)
		tmp = high;
	*in = tmp;
}

static inline void limitCharAdd(unsigned char *in, int add, int low, int high)
{
	int tmp = (int)*in + add;
	if (tmp < low)
		tmp = low;
	if (tmp > high)
		tmp = high;
	*in = tmp;
}

static inline void limitInt(int *in, int low, int high)
{
	if (*in < low)
		*in = low;
	if (*in > high)
		*in = high;
}

static inline void limitFloat(float *in, float low, float high)
{
	if (*in < low)
		*in = low;
	if (*in > high)
		*in = high;
}

static inline void wrapChar(signed char *in, signed char low, signed char high)
{
	if (*in < low)
		*in = high;
	if (*in > high)
		*in = low;
}

static inline void wrapInt(int *in, int low, int high)
{
	if (*in < low)
		*in = high;
	if (*in > high)
		*in = low;
}

static inline void wrapFloat(float *in, float low, float high)
{
	if (*in < low)
		*in = high;
	if (*in > high)
		*in = low;
}

static inline int rrand(int min, int max)
{
	int r = min;

	max++;

	if ((max - min) == 0)
		return min;

	r += rand() % (max - min);

	return r;
}
