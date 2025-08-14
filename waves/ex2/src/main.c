#include <genesis.h>
#include "resources.h"

#define TOTAL_LINES 224

#define BUBBLE_COUNT 20

typedef struct 
{
  Sprite *sprite;
  s16 pos_x;
  s16 pos_y;
  s16 vel_y;
  u16 sinPerLine;
  fix16 amplitude;

} CP_SPRITE;

CP_SPRITE bubbles[BUBBLE_COUNT];

void createBubbles() {
  for( u16 i=0; i < BUBBLE_COUNT; ++i ) {
    bubbles[i].pos_x =  random() % ( 320 - 32);
    bubbles[i].pos_y =  random() % ( 224 - 32);
    bubbles[i].vel_y =  random() % ( 4) + 1;
    bubbles[i].sinPerLine =  random() % 5 + 5;
    bubbles[i].amplitude = FIX16( random() % 7 + 10 ); 
    bubbles[i].sprite = SPR_addSprite( &bubble, F16_toInt( bubbles[i].pos_x),  bubbles[i].pos_y, TILE_ATTR( PAL1, 0, FALSE, FALSE));
  }
}

void updateBubbles() {
  for( u16 i=0; i < BUBBLE_COUNT; ++i ) {
    bubbles[i].pos_y -= bubbles[i].vel_y;
    if( bubbles[i].pos_y < -32 ) {
      bubbles[i].pos_y = 224;
    }
    s16 x = F16_toInt( F16_mul(  sinFix16( bubbles[i].pos_y * bubbles[i].sinPerLine ), bubbles[i].amplitude ) ) + bubbles[i].pos_x;
    SPR_setPosition( bubbles[i].sprite, x, bubbles[i].pos_y);
  }

}

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

  // background
  s16 hScrollB[TOTAL_LINES]; 
  memset( hScrollB, 0, sizeof(hScrollB));


  u16 sinPerLine = 5;    // Elements to jump in sin() per line. Larger values give us faster waves.
  fix16 amplitude = FIX16( 10.0 );  // Amplitude sets how big the waves are.
  s16 offsetB = -10;  // shift left a bit.
  fix16 fOffsetB = FIX16(-10);  // shift left a bit.


  // foreground
  s16 hScrollA[TOTAL_LINES]; 
  s16 offsetA = 0;  // shift left a bit.
  s16 aDir = -1;
  memset( hScrollA, 0, sizeof(hScrollA));


  //////////////////////////////////////////////////////////////
  // sprites 
  SPR_init();
  createBubbles(); 

  //////////////////////////////////////////////////////////////
  // main loop.
  u16 sinOffset = 0; // Step basically tells us where we're starting in the sin table.
  while(TRUE)
  {
    // read joypad to set horizontal scrolling positions.
    u16 joypad  = JOY_readJoypad( JOY_1 );
    if( joypad & BUTTON_LEFT ) {
      offsetA +=2;
      if ( offsetA > 0 ) {
        offsetA = 0;
      } else {
        fOffsetB = fOffsetB + FIX16(0.75);
        offsetB = F16_toInt( fOffsetB );
      }
    } else if( joypad & BUTTON_RIGHT ) {
      offsetA -=2;
      if ( offsetA < -158 ) {
        offsetA = -158;
      } else {
        fOffsetB = fOffsetB - FIX16(0.75);
        offsetB = F16_toInt( fOffsetB );
      }
    }


    //////////////////////////////////////////////////////////////
    // Wavy scrolling
    //    calculate the offsets per line using SGDK's sin table
    //    and adjust with params
    sinOffset++; // move up in the sine table
    for( u16 i = 0; i < TOTAL_LINES; ++i ) {
      // compute horizontal offsets with sine table.
      hScrollB[i] = F16_toInt( F16_mul(  sinFix16(( i + sinOffset ) * sinPerLine ), amplitude ) ) + offsetB;
      hScrollA[i] = offsetA;
    }


    //////////////////////////////////////////////////////////////
    // update sprites
    updateBubbles(); 
    SPR_update();

    // apply scrolling offsets 
    VDP_setHorizontalScrollLine (BG_B, 0, hScrollB, 223, DMA_QUEUE);
    VDP_setHorizontalScrollLine (BG_A, 0, hScrollA, 223, DMA_QUEUE);
    SYS_doVBlankProcess();

  }
  return 0;

}
