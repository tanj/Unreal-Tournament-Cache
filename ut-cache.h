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

/*
 * Basic struct for parsing the cache.ini into
 */
struct cache {
	char cfile[100]; /* cache file name */
	char gfile[100]; /* game file name */
	int action;      /* actions to take on the file */
};

/* 
 * cache handling functions
*/
/* read cache and figure out where each file should go */
int read_cache(unsigned char *cachefile, struct cache *file);
/* figure out where each file should go */
int asign_dir(struct cache *file);
/* perform action */
int cache_action(struct cache *file);

/* General helper functions */
extern void usage(const char *err);
extern int error(const char *string);

#endif /* UT_CACHE_H */
