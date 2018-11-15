#include "Wave.h"
#include "Level.h"

extern Level* levelwaves;

extern bool paused;

void set_cash( int c );
void set_active_mobs( int active, int total );
void set_incoming_mobs( int incoming );
void set_score( int points );

Wave::Wave(){
	timer = 0;
	numtotalmobs  = 0;
	numactivemobs = 0;
}

Wave::~Wave(){
	while( !spawndelays.empty() ){
		spawndelays.pop_front();
	}
	while( !incomingmobs.empty() ){
		incomingmobs.pop_front();
	}
	while( !activemobs.empty() ){
		activemobs.pop_front();
	}
	while( !dieingmobs.empty() ){
		dieingmobs.pop_front();
	}
}

void Wave::send_mob(){
	activemobs.push_back( incomingmobs.front() );
	incomingmobs.pop_front();
	levelwaves->numactivemobs++;
	levelwaves->numincomingmobs--;
	// Update the enemies bar
	set_active_mobs( levelwaves->numactivemobs, levelwaves->numtotalmobs );
	set_incoming_mobs( levelwaves->numincomingmobs );
}

void Wave::add_mob( Mob mob ){
	incomingmobs.push_back( mob );
	numtotalmobs++;
	numactivemobs++;
}

void Wave::add_delay( int delay ){
	spawndelays.push_back( delay );
}

bool Wave::empty(){
	if( numactivemobs <= 0 ){
		return true;
	}
	return false;
}

int Wave::total_mobs(){
	return numtotalmobs;
}

void Wave::show( Uint32 deltaTicks ){
	// Game is not paused: move, spawn, and animate mobs as usual
	if( paused == false ){
		// Track the timer for movement, spawn delays, and mob animation
		timer += deltaTicks;
		// Delays each spawn
		if( !spawndelays.empty() ){
			if( (int)timer >= spawndelays.front() ){
				timer -= spawndelays.front();
				spawndelays.pop_front();
				send_mob();
			}
		}
		std::list<Mob>::iterator curr;
		// Move each mob's pixel coords and then animate and show based on movement
		for( std::list<Mob>::iterator i = activemobs.begin(); i != activemobs.end(); ){
			curr = i;
			i++;
			if( curr->curhealth <= 0 ){
				curr->kill();
				dieingmobs.push_back( *curr );
				set_cash( curr->reward );
				set_score( curr->reward * ( 10 + levelwaves->total_mobs() ) );
				activemobs.erase( curr );
				levelwaves->numactivemobs--;
				// Update the enemies bar
				set_active_mobs( levelwaves->numactivemobs, levelwaves->numtotalmobs );
			}
			else{
				curr->move( deltaTicks );
				curr->show();
			}
		}
		// Show mobs in their death animation and delete them when they're done
		for( std::list<Mob>::iterator i = dieingmobs.begin(); i != dieingmobs.end(); ){
			curr = i;
			i++;
			if( curr->dead == true ){
				dieingmobs.erase( curr );
				numactivemobs--;
			}
			else{
				curr->death( deltaTicks );
			}
		}
	}
	// Game is paused: only show mobs without animating them
	else{
		// Only show each mob without changing animation
		for( std::list<Mob>::iterator i = activemobs.begin(); i != activemobs.end(); i++ ){
			i->show();
		}
		// Show mobs in their death animation without animating them
		for( std::list<Mob>::iterator i = dieingmobs.begin(); i != dieingmobs.end(); i++ ){
			i->show_dead();
		}
	}
}

void Wave::show_health_bars(){
	for( std::list<Mob>::iterator i = activemobs.begin(); i != activemobs.end(); i++ ){
		i->show_health_bar();
	}
}