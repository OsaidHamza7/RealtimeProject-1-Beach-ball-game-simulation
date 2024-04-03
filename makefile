CC = gcc
CFLAGS = -g 

ARGS= arguments.txt

UILIBS = -lglut -lGLU -lGL -lm  -lrt
LIBS = -lpthread
NAMES = parent player

all: parent player

parent: parent.c functions.c
	$(CC) $(CFLAGS) -o parent parent.c functions.c $(LIBS)

player: player.c functions.c
	$(CC) $(CFLAGS) -o player player.c functions.c $(LIBS)
	
run: parent
	./parent $(ARGS) 

clean:
	rm -f $(NAMES)