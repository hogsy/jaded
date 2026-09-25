#include "core/fs/fs_pak.h"

int main( int argc, char **argv )
{
	if ( argc <= 1 )
	{
		printf( "Invalid number of arguments!\n" );
		return EXIT_FAILURE;
	}

	const std::string path = argv[ 1 ];

	core::fs::Pak pak;
	if ( !pak.Open( path ) )
	{
		return EXIT_FAILURE;
	}

	pak.Export( "Dump" );

	return EXIT_SUCCESS;
}
