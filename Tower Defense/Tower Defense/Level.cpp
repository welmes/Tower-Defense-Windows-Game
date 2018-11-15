#include "Level.h"

void set_active_mobs( int active, int total );
void set_incoming_mobs( int incoming );
void set_wave_number( int wave, int total );
void victory();

Level::Level(){
	numtotalmobs    = 0;
	numactivemobs   = 0;
	numincomingmobs = 0;
	set_active_mobs( numactivemobs, numtotalmobs );
	set_incoming_mobs( numincomingmobs );
	wavenum = 0;
	totalwaves = 0;
}

Level::~Level(){
	while( !incomingwaves.empty() ){
		incomingwaves.pop_front();
	}
	while( !activewaves.empty() ){
		activewaves.pop_front();
	}
}

int Level::wave_number(){
	return wavenum;
}

int Level::total_waves(){
	return totalwaves;
}

int Level::total_mobs(){
	return numtotalmobs;
}

void Level::add_wave( Wave wave ){
	// Add a wave to the list of the level's waves
	incomingwaves.push_back( wave );
	totalwaves++;
}

bool Level::send_wave(){
	// Multiple waves need to be able to be spawning concurrently, so store the currently spawning waves in a list
	if( !incomingwaves.empty() ){
		wavenum++;
		set_wave_number( wavenum, totalwaves );
		activewaves.push_back( incomingwaves.front() );
		numtotalmobs += incomingwaves.front().total_mobs();
		numincomingmobs += incomingwaves.front().total_mobs();
		set_active_mobs( numactivemobs, numtotalmobs );
		incomingwaves.pop_front();
		if( !incomingwaves.empty() ){
			return true;
		}
	}
	return false;
}

void Level::show( Uint32 deltaTicks ){
	// Show each active wave's mobs
	for( std::list<Wave>::iterator i = activewaves.begin(); i != activewaves.end(); ){
		std::list<Wave>::iterator curr = i;
		i++;
		if( curr->empty() ){
			numtotalmobs -= curr->total_mobs();
			set_active_mobs( numactivemobs, numtotalmobs );
			activewaves.erase( curr );
			if( activewaves.empty() && incomingwaves.empty() ){
				victory();
			}
		}
		else{
			curr->show( deltaTicks );
		}
	}
}

void Level::show_health_bars(){
	for( std::list<Wave>::iterator i = activewaves.begin(); i != activewaves.end(); i++ ){
		i->show_health_bars();
	}
}