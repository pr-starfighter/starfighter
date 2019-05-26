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
as possible. However, there are a few minor differences in addition to
the changes to graphics, sound, and dialog:

* Where targets are is shown via arrows at the edge of the screen, and
  all targets are indicated in this way; the main target is indicated
  with text that says "Target", unless the target is one of the cast of
  named characters, and all named characters have their arrows indicated
  with their respective arrows.  In contrast, version 1.1 only had an
  optional arrow that rested in the middle of the screen and pointed in
  the general direction of the active (main) target.

* The active (main) target of the Poswic mission is Sid Wilson.  In
  version 1.1, the active target was instead the executive transport.
  The practical effect of this is that Sid Wilson's shield is indicated
  in this mission, whereas it wasn't in version 1.1.

* In the Venus mission, Kline starts some distance away directly to the
  right and immediately begins combat.  In contrast, version 1.1 spawned
  Kline much closer to the player (to the point where his ship was
  visible on the screen) and a bit further down, unmoving until a string
  of dialog boxes finished being displayed.  This change was made to
  accommodate new music and dialog.

* Similar to the above, the time spent to accommodate Kline's banter
  when he first appears in the Elamale mission is different from version
  1.1.

* The executive transport in the Poswic and Ellesh missions has a
  "wandering" A.I. type; in contrast, version 1.1 gave this ship a
  "normal" A.I. type.  The difference is that the "wandering" A.I. type
  never attempts to chase you, travels a further distance away on
  average, and starts to run away when shot at.

* Version 1.1 had a bug in the shop which caused selling something to
  redeem half the cost of the next item, rather than half the cost of
  the item being sold. This led to the last item in a series being sold
  for nothing (actually a result of undefined behavior), and it also
  allowed the player to continuously gain money by buying and selling
  the first permanent cooling upgrade.  This bug is fixed in the current
  release (including Classic difficulty).

* Version 1.1 had some overflow bugs, most notably with plasma ammo.
  The effect of this bug was that you could sometimes gain too much
  plasma ammo, resulting in the recorded amount being lower than before
  the powerup was picked up.  These bugs are fixed in the current
  release (including Classic difficulty).

* The Odeon mission in version 1.1 had a broken event with undefined
  behavior.  Players saw it as a message from Sid Wilson saying that
  Ursula was running away, followed by an error message.  In fact the
  source code never defined a fail condition for Ursula running away, so
  if the event had been defined properly, all it would have done is
  render the mission unwinnable.  The current release simply removes the
  broken event entirely (including for Classic difficulty).

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

