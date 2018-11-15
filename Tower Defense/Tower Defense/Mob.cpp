#include "Mob.h"
#include "GameMap.h"
#include "GameVars.h"

extern GameMap gamemap;

extern const int PIXELS_PER_TILE;

extern SDL_Surface* screen;
extern SDL_Surface* mobhealthbarsheet;

extern int pathsize;
extern Tile* path;

extern int lives;

extern SDL_Rect clipsMobUp[ NUM_MOB_MOVE_FRAMES ];
extern SDL_Rect clipsMobRight[ NUM_MOB_MOVE_FRAMES ];
extern SDL_Rect clipsMobDown[ NUM_MOB_MOVE_FRAMES ];
extern SDL_Rect clipsMobLeft[ NUM_MOB_MOVE_FRAMES ];
extern SDL_Rect clipsMobDeath[ NUM_MOB_MOVE_FRAMES ];
extern SDL_Rect clipsMobHealthBar[ NUM_HEALTH_BARS ];

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL );
void set_lives( int l );
void defeat();

Mob::Mob( int spd, int hp, int arm, int rew, int pen, SDL_Surface* sheet ){
	speed = spd;
	maxhealth = hp;
	curhealth = maxhealth;
	healthpercent = (float)curhealth / (float)maxhealth;
	armor = arm;
	reward = rew;
	penalty = pen;
	spritesheet = sheet;

	frametimer = 0;

	healthbarclip = &clipsMobHealthBar[ (int)( healthpercent * 10.0 ) ];

	tile = 0;

	dead = false;
	slowed = false;

	// Initialize the offsets
	x = (float)( ( gamemap.W + path[ tile ].x ) * PIXELS_PER_TILE - 12 );
	y = (float)( ( gamemap.N + path[ tile ].y ) * PIXELS_PER_TILE - 18 );

	// Initialize the velocity
	xVel = path[ tile ].dirhor;
	yVel = path[ tile ].dirvert;

	// Initialize animation variables
	frame  = 0;
	status = MOB_RIGHT;
}

Mob::~Mob(){
	//SDL_FreeSurface( spritesheet );
}

int Mob::x_pixel(){
	return (int)x;
}

int Mob::y_pixel(){
	return (int)y;
}

void Mob::move( Uint32 deltaTicks ){
	// Update the frame change timer
	frametimer += (int)deltaTicks;

	// Calculate the path's x and y coords
	float pathx = (float)( ( gamemap.W + path[ tile + 1 ].x ) * PIXELS_PER_TILE - 12 );
	float pathy = (float)( ( gamemap.N + path[ tile + 1 ].y ) * PIXELS_PER_TILE - 18 );

	// Move the mob left or right
	x += (float)( xVel * speed ) * ( (float)deltaTicks / 1000.f );
	// Keep the mob on the path
	if( ( xVel < 0 ) && ( x <= pathx ) ){
		tile++;
		x = pathx;
		xVel = path[ tile ].dirhor;
		yVel = path[ tile ].dirvert;
	}
	else if( ( xVel > 0 ) && ( x >= pathx ) ){
		tile++;
		x = pathx;
		xVel = path[ tile ].dirhor;
		yVel = path[ tile ].dirvert;
	}

	// Move the square up or down
	y += (float)( yVel * speed ) * ( (float)deltaTicks / 1000.f );
	// Keep the mob on the path
	if( ( yVel < 0 ) && ( y <= pathy ) ){
		tile++;
		y = pathy;
		xVel = path[ tile ].dirhor;
		yVel = path[ tile ].dirvert;
	}
	else if( ( yVel > 0 ) && ( y >= pathy ) ){
		tile++;
		y = pathy;
		xVel = path[ tile ].dirhor;
		yVel = path[ tile ].dirvert;
	}

	if( tile >= pathsize - 1 ){
		set_lives( penalty );
		if( lives <= 0 ){
			defeat();
		}
		tile = 0;
		x = (float)( ( gamemap.W + path[ tile ].x ) * PIXELS_PER_TILE - 12 );
		y = (float)( ( gamemap.N + path[ tile ].y ) * PIXELS_PER_TILE - 18 );
	}
}

void Mob::attack( int dmg, float armorpen ){
	int effectivearmor = (int)( (float)armor * armorpen );
	int dmgtaken = dmg - effectivearmor;
	if( dmgtaken > 0 ){
		curhealth -= dmgtaken;
		healthpercent = (float)curhealth / (float)maxhealth;
		healthbarclip = &clipsMobHealthBar[ (int)( healthpercent * 10.0 ) ];
	}
	else{
		curhealth -= 1;
		healthpercent = (float)curhealth / (float)maxhealth;
		healthbarclip = &clipsMobHealthBar[ (int)( healthpercent * 10.0 ) ];
	}
}

void Mob::kill(){
	status = MOB_DEATH;
	frame = 0;
	frametimer = 0;
}

bool Mob::death( Uint32 deltaTicks ){
	frametimer += deltaTicks;
	int framechange = 20;
	// Begin the death animation
	if( frametimer > framechange ){
		// Move to the next frame in the animation
		frame++;
		// Reset the frame change timer
		frametimer -= framechange;
		// Death animation is finished
		if( frame >= NUM_MOB_MOVE_FRAMES ){
			dead = true;
			return true;
		}
	}
	show_dead();

	return false;
}

void Mob::show_dead(){
	// Show the dieing mob
	apply_surface( (int)x, (int)y, spritesheet, screen, &clipsMobDeath[ frame ] );
}

bool Mob::slow(){
	if( slowed == false ){
		slowed = true;
		speed = speed / 5;
		return true;
	}
	return false;
}

bool Mob::unslow(){
	if( slowed == true ){
		slowed = false;
		speed = speed * 5;
		return true;
	}
	return false;
}

void Mob::show(){
	int framechange = 2000 / speed;
	// If Mob is moving left
	if( xVel < 0 ){
		// Set the animation to left
		status = MOB_LEFT;

		if( frametimer > framechange ){
			// Move to the next frame in the animation
			frame++;
			// Reset the frame change timer
			frametimer -= framechange;
		}
	}
	// If Mob is moving right
	else if( xVel > 0 ){
		// Set the animation to right
		status = MOB_RIGHT;

		if( frametimer > framechange ){
			// Move to the next frame in the animation
			frame++;
			// Reset the frame change timer
			frametimer -= framechange;
		}
	}
	// If Mob is moving down
	else if( yVel > 0 ){
		// Set the animation to down
		status = MOB_DOWN;

		// Move to the next frame in the animation
		if( frametimer > framechange ){
			// Move to the next frame in the animation
			frame++;
			// Reset the frame change timer
			frametimer -= framechange;
		}
	}
	// If Mob is moving up
	else if( yVel < 0 ){
		// Set the animation to up
		status = MOB_UP;

		if( frametimer > framechange ){
			// Move to the next frame in the animation
			frame++;
			// Reset the frame change timer
			frametimer -= framechange;
		}
	}
	// If Mob standing
	else{
		// Restart the animation
		frame = 0;
	}

	// Loop the animation
	if( frame >= NUM_MOB_MOVE_FRAMES ){
		frame = 1;
	}

	// Show the mob
	switch( status ){
		case MOB_UP:
			apply_surface( (int)x, (int)y, spritesheet, screen, &clipsMobUp[ frame ] );
			break;
		case MOB_RIGHT:
			apply_surface( (int)x, (int)y, spritesheet, screen, &clipsMobRight[ frame ] );
			break;
		case MOB_DOWN:
			apply_surface( (int)x, (int)y, spritesheet, screen, &clipsMobDown[ frame ] );
			break;
		case MOB_LEFT:
			apply_surface( (int)x, (int)y, spritesheet, screen, &clipsMobLeft[ frame ] );
			break;
	}
}

void Mob::show_health_bar(){
	apply_surface( (int)x + ( MOB_WIDTH - HEALTH_BAR_W ) / 2, (int)y + HEALTH_BAR_H, mobhealthbarsheet, screen, healthbarclip );
}