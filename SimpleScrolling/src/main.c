#include <genesis.h>
#include "resources.h"


// Map Position Variables.  These are what matter in this example.
fix32 mapPosX;
fix32 mapPosY;


// sprite variables
#define PLAYER_FRAME_COUNT 16
fix32 deltaX[PLAYER_FRAME_COUNT];
fix32 deltaY[PLAYER_FRAME_COUNT];
int rotationIndex;
Sprite* player;
fix32 playerRotation;
#define MAX_ROTATION_INDEX FIX32(15)
#define MIN_ROTATION_INDEX FIX32(0)


void handleInput() {
	u16 value = JOY_readJoypad(JOY_1);

	if( value & BUTTON_LEFT ) {
		playerRotation += FIX32( 0.25 ); // using fixpoint to slow down the rotation speed a bit. 
		if( playerRotation > MAX_ROTATION_INDEX ) {
			playerRotation = MIN_ROTATION_INDEX;
		}
		rotationIndex = fix32ToInt( playerRotation);
		SPR_setAnim( player, rotationIndex);
	} else if( value & BUTTON_RIGHT ) {
		playerRotation -= FIX32( 0.25 );
		if( playerRotation < MIN_ROTATION_INDEX ) {
			playerRotation = MAX_ROTATION_INDEX;
		}
		rotationIndex = fix32ToInt( playerRotation);
		SPR_setAnim( player, rotationIndex);
	}

	if( value & BUTTON_UP ) {
		// pushing UP on the joypad applies motion delta to the map position.
		mapPosX = fix32Add( mapPosX, deltaX[rotationIndex] );
		if( mapPosX < FIX32(0) ) {
		    mapPosX  = FIX32(0);
		}else if( mapPosX > FIX32(6079) ) { // full width of map image.
				mapPosX = FIX32(6079);
		}
		mapPosY = fix32Add( mapPosY, deltaY[rotationIndex] );
		if( mapPosY < FIX32(0) ) {
		    mapPosY  = FIX32(0);
		}else if( mapPosY > FIX32(2239) ) {  // this seems to be as far as I cna go without corrupting the map
				mapPosY = FIX32(2239);
		}

	}
}

int main( bool hard ) {
	
  PAL_setPalette( PAL1, plane_palette.data, CPU);
  PAL_setPalette( PAL2, ship_pal.data, CPU );

	// get our position in VRAM.
	int ind = TILE_USER_INDEX; 
	// Load the plane tiles into VRAM
	VDP_loadTileSet( &plane_a_tileset, ind, DMA );

	// init plane
	Map *map_a = MAP_create( &plane_a_map, BG_A, TILE_ATTR_FULL( PAL1, FALSE, FALSE, FALSE, ind ) );
	// set initial map position
	mapPosX = FIX32(1440);
	mapPosY = FIX32(1120);

	MAP_scrollTo( map_a, fix32ToInt(mapPosX), fix32ToInt(mapPosY ) );
	

	// Init sprite engine with defaults
	SPR_init();
	fix32 posX = FIX32( 160 - 16 );
	fix32 posY = FIX32( 112 - 16 );
	player = SPR_addSprite( &ship, fix32ToInt(posX), fix32ToInt(posY), TILE_ATTR(PAL2, 0, FALSE, FALSE ));
	playerRotation = MIN_ROTATION_INDEX;
	SPR_setAnim( player, fix32ToInt(playerRotation ) );


	// setup map motion delta based on index (index is shared with sprite so scrolling motion 
	// and player direction match)
	deltaX[0] = FIX32( 0.000000 );
	deltaY[0] = FIX32( -2.000000 );
	deltaX[1] = FIX32( -0.765367 );
	deltaY[1] = FIX32( -1.847759 );
	deltaX[2] = FIX32( -1.414214 );
	deltaY[2] = FIX32( -1.414214 );
	deltaX[3] = FIX32( -1.847759 );
	deltaY[3] = FIX32( -0.765367 );
	deltaX[4] = FIX32( -2.000000 );
	deltaY[4] = FIX32( -0.000000 );
	deltaX[5] = FIX32( -1.847759 );
	deltaY[5] = FIX32( 0.765367 );
	deltaX[6] = FIX32( -1.414214 );
	deltaY[6] = FIX32( 1.414214 );
	deltaX[7] = FIX32( -0.765367 );
	deltaY[7] = FIX32( 1.847759 );
	deltaX[8] = FIX32( -0.000000 );
	deltaY[8] = FIX32( 2.000000 );
	deltaX[9] = FIX32( 0.765367 );
	deltaY[9] = FIX32( 1.847759 );
	deltaX[10] = FIX32( 1.414214 );
	deltaY[10] = FIX32( 1.414214 );
	deltaX[11] = FIX32( 1.847759 );
	deltaY[11] = FIX32( 0.765367 );
	deltaX[12] = FIX32( 2.000000 );
	deltaY[12] = FIX32( 0.000000 );
	deltaX[13] = FIX32( 1.847759 );
	deltaY[13] = FIX32( -0.765367 );
	deltaX[14] = FIX32( 1.414214 );
	deltaY[14] = FIX32( -1.414214 );
	deltaX[15] = FIX32( 0.765367 );
	deltaY[15] = FIX32( -1.847759 );


	while(TRUE) {
					// Read Joypad
					handleInput();

					// Update the sprite
					SPR_update();

					// Set the scrolling position 
					MAP_scrollTo( map_a, fix32ToInt(mapPosX), fix32ToInt(mapPosY ) );


					SYS_doVBlankProcess();
	}	
	return 0;
}

