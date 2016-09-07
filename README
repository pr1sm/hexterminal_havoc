# Hexterminal Havoc

This is an project for Com S 327 at [ISU](https://www.iastate.edu).

Hexterminal Havoc is a roguelike game consisting of monsters, dungeons, and
much more!

Because this is a project for a class.  The progress of this assignment can be
broken into different chunks representing an assignment.  Each assignment is 
displayed below with a short description of the assignment and how I implemented
the features required.

## Building

The project comes with a Makefile, so building is simple:
```bash
$ make
```
> For more information on GNU Make, click [here](https://www.gnu.org/software/make/)

This will create a binary called `hexterm_havoc` which can be executed to run
the game.

```bash
$ ./hexterm_havoc
```

## Assignments

### Assignment 1.01 - Dungeon Generation

For this assignment, we were tasked with generating a dungeon in which our 
players and the monsters would navigate.  Some basic restrictions were given to
us regarding the size of the dungeon, the size of the rooms, the mininum number
of rooms, and the connections between rooms.  The implementation of the 
assignment was very open and largely left to us.  

An overall outline of a dungeon generation process was given to us as well as a
starting point.  I decided to start with this process and change it as I tested
my implementation out.  My process is broken into the following steps:

- Generate Terrain of the dungeon
- Place rooms into dungeon
- Connect rooms to each other

##### Generating Terrain

Each tile of my dungeon has specific attributes to it, and one of those is the
hardness of the tile.  This will also be used later to affect which enemies can
tunnel through specific tiles.  In order to generate a smooth terrain, I first 
start with an empty dungeon and randomly place points of hard, medium and soft 
rock, which I call "accents".  Each group gets 4% of the dungeon size.  I then
diffuse the accents outward until all tiles have been set.  This gets me 
different "zones" on the dungeon map which I then smooth out the map by taking 
the setting tile's hardness to the weighted average of it's neighbors.  This 
changes the sharp edges of zone changes to a smoother gradient.

##### Placing Rooms

When placing my rooms, I don't take into account the terrain of the dungeon map.
I decided that this led to clumped sets of roomss, which I did not like.  There
is a mininum limit of 6 rooms that I first generate 6 rooms, then check for over
laps.  If there is overlap between any rooms, I regenerate those 6 rooms.  Once
I've generated the minimum amount, I check to see if the total area of rooms is
less than 25% of the total space.  If it is, I try to generate a new room that
doesn't overlap with any of the existing rooms, and add that to my list.  I 
continue this process until either I've filled at least 25% of space with rooms,
or there has been 2000 failed attempts to place a new room.

##### Connecting Rooms

This is where I integrate the terrain of the map with the rooms.  For each room
in my list I choose a point in the room i and room i+1 which I use to find the 
shortest path between them.  I use dijkstra's algorithm with the rock hardnesses
as the weights for the graphs. This generates winding paths between each room
and connects them all together.

##### Printing the Dungeon out

For Assignment 1.01, the created dungeon has to be printed out in the terminal 
and then our program should exit.  

##### Other Notes

While this assignment consisted of a somewhat easy process to implement, there 
were a lot of other aggregated data types (structs) I created to organize data 
effectively.  Each struct also came with a define API for manipulations and 
common tasks such as checking if a room overlapped another room.  Additionally,
I also created a logger that could be used to keep track of key events with 
specific levels given to each message.  Because the terminal is where the game 
will run, I instead output messages to a log file.  if a log file already 
exists, that file is moved to a .bak file (up to .bak9) and the new log file is
created.  This will allow me to look at two instances of the game and compare 
events from both runs.  
