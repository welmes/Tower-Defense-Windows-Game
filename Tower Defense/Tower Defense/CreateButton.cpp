#include "CreateButton.h"
#include "Focus.h"
#include "RankButton.h"
#include "CancelButton.h"
#include "FuseButton.h"
#include "BasicTowerButton.h"
#include "BounceTowerButton.h"
#include "SplashTowerButton.h"
#include "PierceTowerButton.h"
#include "SlowTowerButton.h"
#include "Inventory.h"
#include "GameVars.h"
#include <string>

extern CancelButton cancelbutton;
extern FuseButton fusebutton;
extern BasicTowerButton basicbutton;
extern BounceTowerButton bouncebutton;
extern SplashTowerButton splashbutton;
extern PierceTowerButton piercebutton;
extern SlowTowerButton slowbutton;
extern Inventory inventory;

extern const int PIXELS_PER_TILE;

extern SDL_Surface* createbuttonsheet;
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
const int CLIP_CREATE_IDLE      = 0;
const int CLIP_CREATE_HIGHLIGHT = 1;
const int CLIP_CREATE_GREYMASK  = 2;

SDL_Rect createbuttonclips[ 3 ];

CreateButton::CreateButton(){
	cost[0] = 10;
	cost[1] = 25;
	cost[2] = 55;
	cost[3] = 115;

	if( cash < cost[0] ){
		state = 2;
		set_greymask( true );
	}
	else{
		state = 1;
		set_greymask( false );
	}

	for( int i = 0; i < NUM_CREATED_FOCUSES; i++ ){
		openslots[i] = -1;
	}

	for( int i = 0; i < NUM_RANK_BUTTONS; i++ ){
		subbuttons[i] = new RankButton( i );
	}

	set_clips();

	// Set the button's attributes
	int x = 37, y = 8, w = 5, h = 4;

	box.x = x * PIXELS_PER_TILE;
	box.y = y * PIXELS_PER_TILE;
	box.w = w * PIXELS_PER_TILE;
	box.h = h * PIXELS_PER_TILE;

	W = x;
	N = y;
	E = x + w;
	S = y + h;
}

CreateButton::~CreateButton(){
	for( int i = 0; i < NUM_RANK_BUTTONS; i++ ){
		delete subbuttons[i];
		subbuttons[i] = NULL;
	}
	delete [] *subbuttons;
}

void CreateButton::set_clips(){
	// Clip the sprite sheet
	int w = 5 * PIXELS_PER_TILE;
	int h = 4 * PIXELS_PER_TILE;

	createbuttonclips[ CLIP_CREATE_IDLE ].x      = 0;
	createbuttonclips[ CLIP_CREATE_IDLE ].y      = 0;
	createbuttonclips[ CLIP_CREATE_IDLE ].w      = w;
	createbuttonclips[ CLIP_CREATE_IDLE ].h      = h;

	createbuttonclips[ CLIP_CREATE_HIGHLIGHT ].x = w;
	createbuttonclips[ CLIP_CREATE_HIGHLIGHT ].y = 0;
	createbuttonclips[ CLIP_CREATE_HIGHLIGHT ].w = w;
	createbuttonclips[ CLIP_CREATE_HIGHLIGHT ].h = h;

	createbuttonclips[ CLIP_CREATE_GREYMASK ].x  = 0;
	createbuttonclips[ CLIP_CREATE_GREYMASK ].y  = h;
	createbuttonclips[ CLIP_CREATE_GREYMASK ].w  = w;
	createbuttonclips[ CLIP_CREATE_GREYMASK ].h  = h;
}

bool CreateButton::inventory_has_room(){
	int count = 0;
	for( int i = 0; i < NUM_INV_SLOTS && count < NUM_CREATED_FOCUSES; i++ ){
		if( !inventory.slot_is_occupied( i ) ){
			openslots[ count ] = i;
			count++;
		}
	}
	if( count < NUM_CREATED_FOCUSES ){
		return false;
	}
	else{
		return true;
	}
}

void CreateButton::activate_subbuttons( bool enabled ){
	for( int i = 0; i < NUM_RANK_BUTTONS; i++ ){
		subbuttons[i]->activate( enabled );
	}
	cancelbutton.activate_create( enabled );
}

void CreateButton::set_greymask( bool enabled ){
	if( enabled == true ){
		clip = &createbuttonclips[ CLIP_CREATE_GREYMASK ];
	}
	else{
		clip = &createbuttonclips[ CLIP_CREATE_IDLE ];
	}
}

void CreateButton::set_highlight( bool enabled ){
	if( enabled == true ){
		clip = &createbuttonclips[ CLIP_CREATE_HIGHLIGHT ];
	}
	else{
		clip = &createbuttonclips[ CLIP_CREATE_IDLE ];
	}
}

void CreateButton::click(){
	if( state == 1 ){
		idle_active_button();
		idle_focuses();
		if( inventory_has_room() ){
			activate_subbuttons( true );
			set_highlight( true );
			set_tooltip( CREATE_TOOLTIP_INDEX );
			activebutton = CREATE_BUTTON;
			state = 3;
		}
		else{
			set_message( "Inventory is full.", true );
			state = 4;
		}
	}
	else if( state == 4 ){
		if( inventory_has_room() ){
			idle_active_button();
			set_message( "Inventory is full.", true );
			activate_subbuttons( true );
			set_highlight( true );
			set_tooltip( CREATE_TOOLTIP_INDEX );
			activebutton = CREATE_BUTTON;
			state = 3;
		}
	}
	else if( state == 3 ){
		idle();
	}
}

void CreateButton::idle(){
	if( state == 3 ){
		activate_subbuttons( false );
		set_highlight( false );
		set_tooltip( 0 );
		state = 1;
	}
	else if( state == 4 ){
		set_message( "Inventory is full.", false );
		state = 1;
	}
	else if( state == 0 ){
		if( cash < cost[0] ){
			state = 2;
			set_greymask( true );
		}
		else{
			state = 1;
			set_greymask( false );
		}
	}
}

void CreateButton::disable(){
	idle();
	set_greymask( true );
	state = 0;
}

void CreateButton::cash_decreased(){
	if( state == 1 && cash < cost[0] ){
		set_greymask( true );
		state = 2;
	}
}

void CreateButton::cash_increased(){
	if( state == 2 && cash >= cost[0] ){
		set_greymask( false );
		state = 1;
	}
	else if( state == 3 ){
		for( int i = 0; i < NUM_RANK_BUTTONS; i++ ){
			subbuttons[i]->cash_increased();
		}
	}
}

void CreateButton::signal_create( int rank ){
	activate_subbuttons( false );
	set_highlight( false );
	if( inventory_has_room() ){
		// Reduce player's cash by the button's cost
		set_cash( -cost[ rank ] );
		// Tell the other buttons that cash has been reduced
		fusebutton.cash_decreased();
		basicbutton.cash_decreased();
		bouncebutton.cash_decreased();
		splashbutton.cash_decreased();
		piercebutton.cash_decreased();
		slowbutton.cash_decreased();

		for( int i = 0; i < NUM_CREATED_FOCUSES; i++ ){
			inventory.create_focus( openslots[i], rank, i );
		}
		set_tooltip( 0 );
		if( cash < cost[0] ){
			state = 2;
			set_greymask( true );
		}
		else{
			state = 1;
			set_greymask( false );
		}
	}
	else{
		set_message( "Inventory is full.", true );
		set_tooltip( 0 );
		state = 4;
	}
}

void CreateButton::show(){
	// Show the button
	apply_surface( box.x, box.y, createbuttonsheet, screen, clip );

	if( state == 3 ){
		for( int i = 0; i < NUM_RANK_BUTTONS; i++ ){
			subbuttons[i]->show();
		}
		cancelbutton.show();
	}
}