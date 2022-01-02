#include <genesis.h>
#include "resources.h"


s16 offsetA = 0;
s16 imageOffsetA = 0;

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
		++offsetA;
		if( offsetA > 511) offsetA = 0;
		++imageOffsetA;
		if( imageOffsetA > 1279) imageOffsetA = 0; // bg image is 1280 pixels wide

    if( offsetA % 8 == 0)
		{
			// get destination column (in tiles)
			s16 dstCol = (offsetA + 504) / 8;
			if( dstCol > 63 ) {
				dstCol -= 64; // wrap to zero
			} 

			// get source column (in tiles)
			s16 srcCol = (imageOffsetA + 512)/8;
			if( srcCol > 159  ) {
				srcCol -= 160; // wrap to zero
			} 

			VDP_setTileMapEx(BG_A, bg_image_a.tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, indexA),
											 dstCol,			// Plane X destination
											 0,						// plane Y destination
											 srcCol, // Region X start position
											 0,						// Region Y start position
											 1,						// width
											 28,					// height
											 CPU);
		}

		++offsetB;
		if( offsetB > 511) offsetB = 0;
		++imageOffsetB;
		if( imageOffsetB > 1279) imageOffsetB = 0; // bg image is 1280 pixels wide

    if( offsetB % 8 == 0)
		{
			s16 dstCol = (offsetB + 504) / 8;
			if( dstCol > 63 ) {
				dstCol -= 64; // wrap to zero
			} 

			s16 srcCol = (imageOffsetB + 512)/8;
			if( srcCol > 159  ) {
				srcCol -= 160; // wrap to zero
			} 

			KLog_S2( "dstCol: ", dstCol, "srcCol: ", srcCol);
			VDP_setTileMapEx(BG_B, bg_image_b.tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, indexB),
											 dstCol,			// Plane X destination
											 0,						// plane Y destination
											 srcCol, // Region X start position
											 0,						// Region Y start position
											 1,						// width
											 28,					// height
											 CPU);
		}

		VDP_setHorizontalScroll(BG_A, -offsetA); // negative moves plane to left, positive to right
		VDP_setHorizontalScroll(BG_B, -offsetB);

		// let SGDK do its thing
		SYS_doVBlankProcess();
	}
	return 0;
}
