#include <genesis.h>
#include "resources.h"
#include "rotation.h"

/////////////////////////////////////////////////////////////////////
// Scrolling Stuff
#define PLANE_MAX_TILE 64

int main(bool hard)
{
  VDP_setScreenWidth320();
  // set colors
  PAL_setPalette( PAL0, platform_pal.data, CPU );

  // set scrolling mode to LINE for horizontal and TILE for vertical
  VDP_setScrollingMode(HSCROLL_LINE, VSCROLL_COLUMN);

  // get tile positions in VRAM.
  int ind = TILE_USER_INDEX;
  int indexA = ind;
  // Load the plane tiles into VRAM
  VDP_loadTileSet(platform.tileset, ind, DMA);

  // setup the tiles
  VDP_setTileMapEx(BG_A, platform.tilemap, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, indexA),
                   0,               // Plane X destination
                   0,               // plane Y destination
                   0,               // Region X start position
                   0,               // Region Y start position
                   PLANE_MAX_TILE, // width  (went with 64 becasue default width is 64.  Viewable screen is 40)
                   28,             // height
                   CPU);

  // set SGDK scrolling functions to fake the rotaiton.
  VDP_setHorizontalScrollLine(BG_A, START_ROW_A, _hScroll, ROWS_A, CPU);
  VDP_setVerticalScrollTile(BG_A, START_COL_A, _vScroll, COLS_A, CPU);

  while (TRUE)
  {
    // let SGDK do its thing
    SYS_doVBlankProcess();
  }
  return 0;
}