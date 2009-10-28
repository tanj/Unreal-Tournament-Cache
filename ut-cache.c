/*
 * Unreal Tournament Cache Converter
 * Copyright (C) John te Bokkel, 2009
 */

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

	/* Match extentions to game directory */

	/* Animations */
	if(!strcmp( ext, UT_UKX ) )
		memcpy(file->gdir, ANIMATIONS, sizeof(ANIMATIONS));
	/* Force Feedback */
	else if( !strcmp(ext, UT_IFR) )
		memcpy(file->gdir, FORCE_FEEDBACK, sizeof(FORCE_FEEDBACK));
	/* Karma Data */
	else if( !strcmp(ext, UT_KA) )
		memcpy(file->gdir, KARMA_DATA, sizeof(KARMA_DATA));
	/* Maps */
	else if( !strcmp(ext, UT_UNR) )
		memcpy(file->gdir, MAPS, sizeof(MAPS));
	else if( !strcmp(ext, UT_UT2) )
		memcpy(file->gdir, MAPS, sizeof(MAPS));
	/* Music */
	else if( !strcmp(ext, UT_UMX) )
		memcpy(file->gdir, MUSIC, sizeof(MUSIC));
	else if( !strcmp(ext, UT_OGG) )
		memcpy(file->gdir, MUSIC, sizeof(MUSIC));
	/* Saves */
	else if( !strcmp(ext, UT_UVX) )
		memcpy(file->gdir, SAVES, sizeof(SAVES));
	/* Sounds */
	else if( !strcmp(ext, UT_UAX) )
		memcpy(file->gdir, SOUND, sizeof(SOUND));
	/* Speech */
	else if( !strcmp(ext, UT_XML) )
		memcpy(file->gdir, SPEECH, sizeof(SPEECH));
	/* Static Meshes */
	else if( !strcmp(ext, UT_USX) )
		memcpy(file->gdir, STATIC_MESHES, sizeof(STATIC_MESHES));
	/* System */
	else if( !strcmp(ext, UT_U ))
		memcpy(file->gdir, SYSTEM, sizeof(SYSTEM));
	else if( !strcmp(ext, UT_UCL) )
		memcpy(file->gdir, SYSTEM, sizeof(SYSTEM));
	else if( !strcmp(ext, UT_INI) )
		memcpy(file->gdir, SYSTEM, sizeof(SYSTEM));
	else if( !strcmp(ext, UT_INT) )
		memcpy(file->gdir, SYSTEM, sizeof(SYSTEM));
	else if( !strcmp(ext, UT_UPL) )
		memcpy(file->gdir, SYSTEM, sizeof(SYSTEM));
	/* Textures */
	else if( !strcmp(ext, UT_UTX) )
		memcpy(file->gdir, TEXTURES, sizeof(TEXTURES));
	else
		memset(file->gdir, 0, sizeof(file->gdir));

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
	char cache_file_new[MAXLEN];
	memset(cache_file_new,0,MAXLEN);
	strcat(cache_file_new, cache_dir);
	strcat(cache_file_new, "/cache.ini.new");

	FILE *fp = fopen( cache_file_new, "w" );
	char cmd[1024]; //TODO: Get this information from system in ./configure
	int i=0;

	for(i=0;i<num_entries;i++) {
		switch(file[i]->action) {
		case UT_MOVE:
			//mv "file->cfile" "/GAME_PATH/file->gdir/file->gfile"
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
			//rm -f "file->cfile"
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
	sprintf( cmd, "mv \"%s\" \"%s%s\"", cache_file_new, cache_dir, "/cache.ini");
	system( cmd );
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

static struct option const long_options[] =
{
	{"base", required_argument, NULL, 'b'},
	{"config", required_argument, NULL, 'c'},
	{"delete-all", no_argument, NULL, 'd'},
	{"help", no_argument, NULL, 'h'},
	{"interactive", no_argument, NULL, 'i'},
	{"move-all", no_argument, NULL, 'a'},
	{"ut2003", no_argument, NULL, UT2003},
	{"ut2004", no_argument, NULL, UT2004},
	{"ut99", no_argument, NULL, UT99},
	{"verbose", no_argument, NULL, 'v'},
	{0, 0, 0, 0}
};

int main(int argc, char **argv)
{
	struct ut_cache **cache = NULL;
	int num_cache = 0;
	int c = 0;
	int action = UT_NO_TASK;
	int interactive = false;
	int game_type = 0;

	char *home_dir;
	home_dir = getenv("HOME");

	char cache_file[MAXLEN];
	memset( cache_file, 0, sizeof(cache_file));
	char tmp_cache[MAXLEN];
	memset( tmp_cache, 0, sizeof(tmp_cache));
	char tmp_config[MAXLEN];
	memset( tmp_config, 0, sizeof(tmp_config));
	char tmp_move[MAXLEN];
	memset( tmp_move, 0, sizeof(tmp_move));

	cache_dir = NULL;
	move_dir = NULL;
	config_dir = NULL;

	/* Handle args */
	while( (c = getopt_long(argc, argv, "adhivb:c:", long_options, NULL)) != -1 ) {
		switch (c) {
		case 'a':
			if(!action)
				action = UT_MOVE;
			else
				usage("can't move and delete all.");
			break;
		case 'b':
			move_dir = optarg;
			break;
		case 'c':
			config_dir = optarg;
			break;
		case 'd':
			if(!action)
				action = UT_DELETE;
			else
				usage("can't move and delete all.");
			break;
		case 'i':
			interactive = true;
			break;
		case 'v':
			//TODO: verbose echos move or delete action
			break;
		case UT99:
			if(!game_type)
				game_type = UT99;
			else
				usage("only one game type at a time");
			break;
		case UT2003:
			if(!game_type)
				game_type = UT2003;
			else
				usage("only one game type at a time");
			break;
		case UT2004:
			if(!game_type)
				game_type = UT2004;
			else
				usage("only one game type at a time");
			break;
		case 'h':
		default:
			usage(ut_cache_usage_string);
		}
	}

	if( config_dir == NULL ) {
		switch(game_type) {
		case UT99:
			memcpy( tmp_config, home_dir, strlen(home_dir) * sizeof(char) );
			strcat( tmp_config, "/.loki/ut");
			config_dir = tmp_config;
			break;
		case UT2003:
			memcpy( tmp_config, home_dir, strlen(home_dir) * sizeof(char) );
			strcat( tmp_config, "/.ut2003");
			config_dir = tmp_config;
			break;
		case UT2004:
			memcpy( tmp_config, home_dir, strlen(home_dir) * sizeof(char) );
			strcat( tmp_config, "/.ut2004");
			config_dir = tmp_config;
			break;
		default:
			usage(ut_cache_usage_string);
			break;
		}
	}
	memcpy( tmp_cache, config_dir, strlen(config_dir) * sizeof(char));
	strcat( tmp_cache, "/Cache" );
	cache_dir = tmp_cache;
	fprintf(stderr, "using \"%s\" for cache directory\n", cache_dir);

	


	if( move_dir == NULL ) {
		move_dir = config_dir;
		fprintf(stderr, "no move directory specified. using \"%s\"\n", move_dir);
	}

	memcpy( cache_file, cache_dir, strlen(cache_dir) * sizeof(char));
	strcat( cache_file, "/cache.ini" );
	cache = read_cache( cache_file, &num_cache);
	if( cache == NULL ) {
		return 1;
	}

	if( interactive ) {
		do_interactive( cache, num_cache );
	}else{
		do_all( cache, num_cache, action );
	}

	return 0;
}
