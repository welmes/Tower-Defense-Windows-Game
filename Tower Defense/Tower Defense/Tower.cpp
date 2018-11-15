#include "Tower.h"
#include "Focus.h"
#include "Mob.h"
#include "Wave.h"
#include "Level.h"
#include "GameVars.h"
#include <string>

extern const int PIXELS_PER_TILE;

extern SDL_Surface* towersheet;
extern SDL_Surface* screen;

extern Level* levelwaves;

extern bool paused;

extern int pathsize;
extern Tile* path;

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL );

extern SDL_Rect towerclips[ NUM_TOWER_TYPES ];

Tower::Tower( int t, int xbase, int ybase ){
	// Towers are created depending on which button is active when they're placed
	// The lowest tower (basic tower) has an index of 7, so subtract 7 from the type
	type  = t - 7;
	focus = NULL;

	if( type == 3 ){
		armorpen = 0.5;
	}
	else{
		armorpen = 1.0;
	}

	clip = &towerclips[ type ];

	// The inputted coords are the upper-left corner of the tower's 2x2 footprint
	x = xbase;
	y = ybase;

	armingtimer = 0;
	beamtimer   = beamtime;

	slowed = false;

	target = NULL;

	// Find which tiles of the path are within range of the tower
	int xtower, ytower;
	int deltax, deltay, distsquared;
	for( int i = pathsize; i > 0; i-- ){
		// The target tile is to the right of the tower, use the right half of the tower to calculate distance
		if( path[ i ].x > xbase ){
			xtower = x + 1;
		}
		// The target tile is to the left of the tower, use the left half of the tower to calculate distance
		else{
			xtower = x;
		}
		// The target tile is below (y coords increase as they go south) the tower, use the lower half of the tower to calculate distance
		if( path[ i ].y > ybase ){
			ytower = y + 1;
		}
		// The target tile is above (y coords increase as they go south) the tower, use the upper half of the tower to calculate distance
		else{
			ytower = y;
		}
		// Find distance squared to avoid having to find the square root
		deltax = xtower - path[ i ].x;
		deltay = ytower - path[ i ].y;
		distsquared = deltax * deltax + deltay * deltay;
		if( distsquared <= ( ranges[ type ] * ranges[ type ] ) ){
			inrangetiles.push_back( i );
		}
	}
}

Tower::~Tower(){
	delete clip;
	delete focus;
	while( !inrangetiles.empty() ){
		inrangetiles.pop_front();
	}
	while( !secondarytargets.empty() ){
		secondarytargets.pop_front();
	}
}

int Tower::x_pixel(){
	return ( 1 + x ) * PIXELS_PER_TILE;
}

int Tower::y_pixel(){
	return ( 4 + y ) * PIXELS_PER_TILE;
}

void Tower::aim( Uint32 deltaTicks ){
	// Unarmed towers do not shoot
	if( focus == NULL ){
		return;
	}

	if( paused == false ){
		armingtimer += (int)deltaTicks;
	}

	// Don't care about the tower's target until it's ready to fire
	if( armingtimer >= focus->speed ){
		// Drop out of range targets
		// If the tower already has a target
		if( target != NULL ){
			// The target was killed by another tower's attack
			if( target->curhealth <= 0 ){
				// Drop target
				target = NULL;
			}
			else{
				bool inrange = false;
				// Make sure the target is still on an in-range tile
				for( std::list<int>::iterator tileptr = inrangetiles.begin(); tileptr != inrangetiles.end(); tileptr++ ){
					// The target's current tile matches an in-range tile
					if( target->tile == *tileptr ){
						inrange = true;
						// Stop checking
						break;
					}
				}
				// The target's current tile did not match any in-range tiles; the target went out of range
				if( inrange == false ){
					// Drop the target
					target = NULL;
				}
			}
		}

		// Acquire target
		// The tower does not have a target, compare every tile to every mob of every wave currently on the game map
		if( target == NULL ){
			// Traverse the spawned waves of the current level
			for( std::list<Wave>::iterator waveptr = levelwaves->activewaves.begin(); waveptr != levelwaves->activewaves.end(); waveptr++ ){
				// Traverse the spawned mobs of each spawned wave
				for( std::list<Mob>::iterator mobptr = waveptr->activemobs.begin(); mobptr != waveptr->activemobs.end(); mobptr++ ){
					// Check each mob to see if they currently occupy an in-range tile
					for( std::list<int>::iterator tileptr = inrangetiles.begin(); tileptr != inrangetiles.end(); tileptr++ ){
						// Tower does not have a target yet
						if( target == NULL ){
							// If the mob is in range
							if( mobptr->tile == *tileptr ){
								// Acquire the mob as a target
								target = &*mobptr;
								break;
							}
						}
						//// The tower already has acquired a target
						//else{
						//	// Prioritize targetting based on lowest health percentage
						//	if( mobptr->healthpercent < target->healthpercent ){
						//		// Acquire the lower health percentage mob as a target
						//		target = &*mobptr;
						//	}
						//}
					}
					if( target != NULL ){
						break;
					}
				}
				if( target!= NULL ){
					break;
				}
			}
		}

		// Finally attack the target
		if( target != NULL ){
			beamtime = focus->speed / 2;
			// Save the coords of the target in case the target is killed
			xtarget = target->x_pixel();
			ytarget = target->y_pixel();
			//beam.set_points( x_pixel(), y_pixel(), xtarget, ytarget );
			//if( armingtimer >= focus->speed ){
			armingtimer -= focus->speed;
			beamtimer = 0;
			target->attack( focus->damage, armorpen );
			switch( type ){
				case 1:
					bounce_attack();
					break;
				case 2:
					splash_attack();
					break;
				case 4:
					apply_slow();
					break;
				default:
					break;
			}
			// The target was killed by the attack
			if( target->curhealth <= 0 ){
				// Drop target
				target = NULL;
			}
			//}
		}
		// Don't allow a tower build up multiple attacks
		if( armingtimer > focus->speed ){
			armingtimer = focus->speed;
		}
	}
	else if( beamtimer < beamtime ){
		if( target != NULL ){
			if( target->curhealth <= 0 ){
				target = NULL;
			}
			else{
				// Save the coords of the target in case the target is killed
				xtarget = target->x_pixel();
				ytarget = target->y_pixel();
			}
		}
	}
	if( type == 1 ){
			bounce_attack_follow();
	}
}

void Tower::bounce_attack(){
	xbounce = -1;
	ybounce = -1;

	// See if there's a secondary target within a pathtile of the target
	// Traverse the spawned waves of the current level
	for( std::list<Wave>::iterator waveptr = levelwaves->activewaves.begin(); waveptr != levelwaves->activewaves.end(); waveptr++ ){
		// Traverse the spawned mobs of each spawned wave
		for( std::list<Mob>::iterator mobptr = waveptr->activemobs.begin(); mobptr != waveptr->activemobs.end(); mobptr++ ){
			// If the mob is in range of the target for a bounce attack
			if( ( mobptr->tile >= target->tile - 2 ) && ( mobptr->tile <= target->tile + 2 ) && ( &*mobptr != target ) ){
				// Acquire the mob as a target
				secondarytargets.push_back( &*mobptr );
				break;
			}
		}
		if( !secondarytargets.empty() ){
			break;
		}
	}

	if( !secondarytargets.empty() ){
		xbounce = secondarytargets.front()->x_pixel();
		ybounce = secondarytargets.front()->y_pixel();
		int bouncedmg = focus->damage / 2;
		if( bouncedmg == 0 ){
			bouncedmg = 1;
		}
		secondarytargets.front()->attack( bouncedmg, armorpen );
		if( secondarytargets.front()->curhealth <= 0 ){
			secondarytargets.pop_front();
		}
	}
}

void Tower::bounce_attack_follow(){
	if( beamtimer < beamtime ){
		if( !secondarytargets.empty() ){
			if( secondarytargets.front()->curhealth <= 0 ){
				secondarytargets.pop_front();
			}
			else{
				xbounce = secondarytargets.front()->x_pixel();
				ybounce = secondarytargets.front()->y_pixel();
			}
		}
	}
	else if( !secondarytargets.empty() ){
		secondarytargets.pop_front();
	}
}

void Tower::splash_attack(){
	// All mobs within range of of the primary targets are secondary targets
	// Traverse the spawned waves of the current level
	for( std::list<Wave>::iterator waveptr = levelwaves->activewaves.begin(); waveptr != levelwaves->activewaves.end(); waveptr++ ){
		// Traverse the spawned mobs of each spawned wave
		for( std::list<Mob>::iterator mobptr = waveptr->activemobs.begin(); mobptr != waveptr->activemobs.end(); mobptr++ ){
			// If the mob is in range of the target for a bounce attack
			if( ( mobptr->tile >= target->tile - 1 ) && ( mobptr->tile <= target->tile + 1 ) && ( &*mobptr != target ) ){
				// Acquire the mob as a target
				secondarytargets.push_back( &*mobptr );
			}
		}
	}

	while( !secondarytargets.empty() ){
		int splashdmg = focus->damage / 4;
		if( splashdmg == 0 ){
			splashdmg = 1;
		}
		secondarytargets.front()->attack( splashdmg, armorpen );
		secondarytargets.pop_front();
	}
}

void Tower::apply_slow(){
	if( slowed == false ){
		if( target->slow() ){
			slowed = true;
		}
	}
}

void Tower::fade_slow(){
	if( slowed == true ){
		if( target->unslow() ){
			slowed = false;
		}
	}
}

void Tower::shoot( Uint32 deltaTicks ){
	if( beamtimer < beamtime ){
		if( paused == false ){
			beamtimer += (int)deltaTicks;
		}
		switch( type ){
			case 1:
				beam.set_points( x_pixel(), y_pixel(), xtarget, ytarget );
				beam.show();
				if( xbounce != -1 && ybounce != -1 ){
					beam.set_bounce_points( xtarget, ytarget, xbounce, ybounce );
					beam.show_bounce();
				}
				break;
			case 2:
				beam.set_points( x_pixel(), y_pixel(), xtarget, ytarget );
				beam.show_splash();
				beam.show();
				break;
			case 3:
				beam.set_points( x_pixel(), y_pixel(), xtarget, ytarget );
				beam.show();
				break;
			case 4:
				beam.set_points( x_pixel(), y_pixel(), xtarget, ytarget );
				beam.show();
				beam.show_slow();
				break;
			default:
				beam.set_points( x_pixel(), y_pixel(), xtarget, ytarget );
				beam.show();
				break;
		}
	}
	else if( type == 4 ){
		if( target != NULL ){
			fade_slow();
		}
		else{
			slowed = false;
		}
	}
}

void Tower::disarm(){
	if( focus == NULL ){
		// Clear target and armingtimer
		target = NULL;
		armingtimer = 0;
	}
}

void Tower::show( SDL_Rect box, Uint32 deltaTicks ){
	// Show the tower
	apply_surface( box.x, box.y, towersheet, screen, clip );
	if( focus != NULL ){
		focus->show( box );
	}
}