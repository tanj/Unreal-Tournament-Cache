/* ut-cache: Read the UT cache of downloaded files and move them to
   the proper directory */

#include "ut-cache.h"

void usage(const char *err)
{
	fprintf(stderr, "ut-cache: %s\n", err);
	exit(1);
}

int error(const char *string)
{
	fprintf(stderr, "error: %s\n", string);
	return -1;
}

int asign_dir(struct ut_cache *file)
{
/*TODO*/
	return 0;
}

int read_cache(const char *cachefile, struct ut_cache **cache)
{
	int fd = open( *cachefile, O_RDONLY );
	int ret = 0;
	char line[MAXLEN];

	int buf_size = 100;
	int i = 0;

	if( cache == NULL ) {
		cache = malloc(buf_size*sizeof(struct ut_cache *));
		for(i = 0; i < buf_size; i++) 
			cache[i] = malloc(sizeof(struct ut_cache));
	}
	if(fd == -1){
		switch(errno){
		case ENOENT:
			return error("cache file was not found.\n");
			break;
		case EACCES:
			return error("unable to open cache file for reading. Check permissions.\n");
			break;
		default:
			return error("unknown error occurred trying to open cache file\n");
		}
	}
	
	close(fd);
	printf("done!\n");
	
	return 0;
}

int cache_action(struct ut_cache *file)
{
/*TODO*/
	return 0;
}

int main(int argc, char **argv)
{
	if( argc > 1 ) {
		usage("ut-cache \n\n"
		      "Unreal Tournament cache reader.\n"
		      "Convert the *.uxx files into their real names move into configured\n"
		      "directories.");
	}

	struct ut_cache **c = NULL;

	read_cache(CACHE_FILE, c);

	return 0; 
}
