#include <genesis.h>
#include "resources.h"




/////////////////////////////////////////////////////////////////////
// Scrolling Stuff
// 
// default SGDK width is 512 pixels (64 tiles)
#define PLANE_MAX_PIXEL 512
#define PLANE_MAX_TILE 64

// Images in this project are 1280 pixels wide ( tiles)
#define IMAGE_MAX_PIXEL 1280
#define IMAGE_MAX_TILE 160

#define ROWS_A 150
#define START_ROW_A 74
#define COLS_A 20
#define START_COL_A 0
s16 hScrollA[224];
s16 vScrollA[20];


#define ROWS_B 25
#define START_ROW_B 3
fix32 speedB[ROWS_B];
fix32 planeOffsetB[ROWS_B];
fix32 imageOffsetB[ROWS_B];
u16 lastSrcColB[ROWS_B];
u16 lastDstColB[ROWS_B];
s16 hScrollB[ROWS_B*8];

// setup fake rotation by changing the foreground horizontal and vertical scrolling
void setAngle( u16 angle, int centerY ) {
	// angle is defined as [0..1024] mapped to [0..2PI] range.  
	// negative rotation will be 1024 down to 512
	// each value is ~ 0.35 degrees / 0.0061 radians.
	//KLog_S1("angle:",angle);
	//KLog_F2x( 4, "   c: ", cosFix32(angle), " s: ", sinFix32(angle));
	
	 for( int row = START_ROW_A; row < START_ROW_A + ROWS_A; ++row ){
	 	fix32 shift = fix32Mul(FIX32( (row - centerY)>>1 ), sinFix32(angle));	
	 //	KLog_S2( "  row: ", row, " off: ", (row - centerY));
		//KLog_F1x( 4, "     shift: ", shift );
		hScrollA[row-START_ROW_A] = fix32ToInt( shift ) - 24;
	 } 


	// vertical scroll tiles are 16 pixels wide.  Using 8 * (col-10) to scale the scrolling effect
	// at the extreme left and right of the screen  the factor would be -80 and + 80
	for( int col = START_COL_A; col < START_COL_A + COLS_A; ++col ){
		fix32 shift = fix32Mul(FIX32(   (col - 9)<<3 ), sinFix32(angle));	
		vScrollA[col] = fix32ToInt( shift );
	} 


}




// handle horizontal parallax scrolling of background image.
void updateScroll(VDPPlane plane, const TileMap *tilemap, int index,
								 fix32 *speed,
								 fix32 *planeOffset,
								 fix32 *imageOffset,
								 u16 *lastSrcCol,
								 u16 *lastDstCol,
								 s16 *scroll,
								 u16 startRow,
								 u16 rows)
{

	for (s16 row = 0; row < rows; ++row)
	{
		// Set the scrolling position of plane per row
		planeOffset[row] = fix32Add(planeOffset[row], speed[row]);
		if (planeOffset[row] >= FIX32(PLANE_MAX_PIXEL)) // plane in memory is 512 pixels wide
		{
			planeOffset[row] = FIX32(0); 
		}

		// keep track of where we are in the image per row
		imageOffset[row] = fix32Add(imageOffset[row], speed[row]);
		if (imageOffset[row] >= FIX32(IMAGE_MAX_PIXEL)) // bg image is 1280 pixels wide
		{
			imageOffset[row] = FIX32(0); 
		}
		s16 sPlaneOffset = fix32ToInt(planeOffset[row]);
		// check if we need a new tile
		if (sPlaneOffset % 8 == 0)
		{
			// get destination column (in tiles)
			s16 dstCol = (sPlaneOffset + PLANE_MAX_PIXEL - 8) / 8;
			if (dstCol >= PLANE_MAX_TILE)
			{
				dstCol -= PLANE_MAX_TILE; // wrap around to the start of the plane
			}

			// get source column (in tiles)
			s16 sImageOffset = fix32ToInt(imageOffset[row]);
			s16 srcCol = (sImageOffset + PLANE_MAX_PIXEL - 8) / 8;
			if (srcCol >= IMAGE_MAX_TILE)
			{
				srcCol -= IMAGE_MAX_TILE; // wrap around to the start of the image
			}
			// if the current destination column is smaller n
			// the last one, the region is *notnuous
			// Two or more tile moves may be required.
			if (dstCol != lastDstCol[row])
			{
				s16 width = dstCol - lastDstCol[row];
				if (width < 0)
				{
					width += PLANE_MAX_TILE;
				}
				// just loop for now ( probably inefficient if there are a lot of columns to copy)
				s16 tmpDst = lastDstCol[row] + 1;
				s16 tmpSrc = lastSrcCol[row] + 1;
				for (s16 i = 0; i < width; ++i)
				{
					if (tmpDst >= PLANE_MAX_TILE)
					{
						tmpDst = 0;
					}
					if (tmpSrc >= IMAGE_MAX_TILE)
					{
						tmpSrc = 0;
					}
					VDP_setTileMapEx(plane, tilemap, TILE_ATTR_FULL(PAL0, plane == BG_A ? TRUE : FALSE, FALSE, FALSE, index),
													 tmpDst,				 // Plane X destination
													 startRow + row, // plane Y destination
													 tmpSrc,				 // Region X start position
													 startRow + row, // Region Y start position
													 1,							 // width
													 1,							 // height
													 CPU);
					tmpDst++;
					tmpSrc++;
				}
				lastDstCol[row] = tmpDst - 1;
				lastSrcCol[row] = tmpSrc - 1;
			}
		}
		// set offset through all 8 lines for the current "tile"
		for( int i=0; i < 8; ++i ) {
			scroll[row*8 + i] = -sPlaneOffset;
		}
	
	}
}



void setupB()
{
	// setup scrolling vals
	for (int row = 0; row < ROWS_B; ++row)
	{
		planeOffsetB[row] = FIX32(0);
		imageOffsetB[row] = FIX32(0);
		lastSrcColB[row] = PLANE_MAX_TILE - 1;
		lastDstColB[row] = PLANE_MAX_TILE - 1;
		hScrollB[row] = 0;
	}
	// 3-4 fastest cloud
	speedB[0] = FIX32(6);
	speedB[1] = FIX32(6);
	// 5
	speedB[2] = FIX32(3);
	// 6
	speedB[3] = FIX32(1.5);
	// 7-8 slowest cloud
	speedB[4] = FIX32(0.75);
	speedB[5] = FIX32(0.75);
	// 9-11 the distances
	speedB[6] = FIX32(0.075);
	speedB[7] = FIX32(0.075);
	speedB[8] = FIX32(0.075);
	// 12-13 slow land
	speedB[9] = FIX32(0.5);
	speedB[10] = FIX32(1.0);
	// 14
	speedB[11] = FIX32(1.5);
	speedB[12] = FIX32(2.2);
	speedB[13] = FIX32(3.0);
	
	// 17-19
	for (int row = 17; row <= 19; ++row)
	{
		speedB[row - START_ROW_B] = FIX32(3.5);
	}
	// 20-27 fastest ground
	for (int row = 20; row < 23; ++row)
	{
		speedB[row - START_ROW_B] = FIX32(4.7);
	}
	for (int row = 23; row <= 27; ++row)
	{
		speedB[row - START_ROW_B] = FIX32(7.6);
	}
}


/////////////////////////////////////////////////////////////////////
// Joypad Handler
u16 maxAngle = 7;
static void readJoypad( u16 joypadId ) {
  u16 joypadState = JOY_readJoypad( joypadId );
	 if( joypadState & BUTTON_A ) {
	 	maxAngle = 7;
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



int main(bool hard)
{
  VDP_setScreenWidth320();
	// set colors
	PAL_setPalette( PAL0, bg_pal.data, CPU) ;
	PAL_setPalette( PAL1, gun_bike_pal.data, CPU );
	PAL_setPalette( PAL2, gun_bike_rider_pal.data, CPU );
	PAL_setPalette( PAL3, boss_truck_pal.data, CPU );
	
	//setup scroll values
	setupB();

	// set scrolling mode to support rotation
	VDP_setScrollingMode(HSCROLL_LINE, VSCROLL_2TILE);

	// get tile positions in VRAM.
	int ind = TILE_USER_INDEX;
	int indexA = ind;
	// Load the plane tiles into VRAM
	VDP_loadTileSet(bg_boss_truck.tileset, ind, DMA);
	int indexB = ind + bg_boss_truck.tileset->numTile; // new
	VDP_loadTileSet(bg.tileset, indexB, DMA);

	// put out the image
	VDP_setTileMapEx(BG_A, bg_boss_truck.tilemap, TILE_ATTR_FULL(PAL3, TRUE, FALSE, FALSE, indexA),
									 0,							 // Plane X destination
									 0,							 // plane Y destination
									 0,							 // Region X start position
									 0,							 // Region Y start position
									 PLANE_MAX_TILE, // width  (went with 64 becasue default width is 64.  Viewable screen is 40)
									 28,						 // height
									 CPU);
 
	VDP_setTileMapEx(BG_B, bg.tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, indexB),
									 0,
									 0,
									 0,
									 0,
									 PLANE_MAX_TILE,
									 28,
									 CPU);



	//////////////////////////////////////////////////////////////
  SPR_init();

	Sprite * gunbikerider1 = NULL;
	Sprite * gunbike1 = NULL;
	int gunbikerider1_pos_x =  91;
	int gunbikerider1_pos_y = 140;
	gunbikerider1 = SPR_addSprite( &gun_bike_rider_big, 
          gunbikerider1_pos_x, 
          gunbikerider1_pos_y,  
          TILE_ATTR( PAL2,
           1,     // tile priority
           FALSE,    // flip sprite vertically
           FALSE     // flip sprite horizontally
            ));

	int gunbike1_pos_x =  55;
	int gunbike1_pos_y = 152;
	gunbike1 = SPR_addSprite( &gun_bike_big, 
          gunbike1_pos_x, 
          gunbike1_pos_y,
          TILE_ATTR( PAL1, 
            1,
            FALSE,
            FALSE ));


	Sprite *truckWheelSprite1 = NULL;
	fix32 truckWheelPosX1 = FIX32(55.0);
	fix32 truckWheelPosY1 = FIX32(135.0);
	truckWheelSprite1 = SPR_addSprite( &boss_truck_wheel,  // Sprite defined in resources
			fix32ToInt(truckWheelPosX1),// starting X position
			fix32ToInt(truckWheelPosY1),// starting Y position
			TILE_ATTR( PAL3,           // specify palette
				1,        // Tile priority ( with background)
				FALSE,        // flip the sprite vertically?
				FALSE         // flip the sprite horizontally
				));

	Sprite *truckWheelSprite2 = NULL;
	fix32 truckWheelPosX2 = FIX32(186.0);
	fix32 truckWheelPosY2 = FIX32(135.0);
	truckWheelSprite2 = SPR_addSprite( &boss_truck_wheel,  // Sprite defined in resources
			fix32ToInt(truckWheelPosX2),// starting X position
			fix32ToInt(truckWheelPosY2),// starting Y position
			TILE_ATTR( PAL3,           // specify palette
				TRUE,        // Tile priority ( with background)
				FALSE,        // flip the sprite vertically?
				FALSE         // flip the sprite horizontally
				));


	Sprite *treeSprite = NULL;
	fix32 treePosX = FIX32(152.0);
	fix32 treePosY = FIX32(54.0);
	treeSprite = SPR_addSprite( &tree,  // Sprite defined in resources
			fix32ToInt(treePosX),// starting X position
			fix32ToInt(treePosY),// starting Y position
			TILE_ATTR( PAL0,           // specify palette
				FALSE,        // Tile priority ( with background)
				FALSE,        // flip the sprite vertically?
				FALSE         // flip the sprite horizontally
				));



	u16 currAngle = 0;
	setAngle(currAngle, 150);
	int stepDir = 1;
	while (TRUE)
	{
		// read joypad to set max angle dynamically
		readJoypad(JOY_1);

		// sprite
		treePosX = fix32Sub(treePosX, FIX32(2.2));
		SPR_setPosition(treeSprite, fix32ToInt(treePosX), 54);

		// Fake rotation
		currAngle += stepDir;
		if (stepDir == 1 && currAngle < 512)
		{
			if (currAngle == maxAngle)
			{
				stepDir = -1;
			}
		}
		else if (stepDir == -1 && currAngle == 0)
		{
			currAngle = 1024;
		}
		else if (stepDir == -1 && currAngle > 512)
		{
			if (currAngle == 1024 - maxAngle)
			{
				stepDir = 1;
			}
		}
		else if (stepDir == 1 && currAngle == 1024)
		{
			currAngle = 0;
		}
		setAngle(currAngle, 130);

		// set scrolling to fake the rotaiton.
		VDP_setHorizontalScrollLine(BG_A, START_ROW_A, hScrollA, ROWS_A, DMA);
		VDP_setVerticalScrollTile(BG_A, START_COL_A, vScrollA, COLS_A, DMA);

		// parallax scrolling.
		updateScroll(BG_B, bg.tilemap, indexB,
								 speedB,
								 planeOffsetB,
								 imageOffsetB,
								 lastSrcColB,
								 lastDstColB,
								 hScrollB,
								 START_ROW_B,
								 ROWS_B);
		VDP_setHorizontalScrollLine(BG_B, START_ROW_B * 8, hScrollB, ROWS_B * 8, DMA);

		SPR_update();

		// let SGDK do its thing
		SYS_doVBlankProcess();
	}
	return 0;
}
