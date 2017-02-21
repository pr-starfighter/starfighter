#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
#
# This file has been dedicated to the public domain, to the extent
# possible under applicable law, via CC0. See
# http://creativecommons.org/publicdomain/zero/1.0/ for more
# information. This file is offered as-is, without any warranty.

AC_PREREQ([2.69])
AC_INIT([Project: Starfighter], [1.7-dev], [onpon4@riseup.net], [starfighter])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_CONFIG_SRCDIR([src/Starfighter.cpp])
AC_CONFIG_HEADERS([config.h])
PKG_PROG_PKG_CONFIG

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL

STARFIGHTER_CFLAGS="-DVERSION=\\\"$PACKAGE_VERSION\\\""

# Checks for libraries.
PKG_CHECK_EXISTS([SDL2_mixer], [
	PKG_CHECK_MODULES([SDL], [sdl2 SDL2_image SDL2_mixer])
], [
	PKG_CHECK_MODULES([SDL], [sdl2 SDL2_image])
	STARFIGHTER_CFLAGS="$STARFIGHTER_CFLAGS -DNOSOUND"
	echo "Note: SDL_mixer not found; audio will not be supported."
])

AC_ARG_VAR([SF_SCREEN_WIDTH], [The width of the game window in pixels])
AC_ARG_VAR([SF_SCREEN_HEIGHT], [The height of the game window in pixels])
AS_IF([test -n "$SF_SCREEN_WIDTH"], [
	STARFIGHTER_CFLAGS="$STARFIGHTER_CFLAGS -DSCREEN_WIDTH=$SF_SCREEN_WIDTH"
])
AS_IF([test -n "$SF_SCREEN_HEIGHT"], [
	STARFIGHTER_CFLAGS="$STARFIGHTER_CFLAGS -DSCREEN_HEIGHT=$SF_SCREEN_HEIGHT"
])

AC_SUBST([STARFIGHTER_CFLAGS])

# Checks for header files.
AC_CHECK_HEADERS([ctype.h errno.h stdio.h stdlib.h string.h time.h math.h pwd.h sys/stat.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([atexit mkdir])

AC_CONFIG_FILES([
	Makefile
	src/Makefile
	misc/Makefile
])
AC_OUTPUT
