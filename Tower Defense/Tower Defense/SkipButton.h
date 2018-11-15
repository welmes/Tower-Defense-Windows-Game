#ifndef _skip_button_h_included_
#define _skip_button_h_included_

#include "SDL\SDL.h"

class SkipButton{
private:
	int state;
	// The attributes of the button
    SDL_Rect box;
    // The part of the button sprite sheet that will be shown
    SDL_Rect* clip;

	void set_clips();
	void set_greymask( bool enabled );

public:
	int W, N, E, S;

	SkipButton();
	~SkipButton();
	void click();
	void idle();
	void disable();
	void show();
};

#endif