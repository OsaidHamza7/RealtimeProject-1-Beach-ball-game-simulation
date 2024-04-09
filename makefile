CC = gcc
CFLAGS = -g 

ARGS= arguments.txt

UILIBS = -lglut -lGLU -lGL -lm  -lrt
LIBS = -lpthread
NAMES = parent player gui

all: parent player gui

parent: parent.c functions.c
	$(CC) $(CFLAGS) -o parent parent.c functions.c $(LIBS)

player: player.c functions.c player_utilities.c
	$(CC) $(CFLAGS) -o player player.c functions.c player_utilities.c $(LIBS)

gui: gui.c
	$(CC) gui.c functions.c -o gui $(UILIBS)	

run: parent
	./parent $(ARGS) 

clean:
	rm -f $(NAMES)