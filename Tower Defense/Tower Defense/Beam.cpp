#include "Beam.h"

extern SDL_Surface* screen;
extern SDL_Surface* beamparticle;
extern SDL_Surface* beamglow;
extern SDL_Surface* beamspark;
extern SDL_Surface* beamsplash;
extern SDL_Surface* beamslow;

extern bool paused;

extern SDL_Rect beamsparkclips[ NUM_BEAM_SPARKS ];
extern SDL_Rect beamsplashclips[ NUM_BEAM_SPLASHES ];
extern SDL_Rect beamslowclips[ NUM_BEAM_SLOWS ];

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL );

Beam::Beam(){
	armed = false;
	frame = 0;
	splashframe = 0;
	slowframe = 0;
}

// The version of the Bresenham line algorithm used in this function and below were take from a gamedev.net article at http://www.gamedev.net/reference/articles/article1275.asp
void Beam::set_points( int xbeg, int ybeg, int xend, int yend ){
	x0 = xbeg + 23;
	y0 = ybeg + 24;
	x1 = xend + 23;
	y1 = yend + 19;

	int deltax = x1 - x0;
	// Abs( deltax )
	if( deltax < 0 ){
		deltax *= -1;
	}

	int deltay = y1 - y0;
	// Abs( deltay )
	if( deltay < 0 ){
		deltay *= -1;
	}

	if( x1 >= x0 ){
		xinc0 = 1;
		xinc1 = 1;
	}
	else{
		xinc0 = -1;
		xinc1 = -1;
	}

	if( y1 >= y0 ){
		yinc0 = 1;
		yinc1 = 1;
	}
	else{
		yinc0 = -1;
		yinc1 = -1;
	}

	if( deltax >= deltay ){
		xinc0 = 0;
		yinc1 = 0;
		denominator = deltax;
		numerator = deltax / 2;
		numadd = deltay;
		numpixels = deltax;
	}
	else{
		xinc1 = 0;
		yinc0 = 0;
		denominator = deltay;
		numerator = deltay / 2;
		numadd = deltax;
		numpixels = deltay;
	}

	// Ready to fire
	armed = true;
}

void Beam::set_bounce_points( int xbeg, int ybeg, int xend, int yend ){
	x0 = xbeg + 23;
	y0 = ybeg + 19;
	x1 = xend + 23;
	y1 = yend + 19;

	int deltax = x1 - x0;
	// Abs( deltax )
	if( deltax < 0 ){
		deltax *= -1;
	}

	int deltay = y1 - y0;
	// Abs( deltay )
	if( deltay < 0 ){
		deltay *= -1;
	}

	if( x1 >= x0 ){
		xinc0 = 1;
		xinc1 = 1;
	}
	else{
		xinc0 = -1;
		xinc1 = -1;
	}

	if( y1 >= y0 ){
		yinc0 = 1;
		yinc1 = 1;
	}
	else{
		yinc0 = -1;
		yinc1 = -1;
	}

	if( deltax >= deltay ){
		xinc0 = 0;
		yinc1 = 0;
		denominator = deltax;
		numerator = deltax / 2;
		numadd = deltay;
		numpixels = deltax;
	}
	else{
		xinc1 = 0;
		yinc0 = 0;
		denominator = deltay;
		numerator = deltay / 2;
		numadd = deltax;
		numpixels = deltay;
	}

	// Ready to fire
	armed = true;
}

void Beam::show(){
	// Requires the points to be set before every firing
	if( armed == true ){
		int x = x0, y = y0;
		apply_surface( x - 23, y - 24, beamglow, screen );
		apply_surface( x - 4, y - 4, beamspark, screen, &beamsparkclips[ frame ] );
		for( int curpixel = 0; curpixel <= numpixels; curpixel++ ){
			apply_surface( x, y, beamparticle, screen );
			numerator += numadd;
			if( numerator >= denominator ){
				numerator -= denominator;
				x += xinc0;
				y += yinc0;
			}
			x += xinc1;
			y += yinc1;
		}
		apply_surface( x - 4, y - 4, beamspark, screen, &beamsparkclips[ frame ] );
		if( paused == false ){
			frame = ( frame + 1 ) % NUM_BEAM_SPARKS;
		}

		// Points must be set again to be fired
		armed = false;
	}
}

void Beam::show_bounce(){
	// Requires the points to be set before every firing
	if( armed == true ){
		int x = x0, y = y0;
		for( int curpixel = 0; curpixel <= numpixels; curpixel++ ){
			apply_surface( x, y, beamparticle, screen );
			numerator += numadd;
			if( numerator >= denominator ){
				numerator -= denominator;
				x += xinc0;
				y += yinc0;
			}
			x += xinc1;
			y += yinc1;
		}
		apply_surface( x - 4, y - 4, beamspark, screen, &beamsparkclips[ frame ] );

		// Points must be set again to be fired
		armed = false;
	}
}

void Beam::show_splash(){
	// Requires the points to be set before every firing
	if( armed == true ){
		apply_surface( x1 - 19, y1 - 19, beamsplash, screen, &beamsplashclips[ splashframe ] );
		if( paused == false ){
			splashframe = ( splashframe + 1 ) % NUM_BEAM_SPLASHES;
		}
	}
}

void Beam::show_slow(){
	apply_surface( x1 - 9, y1 - 9, beamslow, screen, &beamslowclips[ slowframe ] );
	if( paused == false ){
		slowframe = ( slowframe + 1 ) % NUM_BEAM_SLOWS;
	}
}