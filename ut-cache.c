/* ut-cache: Read the UT cache of downloaded files and move them to
   the proper directory */

#include "ut-cache.h"

const char ut_cache_usage_string[] = 
	"ut-cache [option]\n\n-a Move all files\n-d Remove all files\n";
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
	FILE *fp = fopen( "test/Cache/cache.ini.new", "w" );
	char cmd[1024]; //TODO: Get this information from system in ./configure
	int i=0;

	for(i=0;i<num_entries;i++) {
		switch(file[i]->action) {
		case UT_MOVE:
			//TODO: mv "file->cfile" "/GAME_PATH/file->gdir/file->gfile"
			sprintf( cmd, "mv \"%s/%s.uxx\" \"%s/%s/%s\"",
				 cache_dir, (file[i]->cfile),move_dir,(file[i]->gdir), (file[i]->gfile));
			if(system(cmd)) {
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
				error("rm system call failed\n");
				write_cache( fp, file[i] );
				file[i]->action = UT_NO_TASK;
			}else {
				memset( file[i], 0, sizeof(struct ut_cache));
			}
			break;
		case UT_NO_TASK:
		default:
			//Windows file format:
			//cfile=gfile\r\n
			write_cache( fp, file[i] );
			break;
		}
	}
	//TODO: mv cache.ini.new cache.ini
	fclose(fp);
	system( "mv test/Cache/cache.ini.new test/Cache/cache.ini" );
	return 0;
}

int main(int argc, const char **argv)
{
	struct ut_cache **c = NULL;

	int num_cache = 0;
	c = read_cache(CACHE_FILE, &num_cache);
	if( c == NULL ) {
		return 1;
	}

	/* Handle args */
	/* Skip program name */
	argv++;
	argc--;
	if( argc == 0 )
		do_interactive(c, num_cache);
	while( argc > 0 ) {
		if( !strcmp(*argv, "-a") )
			if( 1 == argc )
				do_all(c, num_cache, UT_MOVE);
			else
				usage(ut_cache_usage_string);
		else if( !strcmp(*argv, "-d") )
			if( 1 == argc )
				do_all(c, num_cache, UT_DELETE);
			else
				usage(ut_cache_usage_string);
		else
			usage(ut_cache_usage_string);
		argv++;
		argc--;
	}


	printf( "%d\n", num_cache);
	printf("done!\n");

	return 0;
}

void do_all(struct ut_cache **file, int num_cache, int action)
{
	int i;
	for(i=0; i<num_cache; i++) {
		file[i]->action = action;
	}
	cache_action( file, num_cache );
}

void do_interactive(struct ut_cache **file, int num_cache)
{
	int i;
	for(i=0; i<num_cache; i++) {
		fprintf(stdout, "%s/%s\n", file[i]->gdir, file[i]->gfile);
		fprintf(stdout, "1) Move\n2) Delete\n3) Keep\nEnter choice:");
		char c;
		while( (c=getchar()) != EOF ) {
			if( '\n' == c )
				break;
			else if( '1' == c ){
				file[i]->action = UT_MOVE;
			}else if('2' == c){
				file[i]->action = UT_DELETE;
			}else if('3' == c){
				file[i]->action = UT_NO_TASK;
			}else {
				fprintf(stdout, "%s/%s\n", file[i]->gdir, file[i]->gfile);
				fprintf(stdout, "1) Move\n2) Delete\n3) Keep\nEnter choice:");
			}
		}
	}
	cache_action(file, num_cache);
}
