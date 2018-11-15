#include "BounceTowerButton.h"
#include "CreateButton.h"
#include "FuseButton.h"
#include "BasicTowerButton.h"
#include "SplashTowerButton.h"
#include "PierceTowerButton.h"
#include "SlowTowerButton.h"
#include "GameMap.h"
#include "GameVars.h"
#include <string>

extern CreateButton createbutton;
extern FuseButton fusebutton;
extern BasicTowerButton basicbutton;
extern SplashTowerButton splashbutton;
extern PierceTowerButton piercebutton;
extern SlowTowerButton slowbutton;
extern GameMap gamemap;

extern const int PIXELS_PER_TILE;

extern SDL_Surface* bouncebuttonsheet;
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
const int CLIP_BOUNCE_IDLE      = 0;
const int CLIP_BOUNCE_HIGHLIGHT = 1;
const int CLIP_BOUNCE_GREYMASK  = 2;

SDL_Rect bouncebuttonclips[ 3 ];

BounceTowerButton::BounceTowerButton(){
	cost = BOUNCE_TOWER_COST;

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
	int x = 8, y = 23, w = 6, h = 3;

	box.x = x * PIXELS_PER_TILE;
	box.y = y * PIXELS_PER_TILE;
	box.w = w * PIXELS_PER_TILE;
	box.h = h * PIXELS_PER_TILE;

	W = x;
	N = y;
	E = x + w;
	S = y + h;
}

BounceTowerButton::~BounceTowerButton(){
	delete clip;
}

void BounceTowerButton::set_clips(){
	// Clip the sprite sheet
	int w = 6 * PIXELS_PER_TILE;
	int h = 3 * PIXELS_PER_TILE;

	bouncebuttonclips[ CLIP_BOUNCE_IDLE ].x      = 0;
	bouncebuttonclips[ CLIP_BOUNCE_IDLE ].y      = 0;
	bouncebuttonclips[ CLIP_BOUNCE_IDLE ].w      = w;
	bouncebuttonclips[ CLIP_BOUNCE_IDLE ].h      = h;

	bouncebuttonclips[ CLIP_BOUNCE_HIGHLIGHT ].x = w;
	bouncebuttonclips[ CLIP_BOUNCE_HIGHLIGHT ].y = 0;
	bouncebuttonclips[ CLIP_BOUNCE_HIGHLIGHT ].w = w;
	bouncebuttonclips[ CLIP_BOUNCE_HIGHLIGHT ].h = h;

	bouncebuttonclips[ CLIP_BOUNCE_GREYMASK ].x  = 0;
	bouncebuttonclips[ CLIP_BOUNCE_GREYMASK ].y  = h;
	bouncebuttonclips[ CLIP_BOUNCE_GREYMASK ].w  = w;
	bouncebuttonclips[ CLIP_BOUNCE_GREYMASK ].h  = h;
}

void BounceTowerButton::set_greymask( bool enabled ){
	if( enabled == true ){
		clip = &bouncebuttonclips[ CLIP_BOUNCE_GREYMASK ];
	}
	else{
		clip = &bouncebuttonclips[ CLIP_BOUNCE_IDLE ];
	}
}

void BounceTowerButton::set_highlight( bool enabled ){
	if( enabled == true ){
		clip = &bouncebuttonclips[ CLIP_BOUNCE_HIGHLIGHT ];
	}
	else{
		clip = &bouncebuttonclips[ CLIP_BOUNCE_IDLE ];
	}
}

void BounceTowerButton::click(){
	if( state == 1 ){
		idle_active_button();
		idle_focuses();
		set_highlight( true );
		gamemap.set_placement_mode( true );
		activebutton = BOUNCE_BUTTON;
		set_tooltip( BOUNCE_TOOLTIP_INDEX );
		state = 3;
	}
	else if( state == 4 ){
		idle_active_button();
		set_message( "Towers must be placed on an open map slot.", false );
		set_highlight( true );
		gamemap.set_placement_mode( true );
		activebutton = BOUNCE_BUTTON;
		set_tooltip( BOUNCE_TOOLTIP_INDEX );
		state = 3;
	}
	else if( state == 3 ){
		idle();
	}
}

void BounceTowerButton::idle(){
	if( state == 3 ){
		set_highlight( false );
		gamemap.set_placement_mode( false );
		set_tooltip( 0 );
		state = 1;
		activebutton = -1;
	}
	else if( state == 4 ){
		set_message( "Towers must be place on an open map slot.", false );
		set_highlight( false );
		state = 1;
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

void BounceTowerButton::disable(){
	idle();
	set_greymask( true );
	state = 0;
}

void BounceTowerButton::cash_increased(){
	if( state == 2 && cash >= cost ){
		set_greymask( false );
		state = 1;
	}
}

void BounceTowerButton::cash_decreased(){
	if( state == 1 && cash < cost ){
		set_greymask( true );
		state = 2;
	}
}

void BounceTowerButton::signal_build(){
	// Reduce player's cash by the button's cost
	set_cash( -cost );
	// Tell the other buttons that cash has been reduced
	createbutton.cash_decreased();
	fusebutton.cash_decreased();
	basicbutton.cash_decreased();
	splashbutton.cash_decreased();
	piercebutton.cash_decreased();
	slowbutton.cash_decreased();

	idle();

	if( cash < cost ){
		state = 2;
		set_greymask( true );
	}
	else{
		state = 1;
	}
}

void BounceTowerButton::show(){
	// Show the button
	apply_surface( box.x, box.y, bouncebuttonsheet, screen, clip );
}