#ifndef _create_button_h_included_
#define _create_button_h_included_

#include "SDL\SDL.h"

class FocusSlot;
class RankButton;

#define NUM_CREATED_FOCUSES 3
#define NUM_RANK_BUTTONS    4

#define CREATE_TOOLTIP_INDEX 1

class CreateButton{
private:
	int state;
	int cost[ NUM_RANK_BUTTONS ];
	int openslots[ NUM_CREATED_FOCUSES ];
	// The attributes of the button
    SDL_Rect box;
    // The part of the button sprite sheet that will be shown
    SDL_Rect* clip;

	void set_clips();
	bool inventory_has_room();
	void activate_subbuttons( bool enabled );
	void set_highlight( bool enabled );
	void set_greymask( bool enabled );

public:
	int W, N, E, S;
	RankButton* subbuttons[ NUM_RANK_BUTTONS ];

	CreateButton();
	~CreateButton();
	void click();
	void idle();
	void disable();
	void cash_increased();
	void cash_decreased();
	void signal_create( int rank );
	void show();
};

#endif