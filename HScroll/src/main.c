#include <genesis.h>
#include "resources.h"


s16 offset = 0;
s16 imageOffset = 0;

int main( u16 hard ) {
	
  VDP_setPalette( PAL0, bg_image.palette->data);

	// set scrolling mode.  HSCROLL_PLANE Affects the WHOLE plane
	VDP_setScrollingMode( HSCROLL_PLANE, VSCROLL_PLANE);

	// get our position in VRAM.
	int ind = TILE_USERINDEX; 
	// Load the plane tiles into VRAM at our position
	VDP_loadTileSet( bg_image.tileset, ind, DMA );

	// put out the image
	VDP_setTileMapEx(BG_A, bg_image.tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind),
									 0,			// Plane X destination
									 0,			// plane Y destination
									 0, 		// Region X start position
									 0,			// Region Y start position
									 64,		// width  (went with 64 becasue default width is 64.  Viewable screen is 40)
									 28,		// height
									 CPU);

	while (TRUE)
	{
		// tile in memory
		offset += 1;
		if (offset > 511)
			offset = 0;

		// tile from image
		imageOffset += 1;
		if (imageOffset > 1279)
			imageOffset = 0; // bg image is 1280 pixels wide

		if (offset % 8 == 0)
		{
			s16 dstCol = (offset + 504) / 8;
			if (dstCol > 63)
			{
				dstCol -= 64; // wrap to zero
			}

			s16 srcCol = (imageOffset + 512) / 8;
			if (srcCol > 159)
			{
				srcCol -= 160; // wrap to zero
			}

			KLog_S2("dstCol: ", dstCol, "srcCol: ", srcCol);
			VDP_setTileMapEx(BG_A, bg_image.tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind),
											 dstCol, // Plane X destination
											 0,			 // plane Y destination
											 srcCol, // Region X start position
											 0,			 // Region Y start position
											 1,			 // width
											 28,		 // height
											 CPU);
		}

		VDP_setHorizontalScroll(BG_A, -offset);

		// let SGDK do its thing
		SYS_doVBlankProcess();
	}
	return 0;
}
