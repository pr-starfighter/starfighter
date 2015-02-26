CXXFLAGS ?= -O2 -Wall -g
CXXFLAGS += `pkg-config --cflags sdl2 SDL2_image SDL2_mixer` -DLINUX
LIBS = `pkg-config --libs sdl2 SDL2_image SDL2_mixer`
OBJS = ai.o aliens.o audio.o bullets.o cargo.o collectable.o comms.o debris.o events.o explosions.o game.o globals.o graphics.o init.o intermission.o loadSave.o messages.o misc.o missions.o player.o resources.o script.o shop.o Starfighter.o title.o unpack.o weapons.o

VERSION = 1.2-SDL2
PROG = starfighter
PACK = starfighter.pak
DOCS = docs/*
DATA = data gfx sound
DATAFILES = data/* gfx/* sound/*

USEPACK ?= 0

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
%.o: src/%.cpp src/*.h
	$(CXX) $(CXXFLAGS) -c -DVERSION=\"$(VERSION)\" -DPACKLOCATION=\"$(DATADIR)$(PACK)\" -DUSEPACK=$(USEPACK) $<

# linking the program.
$(PROG): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJS) -o $(PROG) $(LIBS)

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

dist:
	rm -rf starfighter-$(VERSION)
	mkdir starfighter-$(VERSION)
	cp --parents -lt starfighter-$(VERSION) `git ls-files`
	git log >starfighter-$(VERSION)/ChangeLog
	tar czf starfighter-$(VERSION).tar.gz starfighter-$(VERSION)
	rm -rf starfighter-$(VERSION)

.PHONY: all clean install optimise unpack dist
