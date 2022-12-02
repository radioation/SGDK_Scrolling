#include <genesis.h>
#include "resources.h"

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
// 7-8 slowest cloud                     <<<<<<  START BG_A  (21 rows for A)
// 9-11 the distance.  slowest of all
// 12-13 slowest land
// 14                                    <<<<<<   END BG_B  (15 rows vor B)
// 15
// 16
// 17-19
// 20-27 fastest gorund									<<<<<<< END BG_A
#define ROWS_A 21
#define START_ROW_A 7
fix32 speedA[ROWS_A];
fix32 planeOffsetA[ROWS_A];
fix32 imageOffsetA[ROWS_A];
u16 lastSrcColA[ROWS_A];
u16 lastDstColA[ROWS_A];
s16 scrollA[ROWS_A];

#define ROWS_B 12
#define START_ROW_B 3
fix32 speedB[ROWS_B];
fix32 planeOffsetB[ROWS_B];
fix32 imageOffsetB[ROWS_B];
u16 lastSrcColB[ROWS_B];
u16 lastDstColB[ROWS_B];
s16 scrollB[ROWS_B];

void updateTiles(VDPPlane plane, const TileMap *tilemap, int index,
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
					VDP_setTileMapEx(plane, tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, index),
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

void setupA()
{
	// setup scrolling vals
	for (int row = 0; row < ROWS_A; ++row)
	{
		planeOffsetA[row] = FIX32(0);
		imageOffsetA[row] = FIX32(0);
		lastSrcColA[row] = PLANE_MAX_TILE - 1;
		lastDstColA[row] = PLANE_MAX_TILE - 1;
		scrollA[row] = 0;
	}
	// 7-15 are 2nd slowest land value
	for (int row = 7; row <= 16; ++row)
	{
		speedA[row - START_ROW_A] = FIX32(2.2);
	}
	// 17-19
	for (int row = 17; row <= 19; ++row)
	{
		speedA[row - START_ROW_A] = FIX32(3.5);
	}
	// 20-27 fastest ground
	for (int row = 20; row < 23; ++row)
	{
		speedA[row - START_ROW_A] = FIX32(4.7);
	}
	for (int row = 23; row <= 27; ++row)
	{
		speedA[row - START_ROW_A] = FIX32(7.6);
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
}

int main(bool hard)
{
	//setup values
	setupA();
	setupB();

	// set color palette 0
	PAL_setPalette(PAL0, bg_palette_a.data, CPU);

	// set scrolling mode to TILE for horizontal.
	VDP_setScrollingMode(HSCROLL_TILE, VSCROLL_PLANE);

	// get tile positions in VRAM.
	int ind = TILE_USER_INDEX;
	int indexA = ind;
	// Load the plane tiles into VRAM
	VDP_loadTileSet(bg_image_a.tileset, ind, DMA);
	int indexB = ind + bg_image_a.tileset->numTile; // new
	VDP_loadTileSet(bg_image_b.tileset, indexB, DMA);

	// put out the image
	VDP_setTileMapEx(BG_A, bg_image_a.tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, indexA),
									 0,							 // Plane X destination
									 0,							 // plane Y destination
									 0,							 // Region X start position
									 0,							 // Region Y start position
									 PLANE_MAX_TILE, // width  (went with 64 becasue default width is 64.  Viewable screen is 40)
									 28,						 // height
									 CPU);
	VDP_setTileMapEx(BG_B, bg_image_b.tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, indexB),
									 0,
									 0,
									 0,
									 0,
									 PLANE_MAX_TILE,
									 28,
									 CPU);

	while (TRUE)
	{
		updateTiles(BG_A, bg_image_a.tilemap, indexA,
								speedA,
								planeOffsetA,
								imageOffsetA,
								lastSrcColA,
								lastDstColA,
								scrollA,
								START_ROW_A,
								ROWS_A);
		VDP_setHorizontalScrollTile(BG_A, START_ROW_A, scrollA, ROWS_A, CPU);

		updateTiles(BG_B, bg_image_b.tilemap, indexB,
								speedB,
								planeOffsetB,
								imageOffsetB,
								lastSrcColB,
								lastDstColB,
								scrollB,
								START_ROW_B,
								ROWS_B);
		VDP_setHorizontalScrollTile(BG_B, START_ROW_B, scrollB, ROWS_B, CPU);
		// let SGDK do its thing
		SYS_doVBlankProcess();
	}
	return 0;
}
