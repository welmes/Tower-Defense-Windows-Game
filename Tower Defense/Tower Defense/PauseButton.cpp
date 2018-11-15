#include "PauseButton.h"
#include "CreateButton.h"
#include "FuseButton.h"
#include "BasicTowerButton.h"
#include "BounceTowerButton.h"
#include "SplashTowerButton.h"
#include "PierceTowerButton.h"
#include "SlowTowerButton.h"
#include "SkipButton.h"
#include "Inventory.h"
#include "GameMap.h"
#include "GameVars.h"
#include <string>

extern CreateButton createbutton;
extern FuseButton fusebutton;
extern BasicTowerButton basicbutton;
extern BounceTowerButton bouncebutton;
extern SplashTowerButton splashbutton;
extern PierceTowerButton piercebutton;
extern SlowTowerButton slowbutton;
extern SkipButton skipbutton;
extern Inventory inventory;
extern GameMap gamemap;

extern const int PIXELS_PER_TILE;

extern SDL_Surface* pausebuttonsheet;
extern SDL_Surface* screen;

extern bool paused;

extern int curLevel;

extern int endgame;

void set_message( std::string m, bool enabled );
void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL );
void idle_focuses();
bool load_level( int level );

// The button states in the sprite sheet
const int CLIP_PAUSE_IDLE      = 0;
const int CLIP_PAUSE_HIGHLIGHT = 1;

SDL_Rect pausebuttonclips[ 2 ];

PauseButton::PauseButton(){
	state = 1;

	set_clips();
	clip = &pausebuttonclips[ CLIP_PAUSE_IDLE ];

	// Set the button's attributes
	int x = 1, y = 1, w = 2, h = 2;

	box.x = x * PIXELS_PER_TILE;
	box.y = y * PIXELS_PER_TILE;
	box.w = w * PIXELS_PER_TILE;
	box.h = h * PIXELS_PER_TILE;

	W = x;
	N = y;
	E = x + w;
	S = y + h;
}

PauseButton::~PauseButton(){
	delete clip;
}

void PauseButton::set_clips(){
	// Clip the sprite sheet
	int w = 2 * PIXELS_PER_TILE;
	int h = 2 * PIXELS_PER_TILE;

	pausebuttonclips[ CLIP_PAUSE_IDLE ].x      = 0;
	pausebuttonclips[ CLIP_PAUSE_IDLE ].y      = 0;
	pausebuttonclips[ CLIP_PAUSE_IDLE ].w      = w;
	pausebuttonclips[ CLIP_PAUSE_IDLE ].h      = h;

	pausebuttonclips[ CLIP_PAUSE_HIGHLIGHT ].x = w;
	pausebuttonclips[ CLIP_PAUSE_HIGHLIGHT ].y = 0;
	pausebuttonclips[ CLIP_PAUSE_HIGHLIGHT ].w = w;
	pausebuttonclips[ CLIP_PAUSE_HIGHLIGHT ].h = h;
}

void PauseButton::set_highlight( bool enabled ){
	if( enabled == true ){
		clip = &pausebuttonclips[ CLIP_PAUSE_HIGHLIGHT ];
	}
	else{
		clip = &pausebuttonclips[ CLIP_PAUSE_IDLE ];
	}
}

void PauseButton::click(){
	if( state == 1 ){
		createbutton.disable();
		fusebutton.disable();
		basicbutton.disable();
		bouncebutton.disable();
		splashbutton.disable();
		piercebutton.disable();
		slowbutton.disable();
		skipbutton.disable();
		inventory.disable();
		gamemap.disable();

		paused = true;

		set_message( "Paused.", true );
		set_highlight( true );
		state = 2;
	}
	else if( state == 2 ){
		createbutton.idle();
		fusebutton.idle();
		basicbutton.idle();
		bouncebutton.idle();
		splashbutton.idle();
		piercebutton.idle();
		slowbutton.idle();
		skipbutton.idle();
		idle_focuses();

		paused = false;

		set_message( "Paused.", false );
		set_highlight( false );
		state = 1;
	}
	else if( state == 3 ){
		createbutton.idle();
		fusebutton.idle();
		basicbutton.idle();
		bouncebutton.idle();
		splashbutton.idle();
		piercebutton.idle();
		slowbutton.idle();
		skipbutton.idle();
		idle_focuses();

		paused = false;

		set_message( "", true );
		set_highlight( false );

		load_level( curLevel );

		endgame = 0;

		state = 1;
	}
	else if( state == 4 ){
		createbutton.idle();
		fusebutton.idle();
		basicbutton.idle();
		bouncebutton.idle();
		splashbutton.idle();
		piercebutton.idle();
		slowbutton.idle();
		skipbutton.idle();
		idle_focuses();

		paused = false;

		set_message( "", true );
		set_highlight( false );

		if( curLevel < NUM_LEVELS ){
			curLevel++;
		}
		else{
			curLevel = 1;
		}
		load_level( curLevel );

		endgame = 0;

		state = 1;
	}
}

void PauseButton::defeat_pause(){
	createbutton.disable();
	fusebutton.disable();
	basicbutton.disable();
	bouncebutton.disable();
	splashbutton.disable();
	piercebutton.disable();
	slowbutton.disable();
	skipbutton.disable();
	inventory.disable();
	gamemap.disable();

	paused = true;

	set_message( "", true );
	set_highlight( true );
	state = 3;
}

void PauseButton::victory_pause(){
	createbutton.disable();
	fusebutton.disable();
	basicbutton.disable();
	bouncebutton.disable();
	splashbutton.disable();
	piercebutton.disable();
	slowbutton.disable();
	skipbutton.disable();
	inventory.disable();
	gamemap.disable();

	paused = true;

	set_message( "", true );
	set_highlight( true );
	state = 4;
}

void PauseButton::show(){
	// Show the button
	apply_surface( box.x, box.y, pausebuttonsheet, screen, clip );
}