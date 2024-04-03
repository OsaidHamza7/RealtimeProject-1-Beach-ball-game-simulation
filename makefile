CC = gcc
CFLAGS = -g -Wall

ARGS= arguments.txt

UILIBS = -lglut -lGLU -lGL -lm  -lrt
LIBS = -lpthread

all:  parent player

parent: parent.c
	$(CC) $(CFLAGS) -o parent parent.c functions.c $(LIBS)

player: player.c
	$(CC) $(CFLAGS) -o player player.c functions.c $(LIBS)
	
run: parent
	./prarent $(ARGS) 

clean:
	rm -f $(all)