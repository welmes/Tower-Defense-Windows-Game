#include "Focus.h"
#include "GameVars.h"

extern const int PIXELS_PER_TILE;

extern SDL_Surface* focusessheet;
extern SDL_Surface* screen;

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL );

Focus::Focus( int r, int c ){
	rank  = r;
	color = c;
	highlighted = 0;

	set_clip();

	damage = damages[ c ][ r ];
	speed  = speeds[ c ];
}

void Focus::set_clip(){
	// Clip the sprite sheet
	int w = 2 * PIXELS_PER_TILE;
	int h = 2 * PIXELS_PER_TILE;

	clip.x = color * w;
	clip.y = ( rank * 2 + highlighted ) * h;
	clip.w = w;
	clip.h = h;
}

void Focus::set_highlight( bool enabled ){
	if( enabled == true ){
		highlighted = 1;
	}
	else{
		highlighted = 0;
	}
	set_clip();
}

void Focus::show( SDL_Rect box ){
	// Show the focus
	apply_surface( box.x, box.y, focusessheet, screen, &clip );
}