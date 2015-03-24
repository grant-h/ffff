
all :
	gcc -o ffff ffff.c fuzz.c util.c corrupt.c

clean :
	rm -f ffff
