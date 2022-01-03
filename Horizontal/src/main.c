#include <genesis.h>
#include "resources.h"

// default SGDK width is 512 pixels (64 tiles)
#define PLANE_MAX_PIXEL 512
#define PLANE_MAX_TILE 64

// Images in this project are 1280 pixels wide ( tiles)
#define IMAGE_MAX_PIXEL 1280
#define IMAGE_MAX_TILE 160

fix32 offsetA = FIX32(0);
fix32 imageOffsetA = FIX32(0);
u16 lastSrcColA = -1;
u16 lastDstColA = -1;

fix32 offsetB = FIX32(0);
fix32 imageOffsetB = FIX32(0);
u16 lastSrcColB = -1;
u16 lastDstColB = -1;

s16 scrollPlane(VDPPlane plane, const TileMap *tilemap, int index, fix32 speed, fix32 *planeOffset, fix32 *imageOffset, u16 *lastSrcCol, u16 *lastDstCol)
{
	// Set the scrolling position
	*planeOffset = fix32Add(*planeOffset, speed);
	if (*planeOffset >= FIX32(PLANE_MAX_PIXEL))
		*planeOffset = FIX32(0); // plane in memory is 512 pixels wide
	*imageOffset = fix32Add(*imageOffset, speed);
	if (*imageOffset >= FIX32(IMAGE_MAX_PIXEL))
		*imageOffset = FIX32(0); // bg image is 1280 pixels wide
	s16 sPlaneOffset = fix32ToInt(*planeOffset);
	// check if we need a new tile
	if (sPlaneOffset % 8 == 0)
	{
		// get destination column (in tiles)
		s16 dstCol = (sPlaneOffset + 504) / 8;
		if (dstCol >= PLANE_MAX_TILE)
		{
			dstCol -= PLANE_MAX_TILE; // wrap around to the start of the plane
		}

		// get source column (in tiles)
		s16 sImageOffsetA = fix32ToInt(*imageOffset);
		s16 srcCol = (sImageOffsetA + PLANE_MAX_PIXEL) / 8;
		if (srcCol >= IMAGE_MAX_TILE)
		{
			srcCol -= IMAGE_MAX_TILE; // wrap around to the start of the image
		}
		// if the current destination column is smaller than
		// the last one, the region is *not* continuous.
		// Two or more tile moves may be required.
		if (dstCol < *lastDstCol)
		{
			// from lastDst to end of the plane
			int width1 = PLANE_MAX_TILE - *lastDstCol - 1;
			int width2 = dstCol + 1;
			if (srcCol > *lastSrcCol)
			{
				// copy first half
				VDP_setTileMapEx(plane, tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, index),
												 *lastDstCol + 1, // Plane X destination
												 0,							 // plane Y destination
												 *lastSrcCol + 1, // Region X start position
												 0,							 // Region Y start position
												 width1,				 // width  ( will this wrap? )
												 28,						 // height
												 CPU);
				// copy second half
				VDP_setTileMapEx(plane, tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, index),
												 0,												// Plane X destination
												 0,												// plane Y destination
												 *lastSrcCol + 1 + width1, // Region X start position
												 0,												// Region Y start position
												 width2,									// width  ( will this wrap? )
												 28,											// height
												 CPU);
			}
			else
			{
				// just loop for now ( probably inefficient if there are a lot of columns to copy)
				s16 totalWidth = width1 + width2;
				s16 tmpDst = *lastDstCol + 1;
				s16 tmpSrc = *lastSrcCol + 1;
				for (s16 i = 0; i < totalWidth; ++i)
				{
					VDP_setTileMapEx(plane, tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, index),
													 tmpDst, // Plane X destination
													 0,			 // plane Y destination
													 tmpSrc, // Region X start position
													 0,			 // Region Y start position
													 width2, // width  ( will this wrap? )
													 28,		 // height
													 CPU);

					tmpDst++;
					if (tmpDst >= PLANE_MAX_PIXEL)
					{
						tmpDst = 0;
					}
					tmpSrc++;
					if (tmpSrc >= IMAGE_MAX_PIXEL)
					{
						tmpSrc = 0;
					}
				}
			}
		}
		else if (dstCol > *lastDstCol)
		{
			//
			int width = dstCol - *lastDstCol;
			if (*lastSrcCol + width < IMAGE_MAX_TILE)
			{
				// we're not blowing past
				VDP_setTileMapEx(plane, tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, index),
												 *lastDstCol + 1, // Plane X destination
												 0,							 // plane Y destination
												 *lastSrcCol + 1, // Region X start position
												 0,							 // Region Y start position
												 width,					 // width
												 28,						 // height
												 CPU);
			}
			else
			{
				//  blowing past the end, need two copies
				s16 width1 = IMAGE_MAX_TILE - *lastSrcCol - 1;
				s16 width2 = width - width1;
				VDP_setTileMapEx(plane, tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, index),
												 *lastDstCol + 1, // Plane X destination
												 0,							 // plane Y destination
												 *lastSrcCol + 1, // Region X start position
												 0,							 // Region Y start position
												 width1,				 // width
												 28,						 // height
												 CPU);

				VDP_setTileMapEx(plane, tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, index),
												 *lastDstCol + 1 + width1, // Plane X destination
												 0,												// plane Y destination
												 0,												// Region X start position
												 0,												// Region Y start position
												 width2,									// width
												 28,											// height
												 CPU);
			}
		}
		*lastDstCol = dstCol;
		*lastSrcCol = srcCol;
	}
	return sPlaneOffset;
}

int main(u16 hard)
{

	VDP_setPalette(PAL0, bg_image_b.palette->data);

	// set scrolling mode.  Affects the WHOLE plane
	VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);

	// get our position in VRAM.
	int ind = TILE_USERINDEX;
	int indexA = ind;
	// Load the plane tiles into VRAM
	VDP_loadTileSet(bg_image_a.tileset, ind, DMA);
	int indexB = ind + bg_image_a.tileset->numTile; // new
	VDP_loadTileSet(bg_image_b.tileset, indexB, DMA);

	// put out the image
	VDP_setTileMapEx(BG_A, bg_image_a.tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, indexA),
									 0,								// Plane X destination
									 0,								// plane Y destination
									 0,								// Region X start position
									 0,								// Region Y start position
									 PLANE_MAX_TILE, // width  (went with 64 becasue default width is 64.  Viewable screen is 40)
									 28,							// height
									 CPU);
	lastDstColA = PLANE_MAX_TILE - 1;
	lastSrcColA = PLANE_MAX_TILE - 1;
	VDP_setTileMapEx(BG_B, bg_image_b.tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, indexB),
									 0,
									 0,
									 0,
									 0,
									 PLANE_MAX_TILE,
									 28,
									 CPU);
	lastDstColB = PLANE_MAX_TILE - 1;
	lastSrcColB = PLANE_MAX_TILE - 1;

	while (TRUE)
	{

		s16 sPlaneOffsetA = scrollPlane(BG_A, bg_image_a.tilemap, indexA, FIX32(1.2), &offsetA, &imageOffsetA, &lastSrcColA,  &lastDstColA);
		s16 sPlaneOffsetB = scrollPlane(BG_B, bg_image_b.tilemap, indexB, FIX32(0.8), &offsetB, &imageOffsetB, &lastSrcColB,  &lastDstColB);

		VDP_setHorizontalScroll(BG_A, -sPlaneOffsetA); // negative moves plane to left, positive to right
		VDP_setHorizontalScroll(BG_B, -sPlaneOffsetB);

		// let SGDK do its thing
		SYS_doVBlankProcess();
	}
	return 0;
}
