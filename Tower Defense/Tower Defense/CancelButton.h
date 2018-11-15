#ifndef _cancel_button_h_included_
#define _cancel_button_h_included_

#include "SDL\SDL.h"

class CancelButton{
private:
	int state;
	// The attributes of the button
    SDL_Rect box;

public:
	int W, N, E, S;

	CancelButton();
	void activate_create( bool enabled );
	void activate_fuse( bool enabled );
	void click();
	void show();
};

#endif