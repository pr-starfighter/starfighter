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
  "Classic" difficulty emulates it as closely as possible (see the
  CLASSIC DIFFICULTY DIFFERENCES section to read about the few
  differences that exist).

* Much of the dialog has been changed. There are various reasons for
  this; some of these include fixing bad writing, making the dialog
  match new music queues, and giving the characters more personality.

* In many places, the interface has been considerably improved. As an
  example, the simplistic target arrow of the original has been replaced
  with a system of several arrows, one for each ship.

* Typing "humansdoitbetter" in the title screen no longer enables
  cheats. This is actually because the switch to SDL2 broke the original
  feature, and rather than fixing it, I just replaced it with something
  else. I'll let you figure out what the new method to access the cheat
  menu is. :)

------------------------------------------------------------------------

CLASSIC DIFFICULTY DIFFERENCES

Classic difficulty is designed to emulate the experience of version 1.1
of the game (the last version released by Parallel Realities) as closely
as possible. However, there are several minor differences in addition to
the changes to graphics, sound, and dialog.  Here you can fine a list of
notable differences.

* The arrow pointing toward the current target is colored green if the
  target is an ally, whereas version 1.1 always showed a red arrow.
  The arrow is also positioned lower than in version 1.1 and does not
  disappear when radio messages are broadcast.

* Life bars for named characters are labeled with the respective
  character's name instead of "Target".

* Selling your secondary weapon leaves you with standard Rockets.  In
  version 1.1, it instead left you with no secondary weapon at all (with
  no way to gain standard Rockets back).

* If you buy a secondary weapon while one other than standard Rockets
  is already equipped, the currently equipped secondary weapon is
  automatically sold for you (which gives you money equal to half of
  said weapon's cost). In version 1.1, the old secondary weapon would
  simply disappear in this case, meaning the money was lost unless you
  explicitly used the "Sell" button.

* Travel in Spirit works the same as in every other system, except that
  there is no chance of interceptions in Classic difficulty.  Version
  1.1 instead instantly teleported you to the target planet as soon as
  you clicked on it.

* In the Venus mission, Kline starts some distance away directly to the
  right and immediately begins combat.  In contrast, version 1.1 spawned
  Kline much closer to the player (to the point where his ship was
  visible on the screen) and a bit further down, unmoving until a string
  of dialog boxes finished being displayed.  This change was made to
  accommodate new music and dialog.

* Similar to the above, the time spent to accommodate Kline's banter
  when he first appears in the Elamale mission is different from version
  1.1.

* Several bugs present in version 1.1 have been fixed. Many of these
  bugs affected gameplay in notable ways; such bugs include:

  * A bug in the shop that caused goods to be sold at the wrong price
    (notably exploitable to gain infinite cash by buying and selling
    the permanent cooling upgrade)
  * Overflow bugs that caused various numbers (especially plasma ammo)
    to "wrap around" to zero (leading to cases where gaining plasma
    ammo would cause you to lose plasma ammo instead)
  * A broken event in the Odeon mission that showed a odd error message
  * A bug that caused slaves rescued after the interception is destroyed
    to not count toward the total for the purpose of the Eyananth
    interception mission

------------------------------------------------------------------------

GENERATING CONFIGURE SCRIPT

If you contribute to Project: Starfighter's source code, you will need
to know how to generate a configure script for compiling the program.
NOTE: This is for developers only. End-users simply compiling releases
of Starfighter from source can ignore this section and skip straight to
the COMPILING FROM SOURCE section.

The following components are required to generate the configure script:

* Autoconf
* Automake
* pkg-config

Once these dependencies are installed, simply do the following from a
terminal window:

    autoreconf -vif

The arguments are technically optional, but recommended.

If for some reason you need to remove all of these files from your
directory, you can do so via the following command (requires Git):

	git clean -fdx

Note: automatically generated files are listed in .gitignore, so you
generally don't actually have to do this.

------------------------------------------------------------------------

COMPILING FROM SOURCE

Project: Starfighter depends on the following libraries to build:

* SDL2 <http://libsdl.org>
* SDL2_image <http://www.libsdl.org/projects/SDL_image/>
* SDL2_mixer <http://www.libsdl.org/projects/SDL_mixer/>

Once you have all dependencies installed, you can do the following from
a terminal window:

    ./configure
    make
    make install

Run "./configure --help" to see more options.

The third step ("make install") is technically optional, but highly
recommended.

For most GNU/Linux systems, an icon should be added to your menu when
you run "make install". You can click on this icon to start up the game.
Otherwise, you can manually install or use the launcher found in the
"misc" directory, or you can run the "starfighter" command to start up
the game.

If you choose not to use the "make install" command, you can find the
"starfighter" binary in the "src" directory.

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
keyboard or a gamepad if you prefer.

Other than that, have fun, and good luck!

