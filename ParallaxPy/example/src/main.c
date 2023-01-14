#include <genesis.h>
#include "resources.h"

s16 hScrollB[224];
fix32 fscroll[224];
s16 scrollStep = 0;

static scrollLeftLoop()
{
  ++scrollStep;
  fix32 fStep = FIX32(1.0);
  if (scrollStep < 80)
  {
    for (u16 row = 180; row < 224; ++row)
    {
      fscroll[row] = fix32Sub(fscroll[row], fStep);
      fStep = fix32Add(fStep, FIX32(0.0233));
    }
  }
  else
  {
    scrollStep = 0;
    memset(fscroll, 0, sizeof(fscroll));
  }
}

static scrollRightLoop()
{
  --scrollStep;
  fix32 fStep = FIX32(1.0);
  if (scrollStep >= 0)
  {
    for (u16 row = 180; row < 224; ++row)
    {
      fscroll[row] = fix32Add(fscroll[row], fStep);
      fStep = fix32Add(fStep, FIX32(0.0233));
    }
  }
  else
  {
    scrollStep = 80;             
    fix32 scroll = FIX32(-80.0); 
    for (u16 row = 180; row < 224; ++row)
    {
      fscroll[row] = scroll;
      scroll = fix32Sub(scroll, FIX32(1.8605));
    }
  }
}

int main(bool hard)
{
  memset(hScrollB, 0, sizeof(hScrollB));
  memset(fscroll, 0, sizeof(fscroll));
  VDP_setScreenWidth320();

  PAL_setPalette(PAL0, plane_b_pal.data, CPU);
  PAL_setColor(0, 0x0000);
  // set scrolling modes to support fake rotation
  VDP_setScrollingMode(HSCROLL_LINE, VSCROLL_PLANE);

  // get initial tile position in VRAM
  int ind = TILE_USER_INDEX;
  int indexB = ind;
  VDP_loadTileSet(plane_b.tileset, indexB, CPU);
  VDP_drawImageEx(BG_B, &plane_b, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, indexB), 0, 0, FALSE, TRUE);

  while (TRUE)
  {
    scrollRightLoop();
    //scrollLeftLoop();
    for (int i = 0; i < 224; i++) // Not very efficient.
    {
      hScrollB[i] = fix32ToInt(fscroll[i]);
    }
    VDP_setHorizontalScrollLine(BG_B, 0, hScrollB, 224, DMA);
    SYS_doVBlankProcess();
  }
}