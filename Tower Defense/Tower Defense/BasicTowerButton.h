#ifndef _basic_tower_button_h_included_
#define _basic_tower_button_h_included_

#include "SDL\SDL.h"

#define BASIC_TOOLTIP_INDEX 3

class BasicTowerButton{
private:
	int state;
	int cost;
	// The attributes of the button
    SDL_Rect box;
    // The part of the button sprite sheet that will be shown
    SDL_Rect* clip;

	void set_clips();
	void set_greymask( bool enabled );
	void set_highlight( bool enabled );

public:
	int W, N, E, S;

	BasicTowerButton();
	~BasicTowerButton();
	void click();
	void idle();
	void disable();
	void cash_increased();
	void cash_decreased();
	void signal_build();
	void show();
};

#endif