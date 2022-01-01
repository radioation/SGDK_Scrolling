#include <genesis.h>
#include "resources.h"


// Map Position Variables.  These are what matter in this example.
fix32 mapPosX;
fix32 mapPosY;


int main( u16 hard ) {
	
  VDP_setPalette( PAL1, plane_a_map.palette->data);

	// get our position in VRAM.
	int ind = TILE_USERINDEX; 
	int indexA = ind;
	// Load the plane tiles into VRAM
	ind += VDP_loadTileSet( &plane_a_tileset, ind, DMA );

	// init plane
	Map *map_a = MAP_create( &plane_a_map, BG_A, TILE_ATTR_FULL( PAL1, FALSE, FALSE, FALSE, indexA ) );

	int indexB = ind + plane_a_tileset.numTile; // new 
	VDP_loadTileSet( &plane_b_tileset, indexB, DMA );
	Map *map_b = MAP_create( &plane_b_map, BG_B, TILE_ATTR_FULL( PAL1, FALSE, FALSE, FALSE, indexB ) );

	// set initial map position
	mapPosX = FIX32(0);
	mapPosY = FIX32(0);

	MAP_scrollTo( map_a, fix32ToInt(mapPosX), fix32ToInt(mapPosY ) );

	while(TRUE) {
					// Set the scrolling position 
					MAP_scrollTo( map_a, fix32ToInt(mapPosX), fix32ToInt(mapPosY ) );
					MAP_scrollTo( map_b, fix32ToInt(mapPosX), fix32ToInt(mapPosY ) );

					SYS_doVBlankProcess();
	}	
	return 0;
}

