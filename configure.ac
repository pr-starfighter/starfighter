#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
#
# This file has been dedicated to the public domain, to the extent
# possible under applicable law, via CC0. See
# http://creativecommons.org/publicdomain/zero/1.0/ for more
# information. This file is offered as-is, without any warranty.

AC_PREREQ([2.69])
AC_INIT([Project: Starfighter], [2.3.3], [diligentcircle@riseup.net], [starfighter])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_CONFIG_SRCDIR([src/Starfighter.c])
AC_CONFIG_HEADERS([config.h])
PKG_PROG_PKG_CONFIG

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

STARFIGHTER_CPPFLAGS="-DVERSION=\\\"$PACKAGE_VERSION\\\""
STARFIGHTER_LDADD=""

# Detect MacOS
AC_CANONICAL_HOST

case $host_os in
	darwin*)
		build_mac=1
		;;
esac

# Checks for libraries.

AC_SEARCH_LIBS([atanf], [m])

PKG_CHECK_EXISTS([SDL2_mixer], [
	PKG_CHECK_EXISTS([SDL2_ttf], [
		PKG_CHECK_MODULES([SDL], [sdl2 SDL2_image SDL2_mixer SDL2_ttf])
	], [
		PKG_CHECK_MODULES([SDL], [sdl2 SDL2_image SDL2_mixer])
		STARFIGHTER_CPPFLAGS="$STARFIGHTER_CPPFLAGS -DNOFONT"
		echo "Note: SDL_ttf not found; Unicode will not be supported."
	])
], [
	PKG_CHECK_EXISTS([SDL2_ttf], [
		PKG_CHECK_MODULES([SDL], [sdl2 SDL2_image SDL2_ttf])
	], [
		PKG_CHECK_MODULES([SDL], [sdl2 SDL2_image])
		STARFIGHTER_CPPFLAGS="$STARFIGHTER_CPPFLAGS -DNOFONT"
		echo "Note: SDL_ttf not found; Unicode will not be supported."
	])
	STARFIGHTER_CPPFLAGS="$STARFIGHTER_CPPFLAGS -DNOSOUND"
	echo "Note: SDL_mixer not found; audio will not be supported."
])
PKG_CHECK_MODULES([PANGO], [pango], [
], [
	STARFIGHTER_CPPFLAGS="$STARFIGHTER_CPPFLAGS -DNOFONT"
	echo "Note: Pango not found; Unicode will not be supported."
])

AC_ARG_VAR([SF_WARN], [Set to 1 to enable compiler warnings])
AC_ARG_VAR([SF_SCREEN_WIDTH], [The width of the game window in pixels])
AC_ARG_VAR([SF_SCREEN_HEIGHT], [The height of the game window in pixels])
AC_ARG_VAR([SF_NOFONT], [Set to 1 to manually force the compiler not to include font/Unicode support])
AC_ARG_VAR([SF_RUN_IN_PLACE], [Set to 1 to compile Starfighter to run in-place (instead of installing)])
AS_IF([test -n "$SF_WARN"], [
	STARFIGHTER_CPPFLAGS="$STARFIGHTER_CPPFLAGS -Wall -Wformat-truncation=0"
])
AS_IF([test -n "$SF_SCREEN_WIDTH"], [
	STARFIGHTER_CPPFLAGS="$STARFIGHTER_CPPFLAGS -DSCREEN_WIDTH=$SF_SCREEN_WIDTH"
	echo "Using default screen width of $SF_SCREEN_WIDTH"
], [
	echo "Using built-in screen width default"
])
AS_IF([test -n "$SF_SCREEN_HEIGHT"], [
	STARFIGHTER_CPPFLAGS="$STARFIGHTER_CPPFLAGS -DSCREEN_HEIGHT=$SF_SCREEN_HEIGHT"
	echo "Using default screen height of $SF_SCREEN_HEIGHT"
], [
	echo "Using built-in screen height default"
])

AS_IF([test -n "$SF_NOFONT"], [
	STARFIGHTER_CPPFLAGS="$STARFIGHTER_CPPFLAGS -DNOFONT"
	echo "Font/Unicode support manually disabled"
])

AS_IF([test -n "$SF_RUN_IN_PLACE"], [
	echo "Preparing a run-in-place build"
])

AS_IF([test -n "$build_mac"], [
	STARFIGHTER_LDADD="$STARFIGHTER_LDADD -framework CoreFoundation"
	echo "Building for MacOS"
])

AM_CONDITIONAL([RUN_IN_PLACE], [test -n "$SF_RUN_IN_PLACE"])

AC_SUBST([STARFIGHTER_CPPFLAGS])
AC_SUBST([STARFIGHTER_LDADD])

# Checks for header files.
AC_CHECK_HEADERS([ctype.h errno.h libintl.h locale.h stdio.h stdlib.h string.h time.h math.h pwd.h sys/stat.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([atexit mkdir])

AC_CONFIG_FILES([
	Makefile
	gfx/Makefile
	locale/Makefile
	misc/Makefile
	music/Makefile
	sound/Makefile
	src/Makefile
])
AC_OUTPUT
