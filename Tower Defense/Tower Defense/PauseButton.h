#ifndef _pause_button_h_included_
#define _pause_button_h_included_

#include "SDL\SDL.h"

class PauseButton{
private:
	int state;
	// The attributes of the button
    SDL_Rect box;
    // The part of the button sprite sheet that will be shown
    SDL_Rect* clip;

	void set_clips();
	void set_highlight( bool enabled );

public:
	int W, N, E, S;

	PauseButton();
	~PauseButton();
	void click();
	void defeat_pause();
	void victory_pause();
	void show();
};

#endif