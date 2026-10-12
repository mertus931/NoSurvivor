#include <global.h>

#include "noSurvivors.h"

extern "C"
int main( int argc, char * argv [] )
try
{
	std::ios_base::sync_with_stdio( false );
	NoSurvivors game;
	return game.Run();
}
catch( ... )
{
	print( "Non-SDL Exception has occurred!\n" );

	// put a breakpoint in this line, to prevent the shell from closing
	return 66;
}
