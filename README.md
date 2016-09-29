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

### Command Line Flags

There are options for saving and loading dungeons based on flags passed through
the command line.  To see all options, run `./hexterm_havoc -h`.  A menu 
detailing all options should be printed similar to this: 

```bash
Usage: hexterm_havoc [options]

-l<name>, --load=<name> | Load dungeon with name <name> (in save directory).
-h,       --help        | Print this help message.
-s<name>, --save=<name> | Save the dungeon after loading/generating it with
                        |   name <name> (in save directory).
-x<val> , --xpos <val>  | Start the player at a specified x coord
-y<val> , --ypos <val>  | Start the player at a specified y coord
```

## Assignments

### Assignment 1.03 - Path Finding

For this assignment, we were tasked with generating maps for path finding.
We had to do this by implementing Dijkstra's Algorithm using the rock
hardnesses as weights.  The full range of the rock hardness is 0 to 255, 
but we scaled the these values to a range of 0 to 3.  We also had to use
two different implementations of the setup.  One that worked for all tiles
in the dungeon for tunneling monsters and one that worked for only the 
rooms and corridors for non-tunneling monsters.  

My solution uses the dijkstra's algorithm I already implemented for the
first assignment.  This week I spent a lot of time refactoring my code as the
APIs changed.  There were a lot of small tweaks that I completed which added
up to a pretty big refactor.  One of the big refactors was a full general
Makefile.  This allows me to keep the same Makefile as I add more source code
and have it automatically detect new files/changes and build those instead of
having to create a rule for each individual object file. I also separated a lot 
of the dijkstra's algorithm api into separate files so the apis would be more 
modular.  There is one api that controls all the setup for a graph structure 
and there is another api that controls the path finding setup of the graph as 
well as one for the corridor generation.  The program now includes all the 
functionality from before, but also prints the maps created for tunneling and 
nontunneling.  The player position is also generated everytime the game runs, 
or it can be passed in through the command line.   

### Assignment 1.02 - Dungeon Saving/Loading

For this assignment, we were tasked with implementing saving and loading
features into our game.  Based on two switches (`--save` and `--load`) we had 
to change the flow of our game.  If the `--load` flag was set, instead of 
generating a dungeon, we had to load a dungeon file from a save directory 
(`~/.rlg327/`), reconstruct it, and print it out.  If the `--save` flag was 
set, we had to export our dungeon to binary file following a specific format 
and save it to the save directory.  

To aid in testing, I added optional arguments to both switches.  A name of the 
dungeon could be specified with each option and I would load/save that dungeon 
instead of the default one.  Both are optional and separate, so I can load a 
dungeon with one name and save it with another name.  

The file format of this binary file consisted of a header with the type of the 
file, the version number, and the size of the total file.  After the header, we 
wrote out all of the hardness values of each tile in the dungeon.  Then, we 
wrote out all of the rooms.  Each room was described by 4 bytes, one for the x 
location, width, y location, and height.


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
