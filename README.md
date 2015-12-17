
Growing Spaceships
==================
By Lasty

Game made in 72 hours For Ludum Dare 34 Game Jam.

Themes were "Growing", and "Two Button Controls"

- Patch 1 fixes some critical issues
- Post-competition Patch 2 adds a lot more polish (Sounds and a Tutorial)


Synopsis
--------
- A a 2D top-down space shooter.
- Shoot enemy ships which explodes parts off.
- Attach parts with your tractor beam to improve your ship.


Controls
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
- F9: Load the custom ship
- F1: Toggle AI processing
- F2: New game with a random ship (Also skips tutorial)


Patch 1:
--------
Some post release tweaks:
- Better starting ship, a 10 second calm before enemy ships spawn.
- Fixed disappearing player ship bug.


Patch 2:
--------
(Post competition update)
- Added in some sounds.  (Really wish I spent some time during comp to do this,
because it really helps with the game immersion)
- Some bugfixes:
  - Fixed Camera tracking slightly off center, and not following mouse
  - Could Destroy enemy ships, and player's ship core with "scrap" in salvage mode
- UI Tweaks:
  - Added more audio and visual feedback to attach mechanic.  Should be more obvious now.
  - Easier part selection
  - Better and more visible cursors, and selection markers
- Gameplay changes:
  - Changed "scrap" function to "detach": easily remove parts from player ship without destroying them.
  - Implemented the Armour block, will take 2 shots to destroy.
  - Implemented a Tutorial for first time running.
  - Starting ship is randomized.


Planed features that were cut
----------
Cut features due to time or complexity:
- Music
- Connect more than 1 piece at a time, and angled connections (eg at 45 degrees)
- More part types, including shield pieces, thrusters, turreted weapons
- Energy management (Reactors, capacitors, can only shoot when charged)
- Damage/Repair mechanic
- World map, Space dock, buy/sell ships, economy, Cargo, Trading
- Better GUI, AI
- Particles, explosions


Building
--------
You need two libraries, SDL2 and GLM
If they cannot be found in the standard search paths, edit the toplevel CMakeLists.txt to give it the locations

Requires Clang or GCC with C++ 14 features

SDL2 requires Mixer, Image, and TTF extensions to be installed also

Source Code is available on GitHub:  https://github.com/lasty/ld34_growing_spaceships

My build system was Windows 10 (64 bit), using MSYS2, Mingw64, gcc 5.2.0
