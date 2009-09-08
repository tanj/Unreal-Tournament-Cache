CFLAGS=-g -Wall
CC=gcc

PROG=ut-cache

all: $(PROG)

install: $(PROG)
	install $(PROG) $(HOME)/bin/

ut-cache: ut-cache.o
	$(CC) $(CFLAGS) -o ut-cache ut-cache.o 

ut-cache.o: ut-cache.h

clean:
	rm -f *.o *~ $(PROG)
	rm -rf test/Cache
	rm -rf test/Maps
	rm -rf test/Music
	rm -rf test/Sounds
	rm -rf test/System
	rm -rf test/Textures



.PHONY: test
test:
	test/create_test_files.sh
	mkdir test/Maps
	mkdir test/Music
	mkdir test/Sounds
	mkdir test/System
	mkdir test/Textures

.PHONY: clean-test
clean-test:
	rm -rf test/Cache
	rm -rf test/Maps
	rm -rf test/Music
	rm -rf test/Sounds
	rm -rf test/System
	rm -rf test/Textures
