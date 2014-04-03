/* Engin AYDOGAN 2004
 * This code tries to reset or at least invalidate linux cache and buffers */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* size of the chunk we are going to malloc in each iteration */
#define CHUNK (1024*1024)
#define MAX_CHUNK (1024*1024)

typedef struct mem_struct_t
{
	long int MemFree;
	long int Cached;
	long int Buffers;
} MemStruct;

MemStruct parse_meminfo()
{
	/* to read /proc/meminfo */
	FILE* meminfo;
	MemStruct m;
	char buffer[100] = {0};
	char* end;
	int found = 0;
	
	/* Try to read /proc/meminfo, bail out if fails */
	if( !( meminfo = fopen( "/proc/meminfo", "r" ) ) )
	{
		fprintf( stderr, "Could not open /proc/meminfo [%s]\n",
				 strerror( errno ) );
		fprintf( stderr, "You should explicitly tell the "\
				 "size of memory to be freed.\n" );
		exit( EXIT_FAILURE );
	}

	/* Read each line untill we got all we ned */
	while( fgets( buffer, sizeof( buffer ), meminfo ) )
	{
		/* MemFree is the current free memory */
		if( strstr( buffer, "MemFree:" ) == buffer )
		{
			m.MemFree = strtol( buffer + 8, &end, 10 );
			found++;
		}
		/* Buffers */
		else if( strstr( buffer, "Buffers:" ) == buffer )
		{
			m.Buffers = strtol( buffer + 8, &end, 10 );
			found++;
		}
		/* Cached */
		else if( strstr( buffer, "Cached:" ) == buffer )
		{
			m.Cached = strtol( buffer + 8, &end, 10 );
			found++;
			break;
		}
	}
	fclose(meminfo);
	/* Loosly check if we got all we need */
	if( found != 3 )
	{
		fprintf( stderr, "Could not parse /proc/meminfo\n" );
		exit( EXIT_FAILURE );
	}
	return m;
}

int main( int argc, char** argv )
{
	/* defaults to megabytes */
	int k = 1024 * 1024;
	/* printed characters for percentage */
	int p = 0;
	/* garbage data set by strtol */
	char* end;
	/* r = bytes to be freed */
	long int r;
	/* allocated chunks array, holds up to MAX_CHUNK chunks of CHUNK bytes  */
	void* g[MAX_CHUNK] = {0};
	/* freed bytes so far */
	int freed = 0;
	/* bytes to be freed in current iteration */
	int tobefreed = 0;
	/* /proc/meminfo info */
	MemStruct m;
	/* index of allocated memory spaces array */
	int i = 0;
	/* Any paramter given ? */
	if( argc > 1 )
	{
		/* Help request, print some information */
		if( !strcmp( argv[1], "-h" ) || !strcmp( argv[1], "--help" ) )
		{
			fprintf( stderr, "%s [size[M|k]]\n", argv[0] );
			return EXIT_FAILURE;
		}
		
		/* Try to read the parameter which is supposed to be a number
 		 * followed by an optionail size specifier which is either k or m.
 		 * Both are case insensitive */
		r = strtol( argv[1], &end, 10 );
		
		/* Error check as anyone sane would do */
		if( errno )
		{
			fprintf( stderr, "Could not parse parameter '%s'. Error was '%s'\n", 
							 argv[1], strerror( errno ) );
			return EXIT_FAILURE;
		}
		
		/* Negative value is simply nonsense, how am I supposed to free -10M */
		if( r < 0 )
		{
			fprintf( stderr, "Please provide a positive size.\n" );
			return EXIT_FAILURE;
		}
		
		/* Give feedback that there is some ignored garbage in the paramter */
		if( strlen(end) > 1 )
		{
			fprintf( stderr, "Ignoring garbage input '%s'\n", end+1 );
		}
		
		/* Check for the size specifier */
		if( end[0] == '\0' || end[0] == 'M' || end[0] == 'm' )
		{
			/* Either there's no size specifier or it's m or M.
 			 * I'm leaving k as untouched, since its default value
 			 * is 1024*1024 which is M. */			 
			fprintf( stderr, "Freeing %ldM of memory...\n", r );
		}
		else if( end[0] == 'K' || end[0] == 'k' )
		{
			k = 1024;
			fprintf( stderr, "Freeing %ldk of memory...\n", r );
		}
		else
		{
			fprintf( stderr, "Unknown size specifier ('%c'), " \
							 "defaulting to M\n", end[0] );
			fprintf( stderr, "Freeing %ldM of memory...\n", r );
		}
	}
	else
	{
		/* No argument is given. Try to calculate cache+buffer+free mem
 		 * and free that to naively force of to flush the cache and buffer */
		fprintf( stderr, "Trying to free buffer and cache...\n" );

		m = parse_meminfo();
		r = m.MemFree + m.Cached + m.Buffers;
		k = 1024;
		
		fprintf( stderr, "Before -> MemFree: %ldk Cached: %ldk Buffers: %ldk Total: %ldk\n",
						m.MemFree, m.Cached, m.Buffers, r );
	}
	
	/* How many bytes to allocate and reset */
	r *= k;
	while( freed < r )
	{
		/* How many bytes should we process in this iteration.
 		 * Process whole CHUNK of bytes if we are not exceeding
 		 * our total bytes to be reset. If we're process as few
 		 * as necessary. */
		tobefreed = (freed + CHUNK) > r ? r - freed : CHUNK;

		/* Actually allocate the space and bail if we fail at that */
		if( !( g[i] = malloc( tobefreed ) ) )
		{
			fprintf( stderr, "Could not allocate memory\n" );
			return EXIT_FAILURE;
		}

		/* This is the actual palce where we're allocating and
 		 * resetting this memory space. malloc itself alone just
 		 * allocates the space, does not occupies it. */
		memset( g[i++], 0, tobefreed );

		freed += tobefreed;

		/* Print visual feedback of how much we progressed */		
		while( p-- ) printf( "\b" );
		p = printf( "%.2f%%", ((freed*1.0)/(r))*100);
		fflush( stdout );
	}
	while(i--)
	{
		free(g[i]);
	}
	printf( "\n" );
	m = parse_meminfo();
	fprintf( stderr, "After  -> MemFree: %ldk Cached: %ldk Buffers: %ldk\n",
					 m.MemFree, m.Cached, m.Buffers);
	return EXIT_SUCCESS;
}
