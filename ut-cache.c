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

void chomp(char *line)
{
	char c;
	int i;
	for(i=0;(c=line[i]) != '\0'; i++) {
		if( '\r' == c || '\n' == c )
			line[i] = '\0';
	}
}

void stolower(char *string)
{
	int i;
	int len = strlen(string);

	for(i=0; i<len; i++)
		string[i] = tolower(string[i]);
}

int asign_dir(struct ut_cache *file)
{
	char ext[5]; /* allow for 4 char extentions */
	memset(ext, 0, sizeof(ext));
	char *pdot=NULL;

	if( (pdot=strrchr(file->gfile, '.')) != NULL)
		memcpy(ext, (pdot+1), sizeof(char)*3);

	stolower(ext);

	if( !strcmp(ext, UT_U )){
		memcpy(file->gdir, SYSTEM, sizeof(SYSTEM));
	}else if( !strcmp(ext, UT_UAX) ){
		memcpy(file->gdir, SOUND, sizeof(SOUND));
	}else if( !strcmp(ext, UT_UMX) ){
		memcpy(file->gdir, MUSIC, sizeof(MUSIC));
	}else if( !strcmp(ext, UT_UNR) ){
		memcpy(file->gdir, MAPS, sizeof(MAPS));
	}else if( !strcmp(ext, UT_UTX) ){
		memcpy(file->gdir, TEXTURES, sizeof(TEXTURES));
	}

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
					*size = error( "out of memory\n" );
					goto out_free;
				}
				memset(cache[i], 0, sizeof(struct ut_cache));
			}
	}else
		error("got non NULL cache pointer\n");

	char *equals;
	for( i = 0; (fgets(line, MAXLEN, fp)) != NULL; i++ ){
		if( NULL == (equals = strchr( line, '=' ))){
			i--;
			continue;
		}
		chomp(line);
		memcpy( cache[i]->cfile, line, (sizeof(char) * ( equals - line )));
		memcpy( cache[i]->gfile, (equals+1), sizeof((cache[i]->gfile)));
		asign_dir(cache[i]);
	}
	*size = i;

out_free:
	if(fp != NULL)
		fclose(fp);
	else
		close(fd);

	if( *size < 0 ){
		free(cache);
		cache = NULL;
		ret = -1;
	}

	return (ret == -1)? NULL: cache;
}

int write_cache( FILE *fp, struct ut_cache *file )
{
	if(!ftell(fp))
		fprintf( fp, "[Cache]\r\n" );

	fprintf( fp, "%s=%s\r\n", (file->cfile), (file->gfile));

	return 0;
}


int cache_action(struct ut_cache **file, int num_entries)
{
/*TODO: perform action and write new cache.ini */
	FILE *fp = fopen( "test/cache.ini.new", "w" );
	char cmd[1024]; //TODO: Get this information from system in ./configure
	int i=0;

	for(i=0;i<num_entries;i++) {
		switch(file[i]->action) {
		case UT_MOVE:
			//TODO: mv "file->cfile" "/GAME_PATH/file->gdir/file->gfile"
			sprintf( cmd, "mv \"%s/%s.uxx\" \"%s/%s/%s\"",
				 cache_dir, (file[i]->cfile),move_dir,(file[i]->gdir), (file[i]->gfile));
			if(system(cmd)) {
				//TODO: do something intelligent
				error("mv system call failed\n");
				write_cache( fp, file[i] );
				file[i]->action = UT_NO_TASK;
			}else {
				memset( file[i], 0, sizeof(struct ut_cache));
			}
			break;
		case UT_DELETE:
			//TODO: rm -f "file->cfile"
			sprintf( cmd, "rm -f \"%s/%s.uxx\"", cache_dir, (file[i]->cfile));
			if(system(cmd)) {
				//TODO: do something intelligent
				error("rm system call failed\n");
				write_cache( fp, file[i] );
				file[i]->action = UT_NO_TASK;
			}else {
				memset( file[i], 0, sizeof(struct ut_cache));
			}
			break;
		case UT_NO_TASK:
		default:
			//TODO: write entry back to cache.ini
			//Windows file format:
			//cfile=gfile\r\n
			write_cache( fp, file[i] );
			break;
		}
	}
	//TODO: mv cache.ini.new cache.ini
	fclose(fp);
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
			fprintf(stderr, "gdir:	%s\n", c[i]->gdir);
			fprintf(stderr, "\n");
		}
	}
	printf( "%d\n", num_cache);
	printf("done!\n");

	return 0;
}
