#ifndef _mob_h_included_
#define _mob_h_included_

#include "SDL\SDL.h"

#define MOB_WIDTH  48
#define MOB_HEIGHT 48

#define MOB_UP    0
#define MOB_RIGHT 1
#define MOB_DOWN  2
#define MOB_LEFT  3
#define MOB_DEATH 4

#define NUM_MOB_MOVE_FRAMES 10

class Mob{
private:
	// The sprite sheet of the mob
	SDL_Surface* spritesheet;
	// The screen coords of the mob
	float x, y;
	// The velocity of the mob
	int xVel, yVel;
	// Current frame of the mob
	int frame;
	// Animation status of the mob
	int status;
	// Milliseconds since the last frame change was made
	int frametimer;
	// The health bar sprite clip
	SDL_Rect* healthbarclip;

public:
	// The basic stats of the mob
	int speed;
	int maxhealth, curhealth;
	float healthpercent;
	int armor;
	int reward, penalty;
	// The tile index of the path tile
	int tile;
	bool slowed;
	bool dead;

	Mob( int spd, int hp, int arm, int rew, int pen, SDL_Surface* sheet );
	~Mob();
	int x_pixel();
	int y_pixel();
	void move( Uint32 deltaTicks );
	void attack( int dmg, float armorpen );
	void kill();
	bool death( Uint32 deltaTicks );
	void show_dead();
	bool slow();
	bool unslow();
	void show();
	void show_health_bar();
};

#endif