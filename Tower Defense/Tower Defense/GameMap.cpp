#include "GameMap.h"
#include "Tower.h"
#include "Focus.h"
#include "FuseButton.h"
#include "BasicTowerButton.h"
#include "BounceTowerButton.h"
#include "SplashTowerButton.h"
#include "PierceTowerButton.h"
#include "SlowTowerButton.h"
#include "Inventory.h"
#include "GameVars.h"
#include <string>

extern FuseButton fusebutton;
extern BasicTowerButton basicbutton;
extern BounceTowerButton bouncebutton;
extern SplashTowerButton splashbutton;
extern PierceTowerButton piercebutton;
extern SlowTowerButton slowbutton;
extern Inventory inventory;

extern const int PIXELS_PER_TILE;

extern SDL_Surface* mapslotsheet;
extern SDL_Surface* screen;

extern int activebutton;

void set_message( std::string m, bool enabled );
void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL );
void idle_active_button();

GameMap::GameMap(){
	state = 1;

	placementmode = PLACEMENT_OFF;

	for( int c = 0; c < NUM_MAP_COLS; c++ ){
		for( int r = 0; r < NUM_MAP_ROWS; r++ ){
			// Set the tile's attributes
			int x = 1 + c,
				y = 4 + r,
				w = 2, h = 2;

			boxes[ c ][ r ].x = x * PIXELS_PER_TILE;
			boxes[ c ][ r ].y = y * PIXELS_PER_TILE;
			boxes[ c ][ r ].w = w * PIXELS_PER_TILE;
			boxes[ c ][ r ].h = h * PIXELS_PER_TILE;

			towerslots[ c ][ r ] = NULL;
			tiletypes[ c ][ r ] = TILE_TYPE_OPEN;

			set_clip( c, r );
		}
	}

	W = 1;
	N = 4;
	E = 35;
	S = 22;

	activecol = -1;
	activerow = -1;
}

GameMap::~GameMap(){
	for( int c = 0; c < NUM_MAP_COLS; c++ ){
		for( int r = 0; r < NUM_MAP_ROWS; r++ ){
			delete towerslots[ c ][ r ];
			towerslots[ c ][ r ] = NULL;
		}
	}
	for( int c = 0; c < NUM_MAP_COLS; c++ ){
		delete [] *towerslots[ c ];
	}
	delete [] **towerslots;
}

void GameMap::set_clip( int x, int y ){
	// Clip the sprite sheet
	int w = 1 * PIXELS_PER_TILE;
	int h = 1 * PIXELS_PER_TILE;

	clips[ x ][ y ].x = tiletypes[ x ][ y ] * w;
	clips[ x ][ y ].y = placementmode * h;
	clips[ x ][ y ].w = w;
	clips[ x ][ y ].h = h;
}

bool GameMap::slot_is_buildable( int x, int y ){
	// The slot and its 3 adjacent tiles to left and below it do not hang off the game map grid
	// and are all open tiles
	if( ( x < NUM_MAP_COLS - 1 ) && ( y < NUM_MAP_ROWS - 1 ) &&
		( tiletypes[ x ][ y ] == TILE_TYPE_OPEN ) && ( tiletypes[ x + 1 ][ y ] == TILE_TYPE_OPEN ) && ( tiletypes[ x ][ y + 1 ] == TILE_TYPE_OPEN ) && ( tiletypes[ x + 1 ][ y + 1 ] == TILE_TYPE_OPEN ) ){
			return true;
	}
	else{
		return false;
	}
}

void GameMap::click( int x, int y ){
	if( state == 1 ){
		// Convert from screen tile coords to game map grid coords
		int clickedx = x - W;
		int clickedy = y - N;

		// A tower only occupies 1 slot but covers 4 adjacent slots
		// Link the 4 slots a tower occupies by shifting which slot is clicked tot he upper left corner of the tower

		// Check if the clicked slot is a non upper left corner of a tower
		if( tower_slot_is_empty( clickedx, clickedy ) ){
			// Don't check out of bounds of game map
			if( clickedx > 0 ){
				// If slot is the top right corner of a tower
				if( !tower_slot_is_empty( clickedx - 1, clickedy ) ){
					// Click the slot that actually contains the tower
					clickedx = clickedx - 1;
				}
			}
			// Don't check out of bounds of game map
			if( clickedy > 0 ){
				// If slot is the bottom left corner of a tower
				if( !tower_slot_is_empty( clickedx, clickedy - 1 ) ){
					// Click the slot that actually contains the tower
					clickedy = clickedy - 1;
				}
				// Don't check out of bounds of game map
				else if( clickedx > 0 ){
					// If slot is the bottom right corner of a tower
					if( !tower_slot_is_empty( clickedx - 1, clickedy - 1 ) ){
						// Click the slot that actually contains the tower
						clickedx = clickedx - 1;
						clickedy = clickedy - 1;
					}
				}
			}
		}

		// Clicks trigger a focus fuse sequence
		if( activebutton == FUSE_BUTTON ){
			// Clicked the first focus of a fuse-pair, activating a fuse sequence
			if( ( activecol == -1 || activerow == -1 ) && inventory.activeslot == -1 ){
				// Don't even try to fuse empty focus slots
				if( focus_slot_is_occupied( clickedx, clickedy ) ){
					// Don't even try to fuse max rank focuses
					if( towerslots[ clickedx ][ clickedy ]->focus->rank >= ( NUM_FOCUS_RANKS - 1 ) ){
						set_message( "Cannot fuse max rank focuses.", true );
					}
					else{
						// Clear error messages
						set_message( "", true );
						// Set the active slot
						activecol = clickedx;
						activerow = clickedy;
						towerslots[ activecol ][ activerow ]->focus->set_highlight( true );
					}
				}
			}
			// Re-clicked the first focus of a fuse-pair, deactivating the fuse sequence
			else if( activecol == clickedx && activerow == clickedy ){
				towerslots[ activecol ][ activerow ]->focus->set_highlight( false );
				activecol = -1;
				activerow = -1;
			}
			// Both focuses of the fuse-pair are contained in the game map grid
			else if( activecol != -1 && activerow != -1 ){
				// Make sure the clicked slot is occupied by a focus
				if( focus_slot_is_occupied( clickedx, clickedy ) ){
					// Make sure the clicked slot and active slot are fuseable
					if( towerslots[ clickedx ][ clickedy ]->focus->rank  == towerslots[ activecol ][ activerow ]->focus->rank &&
						towerslots[ clickedx ][ clickedy ]->focus->color == towerslots[ activecol ][ activerow ]->focus->color ){
							// Store the rank and color of the created focus
							int rank  = towerslots[ activecol ][ activerow ]->focus->rank + 1;
							int color = towerslots[ activecol ][ activerow ]->focus->color;
							// Delete the old focuses and create the new fused focus
							delete towerslots[ activecol ][ activerow ]->focus;
							towerslots[ activecol ][ activerow ]->focus = NULL;
							// If a focus was removed from an armed tower, reset the tower's attack variables
							towerslots[ activecol ][ activerow ]->disarm();
							delete towerslots[ clickedx ][ clickedy ]->focus;
							towerslots[ clickedx ][ clickedy ]->focus = new Focus( rank, color );
							// Tell the fuse button a fuse has occured so it can deduct fuse cost, etc
							fusebutton.signal_fuse();
					}
					// The clicked slot and active slots are not fuseable
					else{
						set_message( "Focuses must be same rank and color.", true );
						towerslots[ activecol ][ activerow ]->focus->set_highlight( false );
					}
					// Clear the fuse-pair
					activecol = -1;
					activerow = -1;
				}
				// The clicked slot does not contain a focus
				else{
					set_message( "Must select two focuses to fuse.", true );
					towerslots[ activecol ][ activerow ]->focus->set_highlight( false );
					// Clear the fuse-pair
					activecol = -1;
					activerow = -1;
				}
			}
			// The clicked slot of the fuse-pair is in the game map grid and the active slot is in the inventory grid
			else if( inventory.activeslot != -1 ){
				// Make sure the clicked slot is occupied by a focus
				if( focus_slot_is_occupied( clickedx, clickedy ) ){
					// Make sure the clicked slot and the active slot are fuseable
					if( towerslots[ clickedx ][ clickedy ]->focus->rank  == inventory.inventoryslots[ inventory.activeslot ]->rank &&
						towerslots[ clickedx ][ clickedy ]->focus->color == inventory.inventoryslots[ inventory.activeslot ]->color ){
							// Store the rank and color of the created focus
							int rank  = inventory.inventoryslots[ inventory.activeslot ]->rank + 1;
							int color = inventory.inventoryslots[ inventory.activeslot ]->color;
							// Delete the old focuses and create the new fused focus
							delete inventory.inventoryslots[ inventory.activeslot ];
							inventory.inventoryslots[ inventory.activeslot ] = NULL;
							delete towerslots[ clickedx ][ clickedy ]->focus;
							towerslots[ clickedx ][ clickedy ]->focus = new Focus( rank, color );
							// Tell the fuse button a fuse has occured so it can deduct fuse cost, etc
							fusebutton.signal_fuse();
					}
					// The clicked slot and active slots are not fuseable
					else{
						set_message( "Focuses must be same rank and color.", true );
						inventory.inventoryslots[ inventory.activeslot ]->set_highlight( false );
					}
					// Clear the fuse-pair
					inventory.activeslot = -1;
				}
				// The clicked slot does not contain a focus
				else{
					set_message( "Must select two focuses to fuse.", true );
					inventory.inventoryslots[ inventory.activeslot ]->set_highlight( false );
					// Clear the fuse-pair
					inventory.activeslot = -1;
				}
			}
		}
		// Clicks trigger a build tower event
		else if( activebutton >= BASIC_BUTTON && activebutton <= SLOW_BUTTON ){
			// Make sure the clicked slot has room to build a tower
			if( slot_is_buildable( clickedx, clickedy ) ){
				// Create the new tower and flag the 4 adjacent slots as occupied
				towerslots[ clickedx ][ clickedy ] = new Tower( activebutton, clickedx, clickedy );
				tiletypes[ clickedx ][ clickedy ]         = TILE_TYPE_CLOSED;
				tiletypes[ clickedx + 1 ][ clickedy ]     = TILE_TYPE_CLOSED;
				tiletypes[ clickedx ][ clickedy + 1 ]     = TILE_TYPE_CLOSED;
				tiletypes[ clickedx + 1 ][ clickedy + 1 ] = TILE_TYPE_CLOSED;
				// Tell the correct tower button a build has occured so it can deduct build cost, etc
				switch( activebutton ){
					case BASIC_BUTTON:
						basicbutton.signal_build();
						break;
					case BOUNCE_BUTTON:
						bouncebutton.signal_build();
						break;
					case SPLASH_BUTTON:
						splashbutton.signal_build();
						break;
					case PIERCE_BUTTON:
						piercebutton.signal_build();
						break;
					case SLOW_BUTTON:
						slowbutton.signal_build();
						break;
				}
				// Clear the footprint slot used for the mouse-over placement footprint
				footprintcol = -1;
				footprintrow = -1;
			}
		}
		// Clicks trigger a focus swap sequence
		else{
			// Clicked the first focus of a swap-pair, activating a swap sequence
			if( ( activecol == -1 || activerow == -1 ) && inventory.activeslot == -1 ){
				if( focus_slot_is_occupied( clickedx, clickedy ) ){
					idle_active_button();
					activecol = clickedx;
					activerow = clickedy;
					towerslots[ activecol ][ activerow ]->focus->set_highlight( true );
				}
			}
			// Re-clicked the first focus of a swap-pair, deactivating the swap sequence
			else if( activecol == clickedx && activerow == clickedy ){
				towerslots[ activecol ][ activerow ]->focus->set_highlight( false );
				activecol = -1;
				activerow = -1;
			}
			// Both slots of the swap-pair are contained in the game map grid
			else if( activecol != -1 && activerow != -1 && !tower_slot_is_empty( clickedx, clickedy ) ){
				Focus* temp;
				towerslots[ activecol ][ activerow ]->focus->set_highlight( false );
				temp                                        = towerslots[ activecol ][ activerow ]->focus;
				towerslots[ activecol ][ activerow ]->focus = towerslots[ clickedx ][ clickedy ]->focus;
				towerslots[ clickedx ][ clickedy ]->focus   = temp;
				// If a focus was removed from an armed tower, reset the tower's attack variables
				towerslots[ activecol ][ activerow ]->disarm();
				towerslots[ clickedx ][ clickedy ]->disarm();
				activecol = -1;
				activerow = -1;
			}
			// The clicked slot of the fuse-pair is in the game map grid and the active slot is in the inventory grid
			else if( inventory.activeslot != -1 && !tower_slot_is_empty( clickedx, clickedy ) ){
				Focus* temp;
				inventory.inventoryslots[ inventory.activeslot ]->set_highlight( false );
				temp                                             = inventory.inventoryslots[ inventory.activeslot ];
				inventory.inventoryslots[ inventory.activeslot ] = towerslots[ clickedx ][ clickedy ]->focus;
				towerslots[ clickedx ][ clickedy ]->focus        = temp;
				// If a focus was removed from an armed tower, reset the tower's attack variables
				towerslots[ clickedx ][ clickedy ]->disarm();
				inventory.activeslot = -1;
			}
		}
	}
}

void GameMap::unclick( int x, int y ){
	if( state == 1 ){
		// Convert from screen tile coords to game map grid coords
		int clickedx = x - W;
		int clickedy = y - N;

		// A tower only occupies 1 slot but covers 4 adjacent slots
		// Link the 4 slots a tower occupies by shifting which slot is clicked tot he upper left corner of the tower

		// Check if the clicked slot is a non upper left corner of a tower
		if( tower_slot_is_empty( clickedx, clickedy ) ){
			// Don't check out of bounds of game map
			if( clickedx > 0 ){
				// If slot is the top right corner of a tower
				if( !tower_slot_is_empty( clickedx - 1, clickedy ) ){
					// Click the slot that actually contains the tower
					clickedx = clickedx - 1;
				}
			}
			// Don't check out of bounds of game map
			if( clickedy > 0 ){
				// If slot is the bottom left corner of a tower
				if( !tower_slot_is_empty( clickedx, clickedy - 1 ) ){
					// Click the slot that actually contains the tower
					clickedy = clickedy - 1;
				}
				// Don't check out of bounds of game map
				else if( clickedx > 0 ){
					// If slot is the bottom right corner of a tower
					if( !tower_slot_is_empty( clickedx - 1, clickedy - 1 ) ){
						// Click the slot that actually contains the tower
						clickedx = clickedx - 1;
						clickedy = clickedy - 1;
					}
				}
			}
		}

		if( ( ( activecol != -1 && activerow != -1 ) || inventory.activeslot != -1 ) && ( activecol != clickedx || activerow != clickedy ) ){
			click( x, y );
		}
	}
}

void GameMap::idle(){
	if( activecol != -1 && activerow != -1 ){
		towerslots[ activecol ][ activerow ]->focus->set_highlight( false );
		activecol = -1;
		activerow = -1;
	}
	else if( state == 0 ){
		state = 1;
	}
}

void GameMap::disable(){
	idle();
	state = 0;
}

bool GameMap::tower_slot_is_empty( int x, int y ){
	if( towerslots[ x ][ y ] == NULL ){
		return true;
	}
	else{
		return false;
	}
}

bool GameMap::focus_slot_is_occupied( int x, int y ){
	Tower* tower = towerslots[ x ][ y ];

	if( tower == NULL ){
		return false;
	}
	else{
		if( tower->focus != NULL ){
			return true;
		}
	}
	return false;
}

void GameMap::set_placement_mode( bool enabled ){
	if( enabled == true ){
		placementmode = PLACEMENT_ON;
		footprintcol = -1;
		footprintrow = -1;
	}
	else{
		placementmode = PLACEMENT_OFF;
	}
	for( int c = 0; c < NUM_MAP_COLS; c++ ){
		for( int r = 0; r < NUM_MAP_ROWS; r++ ){
			set_clip( c, r );
		}
	}
}

void GameMap::set_placement_footprint( int x, int y ){
	// Convert from screen tile coords to game map grid coords
	int mousecol = x - W;
	int mouserow = y - N;

	// Reset previous mouse-over slots
	if( footprintcol != -1 && footprintrow != -1 ){
		if( footprinttiletypes[ 0 ] != TILE_TYPE_INVALID ){
			set_clip( footprintcol, footprintrow );
		}
		if( footprinttiletypes[ 1 ] != TILE_TYPE_INVALID ){
			set_clip( footprintcol + 1, footprintrow );
		}
		if( footprinttiletypes[ 2 ] != TILE_TYPE_INVALID ){
			set_clip( footprintcol, footprintrow + 1 );
		}
		if( footprinttiletypes[ 3 ] != TILE_TYPE_INVALID ){
			set_clip( footprintcol + 1, footprintrow + 1 );
		}
	}

	// Mouse is over a buildable 2x2 group of game map tiles
	if( slot_is_buildable( mousecol, mouserow ) ){
		// Temporarily toggle off placement mode to emphasize the mouse-over slots
		placementmode = PLACEMENT_OFF;
		set_clip( mousecol, mouserow );
		set_clip( mousecol + 1, mouserow );
		set_clip( mousecol, mouserow + 1 );
		set_clip( mousecol + 1, mouserow + 1 );
		placementmode = PLACEMENT_ON;
		// Current mouse-over slots become the old mouse-over slots
		footprintcol = mousecol;
		footprintrow = mouserow;
		footprinttiletypes[ 0 ] = TILE_TYPE_OPEN;
		footprinttiletypes[ 1 ] = TILE_TYPE_OPEN;
		footprinttiletypes[ 2 ] = TILE_TYPE_OPEN;
		footprinttiletypes[ 3 ] = TILE_TYPE_OPEN;
	}
	// Mouse is over a 2x2 group of game tiles that contains at least 1 unbuildable tile
	else{
		placementmode = PLACEMENT_HIGHLIGHT;
		set_clip( mousecol, mouserow );
		placementmode = PLACEMENT_ON;

		if( mousecol < NUM_MAP_COLS - 1 ){
			footprinttiletypes[ 1 ] = tiletypes[ mousecol + 1 ][ mouserow ];
			placementmode = PLACEMENT_HIGHLIGHT;
			set_clip( mousecol + 1, mouserow );
			placementmode = PLACEMENT_ON;
		}
		else{
			footprinttiletypes[ 1 ] = TILE_TYPE_INVALID;
		}

		if( mouserow < NUM_MAP_ROWS - 1 ){
			footprinttiletypes[ 2 ] = tiletypes[ mousecol ][ mouserow + 1 ];
			placementmode = PLACEMENT_HIGHLIGHT;
			set_clip( mousecol, mouserow + 1 );
			placementmode = PLACEMENT_ON;
		}
		else{
			footprinttiletypes[ 2 ] = TILE_TYPE_INVALID;
		}

		if( ( mousecol < NUM_MAP_COLS - 1 ) && ( mouserow < NUM_MAP_ROWS - 1 ) ){
			footprinttiletypes[ 3 ] = tiletypes[ mousecol + 1 ][ mouserow + 1 ];
			placementmode = PLACEMENT_HIGHLIGHT;
			set_clip( mousecol + 1, mouserow + 1 );
			placementmode = PLACEMENT_ON;
		}
		else{
			footprinttiletypes[ 3 ] = TILE_TYPE_INVALID;
		}

		// Current mouse-over slots become the old mouse-over slots
		footprintcol = mousecol;
		footprintrow = mouserow;
	}
}

void GameMap::clear_placement_footprint(){
	// Called when a tower button is active and the mouse is outside the game map grid
	// Reset mouse-over slots
	if( footprintcol != -1 && footprintrow != -1 ){
		if( footprinttiletypes[ 0 ] != TILE_TYPE_INVALID ){
			set_clip( footprintcol, footprintrow );
		}
		if( footprinttiletypes[ 1 ] != TILE_TYPE_INVALID ){
			set_clip( footprintcol + 1, footprintrow );
		}
		if( footprinttiletypes[ 2 ] != TILE_TYPE_INVALID ){
			set_clip( footprintcol, footprintrow + 1 );
		}
		if( footprinttiletypes[ 3 ] != TILE_TYPE_INVALID ){
			set_clip( footprintcol + 1, footprintrow + 1 );
		}
		// Reset mouse-over coords
		footprintcol = -1;
		footprintrow = -1;
	}
}

void GameMap::set_tile_type( int x, int y, int type ){
	tiletypes[ x ][ y ] = type;
	set_clip( x, y );
}

void GameMap::reset_map(){
	for( int c = 0; c < NUM_MAP_COLS; c++ ){
		for( int r = 0; r < NUM_MAP_ROWS; r++ ){
			delete towerslots[ c ][ r ];
			towerslots[ c ][ r ] = NULL;
			tiletypes[ c ][ r ] = TILE_TYPE_OPEN;

			set_clip( c, r );
		}
	}
}

void GameMap::show( Uint32 deltaTicks ){
	// Show the map tiles, their towers, and their towers' focuses
	for( int c = 0; c < NUM_MAP_COLS; c++ ){
		for( int r = 0; r < NUM_MAP_ROWS; r++ ){
			apply_surface( boxes[ c ][ r ].x, boxes[ c ][ r ].y, mapslotsheet, screen, &clips[ c ][ r ] );
		}
	}
	for( int c = 0; c < NUM_MAP_COLS; c++ ){
		for( int r = 0; r < NUM_MAP_ROWS; r++ ){
			if( towerslots[ c ][ r ] != NULL ){
				towerslots[ c ][ r ]->aim( deltaTicks );
				towerslots[ c ][ r ]->show( boxes[ c ][ r ], deltaTicks );
			}
		}
	}
}

void GameMap::show_attacks( Uint32 deltaTicks ){
	for( int c = 0; c < NUM_MAP_COLS; c++ ){
		for( int r = 0; r < NUM_MAP_ROWS; r++ ){
			if( towerslots[ c ][ r ] != NULL ){
				towerslots[ c ][ r ]->shoot( deltaTicks );
			}
		}
	}
}