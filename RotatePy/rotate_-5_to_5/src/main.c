#include <genesis.h>
#include "resources.h"
#include "rotation.h"

/////////////////////////////////////////////////////////////////////
// Scrolling Stuff
#define PLANE_MAX_TILE 64

s16 hScrollA[224];
s16 vScrollA[20];


int main(bool hard)
{
  memset(hScrollA, 0, sizeof(hScrollA));
  memset(vScrollA, 0, sizeof(vScrollA));
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

  s16 currAngle = 5;
  s16 stepDir = 1;
  u8 angleDelay = 0;
  while (TRUE)
  {
    // handle rotation
    ++angleDelay;
    if (angleDelay % 6 == 0)
    {
      currAngle += stepDir;
      if (currAngle >= _SCROLL_COUNT)
      {
        stepDir = -1;
        currAngle = 10;
      }
      else if (currAngle < 0)
      {
        stepDir = 1;
        currAngle = 0;
      }

      // Copy current angle offsets into local arrays
      memcpy(hScrollA, _hScroll + currAngle * ROWS_A, ROWS_A * sizeof(s16));
      memcpy(vScrollA, _vScroll + currAngle * COLS_A, COLS_A * sizeof(s16));
    }

    // set SGDK scrolling functions with local arrays to fake the rotation.
    VDP_setHorizontalScrollLine(BG_A, START_ROW_A, hScrollA, ROWS_A, CPU);
    VDP_setVerticalScrollTile(BG_A, START_COL_A, vScrollA, COLS_A, CPU);

    // let SGDK do its thing
    SYS_doVBlankProcess();
  }
  return 0;
}