/*
 * Unreal Tournament Cache Converter
 * Copyright (C) John te Bokkel, 2009
 */

#ifndef UT_CACHE_H
#define UT_CACHE_H

#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "ut-data.h"

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
const char ut_cache_usage_string[] = 
	"ut-cache [option]\n\n\
--base -b Base directory to move the files into their sub directories\n\
--config -c Config directory\n\
--delete-all -d Remove all files\n\
--help -h Display this help\n\
--interactive -i Interactive choice to keep, move, or delete the file from the cache\n\
--move-all -a Move all files\n\
--ut2003 Use default config directory for UT 2003\n\
--ut2004 Use default config directory for UT 2004\n\
--ut99   Use default config directory for UT99\n\
";

/*
 * Config related globals
 */
const char *cache_dir;
const char *config_dir;
const char *move_dir;

/*
 * Basic struct for parsing the cache.ini into
 */
struct ut_cache {
	char cfile[40];  /* cache file name */
	char gfile[100]; /* game file name */
	char gdir[20];   /* game directory */
	int action;      /* actions to take on the file */
};

/*
 * cache handling functions
*/
/* read cache and figure out where each file should go */
void *read_cache(const char *cachefile, int *size);
int write_cache(FILE *fp, struct ut_cache *file);
/* figure out where each file should go */
int asign_dir(struct ut_cache *file);
/* perform action */
int cache_action(struct ut_cache **file, int num_entries);
void do_all(struct ut_cache **file, int num_cache, int action);
void do_interactive(struct ut_cache **file, int num_cache);
/* General helper functions */
extern void usage(const char *err);
extern int error(const char *string);
extern void chomp(char *line);
extern void stolower(char *string);

#endif /* UT_CACHE_H */
