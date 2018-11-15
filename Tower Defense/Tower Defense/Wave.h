#ifndef _wave_h_included_
#define _wave_h_included_

#include <list>
#include "Mob.h"

class Wave{
private:
	Uint32 timer;
	int numtotalmobs;
	int numactivemobs;

	void send_mob();

public:
	std::list<int> spawndelays;
	std::list<Mob> incomingmobs;
	std::list<Mob> activemobs;
	std::list<Mob> dieingmobs;

	Wave();
	~Wave();
	void add_mob( Mob mob );
	void add_delay( int delay );
	bool empty();
	int total_mobs();
	void show( Uint32 deltaTicks );
	void show_health_bars();
};

#endif