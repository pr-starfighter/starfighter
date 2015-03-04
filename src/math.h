/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011 Guus Sliepen
Copyright (C) 2015 Julian Marchant

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

#ifndef MATH_H
#define MATH_H

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
		*in += high - low;
	if (*in > high)
		*in -= high - low;
}

static inline void wrapInt(int *in, int low, int high)
{
	if (*in < low)
		*in += high - low;
	if (*in > high)
		*in -= high - low;
}

static inline void wrapFloat(float *in, float low, float high)
{
	if (*in < low)
		*in += high - low;
	if (*in > high)
		*in -= high - low;
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

#endif
