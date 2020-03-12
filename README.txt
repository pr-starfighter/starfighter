This file has been dedicated to the public domain, to the extent
possible under applicable law, via CC0. See
http://creativecommons.org/publicdomain/zero/1.0/ for more
information. This file is offered as-is, without any warranty.

========================================================================


Thank you for downloading Project: Starfighter!

Project: Starfighter is a space shoot 'em up game originally developed
by Parallel Realities in 2002, and released in 2003. You assume the role
of Chris Bainfield in his quest to put an end to WEAPCO, the weapons
corporation which has enslaved the galaxy.

If you played one of the versions of Project: Starfighter distributed by
Parallel Realities, you might notice some differences in this version
compared to the original:

* The graphics, sounds, and music are all completely different. This is
  because most of the original media was not properly licensed, and
  therefore needed to be replaced to make the game 100% libre. Most of
  this work was done by the previous project at SourceForge; the current
  project completed that work by adding music and replacing one
  non-libre graphic that got into version 1.2 by mistake.

* There are several gameplay changes. These changes were mostly done to
  balance the game better. However, if you want the original experience,
  "Classic" difficulty emulates it as closely as possible.

* Much of the dialog has been changed. There are various reasons for
  this; some of these include fixing bad writing, making the dialog
  match new music queues, and giving the characters more personality.

* In many places, the interface has been considerably improved. As an
  example, the simplistic target arrow of the original has been replaced
  with a system of several arrows, one for each ship.

* Unicode and gettext translation support have been added.

* Typing "humansdoitbetter" in the title screen no longer enables
  cheats. This is actually because the switch to SDL2 broke the original
  feature, and rather than fixing it, I just replaced it with something
  else. I'll let you figure out what the new method to access the cheat
  menu is. :)

------------------------------------------------------------------------

PLAYING THE GAME

The basic controls are the arrow keys, Ctrl, Space, Shift, and Escape.
Other keys on they keyboard can also be used in case of keyjamming or if
you simply prefer other controls; alternative keys include the keypad,
ZXC, ASD, <>?, and 123. A gamepad or joystick can also be used.

The basic objective of Project: Starfighter is simply to complete all
missions. Exactly what entails completing a mission varies and is
explained in-game.

In the system overview screen, various sections can be accessed by
clicking the icons on the bottom of the screen. You can also use the
keyboard or a gamepad if you prefer (use the arrows to move the cursor
and the fire button to "click" on things).

Other than that, have fun, and good luck!

------------------------------------------------------------------------

COMPILING FROM SOURCE

If you are an end-user, it is easiest to use an existing binary
distribution if possible.  If you want or need to compile yourself,
however, instructions follow.

Note: Developers compiling the source code from the Git repository,
please first see the GENERATING CONFIGURE SCRIPT AND BUILDING LOCALES
section below.

Project: Starfighter depends on the following libraries to build:

* SDL2 <http://libsdl.org>
* SDL2_image <http://www.libsdl.org/projects/SDL_image/>
* SDL2_mixer <http://www.libsdl.org/projects/SDL_mixer/>
* SDL2_ttf <http://www.libsdl.org/projects/SDL_ttf/>
* Pango <https://www.pango.org/>

Once you have all dependencies installed,  do the following from the
Project: Starfighter base directory:

    ./configure
    make
    make install

This will perform a system-wide installation, which is recommended for
most users.  For most Linux systems, an icon should be added to your
menu which you can then use to run Starfighter; if not, you can use the
launcher found in the "misc" directory or run the "starfighter" command
manually.

If you would prefer a "run in place" build, you should instead do the
following from the Project: Starfighter base directory:

    ./configure SF_RUN_IN_PLACE=1
    make
    mv src/starfighter .

Note that you may see errors relating to 'TTF_OpenFont' failures if you did not move the binary out of the src directory. E.g:

    Current directory "/Users/yourname/code/starfighter/src"
    ERROR: TTF_OpenFont: Couldn't open data/TakaoPGothic.ttf

Run "./configure --help" to see all options for compiling.

------------------------------------------------------------------------

GENERATING CONFIGURE SCRIPT AND BUILDING LOCALES

If you contribute to Project: Starfighter's source code, you will need
to know how to generate a configure script and build locales needed for
compiling the program. NOTE: This is for developers and other people
compiling source code taken from the Git repository. End-users simply
compiling releases of Starfighter from source can ignore this section.

The following components are required to generate the configure script:

* Autoconf
* Automake
* pkg-config

And the following is required to build locales:

* Python

Once these dependencies are installed, simply do the following from a
terminal window:

    autoreconf -vif
    cd locale
    ./build.py
    cd ..

The arguments to autoreconf are technically optional, but recommended.

The Python script build.py may fail on MacOS due to a missing msgfmt
program. msgfmt is part of gettext and the version that ships on a Mac
does not include the msgfmt utility. This can be solved by using gettext
from Homebrew:

    brew install gettext
    export PATH="$(brew --prefix gettext)/bin:$PATH"

If for some reason you need to remove all of these files from your
directory, you can do so via the following command (requires Git):

    git clean -fdx

Note: automatically generated files are listed in .gitignore, so you
generally don't actually have to do this.
