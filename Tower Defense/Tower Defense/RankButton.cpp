#include "RankButton.h"
#include "CreateButton.h"
#include "GameVars.h"

extern CreateButton createbutton;

extern const int PIXELS_PER_TILE;

extern SDL_Surface* rankbuttonsheet;
extern SDL_Surface* screen;

extern int cash;

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL );

//The button states in the sprite sheet
const int CLIP_RANK_IDLE      = 0;
const int CLIP_RANK_HIGHLIGHT = 1;
const int CLIP_RANK_GREYMASK  = 2;

SDL_Rect rankbuttonclips[ 3 ];

RankButton::RankButton( int r ){
	state   = 3;
	rank    = r;

	switch( rank ){
		case 0:
			cost = 10;
			break;
		case 1:
			cost = 25;
			break;
		case 2:
			cost = 55;
			break;
		case 3:
			cost = 115;
			break;
		default:
			break;
	}
	
	greymask = 0;

	// Set the button's attributes
	int x = 37 + ( 2 * rank );
	int y = 13;
	int w = 2;
	int h = 2;

	box.x = x * PIXELS_PER_TILE;
	box.y = y * PIXELS_PER_TILE;
	box.w = w * PIXELS_PER_TILE;
	box.h = h * PIXELS_PER_TILE;

	W = x;
	N = y;
	E = x + w;
	S = y + h;

	// Set the default sprite
	set_clip();
}

RankButton::~RankButton(){
}

void RankButton::activate( bool enabled ){
	if( enabled == true ){
		if( cash < cost ){
			state   = 2;
			set_greymask( true );
		}
		else{
			state   = 1;
			set_greymask( false );
		}
	}
	else{
		state = 3;
	}
}

void RankButton::set_clip(){
	// Clip the sprite sheet
	clip.x = rank * RANK_W;
	clip.y = greymask * RANK_H;
	clip.w = RANK_W;
	clip.h = RANK_H;
}

void RankButton::set_greymask( bool enabled ){
	if( enabled == true ){
		greymask = 1;
	}
	else{
		greymask = 0;
	}
	set_clip();
}

void RankButton::click(){
	if( state == 1 ){
		createbutton.signal_create( rank );
	}
}

void RankButton::cash_increased(){
	if( state == 2 && cash >= cost ){
		set_greymask( false );
		state = 1;
	}
}

void RankButton::show(){
	// Show the button
	apply_surface( box.x, box.y, rankbuttonsheet, screen, &clip );
}