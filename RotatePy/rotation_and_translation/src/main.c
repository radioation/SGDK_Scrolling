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
  VDP_setScrollingMode(HSCROLL_LINE, VSCROLL_2TILE);

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

  s16 xOffset = 0;
  s16 yOffset = 0;
  s16 yOffsetDir = 1;
  u8 offsetDelay = 0;


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
    }
    ++offsetDelay;
    if( offsetDelay % 3 == 0 ) {
      yOffset += yOffsetDir;
      if( yOffset > 40) {
        yOffsetDir = -1;
      }else if( yOffset < 0 ) {
        yOffsetDir = 1;
      }
      if( currAngle < 4) {
        xOffset+=1;
      } else if ( currAngle > 6) {
        xOffset-=1;
      }
    }


    // could unroll loops to eliminate some overhead
    for(int i=0; i < ROWS_A; ++i ) {
      hScrollA[ i ] = _hScroll[ currAngle * ROWS_A + i] + xOffset;
    }
    for (int i = 0; i < COLS_A; ++i)
    {
      vScrollA[i] = _vScroll[currAngle * COLS_A + i] + yOffset;
    }


    // set SGDK scrolling functions to fake the rotation.
    s16 startHorizontalScroll = START_ROW_A - yOffset;
    s16 totalRows = ROWS_A ;
    if( startHorizontalScroll < 0 ) {
      totalRows = ROWS_A + startHorizontalScroll;
      startHorizontalScroll = 0;
    }
    VDP_setHorizontalScrollLine(BG_A, startHorizontalScroll, hScrollA, totalRows, DMA);
    VDP_setVerticalScrollTile(BG_A, START_COL_A, vScrollA, COLS_A, DMA);

    // let SGDK do its thing
    SYS_doVBlankProcess();
  }
  return 0;
}
