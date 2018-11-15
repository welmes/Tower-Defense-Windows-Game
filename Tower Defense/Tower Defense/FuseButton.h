#ifndef _fuse_button_h_included_
#define _fuse_button_h_included_

#include "SDL\SDL.h"

#define FUSE_TOOLTIP_INDEX 2

class FuseButton{
private:
	int state;
	int cost;
	// The attributes of the button
    SDL_Rect box;
    // The part of the button sprite sheet that will be shown
    SDL_Rect* clip;

	void set_clips();
	void activate_subbutton( bool enabled );
	void set_greymask( bool enabled );
	void set_highlight( bool enabled );

public:
	int W, N, E, S;

	FuseButton();
	~FuseButton();
	void click();
	void idle();
	void disable();
	void cash_increased();
	void cash_decreased();
	void signal_fuse();
	void show();
};

#endif