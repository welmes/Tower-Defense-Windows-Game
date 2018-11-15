#ifndef _game_map_h_included_
#define _game_map_h_included_

#include "SDL\SDL.h"

class Tower;

#define NUM_MAP_COLS  34
#define NUM_MAP_ROWS  18
#define NUM_MAP_TILES 4

#define PLACEMENT_OFF       0
#define PLACEMENT_ON        1
#define PLACEMENT_HIGHLIGHT 2

#define TILE_TYPE_OPEN    0
#define TILE_TYPE_CLOSED  1
#define TILE_TYPE_BLOCKED 2
#define TILE_TYPE_PATH    3
#define TILE_TYPE_INVALID -1

class GameMap{
private:
	int state;
	int activecol, activerow, footprintcol, footprintrow;
	int placementmode;
	int footprinttiletypes[ 4 ];
	Tower* towerslots[ NUM_MAP_COLS ][ NUM_MAP_ROWS ];
	// The attributes of the tiles
    SDL_Rect boxes[ NUM_MAP_COLS ][ NUM_MAP_ROWS ];
	// The part of the map tile sprite sheet that will be shown for each tile
	int tiletypes[ NUM_MAP_COLS ][ NUM_MAP_ROWS ];
    SDL_Rect clips[ NUM_MAP_COLS ][ NUM_MAP_ROWS ];

	void set_clip( int x, int y );
	bool slot_is_buildable( int x, int y );

public:
	int W, N, E, S;

	GameMap();
	~GameMap();
	void click( int x, int y );
	void unclick( int x, int y );
	void idle();
	void disable();
	bool tower_slot_is_empty( int x, int y );
	bool focus_slot_is_occupied( int x, int y );
	void fuse_focuses( int rank, int color );
	void set_placement_mode( bool enabled );
	void set_placement_footprint( int x, int y );
	void clear_placement_footprint();
	void set_tile_type( int x, int y, int type );
	void reset_map();
	void show( Uint32 deltaTicks );
	void show_attacks( Uint32 deltaTicks );

	friend class Inventory;
	friend class Mob;
};

#endif