#include <genesis.h>
#include "resources.h"

s16 hScrollB[224];
fix32 fscroll[224];
s16 scrollStep = 0;

bool doUpdate = false;
int tick = 0;
s16 currentBackgroundStep = 6;

int indexA = 0;
int indexB_0 = 0;
int indexB_1 = 0;
int indexB_2 = 0;
int indexB_3 = 0;
int indexB_4 = 0;
int indexB_5 = 0;
int indexB_6 = 0;

void copyTiles6() {
    // copy tiles that matter to BG_B
    s16 offset = 0;
    for(int tx = 0; tx < 9; tx++)
    {
        u16 tile_id = indexB_6 + offset;
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, tile_id), 40 + tx, 22);
        ++offset;
    }
    for(int ty = 1; ty < 6; ty++)
    {
        for(int tx = 0; tx < 10; tx++)
        {
            u16 tile_id = indexB_6 + offset;
            VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, tile_id), 40 + tx, 22 + ty);
            ++offset;
        }
    }
}

void copyTiles5() {
    // 17 wide.
    s16 offset = 0;
    for(int tx = 0; tx < 9; tx++)
    {
        u16 tile_id = indexB_5 + offset;
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, tile_id), 33 + tx, 22);
        ++offset;
    }
    offset+=6;
    for(int tx = 0; tx < 10; tx++)
    {
        u16 tile_id = indexB_5 + offset;
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, tile_id), 33 + tx, 23);
        ++offset;
    }
    offset+=7;
    offset+=2;
    for(int tx = 2; tx < 12; tx++)
    {
        u16 tile_id = indexB_5 + offset;
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, tile_id), 33 + tx, 24);
        ++offset;
    }
    offset+=5;
    offset+=3;
    for(int tx = 3; tx < 13; tx++)
    {
        u16 tile_id = indexB_5 + offset;
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, tile_id), 33 + tx, 25);
        ++offset;
    }
    offset+=4;
    offset+=4;
    for(int tx = 4; tx < 16; tx++)
    {
        u16 tile_id = indexB_5 + offset;
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, tile_id), 33 + tx, 26);
        ++offset;
    }
    offset+=1;
    offset+=5;
    for(int tx = 5; tx < 17; tx++)
    {
        u16 tile_id = indexB_5 + offset;
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, tile_id), 33 + tx, 27);
        ++offset;
    }
   


}

void copyTiles4() {
    s16 offset = 0;
    for(int tx = 0; tx < 9; tx++)
    {
        u16 tile_id = indexB_4 + offset;
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, tile_id), 26 + tx, 22);
        ++offset;
    }
    offset+=5;
    for(int tx = 0; tx < 9; tx++)
    {
        u16 tile_id = indexB_4 + offset;
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, tile_id), 26 + tx, 23);
        ++offset;
    }
    offset+=5;
    offset+=1;
    for(int tx = 1; tx < 11; tx++)
    {
        u16 tile_id = indexB_4 + offset;
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, tile_id), 26 + tx, 24);
        ++offset;
    }
    offset+=3;
    offset+=2;
    for(int tx = 2; tx < 12; tx++)
    {
        u16 tile_id = indexB_4 + offset;
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, tile_id), 26 + tx, 25);
        ++offset;
    }
    offset+=2;
    offset+=2;
    for(int tx = 2; tx < 13; tx++)
    {
        u16 tile_id = indexB_4 + offset;
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, tile_id), 26 + tx, 26);
        ++offset;
    }
    offset+=1;
    offset+=3;
    for(int tx = 3; tx < 14; tx++)
    {
        u16 tile_id = indexB_4 + offset;
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, tile_id), 26 + tx, 27);
        ++offset;
    }


}

void copyTiles3() {
    s16 offset = 0;
    for(int tx = 0; tx < 7; tx++)
    {
        u16 tile_id = indexB_3 + offset;
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, tile_id), 20 + tx, 22);
        ++offset;
    }
    offset+=2;
    for(int tx = 0; tx < 8; tx++)
    {
        u16 tile_id = indexB_3 + offset;
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, tile_id), 20 + tx, 23);
        ++offset;
    }
    offset+=2;
    for(int tx = 0; tx < 9; tx++)
    {
        u16 tile_id = indexB_3 + offset;
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, tile_id), 20 + tx, 24);
        ++offset;
    }
    offset+=1;
    for(int tx = 0; tx < 9; tx++)
    {
        u16 tile_id = indexB_3 + offset;
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, tile_id), 20 + tx, 25);
        ++offset;
    }
    offset+=1;
    for(int tx = 0; tx < 10; tx++)
    {
        u16 tile_id = indexB_3 + offset;
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, tile_id), 20 + tx, 26);
        ++offset;
    }
    for(int tx = 0; tx < 10; tx++)
    {
        u16 tile_id = indexB_3 + offset;
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, tile_id), 20 + tx, 27);
        ++offset;
    }
}

void copyTiles2() {
    s16 offset = 3;
    for(int tx = 3; tx < 10; tx++)
    {
        u16 tile_id = indexB_2 + offset;
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, tile_id), 10 + tx, 22);
        ++offset;
    }
    offset+=2;
    for(int tx = 2; tx < 10; tx++)
    {
        u16 tile_id = indexB_2 + offset;
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, tile_id), 10 + tx, 23);
        ++offset;
    }
    offset+=1;
    for(int tx = 1; tx < 10; tx++)
    {
        u16 tile_id = indexB_2 + offset;
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, tile_id), 10 + tx, 24);
        ++offset;
    }
    offset+=1;
    for(int tx = 1; tx < 10; tx++)
    {
        u16 tile_id = indexB_2 + offset;
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, tile_id), 10 + tx, 25);
        ++offset;
    }
    for(int tx = 0; tx < 10; tx++)
    {
        u16 tile_id = indexB_2 + offset;
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, tile_id), 10 + tx, 26);
        ++offset;
    }
    for(int tx = 0; tx < 10; tx++)
    {
        u16 tile_id = indexB_2 + offset;
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, tile_id), 10 + tx, 27);
        ++offset;
    }
}

void copyTiles1() {
    s16 offset = 6;
    for(s16 tx = 6; tx < 11; tx++)
    {
        u16 tile_id = indexB_1 + offset;
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, tile_id), tx, 22);
        ++offset;
    }

    VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, indexB_1 + 8), 11, 22);

    for(s16 tx = 12; tx < 14; tx++)
    {
        u16 tile_id = indexB_1 + offset;
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, tile_id), tx, 22);
        ++offset;
    }


    offset+=4;
    for(s16 tx = 4; tx < 14; tx++)
    {
        u16 tile_id = indexB_1 + offset;
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, tile_id), tx, 23);
        ++offset;
    }
    offset+=3;
    for(s16 tx = 3; tx < 13; tx++)
    {
        u16 tile_id = indexB_1 + offset;
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, tile_id), tx, 24);
        ++offset;
    }
    offset+=1;
    offset+=2;
    for(s16 tx = 2; tx < 12; tx++)
    {
        u16 tile_id = indexB_1 + offset;
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, tile_id), tx, 25);
        ++offset;
    }
    offset+=2;
    offset+=1;
    for(s16 tx = 1; tx < 12; tx++)
    {
        u16 tile_id = indexB_1 + offset;
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, tile_id), tx, 26);
        ++offset;
    }
    offset+=2;
    for(s16 tx = 0; tx < 11; tx++)
    {
        u16 tile_id = indexB_1 + offset;
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, tile_id), tx, 27);
        ++offset;
    }


}

void copyTiles0() {
    s16 offset = 0;
    for(s16 ty = 0; ty < 6; ty++)
    {
        for(s16 tx = 0; tx < 7; tx++)
        {
            u16 tile_id = indexB_0 + offset;
            VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, tile_id), tx, 22 + ty);
            ++offset;
        }
    }
}


static void updateBackground() {
    switch( currentBackgroundStep ) {
        case 6:
            break;
        case 5:
            break;
        default:
            break;
    }
}


static void scrollLeft() {
    ++scrollStep;
    if (scrollStep < 53 ) {

        fscroll[180] = fscroll[180] - FIX32( 1.000 );
        fscroll[181] = fscroll[181] - FIX32( 1.012 );
        fscroll[182] = fscroll[182] - FIX32( 1.023 );
        fscroll[183] = fscroll[183] - FIX32( 1.035 );
        fscroll[184] = fscroll[184] - FIX32( 1.047 );
        fscroll[185] = fscroll[185] - FIX32( 1.058 );
        fscroll[186] = fscroll[186] - FIX32( 1.070 );
        fscroll[187] = fscroll[187] - FIX32( 1.081 );
        fscroll[188] = fscroll[188] - FIX32( 1.093 );
        fscroll[189] = fscroll[189] - FIX32( 1.105 );
        fscroll[190] = fscroll[190] - FIX32( 1.116 );
        fscroll[191] = fscroll[191] - FIX32( 1.128 );
        fscroll[192] = fscroll[192] - FIX32( 1.140 );
        fscroll[193] = fscroll[193] - FIX32( 1.151 );
        fscroll[194] = fscroll[194] - FIX32( 1.163 );
        fscroll[195] = fscroll[195] - FIX32( 1.174 );
        fscroll[196] = fscroll[196] - FIX32( 1.186 );
        fscroll[197] = fscroll[197] - FIX32( 1.198 );
        fscroll[198] = fscroll[198] - FIX32( 1.209 );
        fscroll[199] = fscroll[199] - FIX32( 1.221 );
        fscroll[200] = fscroll[200] - FIX32( 1.233 );
        fscroll[201] = fscroll[201] - FIX32( 1.244 );
        fscroll[202] = fscroll[202] - FIX32( 1.256 );
        fscroll[203] = fscroll[203] - FIX32( 1.267 );
        fscroll[204] = fscroll[204] - FIX32( 1.279 );
        fscroll[205] = fscroll[205] - FIX32( 1.291 );
        fscroll[206] = fscroll[206] - FIX32( 1.302 );
        fscroll[207] = fscroll[207] - FIX32( 1.314 );
        fscroll[208] = fscroll[208] - FIX32( 1.326 );
        fscroll[209] = fscroll[209] - FIX32( 1.337 );
        fscroll[210] = fscroll[210] - FIX32( 1.349 );
        fscroll[211] = fscroll[211] - FIX32( 1.360 );
        fscroll[212] = fscroll[212] - FIX32( 1.372 );
        fscroll[213] = fscroll[213] - FIX32( 1.384 );
        fscroll[214] = fscroll[214] - FIX32( 1.395 );
        fscroll[215] = fscroll[215] - FIX32( 1.407 );
        fscroll[216] = fscroll[216] - FIX32( 1.419 );
        fscroll[217] = fscroll[217] - FIX32( 1.430 );
        fscroll[218] = fscroll[218] - FIX32( 1.442 );
        fscroll[219] = fscroll[219] - FIX32( 1.453 );
        fscroll[220] = fscroll[220] - FIX32( 1.465 );
        fscroll[221] = fscroll[221] - FIX32( 1.477 );
        fscroll[222] = fscroll[222] - FIX32( 1.488 );
        fscroll[223] = fscroll[223] - FIX32( 1.500 );
    } else {
        scrollStep = 0;
        memset(fscroll, 0, sizeof(fscroll));
        if( doUpdate ) {
            switch ( currentBackgroundStep ) {
                case 6:
                    copyTiles6();
                    break;
                case 5:
                    copyTiles5();
                    break;
                case 4:
                    copyTiles4();
                    break;
                case 3:
                    copyTiles3();
                    break;
                case 2:
                    copyTiles2();
                    break;
                case 1:
                    copyTiles1();
                    break;
                case 0:
                    copyTiles0();
                    break;
            }
            currentBackgroundStep--;
            if( currentBackgroundStep < 0 ) {   
                doUpdate = false;
            }
        } 
    }
}

static void scrollRight() {
    --scrollStep;
    if (scrollStep >= 0) {

        fscroll[180] = fscroll[180] + FIX32( 1.000 );
        fscroll[181] = fscroll[181] + FIX32( 1.012 );
        fscroll[182] = fscroll[182] + FIX32( 1.023 );
        fscroll[183] = fscroll[183] + FIX32( 1.035 );
        fscroll[184] = fscroll[184] + FIX32( 1.047 );
        fscroll[185] = fscroll[185] + FIX32( 1.058 );
        fscroll[186] = fscroll[186] + FIX32( 1.070 );
        fscroll[187] = fscroll[187] + FIX32( 1.081 );
        fscroll[188] = fscroll[188] + FIX32( 1.093 );
        fscroll[189] = fscroll[189] + FIX32( 1.105 );
        fscroll[190] = fscroll[190] + FIX32( 1.116 );
        fscroll[191] = fscroll[191] + FIX32( 1.128 );
        fscroll[192] = fscroll[192] + FIX32( 1.140 );
        fscroll[193] = fscroll[193] + FIX32( 1.151 );
        fscroll[194] = fscroll[194] + FIX32( 1.163 );
        fscroll[195] = fscroll[195] + FIX32( 1.174 );
        fscroll[196] = fscroll[196] + FIX32( 1.186 );
        fscroll[197] = fscroll[197] + FIX32( 1.198 );
        fscroll[198] = fscroll[198] + FIX32( 1.209 );
        fscroll[199] = fscroll[199] + FIX32( 1.221 );
        fscroll[200] = fscroll[200] + FIX32( 1.233 );
        fscroll[201] = fscroll[201] + FIX32( 1.244 );
        fscroll[202] = fscroll[202] + FIX32( 1.256 );
        fscroll[203] = fscroll[203] + FIX32( 1.267 );
        fscroll[204] = fscroll[204] + FIX32( 1.279 );
        fscroll[205] = fscroll[205] + FIX32( 1.291 );
        fscroll[206] = fscroll[206] + FIX32( 1.302 );
        fscroll[207] = fscroll[207] + FIX32( 1.314 );
        fscroll[208] = fscroll[208] + FIX32( 1.326 );
        fscroll[209] = fscroll[209] + FIX32( 1.337 );
        fscroll[210] = fscroll[210] + FIX32( 1.349 );
        fscroll[211] = fscroll[211] + FIX32( 1.360 );
        fscroll[212] = fscroll[212] + FIX32( 1.372 );
        fscroll[213] = fscroll[213] + FIX32( 1.384 );
        fscroll[214] = fscroll[214] + FIX32( 1.395 );
        fscroll[215] = fscroll[215] + FIX32( 1.407 );
        fscroll[216] = fscroll[216] + FIX32( 1.419 );
        fscroll[217] = fscroll[217] + FIX32( 1.430 );
        fscroll[218] = fscroll[218] + FIX32( 1.442 );
        fscroll[219] = fscroll[219] + FIX32( 1.453 );
        fscroll[220] = fscroll[220] + FIX32( 1.465 );
        fscroll[221] = fscroll[221] + FIX32( 1.477 );
        fscroll[222] = fscroll[222] + FIX32( 1.488 );
        fscroll[223] = fscroll[223] + FIX32( 1.500 );

    } else {
        scrollStep = 53;

        fscroll[180] =  FIX32(-53.000);
        fscroll[181] =  FIX32(-53.620);
        fscroll[182] =  FIX32(-54.240);
        fscroll[183] =  FIX32(-54.860);
        fscroll[184] =  FIX32(-55.481);
        fscroll[185] =  FIX32(-56.101);
        fscroll[186] =  FIX32(-56.721);
        fscroll[187] =  FIX32(-57.341);
        fscroll[188] =  FIX32(-57.961);
        fscroll[189] =  FIX32(-58.581);
        fscroll[190] =  FIX32(-59.202);
        fscroll[191] =  FIX32(-59.822);
        fscroll[192] =  FIX32(-60.442);
        fscroll[193] =  FIX32(-61.062);
        fscroll[194] =  FIX32(-61.682);
        fscroll[195] =  FIX32(-62.302);
        fscroll[196] =  FIX32(-62.922);
        fscroll[197] =  FIX32(-63.543);
        fscroll[198] =  FIX32(-64.163);
        fscroll[199] =  FIX32(-64.783);
        fscroll[200] =  FIX32(-65.403);
        fscroll[201] =  FIX32(-66.023);
        fscroll[202] =  FIX32(-66.643);
        fscroll[203] =  FIX32(-67.264);
        fscroll[204] =  FIX32(-67.884);
        fscroll[205] =  FIX32(-68.504);
        fscroll[206] =  FIX32(-69.124);
        fscroll[207] =  FIX32(-69.744);
        fscroll[208] =  FIX32(-70.364);
        fscroll[209] =  FIX32(-70.984);
        fscroll[210] =  FIX32(-71.605);
        fscroll[211] =  FIX32(-72.225);
        fscroll[212] =  FIX32(-72.845);
        fscroll[213] =  FIX32(-73.465);
        fscroll[214] =  FIX32(-74.085);
        fscroll[215] =  FIX32(-74.705);
        fscroll[216] =  FIX32(-75.326);
        fscroll[217] =  FIX32(-75.946);
        fscroll[218] =  FIX32(-76.566);
        fscroll[219] =  FIX32(-77.186);
        fscroll[220] =  FIX32(-77.806);
        fscroll[221] =  FIX32(-78.426);
        fscroll[222] =  FIX32(-79.047);
        fscroll[223] =  FIX32(-79.667);
    }
}

int main(bool hard)
{
    memset(hScrollB, 0, sizeof(hScrollB));
    memset(fscroll, 0, sizeof(fscroll));
    VDP_setScreenWidth320();

    PAL_setPalette(PAL0, bga_pal.data, CPU);
    PAL_setColor(0, 0x0000);
    // set scrolling modes to support line scrolling.
    VDP_setScrollingMode(HSCROLL_LINE, VSCROLL_PLANE);

    // get initial tile position in VRAM and load image
    int ind = TILE_USER_INDEX;
    VDP_loadTileSet(bga.tileset, indexA, CPU);
    indexA= ind;
    indexB_0 = indexA + bga.tileset->numTile; // next position
    VDP_loadTileSet(bgb_0.tileset, indexB_0, CPU);

    indexB_1 = indexB_0 + bgb_0.tileset->numTile; // next position
    VDP_loadTileSet(bgb_1.tileset, indexB_1, CPU);

    indexB_2 = indexB_1 + bgb_1.tileset->numTile; // next position
    VDP_loadTileSet(bgb_2.tileset, indexB_2, CPU);

    indexB_3 = indexB_2 + bgb_2.tileset->numTile; // next position
    VDP_loadTileSet(bgb_3.tileset, indexB_3, CPU);

    indexB_4 = indexB_3 + bgb_3.tileset->numTile; // next position
    VDP_loadTileSet(bgb_4.tileset, indexB_4, CPU);

    indexB_5 = indexB_4 + bgb_4.tileset->numTile; // next position
    VDP_loadTileSet(bgb_5.tileset, indexB_5, CPU);

    indexB_6 = indexB_5 + bgb_5.tileset->numTile; // next position
    VDP_loadTileSet(bgb_6.tileset, indexB_6, CPU);

    VDP_drawImageEx(BG_B, &bga, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, indexA), 0, 0, FALSE, TRUE);

    while (TRUE)
    {
        tick++;
        if( tick == 200 ) {
          doUpdate = true;
        }
/*
        if( tick == 100 ) {
            // Populate a block using XY or DataRect:
//            copyTiles6();
        } else if( tick == 150 ) {
            copyTiles5();
        } else if( tick == 200 ) {
            copyTiles4();
        } else if( tick == 250 ) {
            copyTiles3();
        } else if( tick == 300 ) {
            copyTiles2();
        } else if( tick == 350 ) {
            copyTiles1();
        } else if( tick == 400 ) {
            copyTiles0();
        }*/


        scrollLeft();
        //scrollRight();
        for (int i = 0; i < 224; i++) // Not very efficient, but works for a demo
        {
            hScrollB[i] = fix32ToInt(fscroll[i]);
        }
        VDP_setHorizontalScrollLine(BG_B, 0, hScrollB, 224, DMA);
        SYS_doVBlankProcess();
    }
}

