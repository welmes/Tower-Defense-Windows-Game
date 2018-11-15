#ifndef _level_h_included_
#define _level_h_included_

#include <list>
#include "Wave.h"

class Level{
private:
	int numtotalmobs;
	int numactivemobs;
	int numincomingmobs;
	int wavenum;
	int totalwaves;

public:
	std::list<Wave> incomingwaves;
	std::list<Wave> activewaves;

	Level();
	~Level();
	int wave_number();
	int total_waves();
	int total_mobs();
	void add_wave( Wave wave );
	bool send_wave();
	void show( Uint32 deltaTicks );
	void show_health_bars();

	friend class Wave;
};

#endif