# This file has been dedicated to the public domain, to the extent
# possible under applicable law, via CC0. See
# http://creativecommons.org/publicdomain/zero/1.0/ for more
# information. This file is offered as-is, without any warranty.

bin_PROGRAMS = starfighter

starfighter_CFLAGS = $(STARFIGHTER_CFLAGS) $(SDL_CFLAGS) -DDATADIR=\"$(pkgdatadir)\" -Wall
starfighter_LDADD = $(SDL_LIBS)

starfighter_SOURCES = \
	Starfighter.c \
	alien.c \
	alien.h \
	audio.c \
	audio.h \
	bullet.c \
	bullet.h \
	cargo.c \
	cargo.h \
	collectable.c \
	collectable.h \
	colors.c \
	colors.h \
	cutscene.c \
	cutscene.h \
	defs.h \
	engine.c \
	engine.h \
	event.c \
	event.h \
	explosion.c \
	explosion.h \
	game.c \
	game.h \
	gfx.c \
	gfx.h \
	info.c \
	info.h \
	intermission.c \
	intermission.h \
	mission.c \
	mission.h \
	player.c \
	player.h \
	radio.c \
	radio.h \
	renderer.c \
	renderer.h \
	save.c \
	save.h \
	screen.c \
	screen.h \
	ship.c \
	ship.h \
	shop.c \
	shop.h \
	structs.h \
	title.c \
	title.h \
	weapons.c \
	weapons.h \
	window.c \
	window.h
