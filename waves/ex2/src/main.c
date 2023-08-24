#include <genesis.h>
#include "resources.h"

#define TOTAL_LINES 224


int main(bool hard) {

  //////////////////////////////////////////////////////////////
  // setup screen and palettes
  VDP_setBackgroundColor(16);
  VDP_setScreenWidth320();

  PAL_setPalette( PAL0, bg_pal.data, CPU );
  PAL_setPalette( PAL1, fg_pal.data, CPU );

  //////////////////////////////////////////////////////////////
  // setup scrolling
  int ind = TILE_USER_INDEX;
  VDP_drawImageEx(BG_B, &bg, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
  ind += bg.tileset->numTile;
  VDP_drawImageEx(BG_A, &fg, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
  ind += fg.tileset->numTile;

  // use LINE scroll for horizontal 
  VDP_setScrollingMode( HSCROLL_LINE, VSCROLL_PLANE);

  s16 hScroll[TOTAL_LINES]; 
  memset( hScroll, 0, sizeof(hScroll));
  fix16 offsetPlaneSky = FIX16(0);
  fix16 landOffsets[TOTAL_LINES];
  memset( landOffsets, 0, sizeof(landOffsets));


  u16 sinPerLine = 5;    // Elements to jump in sin() per line. Larger values give us faster waves.
  fix16 amplitude = FIX16( 10.0 );  // Amplitude sets how big the waves are.
  s16 offset = -40;  // shift left a bit.
  for( u16 i = 0; i < TOTAL_LINES; ++i ) {
    hScroll[i] = fix16ToInt( fix16Mul(  sinFix16(i * sinPerLine ), amplitude ) ) + offset;
  }


  //////////////////////////////////////////////////////////////
  // main loop.
  u16 sinOffset = 0; // Step basically tells us where we're starting in the sin table.
  while(TRUE)
  {
    // read joypad to set sinewave parameters 
    u16 joypad  = JOY_readJoypad( JOY_1 );
    if( joypad & BUTTON_UP ) {
      sinPerLine++; 
    } else if( joypad & BUTTON_DOWN ) {
      sinPerLine--; 
    }
    if( joypad & BUTTON_LEFT ) {
      amplitude = fix16Sub( amplitude, FIX16(1));
    } else if( joypad & BUTTON_RIGHT ) {
      amplitude = fix16Add( amplitude, FIX16(1));
    }

    if( joypad & BUTTON_A) {
      sinPerLine = 5;    
      amplitude = FIX16( 10.0 );
    } 
    if( joypad & BUTTON_B) {
      sinPerLine = 10;    
      amplitude = FIX16( 30.0 );
    } 
    if( joypad & BUTTON_C) {
      sinPerLine = 80;     
      amplitude = FIX16( 0.5 );
    } 

    // write params to the screen.
    char message[40];
    char amps[5];
    fix16ToStr( amplitude, amps, 1 );
    strclr(message);
    sprintf( message, "sin per line: %d amplitude: %s  ", sinPerLine, amps );
    VDP_drawText(message, 3, 1 );

    //////////////////////////////////////////////////////////////
    // This is what matters right here:  
    //    calculate the offsets per line using SGDK's sin table
    //    and adjust with params
    sinOffset++; // move up in the sine table
    for( u16 i = 0; i < TOTAL_LINES; ++i ) {
        // compute horizontal offsets with sine table.
        hScroll[i] = fix16ToInt( fix16Mul(  sinFix16(( i + sinOffset ) * sinPerLine ), amplitude ) ) + offset;
    }

    // apply scrolling offsets 
    VDP_setHorizontalScrollLine (BG_B, 0, hScroll, 223, DMA_QUEUE);
    SYS_doVBlankProcess();

  }
  return 0;

}
