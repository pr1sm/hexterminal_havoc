# Change Log
All notable changes to this project (Hexterminal Havoc) will be documented in
this file.

This project adheres to [Semantic Versioning](http://semver.org/).

## [Unreleased]

### Added
- Dungeon Saving (added CLI flag and optional argument to specify the name)
- Dungeon Loading (added CLI flag and optional argument to specify the name)
- CLI help message to provide info on what flags there are


## [1.01] - 2016-9-7

### Added
- Defined and Implemented API for base components of game including heap, logger, point, room, and tile
- Defined and Implemented API for dungeon, including generating terrain, placing rooms, and placing paths
- Defined and Implemented API for creating a weighted graph and running Dijkstra's Algorithm to find the shortest path between two points (based on weights).
- Created Makefile with incremental build instructions
- Created Readme with short description of Assignment 1.01
- Created this Change Log
- Added Environmental Variable setting for debug mode (`export ENV=DEBUG`)

