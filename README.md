
Growing Spaceships
==================
By Lasty

Game made in 72 hours For Ludum Dare 34 Game Jam.

Themes were "Growing", and "Two Button Controls"

As usual ran out of time, basic gameplay is here, but lacks depth and polish.  There is no sound.

Synopsis
--------
- A a 2D top-down space shooter.
- Shoot enemy ships which explodes parts off.
- Attach parts with your tractor beam to improve your ship.


TL;DR Controls
--------------
- WASD (Or arrows) to move
- TAB to change modes (Combat, or Salvage)
- Left click, Right click:  Depending on mode: tractor beam, or shoot
- Mouse Wheel (Or Keypad +/-) Zoom camera
- ESC Quit


Developer/cheat keys are:
- N : Spawn new random ship
- J : Spawn random part (Space Junk)
- F5: Save current ship to ships/custom.txt
- F9: Load the custom ship (Automatically loads on start anyway)
- F1: Toggle AI processing


Patch 1:
--------
Some post release tweaks:
- Better starting ship, a 10 second calm before enemy ships spawn.
- Fixed disappearing player ship bug.


Known Issues
----------
- There is no sound
- Occasional rare crash when attaching parts that get destroyed before being tractored in (May have been fixed)


Planed features that were cut
----------
Cut features due to time or complexity:
- Sound and/or Music
- More part types, including armour pieces, shield pieces, thrusters
- Energy management (Reactors, capacitors, can only shoot when charged)
- Damage/Repair mechanic
- World map, Space dock, buy/sell ships, economy, Cargo, Trading
- Better GUI, and a tutorial


Building
--------
You need two libraries, SDL2 and GLM
If they cannot be found in the standard search paths, edit the toplevel CMakeLists.txt to give it the locations

Requires Clang or GCC with C++ 14 features

SDL2 requires Mixer, Image, and TTF extensions to be installed also

Source Code is available on GitHub:  https://github.com/lasty/ld34_growing_spaceships

My build system was Windows 10 (64 bit), using MSYS2, Mingw64, gcc 5.2.0
