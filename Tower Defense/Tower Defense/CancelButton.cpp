#include "CancelButton.h"
#include "CreateButton.h"
#include "FuseButton.h"
#include "GameVars.h"

extern CreateButton createbutton;
extern FuseButton fusebutton;

extern const int PIXELS_PER_TILE;

extern SDL_Surface* cancelbuttonsheet;
extern SDL_Surface* screen;

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL );

CancelButton::CancelButton(){
	state = 3;

	// Set the button's attributes
	int x = 45, y = 13, w = 2, h = 2;

	box.x = x * PIXELS_PER_TILE;
	box.y = y * PIXELS_PER_TILE;
	box.w = w * PIXELS_PER_TILE;
	box.h = h * PIXELS_PER_TILE;

	W = x;
	N = y;
	E = x + w;
	S = y + h;
}

void CancelButton::activate_create( bool enabled ){
	if( enabled == true ){
		state = 1;
	}
	else{
		state = 3;
	}
}

void CancelButton::activate_fuse( bool enabled ){
	if( enabled == true ){
		state = 2;
	}
	else{
		state = 3;
	}
}

void CancelButton::click(){
	if( state == 1 ){
		createbutton.idle();
	}
	else if( state == 2 ){
		fusebutton.idle();
	}
}

void CancelButton::show(){
	// Show the button
	apply_surface( box.x, box.y, cancelbuttonsheet, screen );
}