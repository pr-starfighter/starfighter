CFLAGS ?= -O2 -Wall -g
CFLAGS += `sdl-config --cflags` -DLINUX
LIBS = `sdl-config --libs` -lSDL_mixer -lSDL_image
OBJS = ai.o aliens.o audio.o bullets.o cargo.o collectable.o comms.o debris.o events.o explosions.o game.o globals.o graphics.o init.o intermission.o loadSave.o messages.o misc.o missions.o player.o resources.o script.o shop.o Starfighter.o title.o unpack.o weapons.o

VERSION = 1.1
PROG = starfighter
PACK = starfighter.pak
DOCS = docs/*
DATA = data gfx music sound
DATAFILES = data/* gfx/* music/* sound/*

USEPACK ?= 1

PREFIX ?= /usr
BINDIR ?= $(PREFIX)/games/
DATADIR ?= $(PREFIX)/share/games/parallelrealities/
DOCDIR ?= $(PREFIX)/share/doc/$(PROG)/

# top-level rule to create the program.
ALL = $(PROG)
ifeq ($(USEPACK), 1)
	ALL += $(PACK)
endif

all: $(ALL)

# compiling other source files.
%.o: code/%.cpp code/*.h
	$(CXX) $(CFLAGS) -c -DVERSION=\"$(VERSION)\" -DPACKLOCATION=\"$(DATADIR)$(PACK)\" -DUSEPACK=$(USEPACK) $<

# linking the program.
$(PROG): $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) -o $(PROG) $(LIBS)

# cleaning everything that can be automatically recreated with "make".
clean:
	$(RM) $(OBJS) $(ALL)

# install
install: $(ALL)
	mkdir -p $(DESTDIR)$(BINDIR)
	mkdir -p $(DESTDIR)$(DATADIR)
	mkdir -p $(DESTDIR)$(DOCDIR)

	install -m 755 $(PROG) $(DESTDIR)$(BINDIR)$(PROG)
ifeq ($(USEPACK), 1)
	install -m 644 $(PACK) $(DESTDIR)$(DATADIR)$(PACK)
else
	cp -pr $(DATA) $(DESTDIR)$(DATADIR)
endif
	cp -p $(DOCS) $(DESTDIR)$(DOCDIR)

$(PACK): pack.py $(DATAFILES)
	./pack.py $(PACK) $(DATAFILES)

unpack: unpack.py
	./unpack.py $(PACK)

optimise:
	advpng -z gfx/*.png
	jpegoptim --strip-all gfx/*.jpg

.PHONY: all clean install optimise unpack
