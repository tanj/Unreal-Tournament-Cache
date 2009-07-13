/* ut-cache: Read the UT cache of downloaded files and move them to
   the proper directory */

#include "ut-cache.h"

void usage(const char *err)
{
	fprintf(stderr, "ut-cache: %s\n", err);
	exit(1);
}

static int error(const char *string)
{
	fprintf(stderr, "error: %s\n", string);
	return -1;
}

int asign_dir(struct cache *file)
{
/*TODO*/
	return 0;
}

int read_cache(unsigned char *cachefile, struct cache *file)
{
/*TODO*/
	return 0;
}

int cache_action(struct cache *file)
{
/*TODO*/
	return 0;
}

int main(int argc, char **argv)
{
/*TODO*/
	return 0;
}
