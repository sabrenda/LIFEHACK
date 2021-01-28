// resource compiler
// written by Alexey Podshivalov

#include 	<fcntl.h>
#include	<ctype.h>
#include	<io.h>
#include	<stdio.h>
#include 	<string.h>
#include	<sys\stat.h>
#include	<dir.h>
#include	<alloc.h>
#include	<stdlib.h>

#include 	"resfile.h"

#define	BUF_SIZE        1024

ResFile 	* resFile;

char  * getline ( char * line, FILE * fp )
{
	do {
		if ( fgets ( line, BUF_SIZE, fp ) == NULL )
			return NULL;

		for ( int i = 0; line [i]; i++ )
			if ( line [i] == '\n' )
				line [i] = '\0';
	} while ( !strlen ( line ) );

	return line;
}

void	procBinary ( char * name, char * fileName )
{
	int	file = open ( fileName, O_RDONLY | O_BINARY );
	int	size = filelength ( file );
	char  * buf  = new char [size];
	int	res  = read ( file, buf, size );

	close ( file );

	res = resFile -> put ( name, buf, res );

	delete [] buf;
}

int main ( int argc, char * argv [] )
{
	if ( argc < 2 )
	{
		printf ( "\nUsage:\n\trescomp <rc-file> <res-file>" );
		return 1;
	}

	char resFileName [128];

	strcpy ( resFileName, argv [2] );

			// create res-file object
	resFile = new ResFile ( resFileName, O_RDWR );

	FILE * fp = fopen ( argv [1], "rt" );

	if ( fp == NULL )
	{
		printf ( "\nCan't open %s", argv [1] );

		return 1;
	}

	char	commandLine [128]; // *.rc file command

	while ( getline ( commandLine, fp ) != NULL )
	{
		// *.rc file string:
		// <name> <command> <file>
		char  * name    = strtok ( commandLine, " \t" );
		char  * command = strtok ( NULL, " \t" );
		char  * file    = strtok ( NULL, " \t" );

		if ( name == NULL || command == NULL || file == NULL )
		{
			printf ( "\nIllegal command" );
			continue;
		}

		if ( !stricmp ( command, "BINARY" ) )
			procBinary ( name, file );
		else
			printf ( "Rc-file error !\nInvalid command %s", command );
	}

	fclose ( fp );

	delete resFile;

	printf("Resource compiled successfully.\n");

	return 0;
}
