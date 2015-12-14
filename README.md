
Ludum Dare 34
=============

Themes are "Growing", and "Two Button Controls"

By Lasty

Game not finished due to time (as per usual).


TL;DR Controls
--------------
- WASD (Or arrows) to move
- TAB to change modes (Combat, or Salvage)
- Left click, Right click:  Depending on mode: tractor beam, or shoot
- Mouse Wheel (Or KP+ KP-) Zoom camera
- ESC Quit


Since this is not finished, the developer/cheat keys are
- N : Spawn new random ship
- J : Spawn random part (Space Junk)
- F5: Save current ship to ships/custom.txt
- F9: Load the custom ship (Automatically loads on start anyway)


Synopsis
--------
- A a 2D top-down space shooter.
- Shoot enemy ships which explodes parts off.
- Attach parts with your tractor beam to improve your ship.


Planed features that were cut
----------
Cut features due to time or complexity:
- Sound and/or Music
- Enemy AI
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
