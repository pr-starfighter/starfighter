# This file has been dedicated to the public domain, to the extent
# possible under applicable law, via CC0. See
# http://creativecommons.org/publicdomain/zero/1.0/ for more
# information. This file is offered as-is, without any warranty.

bin_PROGRAMS = starfighter

starfighter_CPPFLAGS = $(STARFIGHTER_CFLAGS) -DDATADIR=\"$(pkgdatadir)\" -Wall
starfighter_CXXFLAGS = $(SDL_CFLAGS)
starfighter_LDADD = $(SDL_LIBS)

starfighter_SOURCES = \
	Starfighter.cpp \
	alien.cpp \
	alien.h \
	audio.cpp \
	audio.h \
	bullet.cpp \
	bullet.h \
	cargo.cpp \
	cargo.h \
	collectable.cpp \
	collectable.h \
	colors.cpp \
	colors.h \
	cutscene.cpp \
	cutscene.h \
	defs.h \
	engine.cpp \
	engine.h \
	event.cpp \
	event.h \
	explosion.cpp \
	explosion.h \
	game.cpp \
	game.h \
	gfx.cpp \
	gfx.h \
	info.cpp \
	info.h \
	intermission.cpp \
	intermission.h \
	mission.cpp \
	mission.h \
	player.cpp \
	player.h \
	radio.cpp \
	radio.h \
	renderer.cpp \
	renderer.h \
	save.cpp \
	save.h \
	screen.cpp \
	screen.h \
	ship.cpp \
	ship.h \
	shop.cpp \
	shop.h \
	structs.h \
	title.cpp \
	title.h \
	weapons.cpp \
	weapons.h \
	window.cpp \
	window.h
