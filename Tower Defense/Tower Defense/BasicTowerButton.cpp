#include "BasicTowerButton.h"
#include "CreateButton.h"
#include "FuseButton.h"
#include "BounceTowerButton.h"
#include "SplashTowerButton.h"
#include "PierceTowerButton.h"
#include "SlowTowerButton.h"
#include "GameMap.h"
#include "GameVars.h"
#include <string>

extern CreateButton createbutton;
extern FuseButton fusebutton;
extern BounceTowerButton bouncebutton;
extern SplashTowerButton splashbutton;
extern PierceTowerButton piercebutton;
extern SlowTowerButton slowbutton;
extern GameMap gamemap;

extern const int PIXELS_PER_TILE;

extern SDL_Surface* basicbuttonsheet;
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
const int CLIP_BASIC_IDLE      = 0;
const int CLIP_BASIC_HIGHLIGHT = 1;
const int CLIP_BASIC_GREYMASK  = 2;

SDL_Rect basicbuttonclips[ 3 ];

BasicTowerButton::BasicTowerButton(){
	cost = BASIC_TOWER_COST;

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
	int x = 1, y = 23, w = 6, h = 3;

	box.x = x * PIXELS_PER_TILE;
	box.y = y * PIXELS_PER_TILE;
	box.w = w * PIXELS_PER_TILE;
	box.h = h * PIXELS_PER_TILE;

	W = x;
	N = y;
	E = x + w;
	S = y + h;
}

BasicTowerButton::~BasicTowerButton(){
	delete clip;
}

void BasicTowerButton::set_clips(){
	// Clip the sprite sheet
	int w = 6 * PIXELS_PER_TILE;
	int h = 3 * PIXELS_PER_TILE;

	basicbuttonclips[ CLIP_BASIC_IDLE ].x      = 0;
	basicbuttonclips[ CLIP_BASIC_IDLE ].y      = 0;
	basicbuttonclips[ CLIP_BASIC_IDLE ].w      = w;
	basicbuttonclips[ CLIP_BASIC_IDLE ].h      = h;

	basicbuttonclips[ CLIP_BASIC_HIGHLIGHT ].x = w;
	basicbuttonclips[ CLIP_BASIC_HIGHLIGHT ].y = 0;
	basicbuttonclips[ CLIP_BASIC_HIGHLIGHT ].w = w;
	basicbuttonclips[ CLIP_BASIC_HIGHLIGHT ].h = h;

	basicbuttonclips[ CLIP_BASIC_GREYMASK ].x  = 0;
	basicbuttonclips[ CLIP_BASIC_GREYMASK ].y  = h;
	basicbuttonclips[ CLIP_BASIC_GREYMASK ].w  = w;
	basicbuttonclips[ CLIP_BASIC_GREYMASK ].h  = h;
}

void BasicTowerButton::set_greymask( bool enabled ){
	if( enabled == true ){
		clip = &basicbuttonclips[ CLIP_BASIC_GREYMASK ];
	}
	else{
		clip = &basicbuttonclips[ CLIP_BASIC_IDLE ];
	}
}

void BasicTowerButton::set_highlight( bool enabled ){
	if( enabled == true ){
		clip = &basicbuttonclips[ CLIP_BASIC_HIGHLIGHT ];
	}
	else{
		clip = &basicbuttonclips[ CLIP_BASIC_IDLE ];
	}
}

void BasicTowerButton::click(){
	if( state == 1 ){
		idle_active_button();
		idle_focuses();
		set_highlight( true );
		gamemap.set_placement_mode( true );
		activebutton = BASIC_BUTTON;
		set_tooltip( BASIC_TOOLTIP_INDEX );
		state = 3;
	}
	else if( state == 4 ){
		idle_active_button();
		set_message( "Towers must be placed on an open map slot.", false );
		set_highlight( true );
		gamemap.set_placement_mode( true );
		activebutton = BASIC_BUTTON;
		set_tooltip( BASIC_TOOLTIP_INDEX );
		state = 3;
	}
	else if( state == 3 ){
		idle();
	}
}

void BasicTowerButton::idle(){
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

void BasicTowerButton::disable(){
	idle();
	set_greymask( true );
	state = 0;
}

void BasicTowerButton::cash_increased(){
	if( state == 2 && cash >= cost ){
		set_greymask( false );
		state = 1;
	}
}

void BasicTowerButton::cash_decreased(){
	if( state == 1 && cash < cost ){
		set_greymask( true );
		state = 2;
	}
}

void BasicTowerButton::signal_build(){
	// Reduce player's cash by the button's cost
	set_cash( -cost );
	// Tell the other buttons that cash has been reduced
	createbutton.cash_decreased();
	fusebutton.cash_decreased();
	bouncebutton.cash_decreased();
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

void BasicTowerButton::show(){
	// Show the button
	apply_surface( box.x, box.y, basicbuttonsheet, screen, clip );
}