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

//
// 0-2 don't scroll.  no need
// 3-4  fastest cloud										<<<<<< START BG_B
// 5
// 6
// 7-8 slowest cloud                     
// 9-11 the distance.  slowest of all
// 12-13 slowest land
// 14                                    <<<<<<   END BG_B  (15 rows vor B)
// 15
// 16
// 17-19                                <<<< BGA
// 20-27 fastest gorund									<<<<<<< END BG_B
#define ROWS_A 200
#define START_ROW_A 24
#define COLS_A 20
s16 hScrollA[ROWS_A];
s16 vScrollA[COLS_A];

#define ROWS_B 200
#define START_ROW_B 24
fix32 speedB[ROWS_B];
fix32 planeOffsetB[ROWS_B];
fix32 imageOffsetB[ROWS_B];
u16 lastSrcColB[ROWS_B];
u16 lastDstColB[ROWS_B];
s16 hScrollB[ROWS_B];




void updateBackground(VDPPlane plane, const TileMap *tilemap, int index,
								 fix32 *speed,
								 fix32 *planeOffset,
								 fix32 *imageOffset,
								 u16 *lastSrcCol,
								 u16 *lastDstCol,
								 s16 *scroll,
								 u16 startRow,
								 u16 rows)
{

	for (s16 row = 0; row < rows; row+=8)
	{
		// Set the scrolling position of plane per row
		//planeOffset[row] = fix32Add(planeOffset[row], speed[row]);
		memcpy( planeOffset[row], fix32Add(planeOffset[row], speed[row]),  );

		if (planeOffset[row] >= FIX32(PLANE_MAX_PIXEL)) // plane in memory is 512 pixels wide
		{
			memcpy( planeOffset[row] = FIX32(0); 
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
		scroll[row] = -sPlaneOffset;
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
		scrollB[row] = 0;
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

	// set scrolling mode to TILE for horizontal.
	VDP_setScrollingMode(HSCROLL_LINE, VSCROLL_TILE);

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

	while (TRUE)
	{

		treePosX = fix32Sub(treePosX, FIX32(2.2));
		SPR_setPosition(treeSprite, fix32ToInt(treePosX), 54);


		VDP_setHorizontalScrollTile(BG_A, START_ROW_A, scrollA, ROWS_A, CPU);

		updateBackground(BG_B, bg.tilemap, indexB,
								speedB,
								planeOffsetB,
								imageOffsetB,
								lastSrcColB,
								lastDstColB,
								scrollB,
								START_ROW_B,
								ROWS_B);
		VDP_setHorizontalScrollTile(BG_B, START_ROW_B, scrollB, ROWS_B, CPU);

		SPR_update();

		// let SGDK do its thing
		SYS_doVBlankProcess();
	}
	return 0;
}
