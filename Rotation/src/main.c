#include <genesis.h>
#include "resources.h"


/////////////////////////////////////////////////////////////////////
// Joypad Handler
u16 maxAngle = 5;
static void readJoypad( u16 joypadId ) {
  u16 joypadState = JOY_readJoypad( joypadId );
	 if( joypadState & BUTTON_A ) {
	 	maxAngle = 5;
	 }else if( joypadState & BUTTON_B ) {
	 	maxAngle = 10;
	 }else if( joypadState & BUTTON_C ) {
	 	maxAngle = 15;
	 }else if( joypadState & BUTTON_X ) {
	 	maxAngle = 20;
	 }else if( joypadState & BUTTON_Y ) {
	 	maxAngle = 30;
	 }else if( joypadState & BUTTON_Z ) {
	 	maxAngle = 40;
	 }
}


/////////////////////////////////////////////////////////////////////
// Scrolling Stuff
// 
// default SGDK width is 512 pixels (64 tiles)
#define PLANE_MAX_PIXEL 512
#define PLANE_MAX_TILE 64

#define ROWS_A 200
#define START_ROW_A 24
#define COLS_A 20
#define START_COL_A 0
s16 hScrollA[224];
s16 vScrollA[20];



void setAngle( u16 angle, int centerY ) {
	// angle is defined as [0..1024] mapped to [0..2PI] range.  
	// negative rotation will be 1024 down to 512
	// each value is ~ 0.35 degrees / 0.0061 radians.
	//KLog_F2x( 4, "c: ", cosFix32(angle), " s: ", sinFix32(angle));
	
	 for( int row = START_ROW_A; row < START_ROW_A + ROWS_A; ++row ){
	 	fix32 shift = fix32Mul(FIX32( (row - centerY) ), sinFix32(angle));	
	 //	KLog_S2( "  row: ", row, " off: ", (row - centerY));
		//KLog_F1x( 4, "     shift: ", shift );
		hScrollA[row] = fix32ToInt( shift ) - 24;
	 } 



	// vertical scroll tiles are 16 pixels wide.  Using 8 * (col-10) to scale the scrolling effect
	// at the extreme left and right of the screen  the factor would be -80 and + 80
	for( int col = START_COL_A; col < START_COL_A + COLS_A; ++col ){
		fix32 shift = fix32Mul(FIX32(  16 * (col - 10) ), sinFix32(angle));	
		vScrollA[col] = fix32ToInt( shift );
	} 


}


int main(bool hard)
{
  VDP_setScreenWidth320();
	// set colors
	PAL_setPalette( PAL0, boss_truck_pal.data, CPU );

  memset( hScrollA, 0, sizeof(hScrollA));
  memset( vScrollA, 0, sizeof(vScrollA));

	// set scrolling mode to LINE for horizontal and TILE for vertical
	VDP_setScrollingMode(HSCROLL_LINE, VSCROLL_2TILE);

	// get tile positions in VRAM.
	int ind = TILE_USER_INDEX;
	int indexA = ind;
	// Load the plane tiles into VRAM
	VDP_loadTileSet(bg_boss_truck.tileset, ind, DMA);

	// setup the tiles
	VDP_setTileMapEx(BG_A, bg_boss_truck.tilemap, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, indexA),
									 0,							 // Plane X destination
									 0,							 // plane Y destination
									 0,							 // Region X start position
									 0,							 // Region Y start position
									 PLANE_MAX_TILE, // width  (went with 64 becasue default width is 64.  Viewable screen is 40)
									 28,						 // height
									 CPU);



	u16 currAngle = 0;
	setAngle(currAngle, 150);
	int stepDir = 1;
	while (TRUE)
	{
		// read joypad to set max angle dynamically
		readJoypad(JOY_1);


		// handle rotation
		currAngle += stepDir;
		if( stepDir ==1 && currAngle <512 ) {
			if( currAngle > maxAngle) {
				stepDir = -1;
			}
		} else if ( stepDir == -1 && currAngle == 0 ) {
			currAngle =1024;
		} else if ( stepDir == -1 && currAngle > 512 ) {
			if( currAngle < 1024-maxAngle) {
				stepDir = 1;
			}
		} else if ( stepDir == 1 && currAngle > 1024 ) {
			currAngle =0;
		}
		setAngle(currAngle, 130);



		// set scrolling to fake the rotaiton.
		VDP_setHorizontalScrollLine(BG_A, START_ROW_A, hScrollA, ROWS_A, DMA);
		VDP_setVerticalScrollTile(BG_A, START_COL_A, vScrollA, COLS_A, DMA);


		// let SGDK do its thing
		SYS_doVBlankProcess();
	}
	return 0;
}
