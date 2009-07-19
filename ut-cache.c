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

void *read_cache(const char *cachefile, int *size)
{
	int fd = open( cachefile, O_RDONLY );
	int fd_err = errno;
	int ret=0;
	FILE *fp=NULL;

	char line[MAXLEN];
	struct ut_cache **cache=NULL;

	int buf_size = 0;
	int i = 0;

	if(fd == -1){
		switch(fd_err){
		case ENOENT:
			error("cache file was not found.\n");
			return NULL;
			break;
		case EACCES:
			error("unable to open cache file for reading. Check permissions.\n");
			return NULL;
			break;
		default:
			fprintf(stderr, "error: %d\n", fd_err);
			error("unknown error occurred trying to open cache file\n");
			return NULL;
		}
	}
	/* set up file pointer for streams */
	if( (fp = fdopen( fd, "r" )) == NULL ){
		ret = error("unable to create FILE *\n");
		goto out_free;
	}
	/* get number of lines in  the cache file */
	int num_lines = 0;
	int c = 0;
	while( (c=getc(fp)) != EOF) {
		if( '\n' == c )
			++num_lines;
	}
	rewind(fp);
	/* allocate struct for each line in the file */
	buf_size = num_lines;
	if( cache == NULL ) {
		cache = malloc(buf_size*sizeof(struct ut_cache *));
		/* check for out of memory error */
		if(cache != NULL)
			for(i = 0; i < buf_size; i++) {
				cache[i] = malloc(sizeof(struct ut_cache));
				if(cache[i] == NULL) {
					ret = error( "out of memory\n" );
					goto out_free;
				}
			}
	}else
		error("got non NULL cache pointer\n");

	char *equals;
	for( i = 0; (fgets(line, MAXLEN, fp)) != NULL; i++ ){
		if( NULL == (equals = strchr( line, '=' ))){
			i--;
			continue;
		}
		memcpy( cache[i]->cfile, line, (sizeof(char) * ( equals - line )));
		memcpy( cache[i]->gfile, (equals+1), sizeof((cache[i]->gfile)));
	}
	*size = i;

out_free:
	if(fp != NULL)
		fclose(fp);
	else
		close(fd);

	return (ret == -1)? NULL: cache;
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

	int num_cache = 0;
	c = read_cache(CACHE_FILE, &num_cache);

	if( c != NULL ) {
		int i;
		for(i=0; i<num_cache; i++) {
			fprintf(stderr, "cfile: %s\n", c[i]->cfile);
			fprintf(stderr, "gfile: %s\n", c[i]->gfile);
			fprintf(stderr, "\n");
		}
	}
	printf( "%d\n", num_cache);
	printf("done!\n");

	return 0; 
}
