#include "Inventory.h"
#include "Focus.h"
#include "FuseButton.h"
#include "GameMap.h"
#include "Tower.h"
#include "GameVars.h"
#include <string>

extern FuseButton fusebutton;
extern GameMap gamemap;

extern const int PIXELS_PER_TILE;

extern SDL_Surface* inventoryslotsheet;
extern SDL_Surface* screen;

extern int activebutton;

void set_message( std::string m, bool enabled );
void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL );
void idle_active_button();

Inventory::Inventory(){
	state = 1;

	for( int i = 0; i < NUM_INV_SLOTS; i++ ){
		inventoryslots[i] = NULL;

		// Set the button's attributes
		int x = 37 + 2 * ( i % NUM_INV_COLS ),
			y = 1  + 2 * ( i / NUM_INV_COLS ),
			w = 2, h = 2;

		boxes[i].x = x * PIXELS_PER_TILE;
		boxes[i].y = y * PIXELS_PER_TILE;
		boxes[i].w = w * PIXELS_PER_TILE;
		boxes[i].h = h * PIXELS_PER_TILE;
	}
	
	W = 37;
	N = 1;
	E = 47;
	S = 7;

	activeslot  = -1;
}

Inventory::~Inventory(){
	for( int i = 0; i < NUM_INV_SLOTS; i++ ){
		delete inventoryslots[i];
		inventoryslots[i] = NULL;
	}
	delete [] *inventoryslots;
}

void Inventory::click( int x, int y ){
	if( state == 1 ){
		// Convert from screen tile coords to inventory grid coords
		int col = ( x - W ) / 2, row = ( y - N ) / 2;
		int clickedslot = col + row * NUM_INV_COLS;

		if( activebutton == FUSE_BUTTON ){
			// Clicked the first focus of a fuse-pair, activating a fuse sequence
			if( activeslot == -1 && ( gamemap.activecol == -1 || gamemap.activerow == -1 ) ){
				if( slot_is_occupied( clickedslot ) ){
					if( inventoryslots[ clickedslot ]->rank >= ( NUM_FOCUS_RANKS - 1 ) ){
						set_message( "Cannot fuse max rank focuses.", true );
					}
					else{
						set_message( "", true );
						activeslot = clickedslot;
						inventoryslots[ activeslot ]->set_highlight( true );
					}
				}
			}
			// Re-clicked the first focus of a fuse-pair, deactivating the fuse sequence
			else if( activeslot == clickedslot ){
				inventoryslots[ activeslot ]->set_highlight( false );
				activeslot = -1;
			}
			// Both focuses of the fuse-pair are contained in the game inventory grid
			else if( activeslot != -1 ){
				// Make sure the clicked slot is occupied by a focus
				if( slot_is_occupied( clickedslot ) ){
					// Make sure the clicked slot and active slot are fuseable
					if( inventoryslots[ clickedslot ]->rank  == inventoryslots[ activeslot ]->rank &&
						inventoryslots[ clickedslot ]->color == inventoryslots[ activeslot ]->color ){
						// Store the rank and color of the created focus
						int rank  = inventoryslots[ activeslot ]->rank + 1;
						int color = inventoryslots[ activeslot ]->color;
						// Delete the old focuses and create the new fused focus
						delete inventoryslots[ activeslot ];
						inventoryslots[ activeslot ] = NULL;
						delete inventoryslots[ clickedslot ];
						inventoryslots[ clickedslot ] = new Focus( rank, color );
						// Tell the fuse button a fuse has occured so it can deduct fuse cost, etc
						fusebutton.signal_fuse();
					}
					// The clicked slot and active slots are not fuseable
					else{
						set_message( "Focuses must be same rank and color.", true );
						inventoryslots[ activeslot ]->set_highlight( false );
					}
					// Clear the fuse-pair
					activeslot = -1;
				}
				// The clicked slot does not contain a focus
				else{
					set_message( "Must select two focuses to fuse.", true );
					inventoryslots[ activeslot ]->set_highlight( false );
					// Clear the fuse-pair
					activeslot = -1;
				}
			}
			// The clicked slot of the fuse-pair is in the game map grid and the active slot is in the inventory grid
			else if( gamemap.activecol != -1 && gamemap.activerow != -1 ){
				// Make sure the clicked slot is occupied by a focus
				if( slot_is_occupied( clickedslot ) ){
					// Make sure the clicked slot and active slot are fuseable
					if( inventoryslots[ clickedslot ]->rank  == gamemap.towerslots[ gamemap.activecol ][ gamemap.activerow ]->focus->rank &&
						inventoryslots[ clickedslot ]->color == gamemap.towerslots[ gamemap.activecol ][ gamemap.activerow ]->focus->color ){
						// Store the rank and color of the created focus
						int rank  = gamemap.towerslots[ gamemap.activecol ][ gamemap.activerow ]->focus->rank + 1;
						int color = gamemap.towerslots[ gamemap.activecol ][ gamemap.activerow ]->focus->color;
						// Delete the old focuses and create the new fused focus
						delete gamemap.towerslots[ gamemap.activecol ][ gamemap.activerow ]->focus;
						gamemap.towerslots[ gamemap.activecol ][ gamemap.activerow ]->focus = NULL;
						delete inventoryslots[ clickedslot ];
						inventoryslots[ clickedslot ] = new Focus( rank, color );
						// Tell the fuse button a fuse has occured so it can deduct fuse cost, etc
						fusebutton.signal_fuse();
					}
					// The clicked slot and active slots are not fuseable
					else{
						set_message( "Focuses must be same rank and color.", true );
						gamemap.towerslots[ gamemap.activecol ][ gamemap.activerow ]->focus->set_highlight( false );
					}
					// Clear the fuse-pair
					gamemap.activecol = -1;
					gamemap.activerow = -1;
				}
				// The clicked slot does not contain a focus
				else{
					set_message( "Must select two focuses to fuse.", true );
					gamemap.towerslots[ gamemap.activecol ][ gamemap.activerow ]->focus->set_highlight( false );
					// Clear the fuse-pair
					gamemap.activecol = -1;
					gamemap.activerow = -1;
				}
			}
		}
		else{
			// Clicked the first focus of a swap-pair, activating a swap sequence
			if( activeslot == -1 && ( gamemap.activecol == -1 || gamemap.activerow == -1 ) ){
				if( slot_is_occupied( clickedslot ) ){
					idle_active_button();
					activeslot = clickedslot;
					inventoryslots[ activeslot ]->set_highlight( true );
				}
			}
			// Both slots of the swap-pair are contained in the inventory grid
			else if( activeslot != -1 ){
				Focus* temp;
				inventoryslots[ activeslot ]->set_highlight( false );
				temp                          = inventoryslots[ activeslot ];
				inventoryslots[ activeslot ]  = inventoryslots[ clickedslot ];
				inventoryslots[ clickedslot ] = temp;
				activeslot = -1;
			}
			// The clicked slot of the fuse-pair is in the inventory grid and the active slot is in the game map grid
			else if( gamemap.activecol != -1 && gamemap.activerow != -1 ){
				Focus* temp;
				gamemap.towerslots[ gamemap.activecol ][ gamemap.activerow ]->focus->set_highlight( false );
				temp                                                                = gamemap.towerslots[ gamemap.activecol ][ gamemap.activerow ]->focus;
				gamemap.towerslots[ gamemap.activecol ][ gamemap.activerow ]->focus = inventoryslots[ clickedslot ];
				inventoryslots[ clickedslot ]                                       = temp;
				gamemap.towerslots[ gamemap.activecol ][ gamemap.activerow ]->disarm();
				gamemap.activecol = -1;
				gamemap.activerow = -1;
			}
		}
	}
}

void Inventory::unclick( int x, int y ){
	if( state == 1 ){
		// Convert from screen tile coords to inventory grid coords
		int col = ( x - W ) / 2, row = ( y - N ) / 2;
		int clickedslot = col + row * NUM_INV_COLS;

		if( ( activeslot != -1 || ( gamemap.activecol != -1 && gamemap.activerow != -1 ) ) && clickedslot != activeslot ){
			click( x, y );
		}
	}
}

void Inventory::idle(){
	if( activeslot != -1 ){
		inventoryslots[ activeslot ]->set_highlight( false );
		activeslot = -1;
	}
	else if( state == 0 ){
		state = 1;
	}
}

void Inventory::disable(){
	idle();
	state = 0;
}

bool Inventory::slot_is_occupied( int slot ){
	if( inventoryslots[ slot ] != NULL ){
		return true;
	}
	else{
		return false;
	}
}

void Inventory::create_focus( int slot, int rank, int color ){
	delete inventoryslots[ slot ];
	inventoryslots[ slot ] = new Focus( rank, color );
}

void Inventory::clear_focuses(){
	for( int i = 0; i < NUM_INV_SLOTS; i++ ){
		delete inventoryslots[i];
		inventoryslots[i] = NULL;
	}
}

void Inventory::show(){
	// Show the inventory slots
	for( int i = 0; i < NUM_INV_SLOTS; i++ ){
		apply_surface( boxes[i].x, boxes[i].y, inventoryslotsheet, screen );
		if( inventoryslots[i] != NULL ){
			inventoryslots[i]->show( boxes[i] );
		}
	}
}