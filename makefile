G = gcc -g
O = -o
names = parent player

files:
	$(G) parent.c $(O) parent
	$(G) player.c $(O) player
clean:
	rm -f $(names)