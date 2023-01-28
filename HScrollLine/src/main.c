#include <genesis.h>
#include "resources.h"

#define TOTAL_LINES 224
#define FAR 89  
#define LAND_LINES TOTAL_LINES - FAR

int main(bool hard) {

  //////////////////////////////////////////////////////////////
  // setup screen and palettes
  VDP_setBackgroundColor(16);
  VDP_setScreenWidth320();

  PAL_setPalette( PAL0, desert_pal.data, CPU );

  //////////////////////////////////////////////////////////////
  // setup scrolling
  int ind = TILE_USER_INDEX;
  VDP_drawImageEx(BG_B, &desert, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
  ind += desert.tileset->numTile;

  // use LINE scroll for horizontal 
  VDP_setScrollingMode( HSCROLL_LINE, VSCROLL_PLANE);

  s16 hScroll[TOTAL_LINES]; 
  memset( hScroll, 0, sizeof(hScroll));
  fix16 offsetPlaneSky = FIX16(0);
  fix16 landOffsets[LAND_LINES];
  memset( landOffsets, 0, sizeof(landOffsets));

  while(TRUE)
  {
    // scroll the sky
    offsetPlaneSky = fix16Sub( offsetPlaneSky, FIX16(0.05));
    for (u16 i = 0; i <FAR; i ++) {
      hScroll[i] = fix16ToInt(offsetPlaneSky);
    }

    // scroll the land.  
    fix16 delta = FIX16(0.05);
    fix16 offset = FIX16(0.05);
    for (u16 i = FAR; i < 224; i ++) {
      // increase the amount we scroll as we get closer to the bottom of the screen.
      offset = fix16Add( offset, delta);
      landOffsets[i-FAR] = fix16Sub( landOffsets[i-FAR], offset);
      hScroll[i] = fix16ToInt(landOffsets[i-FAR]);
    }

    VDP_setHorizontalScrollLine (BG_B, 0, hScroll, 223, CPU);
    SYS_doVBlankProcess();

  }
  return 0;

}
