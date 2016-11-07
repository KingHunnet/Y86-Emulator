all: y86emul

y86emul: y86emul.c y86emul.h
	gcc -o y86emul y86emul.c

clean:
	-rm -f y86emul
