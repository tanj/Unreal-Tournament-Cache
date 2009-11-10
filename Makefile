CFLAGS=-g -Wall
CC=gcc

PROG=ut-cache

UT_DIRS  = test/Animations
UT_DIRS += test/ForceFeedback
UT_DIRS += test/KarmaData
UT_DIRS += test/Maps
UT_DIRS += test/Music
UT_DIRS += test/Saves
UT_DIRS += test/Sounds
UT_DIRS += test/Speech
UT_DIRS += test/StaticMeshes
UT_DIRS += test/System
UT_DIRS += test/Textures

MKDIR = mkdir -p
RM_DIR = rm -rf
RM = rm -f




all: $(PROG)

install: $(PROG)
	install $(PROG) $(HOME)/bin/

ut-cache: ut-cache.o
	$(CC) $(CFLAGS) -o ut-cache ut-cache.o

ut-cache.o: ut-cache.h

clean:
	$(RM) *.o *~ $(PROG)
	$(RM_DIR) test/Cache
	$(RM_DIR) $(UT_DIRS)



.PHONY: test-setup
test-setup:
	test/create_test_files.sh
	$(MKDIR) $(UT_DIRS)

.PHONY: test-clean
test-clean:
	$(RM_DIR) test/Cache
	$(RM_DIR) $(UT_DIRS)

.PHONY: test-run
test-run:
	./ut-cache -c "test/" -i
