#include "FuseButton.h"
#include "CancelButton.h"
#include "CreateButton.h"
#include "BasicTowerButton.h"
#include "BounceTowerButton.h"
#include "SplashTowerButton.h"
#include "PierceTowerButton.h"
#include "SlowTowerButton.h"
#include "Focus.h"
#include "GameVars.h"
#include <string>

extern CancelButton cancelbutton;
extern CreateButton createbutton;
extern BasicTowerButton basicbutton;
extern BounceTowerButton bouncebutton;
extern SplashTowerButton splashbutton;
extern PierceTowerButton piercebutton;
extern SlowTowerButton slowbutton;

extern const int PIXELS_PER_TILE;

extern SDL_Surface* fusebuttonsheet;
extern SDL_Surface* screen;

extern int cash;

extern int activebutton;

void set_message( std::string m, bool enabled );
void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL );
void idle_active_button();
void idle_focuses();
void set_cash( int c );
void set_tooltip( int index );

// The button states in the sprite sheet
const int CLIP_FUSE_IDLE      = 0;
const int CLIP_FUSE_HIGHLIGHT = 1;
const int CLIP_FUSE_GREYMASK  = 2;

SDL_Rect fusebuttonclips[ 3 ];

FuseButton::FuseButton(){
	cost = 10;

	if( cash < cost ){
		state = 2;
		set_greymask( true );
	}
	else{
		state = 1;
		set_greymask( false );
	}

	set_clips();

	// Set the button's attributes
	int x = 42, y = 8, w = 5, h = 4;

	box.x = x * PIXELS_PER_TILE;
	box.y = y * PIXELS_PER_TILE;
	box.w = w * PIXELS_PER_TILE;
	box.h = h * PIXELS_PER_TILE;

	W = x;
	N = y;
	E = x + w;
	S = y + h;
}

FuseButton::~FuseButton(){
	delete clip;
}

void FuseButton::set_clips(){
	// Clip the sprite sheet
	int w = 5 * PIXELS_PER_TILE;
	int h = 4 * PIXELS_PER_TILE;

	fusebuttonclips[ CLIP_FUSE_IDLE ].x      = 0;
	fusebuttonclips[ CLIP_FUSE_IDLE ].y      = 0;
	fusebuttonclips[ CLIP_FUSE_IDLE ].w      = w;
	fusebuttonclips[ CLIP_FUSE_IDLE ].h      = h;

	fusebuttonclips[ CLIP_FUSE_HIGHLIGHT ].x = w;
	fusebuttonclips[ CLIP_FUSE_HIGHLIGHT ].y = 0;
	fusebuttonclips[ CLIP_FUSE_HIGHLIGHT ].w = w;
	fusebuttonclips[ CLIP_FUSE_HIGHLIGHT ].h = h;

	fusebuttonclips[ CLIP_FUSE_GREYMASK ].x  = 0;
	fusebuttonclips[ CLIP_FUSE_GREYMASK ].y  = h;
	fusebuttonclips[ CLIP_FUSE_GREYMASK ].w  = w;
	fusebuttonclips[ CLIP_FUSE_GREYMASK ].h  = h;
}

void FuseButton::activate_subbutton( bool enabled ){
	cancelbutton.activate_fuse( enabled );
}

void FuseButton::set_greymask( bool enabled ){
	if( enabled == true ){
		clip = &fusebuttonclips[ CLIP_FUSE_GREYMASK ];
	}
	else{
		clip = &fusebuttonclips[ CLIP_FUSE_IDLE ];
	}
}

void FuseButton::set_highlight( bool enabled ){
	if( enabled == true ){
		clip = &fusebuttonclips[ CLIP_FUSE_HIGHLIGHT ];
	}
	else{
		clip = &fusebuttonclips[ CLIP_FUSE_IDLE ];
	}
}

void FuseButton::click(){
	if( state == 1 ){
		idle_active_button();
		idle_focuses();
		activate_subbutton( true );
		set_highlight( true );
		activebutton = FUSE_BUTTON;
		set_tooltip( FUSE_TOOLTIP_INDEX );
		state = 3;
	}
	else if( state == 3 ){
		idle();
	}
}

void FuseButton::idle(){
	if( state == 3 ){
		set_message( "", true );
		activate_subbutton( false );
		set_highlight( false );
		idle_focuses();
		set_tooltip( 0 );
		state = 1;
		activebutton = -1;
	}
	else if( state == 0 ){
		if( cash < cost ){
			state = 2;
			set_greymask( true );
		}
		else{
			state = 1;
			set_greymask( false );
		}
	}
}

void FuseButton::disable(){
	idle();
	set_greymask( true );
	state = 0;
}

void FuseButton::cash_increased(){
	if( state == 2 && cash >= cost ){
		set_greymask( false );
		state = 1;
	}
}

void FuseButton::cash_decreased(){
	if( state == 1 && cash < cost ){
		set_greymask( true );
		state = 2;
	}
}

void FuseButton::signal_fuse(){
	// Reduce player's cash by the button's cost
	set_cash( -cost );
	// Tell the other buttons that cash has been reduced
	createbutton.cash_decreased();
	basicbutton.cash_decreased();
	bouncebutton.cash_decreased();
	splashbutton.cash_decreased();
	piercebutton.cash_decreased();
	slowbutton.cash_decreased();

	if( cash < cost ){
		set_highlight( false );
		state = 2;
		set_greymask( true );
		set_tooltip( 0 );
		activebutton = -1;
	}
}

void FuseButton::show(){
	// Show the button
	apply_surface( box.x, box.y, fusebuttonsheet, screen, clip );

	if( state == 3 || state == 4 ){
		cancelbutton.show();
	}
}