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
	rm -f *.o $(PROG)
	rm -rf test/Cache

.PHONY: test
test:
	test/create_test_files.sh