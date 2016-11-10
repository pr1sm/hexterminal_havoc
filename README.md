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

-a      , --ai          | Enable AI mode for PC (pc control used by default)
-h      , --help        | Print this help message.
-l<name>, --load=<name> | Load dungeon with name <name> (in save directory).
-m<val> , --nummon=<val>| Set the number of monsters in the dungeon
-n      , --ncurses     | Use Ncurses to render game
-s<name>, --save=<name> | Save the dungeon after loading/generating it with
                        |   name <name> (in save directory).
-x<val> , --xpos <val>  | Start the player at a specified x coord
-y<val> , --ypos <val>  | Start the player at a specified y coord
```

## Assignments

### Assignment 1.08 - Loading Monsters and Objects

For this assignment, we had to use our parser to generate monsters and items within
the dungeon.  Rather than have random attributes, we picked specific attributes from
a random description in our parser.  Because I already had a factory implementation 
in the `character_store` class, I simply created a `gen_npc()` method and used that.
I also implemented an item parser and used that to generate items and place them in
the dungeon.  I also added colors to the dungeon and the monsters and items are the
color of their description.  

### Assignment 1.07 - Parsing Monster Definitions

For this assignment, we had to build a parser that would parse monster descriptions.
These descriptions had specific attributes and we were to parse them if all attributes
were specificied or move onto the next monster.  Using C++ I built a class that parses
the monster file and prints out the descriptions to the console.  I have temporarily
disabled the rest of the game for this assignment, but it is controlled by an interal
switch that can be set to '0' and upon recompilation, the game will run as it did in 
1.06.  

I also refactored the entire codebase to move to C++.  This included moving all my
implementation over to Classes instead of Structs, and making use of the `new` and
`delete` keywords for memory management when applicable.  This was relatively
simple, however it was very time consuming.  The C code as of 1.06 has been saved
to another folder and can still be built using the `make c` rule.  

### Assignment 1.06 - "Fog of War" and interfacing with C and C++

For this assignment, we had to start switching our codebase over to C++ and write
an interface in C++ so our C codebase could interact with the class we created. 
We had to port our character structs (for the pc and npcs) over to a Character
class in C++ and write a set of mutators and accessors to interface between the
two languages.  Most of the codebase remained largely unchanged, but because I 
now had these mutators and accessors, I had to use these instead of accessing
the character_t's elements.  I took this a step further and maintained compatibility
with gcc as well.  A rule was added to the Makefile so the project could be still
compiled with gcc and the old characterAPI and character struct would be used.  
The default make rule will now compile using g++ and the character class interface
will be used.  The Makefile is now also able to tell which files have been converted
to .cpp files and will only build those when compiling for C++.  If compiling for C,
the .c file will be used.  

I also added fog of war support, which was trivial to add.  This allows the player
to only see a radius of 3 around it and changes to the dungeon in other places are
not rendered until the player moves to a position closer.  The only issue that I 
had while writing this feature was an interesting error where the npc count would
be much lower than the specified count.  This turned out to be an initialization
error with the class where I wasn't specifically initializing the `is_dead` property
to 0 in the class constructor for the Character.  There was another minor bug I 
discovered during testing where a crash would occur when the player stepped on a
tile that had been tunneled through by an npc.  This was a pretty easy fix and 
the error is logged and resolved automatically now.  

UPDATE:
There were many outstanding issues I had logged that I thought should be included 
in this assignment, but I was not able to get to them due to time constraints.  I
now have addressed them and have released a 1.06.1 tag that adds more memory leak
fixes found when going through control flow paths other than the standard game.
I also added CLI flags to control whether or not the PC AI should be used (disabled
by default) and whether or not ncurses should be used for rendering the dungeon.
I also fixed an issue with the player spawn point.  This can be specified through
the command line, however there were not any checks besides the range of the input.
I added checks on if the proposed spawn point was valid (in a path or room).  If it
was not, I changed the spawn point to a random room in the dungeon.  

### Assignment 1.05 - User Interface with Ncurses

For this assignment, we had to implement a user interface for the player and
use ncurses as the default rendering for our dungeon.  The move the ncurses 
was very simple for me since there were only a couple of places I printed the
dungeon out.  A global enviroment flag was added to support any changes that 
needed to be made based on the presence of ncurses or not.  I also added an
option to turn on the player AI I used in assignment 1.04, or use the user
interface I implemented in this assignment.  Both of these flags currently do
not have a runtime configuration setting, so recompilation is necessary to change
these settings, however that will be address in later assignments.  

The implementation of the character control was also very simple since similar
steps were required for the pc AI.  The only change was that I asked ncurses to 
get the input of the key during the pc's move.  I kept in the sleep timer 
between moves because I like the feeling of the monsters moving one step at a time
versus having them move 2 steps automatically.  In my opionion this added an 
immersion into the game because the monsters moved on their own in between the pause
for the pc control.  

The implementation of the monster list was also simple and is done within on 
function so the list of monsters is only allocated once and all the control
for that screen is done separately in that function.  Once the close button is
pressed, control is then returned to the move function for the pc control, which
prompts the user for their next pc move.  The function also cleans up itself,
so it can be called over and over again without leaking memory

The staircases generation and implementation was slightly harder since there 
was a fair amount of cleanup necessary to do this.  The overall approach was
to free all the memory with the dungeon and the npcs and generate a new dungeon
and characters, placing the pc at the correct location where the stairs were.  
When the pc moves downstairs, they are placed at the spot where the stairs upward
is generated.  Similarly when they move upstairs, they are placed at the stairs 
going downward.  I had all the functions necessary to cleanup all my data structures
and reallocate them, I just had to create a function that did both in the correct 
order and call that at the right time.  

### Assignment 1.04 - Player Character and Monsters

For this assignment, we had to implement the Monster AI as well as implement
a temporary player AI so we could watch the monsters chase the player.  My 
implementation of this consisted of creating a new data type to hold the 
character information.  I found that much of the information between the pc
and the npcs were the same, so they are all in the same data type.  This allowed
me to use a priority queue for my event system.  The event system uses events
as the input and returns the event that should be run first based on the characters
speed.  I also implemented bresenhams line pathing algorithm to check line of
sight between the monsters and the player.  This was used to check if the 
destination of the monster should be set to the player (player is in line of sight)
or another random point in the dungeon.  This of course was also affected by the 
monsters inert attributes and modifiers such as telepathy overrode this los check.
I also performed a major refactor of my codebase to move to a more object oriented
style.  Many of the functions were move to be function pointers in the data type
itself rather than an API function.  I did this because it made more sense to me
when reading the code that the struct should be calling the function rather than
another API struct that holds all the function names.  I also fixed some outstanding
bugs and tweaks from previous assignments that I had put in a backlog.  This seemed
appropriate since I did a major refactor of the code base. 

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
