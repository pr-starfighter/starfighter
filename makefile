CFLAGS = `sdl-config --cflags` -Wall -DLINUX
LIBS = `sdl-config --libs` -lSDL_mixer -lSDL_image
OBJS = ai.o aliens.o audio.o bullets.o cargo.o collectable.o comms.o debris.o events.o explosions.o game.o globals.o graphics.o init.o intermission.o loadSave.o messages.o misc.o missions.o player.o resources.o script.o shop.o Starfighter.o title.o unpack.o weapons.o

VERSION = 1.1
PROG = starfighter
PACK = starfighter.pak
DOCS = docs/*
DATA = data/* gfx/* music/* sound/*

BINDIR = /usr/games/
DATADIR = /usr/share/games/parallelrealities/
DOCDIR = /usr/share/doc/starfighter/
# top-level rule to create the program.
all: $(PROG)

# compiling other source files.
%.o: code/%.cpp code/%.h code/structs.h code/defs.h code/classes.h
	$(CXX) $(CFLAGS) -c -O3 -DVERSION=\"$(VERSION)\" -DPACKLOCATION=\"$(DATADIR)$(PACK)\" $<

# linking the program.
$(PROG): $(OBJS)
	$(CXX) $(LIBS) $(OBJS) -o $(PROG)

# cleaning everything that can be automatically recreated with "make".
clean:
	$(RM) $(OBJS)

distclean:
	$(RM) $(PROG)

# install
install:
	mkdir -p $(DATADIR)
	strip $(PROG)
	install -o root -g games -m 755 $(PROG) $(BINDIR)$(PROG)
	install -o root -g games -m 644 $(PACK) $(DATADIR)$(PACK)
	cp $(DOCS) $(DOCDIR)

$(PACK): pack.py $(DATA)
	./pack.py $(PACK) $(DATA)

unpack: unpack.py
	./unpack.py $(PACK)

.PHONY: all clean distclean unpack
