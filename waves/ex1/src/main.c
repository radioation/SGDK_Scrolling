#include <genesis.h>
#include "resources.h"

#define TOTAL_LINES 224


int main(bool hard) {

  //////////////////////////////////////////////////////////////
  // setup screen and palettes
  VDP_setBackgroundColor(16);
  VDP_setScreenWidth320();

  PAL_setPalette( PAL0, bg_pal.data, CPU );

  //////////////////////////////////////////////////////////////
  // setup scrolling
  int ind = TILE_USER_INDEX;
  VDP_drawImageEx(BG_B, &bg, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
  ind += bg.tileset->numTile;

  // use LINE scroll for horizontal 
  VDP_setScrollingMode( HSCROLL_LINE, VSCROLL_PLANE);

  s16 hScroll[TOTAL_LINES]; 
  memset( hScroll, 0, sizeof(hScroll));
  fix16 offsetPlaneSky = FIX16(0);
  fix16 landOffsets[TOTAL_LINES];
  memset( landOffsets, 0, sizeof(landOffsets));


  u16 rads = 5; // radians per step.   
  fix16 amplitude = FIX16( 20.0 );
  s16 offset = -40;  // shift left a bit.
  for( u16 i = 0; i < TOTAL_LINES; ++i ) {
    hScroll[i] = fix16ToInt( fix16Mul(  sinFix16(i * rads ), amplitude ) ) + offset;
  }


  //////////////////////////////////////////////////////////////
  // main loop.
  u16 step = 0;
  while(TRUE)
  {
    // calculate the offsets using SGDK's sin table.
    step++;
    for( u16 i = 0; i < TOTAL_LINES; ++i ) {
      hScroll[i] = fix16ToInt( fix16Mul(  sinFix16(( i + step ) * rads ), amplitude ) ) + offset;
    }

    // apply scrolling
    VDP_setHorizontalScrollLine (BG_B, 0, hScroll, 223, DMA_QUEUE);
    SYS_doVBlankProcess();

  }
  return 0;

}
