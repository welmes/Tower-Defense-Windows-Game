#ifndef _rank_button_h_included_
#define _rank_button_h_included_

#include "SDL\SDL.h"

#define RANK_H 48
#define RANK_W 48

class RankButton{
private:
	int state;
	int cost;
	int greymask;
	// The attributes of the button
    SDL_Rect box;
    // The part of the button sprite sheet that will be shown
    SDL_Rect clip;

	void set_clip();
	void set_greymask( bool enabled );

public:
	int rank;
	int W, N, E, S;

	RankButton( int r );
	~RankButton();
	void activate( bool enabled );
	void click();
	void cash_increased();
	void show();
};

#endif