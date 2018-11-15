#include "SkipButton.h"
#include "Level.h"
#include "GameVars.h"
#include <string>

extern Level* levelwaves;

extern const int PIXELS_PER_TILE;

extern SDL_Surface* skipbuttonsheet;
extern SDL_Surface* screen;

extern int activebutton;

void set_message( std::string m, bool enabled );
void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL );
void idle_active_button();
void idle_focuses();

// The button states in the sprite sheet
const int CLIP_SKIP_IDLE     = 0;
const int CLIP_SKIP_GREYMASK = 1;

SDL_Rect skipbuttonclips[ 2 ];

SkipButton::SkipButton(){
	state = 1;

	set_clips();
	clip = &skipbuttonclips[ CLIP_SKIP_IDLE ];

	// Set the button's attributes
	int x = 4, y = 1, w = 2, h = 2;

	box.x = x * PIXELS_PER_TILE;
	box.y = y * PIXELS_PER_TILE;
	box.w = w * PIXELS_PER_TILE;
	box.h = h * PIXELS_PER_TILE;

	W = x;
	N = y;
	E = x + w;
	S = y + h;
}

SkipButton::~SkipButton(){
	delete clip;
}

void SkipButton::set_clips(){
	// Clip the sprite sheet
	int w = 2 * PIXELS_PER_TILE;
	int h = 2 * PIXELS_PER_TILE;

	skipbuttonclips[ CLIP_SKIP_IDLE ].x     = 0;
	skipbuttonclips[ CLIP_SKIP_IDLE ].y     = 0;
	skipbuttonclips[ CLIP_SKIP_IDLE ].w     = w;
	skipbuttonclips[ CLIP_SKIP_IDLE ].h     = h;

	skipbuttonclips[ CLIP_SKIP_GREYMASK ].x = w;
	skipbuttonclips[ CLIP_SKIP_GREYMASK ].y = 0;
	skipbuttonclips[ CLIP_SKIP_GREYMASK ].w = w;
	skipbuttonclips[ CLIP_SKIP_GREYMASK ].h = h;
}

void SkipButton::set_greymask( bool enabled ){
	if( enabled == true ){
		clip = &skipbuttonclips[ CLIP_SKIP_GREYMASK ];
	}
	else{
		clip = &skipbuttonclips[ CLIP_SKIP_IDLE ];
	}
}

void SkipButton::click(){
	idle_active_button();
	idle_focuses();
	if( state == 1 ){
		set_message( "Next wave started.", true );
		activebutton = SKIP_BUTTON;
		if( levelwaves->send_wave() == false ){
			// There are no more waves to be sent
			disable();
		}
	}
}

void SkipButton::idle(){
	if( state == 1 ){
		set_message( "Next wave started.", false );
	}
	else if( state == 0 ){
		set_greymask( false );
		state = 1;
	}
}

void SkipButton::disable(){
	if( state == 1 ){
		set_greymask( true );
		state = 0;
	}
}

void SkipButton::show(){
	// Show the button
	apply_surface( box.x, box.y, skipbuttonsheet, screen, clip );
}