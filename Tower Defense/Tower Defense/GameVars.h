#ifndef _game_vars_h_
#define _game_vars_h_

const int SCREEN_WIDTH_IN_TILES  = 48;
const int SCREEN_HEIGHT_IN_TILES = 27;
const int SCREEN_BPP             = 32;
const int PIXELS_PER_TILE        = 24;
const int FRAMES_PER_SECOND      = 20;

#define CREATE_BUTTON     0
#define RANK_ONE_BUTTON   1
#define RANK_TWO_BUTTON   2
#define RANK_THREE_BUTTON 3
#define RANK_FOUR_BUTTON  4
#define FUSE_BUTTON       5
#define CANCEL_BUTTON     6
#define BASIC_BUTTON      7
#define BOUNCE_BUTTON     8
#define SPLASH_BUTTON     9
#define PIERCE_BUTTON     10
#define SLOW_BUTTON       11
#define PAUSE_BUTTON      12
#define SKIP_BUTTON       13
#define INVENTORY_GRID    14
#define GAME_MAP_GRID     15

#define CREATE_RANK1_COST 10
#define CREATE_RANK2_COST 25
#define CREATE_RANK3_COST 55
#define CREATE_RANK4_COST 115

#define FUSE_COST 10

#define BASIC_TOWER_COST  15
#define BOUNCE_TOWER_COST 30
#define SPLASH_TOWER_COST 50
#define PIERCE_TOWER_COST 50
#define SLOW_TOWER_COST   60

#define HEALTH_BAR_W 20
#define HEALTH_BAR_H 3
#define NUM_HEALTH_BARS 11

#define PATH_DIR_UP     1
#define PATH_DIR_RIGHT  1
#define PATH_DIR_DOWN  -1
#define PATH_DIR_LEFT  -1

#define STARTING_CASH  40
#define STARTING_LIVES 10

#define MSG_X 37
#define MSG_Y 16
#define MSG_W 10
#define MSG_H 2
const int MSG_PXL_X = MSG_X * PIXELS_PER_TILE;
const int MSG_PXL_Y = MSG_Y * PIXELS_PER_TILE;
const int MSG_PXL_W = MSG_W * PIXELS_PER_TILE;
const int MSG_PXL_H = MSG_H * PIXELS_PER_TILE;

#define HUD_X 7
#define HUD_Y 1
#define HUD_W 28
#define HUD_H 2

const int CASH_ICON_W = (int)( 30.0 * (float)PIXELS_PER_TILE );
const int CASH_ICON_N = (int)(  1.0 * (float)PIXELS_PER_TILE );
const int CASH_MSG_W  = (int)( 31.5 * (float)PIXELS_PER_TILE );
const int CASH_MSG_N  = (int)( 1.75 * (float)PIXELS_PER_TILE );

const int LIVES_ICON_W = (int)( 32.5 * (float)PIXELS_PER_TILE );
const int LIVES_ICON_N = (int)(  1.0 * (float)PIXELS_PER_TILE );
const int LIVES_MSG_W  = (int)( 34.0 * (float)PIXELS_PER_TILE );
const int LIVES_MSG_N  = (int)( 1.75 * (float)PIXELS_PER_TILE );

const int ENEMIES_TITLE_W    = (int)( 7.5    * (float)PIXELS_PER_TILE );
const int ENEMIES_TITLE_N    = (int)( 1.375  * (float)PIXELS_PER_TILE );
const int ENEMIES_ACTIVE_W   = (int)( 10.75  * (float)PIXELS_PER_TILE );
const int ENEMIES_ACTIVE_N   = (int)( 1.375  * (float)PIXELS_PER_TILE );
const int ENEMIES_INCOMING_W = (int)( 10.375 * (float)PIXELS_PER_TILE );
const int ENEMIES_INCOMING_N = (int)( 2.125  * (float)PIXELS_PER_TILE );

const int WAVE_MSG_W  = (int)( 16.50 * (float)PIXELS_PER_TILE );
const int WAVE_MSG_N  = (int)( 1.375 * (float)PIXELS_PER_TILE );
const int LEVEL_MSG_W = (int)( 16.50 * (float)PIXELS_PER_TILE );
const int LEVEL_MSG_N = (int)( 2.125 * (float)PIXELS_PER_TILE );

const int SCORE_W  = (int)( 22.50 * (float)PIXELS_PER_TILE );
const int SCORE_N  = (int)( 1.375 * (float)PIXELS_PER_TILE );

const int TOOLTIP_X = 37 * PIXELS_PER_TILE;
const int TOOLTIP_Y = 19 * PIXELS_PER_TILE;
const int TOOLTIP_W = 240;
const int TOOLTIP_H = 120;

#define END_SCREEN_W 500
#define END_SCREEN_H 250

#define NUM_LEVELS 2

struct Tile{
	int x;
	int y;
	int dirhor;
	int dirvert;
};

#endif