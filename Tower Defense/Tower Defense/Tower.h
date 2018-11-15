#ifndef _tower_h_included_
#define _tower_h_included_

#include "SDL\SDL.h"
#include "Beam.h"
#include <list>

class Focus;
class Mob;

#define NUM_TOWER_TYPES 5

// The tower types in the sprite sheet
#define CLIP_BASIC_TOWER  0
#define CLIP_BOUNCE_TOWER 1
#define CLIP_SPLASH_TOWER 2
#define CLIP_PIERCE_TOWER 3
#define CLIP_SLOW_TOWER   4

const int ranges[ NUM_TOWER_TYPES ] = {
	3, 3, 2, 4, 2
};

class Tower{
private:
	int type;
	int beamtime;
	float armorpen;
	int x, y;
	int xtarget, ytarget;
	int xbounce, ybounce;
	int armingtimer;
	int beamtimer;
	bool slowed;
	std::list<int> inrangetiles;
	// The part of the tower sprite sheet that will be shown
    SDL_Rect* clip;
	// The enemy target of the tower
	Mob* target;
	// The attack beam of the tower
	Beam beam;
	// The targets of the tower's special ability
	std::list<Mob*> secondarytargets;

public:
	Focus* focus;

	Tower( int t, int xbase, int ybase );
	~Tower();
	int x_pixel();
	int y_pixel();
	void aim( Uint32 deltaTicks );
	void shoot( Uint32 deltaTicks );
	void bounce_attack();
	void bounce_attack_follow();
	void splash_attack();
	void apply_slow();
	void fade_slow();
	void disarm();
	void show( SDL_Rect box, Uint32 deltaTicks );
};

#endif