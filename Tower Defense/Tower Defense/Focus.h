#ifndef _focus_h_included_
#define _focus_h_included_

#include "SDL\SDL.h"

#define NUM_FOCUS_RANKS  5
#define NUM_FOCUS_COLORS 3

const int damages[ NUM_FOCUS_COLORS ][ NUM_FOCUS_RANKS ] = {
	4, 6,  9, 16, 28,
	2, 3,  5,  8, 14,
	6, 9, 13, 24, 42
};
const int speeds[ NUM_FOCUS_COLORS ] = {
	400, 200, 600
};

class Focus{
private:
	int highlighted;
	// The part of the focus sprite sheet that will be shown
    SDL_Rect clip;

	void set_clip();

public:
	int rank;
	int color;
	int damage, range, speed;

	Focus( int r, int c );
	void set_highlight( bool enabled );
	void show( SDL_Rect box );
};

#endif