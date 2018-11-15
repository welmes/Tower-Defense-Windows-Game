#ifndef _inventory_h_included_
#define _inventory_h_included_

#include "SDL\SDL.h"

class Focus;

#define NUM_INV_SLOTS 15
#define NUM_INV_COLS      5
#define NUM_INV_ROWS      3

class Inventory{
private:
	int state;
	int activeslot;
	Focus* inventoryslots[ NUM_INV_SLOTS ];
	// The attributes of the button
    SDL_Rect boxes[ NUM_INV_SLOTS ];

public:
	int W, N, E, S;

	Inventory();
	~Inventory();
	void click( int x, int y );
	void unclick( int x, int y );
	void idle();
	void disable();
	bool slot_is_occupied( int slot );
	void create_focus( int slot, int rank, int color );
	void clear_focuses();
	void show();

	friend class GameMap;
};

#endif