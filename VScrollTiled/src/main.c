#include <genesis.h>
#include "resources.h"


/////////////////////////////////////////////////////////////////////
// Vertical Tiled Scrolling  
// 
// default SGDK width is 512 pixels (64 tiles)
#define PLANE_MAX_PIXEL 384
#define PLANE_MAX_HORIZONTAL_TILE 48
#define PLANE_MAX_VERTICAL_TILE 32

s16 vScrollB[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
s16 planeBDeltas[20] = {9, 9, 7, 7, 5, 5, 5, 4, 3, 2, 2, 3, 4, 5, 5, 5, 7, 7, 9, 9};



int main(bool hard)
{

  // SETUP backgroupd
  VDP_setBackgroundColor(16);
  VDP_setScreenWidth320();

  // Set Colors
  PAL_setPalette(PAL0, plane_b_pal.data, CPU);
  PAL_setColor(0, 0x0000);

  // set scrolling modes to support fake rotation
  VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_COLUMN);

  // get initial tile position in VRAM
  int ind = TILE_USER_INDEX;
  VDP_loadTileSet(plane_b.tileset, ind, DMA);

  // SImple image for BG_B.  We're not changing  it during the level
  VDP_drawImageEx(BG_B, &plane_b, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);


  while (TRUE)
  {
    // scroll the asteroids in BG_B
    for (int i = 0; i < 20; i++)
    {
      vScrollB[i] -= planeBDeltas[i];
    }

    VDP_setVerticalScrollTile(BG_B, 0, vScrollB, 20, DMA); // use array to set plane offsets
    SYS_doVBlankProcess();
  }
}
