G = gcc -g
O = -o
names = parent player

files:
	$(G) parent.c functions.c $(O) parent
	$(G) player.c functions.c $(O) player
clean:
	rm -f $(names)