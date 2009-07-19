#ifndef UT_CACHE_H
#define UT_CACHE_H

#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

/* Unreal Tournament versions */
#define UT99 1
#define UT2003 2
#define UT2004 3

/* Cache Actions */
#define UT_NO_TASK 0
#define UT_MOVE 1
#define UT_DELETE -1

/* Default buffers and things */
#define MAXLEN 1000

/* Default strings */
#define CACHE_FILE "test/ut99_cache.ini"

/*
 * Basic struct for parsing the cache.ini into
 */
struct ut_cache {
	char cfile[100]; /* cache file name */
	char gfile[100]; /* game file name */
	char gdir[20];   /* game directory */
	int action;      /* actions to take on the file */
};

/* 
 * cache handling functions
*/
/* read cache and figure out where each file should go */
int read_cache(const char *cachefile, struct ut_cache **cache);
/* figure out where each file should go */
int asign_dir(struct ut_cache *file);
/* perform action */
int cache_action(struct ut_cache *file);

/* General helper functions */
extern void usage(const char *err);
extern int error(const char *string);

#endif /* UT_CACHE_H */
