# Change Log
All notable changes to this project (Hexterminal Havoc) will be documented in
this file.

This project adheres to [Semantic Versioning](http://semver.org/).

## [1.09] - 2016-11-17

### Added
- UI screen for inventory
- UI screen for equipment
- UI screen for inspecting items
- action for equipping items
- action for dropping items
- action for unequipping items
- action for expunging items
- player health now shows up on the bottom left

### Changed
- dealing damage now uses damage dices
- speed is not affected by items equipped
- npcs will displace other npcs when they move to a position

## [1.08] - 2016-11-9

### Added
- added item parser
- implemented monster factory
- implemented item factory
- added colors to dungeon!

## [1.07] - 2016-11-2

### Added
- parser class to parse monster description file
- monster_description class to hold all parsed monster attributes
- dice class to hold dice information (will be fully implemented later)

## Changed
- split codebase into C and C++ files
  - `csrc/` now contains all C files and is a snapshot of version 1.06.1
  - `src/` now contains all C++ filese and will be where updates are
- refactored codebase form structs to classes
- Makefile changed to accomodate file structure change

## [1.06.1] - 2016-10-29

### Added
- pc ai mode flag (--ai or -a)
- ncurses render mode flag (--ncurses or -n), enabled automatically when using keyboard control
- help message updated

### Changed
- teardown methods are more robust and not prone to segmentation faults in some control flow paths
- Spawn points passed in through CLI are verified before placing the pc at that position
- If a spawn point passed is invalid a log message is generated and it is placed at a random point

### Fixed
- memory leak fixes with the line of sight pathing in the npc ai
- memory leak fixes with the .cpp character management
- floating point exception when using some CLI flags
- small bug fixes with control flow path

## [1.06] - 2016-10-26

### Added
- character.cpp (start of C ~> C++ conversion)
- fog of war 

### Changed
- Makefile can compile for C or C++ (use `make` or `make cxx` for C++ and `make c` for C)
- Refactors to existing codebase to accomodate the change to C++

### Fixed
- Crash when pc moved to path that had been tunnelled by an npc
- initialization error with character class
- small bug fixes
- memory leaks

## [1.05] - 2016-10-19

### Added
- ncurses support (on by default)
- player control (pc ai is still available, but recompilation is necessary to turn it on)
- monster list that shows monster and position relative to player
- staircases and movement between dungeon floors

### Changed
- Refactors to cleanup code at the end of the program (all cleanup is housed in one function)
- character array now only holds npcs (pc is separate)

### Fixed
- small bug fixes
- fixed memory leak due to player ai and character variable

## [1.04] - 2016-10-4

### Added
- Event queue to perform different npc and player events
- Player AI to control player in dungeon (temporary!)
- NPC AI to have monsters chase the player
- More Logging!
- bresenhams algorithm for line of sight checking

### Changed
- Major refactors towards a more object oriented style
- Change functions to have funtion pointers in structs instead of separate API (when applicable)

### Fixed
- Various bug fixes

## [1.03] - 2016-9-21

### Added
- Player Marker on the dungeon (gets randomly spawned on dungeon load or generate)
- Path maps for NPCs (different ones for tunneling and non-tunneling npcs)
- New pathfinderAPI to control path maps
- Print mode for printing the dungeon or path maps
- More Logging!

### Changed
- Makefile is now generic and will find and build all source code
- Broke dijkstraAPI up into 3 APIs
    - dijkstraAPI - controls dikjstras algorithm
    - graphAPI - controls graph related functions
    - corridorAPI - controls corridor generation
- Changed all functions to be static (now only accessible through apis)
- Corridor generation
    - Marks rooms as connected when path goes through them without being the target
    - Does not create paths if both rooms are already connected
- Various API changes (to make things clearer)

### Fixed
- Bug with heap re-inserting items already on the queue
- Memory leaks when destructing path map graphs

## [1.02] - 2016-9-14

### Added
- Dungeon Saving (added CLI flag and optional argument to specify the name)
- Dungeon Loading (added CLI flag and optional argument to specify the name)
- CLI help message to provide info on flags

### Changed
- Updated Dungeon API to include saving and loading
- Updated Dungeon API to have only one function for generating dungeons
- Changed Point, Tile, Room components to use uint8_t instead of int

## [1.01] - 2016-9-7

### Added
- Defined and Implemented API for base components of game including heap, logger, point, room, and tile
- Defined and Implemented API for dungeon, including generating terrain, placing rooms, and placing paths
- Defined and Implemented API for creating a weighted graph and running Dijkstra's Algorithm to find the shortest path between two points (based on weights).
- Created Makefile with incremental build instructions
- Created Readme with short description of Assignment 1.01
- Created this Change Log
- Added Environmental Variable setting for debug mode (`export ENV=DEBUG`)

