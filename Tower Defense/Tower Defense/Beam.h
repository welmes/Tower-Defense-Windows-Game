#ifndef _beam_h_included_
#define _beam_h_included_

#include "SDL\SDL.h"

#define NUM_BEAM_SPARKS 2
#define BEAM_SPARK_W    10
#define BEAM_SPARK_H    10

#define NUM_BEAM_SPLASHES 2
#define BEAM_SPLASH_W     40
#define BEAM_SPLASH_H     40

#define NUM_BEAM_SLOWS 2
#define BEAM_SLOW_W    20
#define BEAM_SLOW_H    20

class Beam{
private:
	int x0, y0;
	int x1, y1;
	int xinc0, xinc1;
	int yinc0, yinc1;
	int denominator;
	int numerator;
	int numadd;
	int numpixels;
	// The current frame of the spark
	int frame;
	int splashframe;
	int slowframe;

public:
	bool armed;

	Beam();
	void set_points( int xbeg, int ybeg, int xend, int yend );
	void set_bounce_points( int xbeg, int ybeg, int xend, int yend );
	void show();
	void show_bounce();
	void show_splash();
	void show_slow();
};

#endif