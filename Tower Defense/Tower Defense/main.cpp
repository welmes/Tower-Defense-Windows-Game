// William Elmes
// welmes@csu.fullerton.edu
// 12/6/2010

#include <string>
#include <list>
#include <sstream>
#include <fstream>
#include "SDL\SDL.h"
#include "SDL\SDL_image.h"
#include "SDL\SDL_ttf.h"

#include "GameVars.h"
#include "Timer.h"
#include "CreateButton.h"
#include "RankButton.h"
#include "FuseButton.h"
#include "CancelButton.h"
#include "BasicTowerButton.h"
#include "BounceTowerButton.h"
#include "SplashTowerButton.h"
#include "PierceTowerButton.h"
#include "SlowTowerButton.h"
#include "PauseButton.h"
#include "SkipButton.h"
#include "Focus.h"
#include "Inventory.h"
#include "Tower.h"
#include "GameMap.h"
#include "Mob.h"
#include "Wave.h"
#include "Level.h"

extern const int SCREEN_WIDTH_IN_TILES;
extern const int SCREEN_HEIGHT_IN_TILES;
extern const int SCREEN_BPP;
extern const int PIXELS_PER_TILE;
extern const int FRAMES_PER_SECOND;

SDL_Surface* createbuttonsheet   = NULL;
SDL_Surface* rankbuttonsheet     = NULL;
SDL_Surface* fusebuttonsheet     = NULL;
SDL_Surface* cancelbuttonsheet   = NULL;
SDL_Surface* basicbuttonsheet    = NULL;
SDL_Surface* bouncebuttonsheet   = NULL;
SDL_Surface* splashbuttonsheet   = NULL;
SDL_Surface* piercebuttonsheet   = NULL;
SDL_Surface* slowbuttonsheet     = NULL;
SDL_Surface* pausebuttonsheet    = NULL;
SDL_Surface* skipbuttonsheet     = NULL;
SDL_Surface* inventoryslotsheet  = NULL;
SDL_Surface* focusessheet        = NULL;
SDL_Surface* towersheet          = NULL;
SDL_Surface* mapslotsheet        = NULL;
SDL_Surface* message             = NULL;
SDL_Surface* cashicon            = NULL;
SDL_Surface* livesicon           = NULL;
SDL_Surface* cashbartext         = NULL;
SDL_Surface* livesbartext        = NULL;
SDL_Surface* scorebartext        = NULL;
SDL_Surface* enemiestitletext    = NULL;
SDL_Surface* enemiesactivetext   = NULL;
SDL_Surface* enemiesincomingtext = NULL;
SDL_Surface* wavetext            = NULL;
SDL_Surface* leveltext           = NULL;
SDL_Surface* marinesheet         = NULL;
SDL_Surface* mobhealthbarsheet   = NULL;
SDL_Surface* beamparticle        = NULL;
SDL_Surface* beamglow            = NULL;
SDL_Surface* beamspark           = NULL;
SDL_Surface* beamsplash          = NULL;
SDL_Surface* beamslow            = NULL;
SDL_Surface* tooltipssheet       = NULL;
SDL_Surface* endscreensheet      = NULL;
SDL_Surface* screen              = NULL;

// The input event
SDL_Event event;

// The text font
TTF_Font *font = NULL;
// The color of the font
SDL_Color textcolor = { 0x00, 0x00, 0x00 };

int cash  = STARTING_CASH;
int lives = STARTING_LIVES;
int score = 0;

std::string messagetext = "";

CreateButton createbutton;
FuseButton fusebutton;
CancelButton cancelbutton;
BasicTowerButton basicbutton;
BounceTowerButton bouncebutton;
SplashTowerButton splashbutton;
PierceTowerButton piercebutton;
SlowTowerButton slowbutton;
PauseButton pausebutton;
SkipButton skipbutton;

int tooltip = 0;
SDL_Rect tooltipclip;

Inventory inventory;
GameMap gamemap;
Level* levelwaves;

int activebutton = -1;
bool paused = false;

int pathsize;
Tile* path;

int curLevel = 1;

int endgame = 0;
SDL_Rect clipendscreen;

// The areas of the Mob sprite
SDL_Rect clipsMobUp[ NUM_MOB_MOVE_FRAMES ];
SDL_Rect clipsMobRight[ NUM_MOB_MOVE_FRAMES ];
SDL_Rect clipsMobDown[ NUM_MOB_MOVE_FRAMES ];
SDL_Rect clipsMobLeft[ NUM_MOB_MOVE_FRAMES ];
SDL_Rect clipsMobDeath[ NUM_MOB_MOVE_FRAMES ];

SDL_Rect clipsMobHealthBar[ NUM_HEALTH_BARS ];

// The areas of the Tower sprite
SDL_Rect towerclips[ NUM_TOWER_TYPES ];

// The impact effect of tower attack beams
SDL_Rect beamsparkclips[ NUM_BEAM_SPARKS ];
SDL_Rect beamsplashclips[ NUM_BEAM_SPLASHES ];
SDL_Rect beamslowclips[ NUM_BEAM_SLOWS ];

void set_cash( int c );
void set_lives( int l );
void set_message( std::string m, bool enabled );
void set_level_number( int level );
void set_wave_number( int wave, int total );
void idle_active_button();
void reset_score();

// The design of this function was taken from a lazyfoo.net tutorial at http://lazyfoo.net/SDL_tutorials/index.php
SDL_Surface *load_image( std::string filename )
{
    // The image that's loaded
    SDL_Surface* loadedImage = NULL;

    // The optimized surface that will be used
    SDL_Surface* optimizedImage = NULL;

    // Load the image
    loadedImage = IMG_Load( filename.c_str() );

    // If the image loaded
    if( loadedImage != NULL )
    {
        // Create an optimized surface
        optimizedImage = SDL_DisplayFormat( loadedImage );

        // Free the old surface
        SDL_FreeSurface( loadedImage );

        // If the surface was optimized
        if( optimizedImage != NULL )
        {
            // Color key surface
            SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, SDL_MapRGB( optimizedImage->format, 0, 0xFF, 0xFF ) );
        }
    }

    // Return the optimized surface
    return optimizedImage;
}

// The design of this function was taken from a lazyfoo.net tutorial at http://lazyfoo.net/SDL_tutorials/index.php
void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL )
{
    // Holds offsets
    SDL_Rect offset;

    // Get offsets
    offset.x = x;
    offset.y = y;

    // Blit
    SDL_BlitSurface( source, clip, destination, &offset );
}

// The design of this function was taken from a lazyfoo.net tutorial at http://lazyfoo.net/SDL_tutorials/index.php
bool init(){
	// Initialize all SDL subsystems
	if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 ){
		return false;
	}

	// Set up the screen
	screen = SDL_SetVideoMode( SCREEN_WIDTH_IN_TILES * PIXELS_PER_TILE, SCREEN_HEIGHT_IN_TILES * PIXELS_PER_TILE, SCREEN_BPP, SDL_SWSURFACE );

	// If there was an error in setting up the screen
	if( screen == NULL ){
		return false;
	}

	// Initialize SDL_ttf
	if( TTF_Init() == -1 ){
		return false;
	}

	// Set the window caption
	SDL_WM_SetCaption( "Tower Defense", NULL );

	// If everything initialized fine
	return true;
}

// The design of this function was taken from a lazyfoo.net tutorial at http://lazyfoo.net/SDL_tutorials/index.php
void set_clips(){
	// Clip the Mob sprites
	// Sprite sheet has positions from left to right as: Up, Right, Down, Left
	int x = 0;
	int y = 0;
	for( int i = 0; i < NUM_MOB_MOVE_FRAMES; i++ ){
		clipsMobUp[ i ].x = x;
		clipsMobUp[ i ].y = y;
		clipsMobUp[ i ].w = MOB_WIDTH;
		clipsMobUp[ i ].h = MOB_HEIGHT;
		y += MOB_HEIGHT;
	}

	x =	MOB_WIDTH;
	y = 0;
	for( int i = 0; i < NUM_MOB_MOVE_FRAMES; i++ ){
		clipsMobRight[ i ].x = x;
		clipsMobRight[ i ].y = y;
		clipsMobRight[ i ].w = MOB_WIDTH;
		clipsMobRight[ i ].h = MOB_HEIGHT;
		y += MOB_HEIGHT;
	}

	x =	MOB_WIDTH * 2;
	y = 0;
	for( int i = 0; i < NUM_MOB_MOVE_FRAMES; i++ ){
		clipsMobDown[ i ].x = x;
		clipsMobDown[ i ].y = y;
		clipsMobDown[ i ].w = MOB_WIDTH;
		clipsMobDown[ i ].h = MOB_HEIGHT;
		y += MOB_HEIGHT;
	}

	x =	MOB_WIDTH * 3;
	y = 0;
	for( int i = 0; i < NUM_MOB_MOVE_FRAMES; i++ ){
		clipsMobLeft[ i ].x = x;
		clipsMobLeft[ i ].y = y;
		clipsMobLeft[ i ].w = MOB_WIDTH;
		clipsMobLeft[ i ].h = MOB_HEIGHT;
		y += MOB_HEIGHT;
	}

	x =	MOB_WIDTH * 4;
	y = 0;
	for( int i = 0; i < NUM_MOB_MOVE_FRAMES; i++ ){
		clipsMobDeath[ i ].x = x;
		clipsMobDeath[ i ].y = y;
		clipsMobDeath[ i ].w = MOB_WIDTH;
		clipsMobDeath[ i ].h = MOB_HEIGHT;
		y += MOB_HEIGHT;
	}

	x = 0;
	y = 0;
	for( int i = 0; i < NUM_HEALTH_BARS; i++ ){
		clipsMobHealthBar[ i ].x = 0;
		clipsMobHealthBar[ i ].y = i * HEALTH_BAR_H;
		clipsMobHealthBar[ i ].w = HEALTH_BAR_W;
		clipsMobHealthBar[ i ].h = HEALTH_BAR_H;
	}

	// Clip the tower sprite sheet
	int w = 2 * PIXELS_PER_TILE;
	int h = 2 * PIXELS_PER_TILE;

	towerclips[ CLIP_BASIC_TOWER ].x  = 0;
	towerclips[ CLIP_BASIC_TOWER ].y  = 0;
	towerclips[ CLIP_BASIC_TOWER ].w  = w;
	towerclips[ CLIP_BASIC_TOWER ].h  = h;

	towerclips[ CLIP_BOUNCE_TOWER ].x = w;
	towerclips[ CLIP_BOUNCE_TOWER ].y = 0;
	towerclips[ CLIP_BOUNCE_TOWER ].w = w;
	towerclips[ CLIP_BOUNCE_TOWER ].h = h;

	towerclips[ CLIP_SPLASH_TOWER ].x = w * 2;
	towerclips[ CLIP_SPLASH_TOWER ].y = 0;
	towerclips[ CLIP_SPLASH_TOWER ].w = w;
	towerclips[ CLIP_SPLASH_TOWER ].h = h;

	towerclips[ CLIP_PIERCE_TOWER ].x = w * 3;
	towerclips[ CLIP_PIERCE_TOWER ].y = 0;
	towerclips[ CLIP_PIERCE_TOWER ].w = w;
	towerclips[ CLIP_PIERCE_TOWER ].h = h;

	towerclips[ CLIP_SLOW_TOWER ].x   = w * 4;
	towerclips[ CLIP_SLOW_TOWER ].y   = 0;
	towerclips[ CLIP_SLOW_TOWER ].w   = w;
	towerclips[ CLIP_SLOW_TOWER ].h   = h;

	// Clip the tower impact effect sprite
	w = BEAM_SPARK_W;
	h = BEAM_SPARK_H;
	for( int i = 0; i < NUM_BEAM_SPARKS; i++ ){
		beamsparkclips[ i ].x = i * w;
		beamsparkclips[ i ].y = 0;
		beamsparkclips[ i ].w = w;
		beamsparkclips[ i ].h = h;
	}

	w = BEAM_SPLASH_W;
	h = BEAM_SPLASH_H;
	for( int i = 0; i < NUM_BEAM_SPLASHES; i++ ){
		beamsplashclips[ i ].x = i * w;
		beamsplashclips[ i ].y = 0;
		beamsplashclips[ i ].w = w;
		beamsplashclips[ i ].h = h;
	}

	w = BEAM_SLOW_W;
	h = BEAM_SLOW_H;
	for( int i = 0; i < NUM_BEAM_SLOWS; i++ ){
		beamslowclips[ i ].x = i * w;
		beamslowclips[ i ].y = 0;
		beamslowclips[ i ].w = w;
		beamslowclips[ i ].h = h;
	}
}

bool load_level( int level ){
	idle_active_button();
	// Clear messages
	set_message( "", true );
	// Delete the old level's waves
	delete levelwaves;
	levelwaves = NULL;
	// Delete the old level's path
	delete path;
	path = NULL;
	pathsize = 0;
	// Reset score
	reset_score();
	// Reset cash and lives
	cash  = STARTING_CASH;
	lives = STARTING_LIVES;
	set_cash( 0 );
	set_lives( 0 );
	// Make sure the buttons are properly lit up based on cash
	createbutton.cash_increased();
	fusebutton.cash_increased();
	basicbutton.cash_increased();
	bouncebutton.cash_increased();
	splashbutton.cash_increased();
	piercebutton.cash_increased();
	slowbutton.cash_increased();
	// Also make sure the buttons are properly greyed out based on cash
	createbutton.cash_decreased();
	fusebutton.cash_decreased();
	basicbutton.cash_decreased();
	bouncebutton.cash_decreased();
	splashbutton.cash_decreased();
	piercebutton.cash_decreased();
	slowbutton.cash_decreased();
	// Make the the skip button is enabled again
	skipbutton.idle();
	// Reset the old level's game map
	gamemap.reset_map();
	// Clear the old level's inventory
	inventory.clear_focuses();
	// Convert from an integer level number to a string level file name
	std::string levelfilename;
	std::stringstream namestream;
	namestream << level;
	levelfilename.append( "Level" );
	levelfilename.append( namestream.str() );
	levelfilename.append( ".txt" );
	// Open the level's text file
	std::fstream levelfile( levelfilename.c_str() );

	std::list<int> pathdata;
	int x, y, type, s, h, a, r, p, d;
	char sprite;
	Mob* mob;
	Wave* wave;
	SDL_Surface* sheet;
	if( levelfile.good() ){
		// Get the path from the file
		while( levelfile.good() ){
			levelfile >> x;
			// -1 marks the end of the level's tile elements
			if( x == -1 ){
				break;
			}
			levelfile >> y;
			levelfile >> type;
			// Store the data for each path tile in a list;
			pathdata.push_back( x );
			pathdata.push_back( y );
			// Display the new tile on the game map
			gamemap.set_tile_type( x, y, type );
			// Increment the number of path tiles
			pathsize++;
		}
		// Load the blocked game map tiles
		while( levelfile.good() ){
			levelfile >> x;
			// -1 marks the end of the level's tile elements
			if( x == -1 ){
				break;
			}
			levelfile >> y;
			levelfile >> type;
			// Display the new tile on the game map
			gamemap.set_tile_type( x, y, type );
		}

		// Create a new path
		path = new Tile[ pathsize ];
		// Initialize the tiles of the new path with the path data that was pushed onto the list
		for( int i = 0; i < pathsize; i++ ){
			// Get the path data one pair of coords at a time
			path[ i ].x = pathdata.front();
			pathdata.pop_front();
			path[ i ].y = pathdata.front();
			pathdata.pop_front();
			// Find the direction to the current tile based on the last tile
			if( i > 0 ){
				if( path[ i ].x > path[ i - 1 ].x ){
					path[ i - 1 ].dirhor = PATH_DIR_RIGHT;
				}
				else if( path[ i ].x < path[ i - 1 ].x ){
					path[ i - 1 ].dirhor = PATH_DIR_LEFT;
				}
				else{
					path[ i - 1 ].dirhor = 0;
				}

				if( path[ i ].y > path[ i - 1 ].y ){
					path[ i - 1 ].dirvert = PATH_DIR_UP;
				}
				else if( path[ i ].y < path[ i - 1 ].y ){
					path[ i - 1 ].dirvert = PATH_DIR_DOWN;
				}
				else{
					path[ i - 1 ].dirvert = 0;
				}
			}
			// Manually plug in the last tile's direction since it has no next tile
			if( i == pathsize - 1 ){
				path[ i ].dirhor = PATH_DIR_RIGHT;
				path[ i ].dirvert = 0;
			}
		}

		// Make a new set of waves for the level
		levelwaves = new Level;
		// Get the waves from the file
		while( levelfile.good() ){
			// Create a new wave and add the new mobs to it
			wave = new Wave;
			// Get each wave one at a time until -1 is reached for each wave
			while( levelfile.good() ){
				// Get the mob's variables from the file
				levelfile >> s;
				// -1 indicates end of wave
				if( s == -1 ){
					break;
				}
				levelfile >> h;
				levelfile >> a;
				levelfile >> r;
				levelfile >> p;
				levelfile >> sprite;
				levelfile >> d;
				// Determine which sprite sheet this mob will use
				switch( sprite ){
					case 'm':
						sheet = marinesheet;
						break;
					default:
						// Couldn't identify the sprite sheet
						return false;
				}
			
				// Create a new mob with the input variables
				mob = new Mob( s, h, a, r, p, sheet );
				// Add the new mob and spawn delay to the wave
				wave->add_mob( *mob );
				wave->add_delay( d );
				// Clear the mob for the next one
				delete mob;
				mob = NULL;
			}
			// Add the new wave to the level
			levelwaves->add_wave( *wave );
			// Clear the wave for thenext one
			delete wave;
			wave = NULL;
		}
		// Done loading from the file
		levelfile.close();
	}
	// The level file could not be opened, critical error
	else{
		return false;
	}
	// Set the level and wave number
	set_level_number( level );
	set_wave_number( 0, levelwaves->total_waves() );
	// Level was loaded successfully
	return true;
}

// The design of this function was taken from a lazyfoo.net tutorial at http://lazyfoo.net/SDL_tutorials/index.php
bool load_files(){
	// Load the button sprite sheet
	createbuttonsheet = load_image( "CreateFocusesButton.png" );
	// If there was a problem in loading the button sprite sheet
	if( createbuttonsheet == NULL ){
		return false;
	}

	// Load the button sprite sheet
	rankbuttonsheet = load_image( "RankButton.png" );
	// If there was a problem in loading the button sprite sheet
	if( rankbuttonsheet == NULL ){
		return false;
	}

	// Load the button sprite sheet
	fusebuttonsheet = load_image( "FuseFocusesButton.png" );
	// If there was a problem in loading the button sprite sheet
	if( fusebuttonsheet == NULL ){
		return false;
	}

	// Load the button sprite sheet
	cancelbuttonsheet = load_image( "CancelButton.png" );
	// If there was a problem in loading the button sprite sheet
	if( cancelbuttonsheet == NULL ){
		return false;
	}

	// Load the button sprite sheet
	basicbuttonsheet = load_image( "BasicTowerButton.png" );
	// If there was a problem in loading the button sprite sheet
	if( basicbuttonsheet == NULL ){
		return false;
	}

	// Load the button sprite sheet
	bouncebuttonsheet = load_image( "BounceTowerButton.png" );
	// If there was a problem in loading the button sprite sheet
	if( bouncebuttonsheet == NULL ){
		return false;
	}

	// Load the button sprite sheet
	splashbuttonsheet = load_image( "SplashTowerButton.png" );
	// If there was a problem in loading the button sprite sheet
	if( splashbuttonsheet == NULL ){
		return false;
	}

	// Load the button sprite sheet
	piercebuttonsheet = load_image( "PierceTowerButton.png" );
	// If there was a problem in loading the button sprite sheet
	if( piercebuttonsheet == NULL ){
		return false;
	}

	// Load the button sprite sheet
	slowbuttonsheet = load_image( "SlowTowerButton.png" );
	// If there was a problem in loading the button sprite sheet
	if( slowbuttonsheet == NULL ){
		return false;
	}

	// Load the button sprite sheet
	pausebuttonsheet = load_image( "PauseButton.png" );
	// If there was a problem in loading the button sprite sheet
	if( pausebuttonsheet == NULL ){
		return false;
	}

	// Load the button sprite sheet
	skipbuttonsheet = load_image( "SkipButton.png" );
	// If there was a problem in loading the button sprite sheet
	if( skipbuttonsheet == NULL ){
		return false;
	}

	// Load the sprite sheet
	inventoryslotsheet = load_image( "InventorySlot.png" );
	// If there was a problem in loading the sprite sheet
	if( inventoryslotsheet == NULL ){
		return false;
	}

	// Load the sprite sheet
	focusessheet = load_image( "Focuses.png" );
	// If there was a problem in loading the sprite sheet
	if( focusessheet == NULL ){
		return false;
	}

	// Load the sprite sheet
	towersheet = load_image( "Towers.png" );
	// If there was a problem in loading the sprite sheet
	if( towersheet == NULL ){
		return false;
	}

	// Load the sprite sheet
	mapslotsheet = load_image( "MapSlots.png" );
	// If there was a problem in loading the sprite sheet
	if( mapslotsheet == NULL ){
		return false;
	}

	// Load the icon sprite sheet
	cashicon = load_image( "CashIcon.png" );
	// If there was a problem in loading the icon sprite sheet
	if( cashicon == NULL ){
		return false;
	}

	// Load the icon sprite sheet
	livesicon = load_image( "LivesIcon.png" );
	// If there was a problem in loading the icon sprite sheet
	if( livesicon == NULL ){
		return false;
	}

	// Load the sprite sheet
	marinesheet = load_image( "MarineSpriteSheet.png" );
	// If there was a problem in loading the sprite sheet
	if( marinesheet == NULL ){
		return false;
	}

	// Load the sprite sheet
	mobhealthbarsheet = load_image( "HealthBar.png" );
	// If there was a problem in loading the sprite sheet
	if( mobhealthbarsheet == NULL ){
		return false;
	}

	// Load the sprite sheet
	beamparticle = load_image( "BeamParticle.png" );
	// If there was a problem in loading the sprite sheet
	if( beamparticle == NULL ){
		return false;
	}

	// Load the sprite sheet
	beamglow = load_image( "BeamGlow.png" );
	// If there was a problem in loading the sprite sheet
	if( beamglow == NULL ){
		return false;
	}

	// Load the sprite sheet
	beamspark = load_image( "BeamSpark.png" );
	// If there was a problem in loading the sprite sheet
	if( beamspark == NULL ){
		return false;
	}

	// Load the sprite sheet
	beamsplash = load_image( "BeamSplash.png" );
	// If there was a problem in loading the sprite sheet
	if( beamsplash == NULL ){
		return false;
	}

	// Load the sprite sheet
	beamslow = load_image( "BeamSlow.png" );
	// If there was a problem in loading the sprite sheet
	if( beamslow == NULL ){
		return false;
	}

	// Load the sprite sheet
	tooltipssheet = load_image( "Tooltips.png" );
	// If there was a problem in loading the sprite sheet
	if( tooltipssheet == NULL ){
		return false;
	}

	// Load the sprite sheet
	endscreensheet = load_image( "EndScreen.png" );
	// If there was a problem in loading the sprite sheet
	if( endscreensheet == NULL ){
		return false;
	}

	// Open the font
	font = TTF_OpenFont( "lazy.ttf", 16 );
	// If there was an error in loading the font
	if( font == NULL ){
		return false;
	}

	// If everything loaded fine
	return true;
}

// The design of this function was taken from a lazyfoo.net tutorial at http://lazyfoo.net/SDL_tutorials/index.php
void clean_up(){
	// Delete the dynamically allocated path elements of the game map
	delete [] path;

	// Free the surfaces
	SDL_FreeSurface( createbuttonsheet );
	SDL_FreeSurface( rankbuttonsheet );
	SDL_FreeSurface( fusebuttonsheet );
	SDL_FreeSurface( cancelbuttonsheet );
	SDL_FreeSurface( basicbuttonsheet );
	SDL_FreeSurface( bouncebuttonsheet );
	SDL_FreeSurface( splashbuttonsheet );
	SDL_FreeSurface( piercebuttonsheet );
	SDL_FreeSurface( slowbuttonsheet );
	SDL_FreeSurface( pausebuttonsheet );
	SDL_FreeSurface( skipbuttonsheet );
	SDL_FreeSurface( inventoryslotsheet );
	SDL_FreeSurface( focusessheet );
	SDL_FreeSurface( towersheet );
	SDL_FreeSurface( mapslotsheet );
	SDL_FreeSurface( marinesheet );
	SDL_FreeSurface( mobhealthbarsheet );
	SDL_FreeSurface( beamparticle );
	SDL_FreeSurface( beamglow );
	SDL_FreeSurface( beamspark );
	SDL_FreeSurface( beamsplash );
	SDL_FreeSurface( beamslow );
	SDL_FreeSurface( message );
	SDL_FreeSurface( cashicon );
	SDL_FreeSurface( livesicon );
	SDL_FreeSurface( cashbartext );
	SDL_FreeSurface( livesbartext );
	SDL_FreeSurface( scorebartext );
	SDL_FreeSurface( enemiestitletext );
	SDL_FreeSurface( enemiesactivetext );
	SDL_FreeSurface( enemiesincomingtext );
	SDL_FreeSurface( wavetext );
	SDL_FreeSurface( leveltext );
	SDL_FreeSurface( tooltipssheet );
	SDL_FreeSurface( endscreensheet );

	// Close the font that was used
	TTF_CloseFont( font );

	// Quit SDL_ttf
	TTF_Quit();

	// Quit SDL
	SDL_Quit();
}

void idle_focuses(){
	inventory.idle();
	gamemap.idle();
}

void idle_active_button(){
	if( activebutton != -1 ){
		switch( activebutton ){
			case CREATE_BUTTON:
				createbutton.idle();
				break;
			case FUSE_BUTTON:
				fusebutton.idle();
				idle_focuses();
				break;
			case BASIC_BUTTON:
				basicbutton.idle();
				break;
			case BOUNCE_BUTTON:
				bouncebutton.idle();
				break;
			case SPLASH_BUTTON:
				splashbutton.idle();
				break;
			case PIERCE_BUTTON:
				piercebutton.idle();
				break;
			case SLOW_BUTTON:
				slowbutton.idle();
				break;
			case SKIP_BUTTON:
				skipbutton.idle();
				break;
			default:
				break;
		}
	}
	activebutton = -1;
}

int find_field( int x, int y ){
	if( x >= createbutton.W && x < createbutton.E && y >= createbutton.N && y < createbutton.S ){
		return CREATE_BUTTON;
	}

	for( int c = 0; c < NUM_RANK_BUTTONS; c++ ){
		if( x >= createbutton.subbuttons[c]->W && x < createbutton.subbuttons[c]->E && y >= createbutton.subbuttons[c]->N && y < createbutton.subbuttons[c]->S ){
			return CREATE_BUTTON + c + 1;
		}
	}

	if( x >= cancelbutton.W && x < cancelbutton.E && y >= cancelbutton.N && y < cancelbutton.S ){
		return CANCEL_BUTTON;
	}
	
	if( x >= fusebutton.W && x < fusebutton.E && y >= fusebutton.N && y < fusebutton.S ){
		return FUSE_BUTTON;
	}

	if( x >= pausebutton.W && x < pausebutton.E && y >= pausebutton.N && y < pausebutton.S ){
		return PAUSE_BUTTON;
	}

	if( x >= skipbutton.W && x < skipbutton.E && y >= skipbutton.N && y < skipbutton.S ){
		return SKIP_BUTTON;
	}

	if( x >= basicbutton.W && x < basicbutton.E && y >= basicbutton.N && y < basicbutton.S ){
		return BASIC_BUTTON;
	}

	if( x >= bouncebutton.W && x < bouncebutton.E && y >= bouncebutton.N && y < bouncebutton.S ){
		return BOUNCE_BUTTON;
	}

	if( x >= splashbutton.W && x < splashbutton.E && y >= splashbutton.N && y < splashbutton.S ){
		return SPLASH_BUTTON;
	}

	if( x >= piercebutton.W && x < piercebutton.E && y >= piercebutton.N && y < piercebutton.S ){
		return PIERCE_BUTTON;
	}

	if( x >= slowbutton.W && x < slowbutton.E && y >= slowbutton.N && y < slowbutton.S ){
		return SLOW_BUTTON;
	}

	if( x >= inventory.W && x < inventory.E && y >= inventory.N && y < inventory.S ){
		return INVENTORY_GRID;
	}

	if( x >= gamemap.W && x < gamemap.E && y >= gamemap.N && y < gamemap.S ){
		return GAME_MAP_GRID;
	}

	return -1;
}

void handle_input(){
	// The mouse offsets
	int x = 0, y = 0, i = -1;

	// If a mouse button was pressed
	if( event.type == SDL_MOUSEBUTTONDOWN ){
		// If the left mouse button was pressed
		if( event.button.button == SDL_BUTTON_LEFT ){
			// Get the mouse offsets
			x = (int)( (float)event.button.x / (float)PIXELS_PER_TILE );
			y = (int)( (float)event.button.y / (float)PIXELS_PER_TILE );

			i = find_field( x, y );
			switch( i ){
				case CREATE_BUTTON:
					createbutton.click();
					break;
				case RANK_ONE_BUTTON:
					createbutton.subbuttons[0]->click();
					break;
				case RANK_TWO_BUTTON:
					createbutton.subbuttons[1]->click();
					break;
				case RANK_THREE_BUTTON:
					createbutton.subbuttons[2]->click();
					break;
				case RANK_FOUR_BUTTON:
					createbutton.subbuttons[3]->click();
					break;
				case FUSE_BUTTON:
					fusebutton.click();
					break;
				case CANCEL_BUTTON:
					cancelbutton.click();
					break;
				case PAUSE_BUTTON:
					pausebutton.click();
					break;
				case SKIP_BUTTON:
					skipbutton.click();
					break;
				case BASIC_BUTTON:
					basicbutton.click();
					break;
				case BOUNCE_BUTTON:
					bouncebutton.click();
					break;
				case SPLASH_BUTTON:
					splashbutton.click();
					break;
				case PIERCE_BUTTON:
					piercebutton.click();
					break;
				case SLOW_BUTTON:
					slowbutton.click();
					break;
				case INVENTORY_GRID:
					inventory.click( x, y );
					break;
				case GAME_MAP_GRID:
					gamemap.click( x, y );
					break;
				default:
					break;
			}
		}
	}
	// If a mouse button was released
	if( event.type == SDL_MOUSEBUTTONUP ){
		// If the left mouse button was released
		if( event.button.button == SDL_BUTTON_LEFT ){
			// Get the mouse offsets
			x = (int)( (float)event.button.x / (float)PIXELS_PER_TILE );
			y = (int)( (float)event.button.y / (float)PIXELS_PER_TILE );

			i = find_field( x, y );
			switch( i ){
				case INVENTORY_GRID:
					inventory.unclick( x, y );
					break;
				case GAME_MAP_GRID:
					gamemap.unclick( x, y );
					break;
				default:
					break;
			}
		}
	}
	else if( event.type == SDL_KEYDOWN ){
		switch( event.key.keysym.sym ) {
			case SDLK_UP:
				set_cash( 10 );
				createbutton.cash_increased();
				fusebutton.cash_increased();
				basicbutton.cash_increased();
				bouncebutton.cash_increased();
				splashbutton.cash_increased();
				piercebutton.cash_increased();
				slowbutton.cash_increased();
				break;
			case SDLK_DOWN:
				set_cash( -10 );
				createbutton.cash_decreased();
				fusebutton.cash_decreased();
				basicbutton.cash_decreased();
				bouncebutton.cash_decreased();
				splashbutton.cash_decreased();
				piercebutton.cash_decreased();
				slowbutton.cash_decreased();
				break;
			case SDLK_RIGHT:
				load_level( 2 );
				break;
			case SDLK_LEFT:
				load_level( 1 );
				break;
			case SDLK_ESCAPE:
				idle_active_button();
				idle_focuses();
				break;
			case SDLK_f:
				fusebutton.click();
				activebutton = FUSE_BUTTON;
				break;
			case SDLK_c:
				createbutton.click();
				activebutton = CREATE_BUTTON;
				break;
			case SDLK_F1:
				createbutton.subbuttons[0]->click();
				break;
			case SDLK_F2:
				createbutton.subbuttons[1]->click();
				break;
			case SDLK_F3:
				createbutton.subbuttons[2]->click();
				break;
			case SDLK_F4:
				createbutton.subbuttons[3]->click();
				break;
			case SDLK_1:
				basicbutton.click();
				activebutton = BASIC_BUTTON;
				break;
			case SDLK_2:
				bouncebutton.click();
				activebutton = BOUNCE_BUTTON;
				break;
			case SDLK_3:
				splashbutton.click();
				activebutton = SPLASH_BUTTON;
				break;
			case SDLK_4:
				piercebutton.click();
				activebutton = PIERCE_BUTTON;
				break;
			case SDLK_5:
				slowbutton.click();
				activebutton = SLOW_BUTTON;
				break;
			default:
				break;
		}
	}
	else if( activebutton >= BASIC_BUTTON && activebutton <= SLOW_BUTTON && event.type == SDL_MOUSEMOTION ){
		// Get the mouse offsets
		x = (int)( (float)event.button.x / (float)PIXELS_PER_TILE );
		y = (int)( (float)event.button.y / (float)PIXELS_PER_TILE );

		i = find_field( x, y );
		if( i == GAME_MAP_GRID ){
			gamemap.set_placement_footprint( x, y );
		}
		else{
			gamemap.clear_placement_footprint();
		}
	}
}

void set_message( std::string m, bool enabled ){
	if( enabled == true ){
		messagetext = m;
	}
	else{
		if( messagetext == m ){
			messagetext = "";
		}
	}
	// Set the message
	message = TTF_RenderText_Solid( font, messagetext.c_str(), textcolor );
}

void show_message(){
	if( messagetext != "" ){
		apply_surface( MSG_PXL_X + ( MSG_PXL_W - message->w ) / 2, MSG_PXL_Y + ( MSG_PXL_H - message->h ) / 2, message, screen );
	}
}

void set_cash( int c ){
	cash += c;
	std::stringstream cashtext;
	cashtext << cash;
	// Set the cash bar's text
	cashbartext = TTF_RenderText_Solid( font, (cashtext.str()).c_str(), textcolor );
	if( cash > 0 ){
		// Make sure the buttons are properly lit up based on cash
		createbutton.cash_increased();
		fusebutton.cash_increased();
		basicbutton.cash_increased();
		bouncebutton.cash_increased();
		splashbutton.cash_increased();
		piercebutton.cash_increased();
		slowbutton.cash_increased();
	}
	else if( cash < 0 ){
		// Make sure the buttons are properly greyed out based on cash
		createbutton.cash_decreased();
		fusebutton.cash_decreased();
		basicbutton.cash_decreased();
		bouncebutton.cash_decreased();
		splashbutton.cash_decreased();
		piercebutton.cash_decreased();
		slowbutton.cash_decreased();
	}
}

void set_lives( int l ){
	lives -= l;
	std::stringstream livestext;
	livestext << lives;
	// Set the lives bar's text
	livesbartext = TTF_RenderText_Solid( font, (livestext.str()).c_str(), textcolor );
}

void reset_score(){
	score = 0;
	std::stringstream scoretext;
	scoretext << "Score: " << score;
	// Set the score bar's text
	scorebartext = TTF_RenderText_Solid( font, (scoretext.str()).c_str(), textcolor );
}

void set_score( int points ){
	score += points;
	std::stringstream scoretext;
	scoretext << "Score: " << score;
	// Set the score bar's text
	scorebartext = TTF_RenderText_Solid( font, (scoretext.str()).c_str(), textcolor );
}

void set_active_mobs( int active, int total ){
	std::stringstream activemobstext;
	activemobstext << active << " / " << total;
	// Set the active/total mobs bar's text
	enemiesactivetext = TTF_RenderText_Solid( font, (activemobstext.str()).c_str(), textcolor );
}

void set_incoming_mobs( int incoming ){
	std::stringstream incomingmobstext;
	incomingmobstext << "(" << incoming << " incoming)";
	// Set the incoming mobs bar's text
	enemiesincomingtext = TTF_RenderText_Solid( font, (incomingmobstext.str()).c_str(), textcolor );
}

void set_wave_number( int wave, int total ){
	std::stringstream wavenumbertext;
	wavenumbertext << "Wave:  " << wave << " / " << total;
	// Set the wave number bar's text
	wavetext = TTF_RenderText_Solid( font, (wavenumbertext.str()).c_str(), textcolor );
}

void set_level_number( int level ){
	std::stringstream levelnumbertext;
	levelnumbertext << "Level: " << level;
	// Set the level number bar's text
	leveltext = TTF_RenderText_Solid( font, (levelnumbertext.str()).c_str(), textcolor );
}

void set_tooltip( int index ){
	tooltip = index;
	tooltipclip.x = 0;
	tooltipclip.y = ( tooltip - 1 ) * TOOLTIP_H;
	tooltipclip.w = TOOLTIP_W;
	tooltipclip.h = TOOLTIP_H;
}

void show_tooltip(){
	if( tooltip > 0 ){
		apply_surface( TOOLTIP_X, TOOLTIP_Y, tooltipssheet, screen, &tooltipclip );
	}
}

void show_hud(){
	apply_surface( CASH_ICON_W, CASH_ICON_N, cashicon, screen );
	apply_surface( CASH_MSG_W, CASH_MSG_N, cashbartext, screen );
	apply_surface( LIVES_ICON_W, LIVES_ICON_N, livesicon, screen );
	apply_surface( LIVES_MSG_W, LIVES_MSG_N, livesbartext, screen );
	apply_surface( ENEMIES_TITLE_W, ENEMIES_TITLE_N, enemiestitletext, screen );
	apply_surface( ENEMIES_ACTIVE_W, ENEMIES_ACTIVE_N, enemiesactivetext, screen );
	apply_surface( ENEMIES_INCOMING_W, ENEMIES_INCOMING_N, enemiesincomingtext, screen );
	apply_surface( WAVE_MSG_W, WAVE_MSG_N, wavetext, screen );
	apply_surface( LEVEL_MSG_W, LEVEL_MSG_N, leveltext, screen );
	apply_surface( SCORE_W, SCORE_N, scorebartext, screen );
	show_tooltip();
}

void show_ui( Uint32 deltaTicks ){
	createbutton.show();
	fusebutton.show();
	pausebutton.show();
	skipbutton.show();
	basicbutton.show();
	bouncebutton.show();
	splashbutton.show();
	piercebutton.show();
	slowbutton.show();
	show_message();
	show_hud();
	inventory.show();
	gamemap.show( deltaTicks );
	levelwaves->show( deltaTicks );
	gamemap.show_attacks( deltaTicks );
	levelwaves->show_health_bars();
	if( endgame > 0 ){
		apply_surface( ( SCREEN_WIDTH_IN_TILES * PIXELS_PER_TILE - END_SCREEN_W ) / 2, ( SCREEN_HEIGHT_IN_TILES * PIXELS_PER_TILE - END_SCREEN_H ) / 2, endscreensheet, screen, &clipendscreen );
	}
}

void victory(){
	pausebutton.victory_pause();
	endgame = 2;
	clipendscreen.x = 0;
	clipendscreen.y = END_SCREEN_H;
	clipendscreen.w = END_SCREEN_W;
	clipendscreen.h = END_SCREEN_H;
}

void defeat(){
	pausebutton.defeat_pause();
	endgame = 1;
	clipendscreen.x = 0;
	clipendscreen.y = 0;
	clipendscreen.w = END_SCREEN_W;
	clipendscreen.h = END_SCREEN_H;
}

// The design of this function was taken from a lazyfoo.net tutorial at http://lazyfoo.net/SDL_tutorials/index.php
int main( int argc, char* args[] ){
	bool quit = false;

	// Timer that keeps track of change in time since the last frame
	Timer timer;

	// Initialize SDL components
	if( init() == false ){
        return 1;
    }

	// Set the clips for objects that may have multiple instances
	set_clips();

	// Load the files
	if( load_files() == false ){
		return 1;
	}

	// Start at the first level and load it
	if( load_level( curLevel ) == false ){
		return 1;
	}

	// Background area of the message bar
	SDL_Rect msg;
	msg.x = MSG_X * PIXELS_PER_TILE;
	msg.y = MSG_Y * PIXELS_PER_TILE;
	msg.w = MSG_W * PIXELS_PER_TILE;
	msg.h = MSG_H * PIXELS_PER_TILE;

	// Background area of the hud bar
	SDL_Rect hud;
	hud.x = HUD_X * PIXELS_PER_TILE;
	hud.y = HUD_Y * PIXELS_PER_TILE;
	hud.w = HUD_W * PIXELS_PER_TILE;
	hud.h = HUD_H * PIXELS_PER_TILE;

	// Initialize the hud bar surfaces
	set_cash( 0 );
	set_lives( 0 );
	// Initialize the wave and level bar surfaces
	set_level_number( curLevel );
	set_wave_number( 0, levelwaves->total_waves() );
	// Set the title text for the enemies incoming/active bar
	enemiestitletext = TTF_RenderText_Solid( font, "Enemies:", textcolor );

	// Start the game timer
	timer.start();

	while( quit == false ){
        // While there's events to handle
        while( SDL_PollEvent( &event ) ){
			handle_input();

            // If the user has X'ed out the window
            if( event.type == SDL_QUIT ){
                // Quit the program
                quit = true;
            }
        }

		// Fill the screen with its background color
		SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB( screen->format, 0xAA, 0xAA, 0xAA ) );
		// Fill the background of the message bar with its color
		SDL_FillRect( screen, &msg, SDL_MapRGB( screen->format, 0xCC, 0xCC, 0xCC ) );
		// Fill the background of the hud bar with its color
		SDL_FillRect( screen, &hud, SDL_MapRGB( screen->format, 0xCC, 0xCC, 0xCC ) );

		// Show the UI elements
		show_ui( timer.get_ticks() );

		// Restart game timer to keep track of change in time since last frame
		timer.start();

        // Update the screen
        if( SDL_Flip( screen ) == -1 ){
            return 1;
        }
    }

    // Clean up
    clean_up();

    return 0;
}