#include "noSurvivors.h"

constexpr bool doBenchmark = true;

NoSurvivors::NoSurvivors()
	: Game( "Example SDL Game", Point{ 1280, 960 }, !doBenchmark )
{
	// Will be freed in Game dtor
	allStates = {
		new IntroState      ( *this, renderer ),   // 0
		new MapState        ( *this, renderer ),   // 1
	};

	// The virtual call is ok here
	SetNextState( 1 );

	SetPerfDrawMode( PerformanceDrawMode::Title );
	//SDL_SetWindowFullscreen(this->window,SDL_WINDOW_FULLSCREEN);

}

bool NoSurvivors::HandleEvent( const Event & event )
{
	// Global Keybindings, will be checked before the ones in GameState
	if( Game::HandleEvent( event ) )
		return true;

	switch( event.type )
	{
		case SDL_KEYDOWN:
		{
			const Keysym & what_key = event.key.keysym;

			if( what_key.scancode == SDL_SCANCODE_1 )
			{
				// Both Game and GameState can change the State,
				// but it will only be changed after the current frame
				SetNextState( 0 );
				return true;
			}
			else if( what_key.scancode == SDL_SCANCODE_2 )
			{
				SetNextState( 1 );
				return true;
			}
			else if ( what_key.scancode == SDL_SCANCODE_F1 )
			{
				debugEnemyHitboxes = !debugEnemyHitboxes;
			}
			else if ( what_key.scancode == SDL_SCANCODE_F2 )
			{
				debugEnemyBumpingHitboxes = !debugEnemyBumpingHitboxes;
			}
			break;
		}

		default:
			break;
	}

	return false;
}
