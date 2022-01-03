#include <genesis.h>
#include "resources.h"


fix32 offsetA = FIX32(0);
fix32 imageOffsetA = FIX32(0);
u16 lastSrcColA = -1;
u16 lastDstColA = -1;

s16 offsetB = 0;
s16 imageOffsetB = 0;

int main( u16 hard ) {
	
  VDP_setPalette( PAL0, bg_image_a.palette->data);

	// set scrolling mode.  Affects the WHOLE plane
	VDP_setScrollingMode( HSCROLL_PLANE, VSCROLL_PLANE);

	// get our position in VRAM.
	int ind = TILE_USERINDEX; 
	int indexA = ind;
	// Load the plane tiles into VRAM
	VDP_loadTileSet( bg_image_a.tileset, ind, DMA );
	int indexB = ind + bg_image_a.tileset->numTile; // new 
	VDP_loadTileSet( bg_image_b.tileset, indexB, DMA );

	// put out the image
	VDP_setTileMapEx(BG_A, bg_image_a.tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, indexA),
									 0,			// Plane X destination
									 0,			// plane Y destination
									 0, 		// Region X start position
									 0,			// Region Y start position
									 64,		// width  (went with 64 becasue default width is 64.  Viewable screen is 40)
									 28,		// height
									 CPU);
	lastDstColA = 63;
	VDP_setTileMapEx(BG_B, bg_image_b.tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, indexB),
									 0,
									 0,
									 0,
									 0,
									 64,
									 28,
									 CPU);


	while (TRUE)
	{

		// Set the scrolling position
		offsetA = fix32Add( offsetA, FIX32(1.2));
		if( offsetA > FIX32(511)) offsetA = FIX32(0);  // plane in memory is 512 pixels wide
		imageOffsetA = fix32Add( imageOffsetA, FIX32(1.2));
		if( imageOffsetA > FIX32(1279)) imageOffsetA = FIX32(0); // bg image is 1280 pixels wide

		s16 sOffsetA = fix32ToInt( offsetA );
		// check if we need a new tile
    if( sOffsetA % 8 == 0) 
		{
			// get destination column (in tiles)
			s16 dstCol = (sOffsetA + 504) / 8;
			if( dstCol > 63 ) {
				dstCol -= 64; // wrap around to the start of the plane
			} 

			// get source column (in tiles)
			s16 sImageOffsetA = fix32ToInt( imageOffsetA );
			s16 srcCol = (sImageOffsetA + 512)/8;
			if( srcCol > 159  ) {
				srcCol -= 160; // wrap around to the start of the image
			}
			// if the current destination column is smaller than 
			// the last one, the region is *not* continuous.
			// Two or more tile moves may be required.
			if (dstCol < lastDstColA) 
			{
				// from lastDst to end of the plane
				int width1 = 63 - lastDstColA;
				int width2 = dstCol + 1;
				if( srcCol > lastSrcColA ) {
					// copy first half
					VDP_setTileMapEx(BG_A, bg_image_a.tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, indexA),
													 lastDstColA + 1, // Plane X destination
													 0,								// plane Y destination
													 lastSrcColA + 1,	// Region X start position
													 0,								// Region Y start position
													 width1,					// width  ( will this wrap? )
													 28,							// height
													 CPU);
					// copy second half
					VDP_setTileMapEx(BG_A, bg_image_a.tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, indexA),
													 0,								// Plane X destination
													 0,								// plane Y destination
													 lastSrcColA + 1 + width1,					// Region X start position
													 0,								// Region Y start position
													 width2,					// width  ( will this wrap? )
													 28,							// height
													 CPU);

				} else {
					// just loop for now ( probably inefficient if there are a lot of columns to copy)
					s16 totalWidth = width1 + width2;
					s16 tmpDst = lastDstColA + 1;
					s16 tmpSrc = lastSrcColA + 1;
					for (s16 i = 0; i < totalWidth; ++i)
					{
						VDP_setTileMapEx(BG_A, bg_image_a.tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, indexA),
														 tmpDst,									// Plane X destination
														 0,												// plane Y destination
														 tmpSrc,									// Region X start position
														 0,												// Region Y start position
														 width2,									// width  ( will this wrap? )
														 28,											// height
														 CPU);

						tmpDst++;
						if( tmpDst > 511) {
							tmpDst = 0;
						}
						tmpSrc++;
						if( tmpSrc > 1279) {
							tmpSrc = 0;
						}
					}
				}
			}
			else if (dstCol > lastDstColA)
			{
				//
				int width = dstCol - lastDstColA;
				if (lastSrcColA + width < 160)
				{
					// we're not blowing past
					VDP_setTileMapEx(BG_A, bg_image_a.tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, indexA),
													 lastDstColA + 1, // Plane X destination
													 0,								// plane Y destination
													 lastSrcColA + 1,	// Region X start position
													 0,								// Region Y start position
													 width,						// width
													 28,							// height
													 CPU);
				}
				else
				{
					//  blowing past the end, need two copies
					s16 width1 = 159 - lastSrcColA;
					s16 width2 = width - width1;
					VDP_setTileMapEx(BG_A, bg_image_a.tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, indexA),
													 lastDstColA + 1, // Plane X destination
													 0,								// plane Y destination
													 lastSrcColA + 1,	// Region X start position
													 0,								// Region Y start position
													 width1,					// width
													 28,							// height
													 CPU);

					VDP_setTileMapEx(BG_A, bg_image_a.tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, indexA),
													 lastDstColA + 1 + width1, // Plane X destination
													 0,								// plane Y destination
													 0,								// Region X start position
													 0,								// Region Y start position
													 width2,					// width
													 28,							// height
													 CPU);
				}
			}
			lastDstColA = dstCol;
			lastSrcColA = srcCol;
		}

		offsetB += 1;
		if (offsetB > 511)
			offsetB = 0;
		imageOffsetB += 1;
		if (imageOffsetB > 1279)
			imageOffsetB = 0; // bg image is 1280 pixels wide

		if (offsetB % 8 == 0)
		{
			s16 dstCol = (offsetB + 504) / 8;
			if (dstCol > 63)
			{
				dstCol -= 64; // wrap to zero
			}

			s16 srcCol = (imageOffsetB + 512) / 8;
			if (srcCol > 159)
			{
				srcCol -= 160; // wrap to zero
			}

			KLog_S2("dstCol: ", dstCol, "srcCol: ", srcCol);
			VDP_setTileMapEx(BG_B, bg_image_b.tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, indexB),
											 dstCol, // Plane X destination
											 0,			 // plane Y destination
											 srcCol, // Region X start position
											 0,			 // Region Y start position
											 1,			 // width
											 28,		 // height
											 CPU);
		}

		VDP_setHorizontalScroll(BG_A, -sOffsetA); // negative moves plane to left, positive to right
		VDP_setHorizontalScroll(BG_B, -offsetB);

		// let SGDK do its thing
		SYS_doVBlankProcess();
	}
	return 0;
}
