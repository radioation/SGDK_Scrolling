#include <genesis.h>
#include "resources.h"


s16 offsetA = 0;
s16 pixelA = 0;
s16 offsetB = 0;
s16 pixelB = 0;


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
									 DMA);
	VDP_setTileMapEx(BG_B, bg_image_b.tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, indexB),
									 0,
									 0,
									 0,
									 0,
									 64,
									 28,
									 DMA);


	while (TRUE)
	{
		/*
		// Set the scrolling position
		++offsetA;
		if( offsetA > 1279) offsetA = 0;
		++pixelA;
		if( pixelA > 7 ) {
			pixelA = 0;
		} 

    if(pixelA==0)
    {
        s16 colNum = (((offsetA + 320) >> 3)+ 23) & 127;

        //VDP_setMapEx(BG_A, bg_image_a.tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, indexA),
        //            colNum, 0, colNum, 0, 1, 28);
        VDP_setTileMapEx(BG_A, bg_image_a.tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, indexA),
                    colNum, // Plane X destination 
										0, 			// plane Y destination
										colNum + 64,  // Region X start position
										0,  		// Region Y start position
										1, 			// width
										28,			// height
										DMA
										);
    }
*/

		++offsetB;
		if (offsetB > 1279)
			offsetB = 0;
		++pixelB;
		if (pixelB > 7)
		{
			pixelB = 0;
		}

		VDP_setHorizontalScroll(BG_A, -offsetA); // negative moves plane to left, positive to right

		VDP_setHorizontalScroll(BG_B, 0);

		// let SGDK do its thing
		SYS_doVBlankProcess();
	}
	return 0;
}
