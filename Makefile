#  the compiler: using gcc for C
CC = gcc

#  debug flags:
#  -ggdb ~> add gdb debugging information to the binary
DFLAGS = -ggdb

#  compiler flags:
#  -Wall ~> display all warnings
#  -Werror ~> treat warnings as errors
#  also include DFLAGS
CFLAGS = -Wall -Werror $(DFLAGS)

#  the build target
TARGET = hexterm_havoc

#  The current dependencies of the project
#  Dungeon, Dijkstra, Room, Heap, Logger, Point, Tile
DEPS = dungeon.o dijkstra.o room.o heap.o logger.o point.o tile.o

#  define the target build instructions
$(TARGET): $(TARGET).o libHexterm.a
	$(CC) $(CFLAGS) $^ -o $@

#  define target.o build instructions
$(TARGET).o: main.c
	$(CC) $(CFLAGS) -c -o $@ $<

#  define lib build instructions
libHexterm.a: $(DEPS)
	ar rcs $@ $^

dungeon.o: dungeon/dungeon.c dungeon/dungeon.h
	$(CC) $(CFLAGS) -c -o $@ $<

dijkstra.o: dungeon/dijkstra.c dungeon/dijkstra.h
	$(CC) $(CFLAGS) -c -o $@ $<

room.o: room/room.c room/room.h
	$(CC) $(CFLAGS) -c -o $@ $<
	
heap.o: heap/heap.c heap/heap.h
	$(CC) $(CFLAGS) -c -o $@ $<

logger.o: logger/logger.c logger/logger.h
	$(CC) $(CFLAGS) -c -o $@ $<

point.o: point/point.c point/point.h
	$(CC) $(CFLAGS) -c -o $@ $<

tile.o: tile/tile.c tile/tile.h
	$(CC) $(CFLAGS) -c -o $@ $<


clean: 
	$(RM) -r $(TARGET) *.o *.a *~ *.dSYM/ logs/
