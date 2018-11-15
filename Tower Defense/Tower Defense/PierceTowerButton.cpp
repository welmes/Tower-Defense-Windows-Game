#include "PierceTowerButton.h"
#include "CreateButton.h"
#include "FuseButton.h"
#include "BasicTowerButton.h"
#include "BounceTowerButton.h"
#include "SplashTowerButton.h"
#include "SlowTowerButton.h"
#include "GameMap.h"
#include "GameVars.h"
#include <string>

extern CreateButton createbutton;
extern FuseButton fusebutton;
extern BasicTowerButton basicbutton;
extern BounceTowerButton bouncebutton;
extern SplashTowerButton splashbutton;
extern SlowTowerButton slowbutton;
extern GameMap gamemap;

extern const int PIXELS_PER_TILE;

extern SDL_Surface* piercebuttonsheet;
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
const int CLIP_PIERCE_IDLE      = 0;
const int CLIP_PIERCE_HIGHLIGHT = 1;
const int CLIP_PIERCE_GREYMASK  = 2;

SDL_Rect piercebuttonclips[ 3 ];

PierceTowerButton::PierceTowerButton(){
	cost = PIERCE_TOWER_COST;

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
	int x = 22, y = 23, w = 6, h = 3;

	box.x = x * PIXELS_PER_TILE;
	box.y = y * PIXELS_PER_TILE;
	box.w = w * PIXELS_PER_TILE;
	box.h = h * PIXELS_PER_TILE;

	W = x;
	N = y;
	E = x + w;
	S = y + h;
}

PierceTowerButton::~PierceTowerButton(){
	delete clip;
}

void PierceTowerButton::set_clips(){
	// Clip the sprite sheet
	int w = 6 * PIXELS_PER_TILE;
	int h = 3 * PIXELS_PER_TILE;

	piercebuttonclips[ CLIP_PIERCE_IDLE ].x      = 0;
	piercebuttonclips[ CLIP_PIERCE_IDLE ].y      = 0;
	piercebuttonclips[ CLIP_PIERCE_IDLE ].w      = w;
	piercebuttonclips[ CLIP_PIERCE_IDLE ].h      = h;

	piercebuttonclips[ CLIP_PIERCE_HIGHLIGHT ].x = w;
	piercebuttonclips[ CLIP_PIERCE_HIGHLIGHT ].y = 0;
	piercebuttonclips[ CLIP_PIERCE_HIGHLIGHT ].w = w;
	piercebuttonclips[ CLIP_PIERCE_HIGHLIGHT ].h = h;

	piercebuttonclips[ CLIP_PIERCE_GREYMASK ].x  = 0;
	piercebuttonclips[ CLIP_PIERCE_GREYMASK ].y  = h;
	piercebuttonclips[ CLIP_PIERCE_GREYMASK ].w  = w;
	piercebuttonclips[ CLIP_PIERCE_GREYMASK ].h  = h;
}

void PierceTowerButton::set_greymask( bool enabled ){
	if( enabled == true ){
		clip = &piercebuttonclips[ CLIP_PIERCE_GREYMASK ];
	}
	else{
		clip = &piercebuttonclips[ CLIP_PIERCE_IDLE ];
	}
}

void PierceTowerButton::set_highlight( bool enabled ){
	if( enabled == true ){
		clip = &piercebuttonclips[ CLIP_PIERCE_HIGHLIGHT ];
	}
	else{
		clip = &piercebuttonclips[ CLIP_PIERCE_IDLE ];
	}
}

void PierceTowerButton::click(){
	if( state == 1 ){
		idle_active_button();
		idle_focuses();;
		set_highlight( true );
		gamemap.set_placement_mode( true );
		activebutton = PIERCE_BUTTON;
		set_tooltip( PIERCE_TOOLTIP_INDEX );
		state = 3;
	}
	else if( state == 4 ){
		idle_active_button();
		set_message( "Towers must be placed on an open map slot.", false );
		set_highlight( true );
		gamemap.set_placement_mode( true );
		activebutton = PIERCE_BUTTON;
		set_tooltip( PIERCE_TOOLTIP_INDEX );
		state = 3;
	}
	else if( state == 3 ){
		idle();
	}
}

void PierceTowerButton::idle(){
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

void PierceTowerButton::disable(){
	idle();
	set_greymask( true );
	state = 0;
}

void PierceTowerButton::cash_increased(){
	if( state == 2 && cash >= cost ){
		set_greymask( false );
		state = 1;
	}
}

void PierceTowerButton::cash_decreased(){
	if( state == 1 && cash < cost ){
		set_greymask( true );
		state = 2;
	}
}

void PierceTowerButton::signal_build(){
	// Reduce player's cash by the button's cost
	set_cash( -cost );
	// Tell the other buttons that cash has been reduced
	createbutton.cash_decreased();
	fusebutton.cash_decreased();
	basicbutton.cash_decreased();
	bouncebutton.cash_decreased();
	splashbutton.cash_decreased();
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

void PierceTowerButton::show(){
	// Show the button
	apply_surface( box.x, box.y, piercebuttonsheet, screen, clip );
}