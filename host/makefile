CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic -Wno-unused-result -O3
all: pea_h 

pet.o: pet.c pet.h
backup.o: backup.c backup.h pet.o
pea_h: pea.c backup.o pet.o ../PET_OPT.h

clean:
	rm -f pea_h *.o
