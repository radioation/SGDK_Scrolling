#include <genesis.h>
#include "resources.h"


/////////////////////////////////////////////////////////////////////
// Scrolling  
// 
// default SGDK width is 512 pixels (64 tiles)
#define PLANE_MAX_PIXEL 384
#define PLANE_MAX_HORIZONTAL_TILE 48
#define PLANE_MAX_VERTICAL_TILE 32
s16 hScrollA[224];
s16 vScrollA[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
s16 vScrollUpperA[20];
s16 vScrollLowerA[20];
s16 planeADeltas[20] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

s16 vScrollB[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
s16 planeBDeltas[20] = {9, 9, 7, 7, 5, 5, 5, 4, 3, 2, 2, 3, 4, 5, 5, 5, 7, 7, 9, 9};
//s16 planeBDeltas[20] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

// setup fake rotation by changing the foreground horizontal and vertical scrolling
void setAngle( u16 angle,  s16 startY, s16 endY, s16 centerY, s16 vscroll[] ) {
	 for( int row = startY; row < endY; ++row ){
	 	fix32 shift = fix32Mul(FIX32( (row - centerY)>>1 ), sinFix32(angle));	
		hScrollA[row] = fix32ToInt( shift ) - 32;
	 } 

	// vertical scroll tiles are 16 pixels wide.  Using 8 * (col-10) to scale the scrolling effect
	// at the extreme left and right of the screen  the factor would be -80 and + 80
	for( int col = 1; col < 19; ++col ){
		fix32 shift = fix32Mul(FIX32(   (col - 9)<<3 ), sinFix32(angle));	
		vscroll[col] = fix32ToInt( shift );
	} 
}




/////////////////////////////////////////////////////////////////////
//  Interrupt handlers
//
static vu16  lineDisplay   = 0;             // line position on display screen

HINTERRUPT_CALLBACK HIntHandler()
{
	if( lineDisplay == 104  ) {
		// set vertical rotation component for lwoer part of BG_A
		memcpy( vScrollA, vScrollLowerA, sizeof(vScrollLowerA));
		VDP_setVerticalScrollTile(BG_A, 0, vScrollA, 20, CPU);
	} 
  // Count raster lines
  lineDisplay++;

}
void VBlankHandler()
{
    // Reset to line 0
    lineDisplay = 0;

		// set vertical rotation component for upper part of BG_A
		memcpy( vScrollA, vScrollUpperA, sizeof(vScrollUpperA));
		VDP_setVerticalScrollTile(BG_A, 0, vScrollA, 20, CPU);
 }



/////////////////////////////////////////////////////////////////////
// Player 
#define LEFT_EDGE 0
#define RIGHT_EDGE 320
#define TOP_EDGE 0
#define BOTTOM_EDGE 224

int shipAnim = 5;
// sprite info
struct CP_SPRITE
{
	Sprite *sprite;
	int pos_x;
	int pos_y;
	int vel_x;
	int vel_y;

	int hitbox_x1;
	int hitbox_y1;
	int hitbox_x2;
	int hitbox_y2;

	bool active;
	int state;
};

struct CP_SPRITE shipSprite;



void myJoyHandler(u16 joy, u16 changed, u16 state)
{
	/*Set player velocity if left or right are pressed;
	 *set velocity to 0 if no direction is pressed */
	if (state & BUTTON_RIGHT)
	{
		shipSprite.vel_x = 2;
	}
	else if (state & BUTTON_LEFT)
	{
		shipSprite.vel_x = -2;
	}
	else
	{
		if ((changed & BUTTON_RIGHT) | (changed & BUTTON_LEFT))
		{
			shipSprite.vel_x = 0;
		}
	}

	if (state & BUTTON_UP)
	{
		shipSprite.vel_y = -2;
	}
	else if (state & BUTTON_DOWN)
	{
		shipSprite.vel_y = 2;
	}
	else
	{
		if ((changed & BUTTON_UP) | (changed & BUTTON_DOWN))
		{
			shipSprite.vel_y = 0;
		}
	}
}

void update()
{
	// Check horizontal bounds
	if (shipSprite.pos_x < LEFT_EDGE)
	{
		shipSprite.pos_x = LEFT_EDGE;
		shipSprite.vel_x = -shipSprite.vel_x;
	}
	else if (shipSprite.pos_x + (shipSprite.hitbox_x2 - shipSprite.hitbox_x1) > RIGHT_EDGE)
	{
		shipSprite.pos_x = RIGHT_EDGE - (shipSprite.hitbox_x2 - shipSprite.hitbox_x1);
		shipSprite.vel_x = -shipSprite.vel_x;
	}

	if (shipSprite.vel_x == 0)
	{
		if (shipAnim > 5)
		{
			--shipAnim;
		}
		else if (shipAnim < 5)
		{
			++shipAnim;
		}
	}
	else
	{
		if (shipSprite.vel_x > 0)
		{
			++shipAnim;
			if (shipAnim > 10)
			{
				shipAnim = 10;
			}
		}
		else
		{
			--shipAnim;
			if (shipAnim < 0)
			{
				shipAnim = 0;
			}
		}
	}

	// Check vertical bounds
	if (shipSprite.pos_y < TOP_EDGE)
	{
		shipSprite.pos_y = TOP_EDGE;
		shipSprite.vel_y = -shipSprite.vel_y;
	}
	else if (shipSprite.pos_y + (shipSprite.hitbox_y2 - shipSprite.hitbox_y1) > BOTTOM_EDGE)
	{
		shipSprite.pos_y = BOTTOM_EDGE - (shipSprite.hitbox_y2 - shipSprite.hitbox_y1);
		shipSprite.vel_y = -shipSprite.vel_y;
	}

	// Position the ship
	shipSprite.pos_x += shipSprite.vel_x;
	shipSprite.pos_y += shipSprite.vel_y;
}


int main(bool hard)
{

	// SETUP backgroupd
	VDP_setBackgroundColor(16);
	VDP_setScreenWidth320();
  
	// Set Colors
	PAL_setPalette(PAL0, plane_b_pal.data, CPU);
	PAL_setPalette(PAL1, plane_a_pal.data, CPU);
	PAL_setPalette(PAL2, ships_pal.data, CPU);
	
  PAL_setColor(0, 0x0000);

	// set scrolling modes to support fake rotation
	VDP_setScrollingMode(HSCROLL_LINE, VSCROLL_2TILE);

	// get initial tile position in VRAM
	int ind = TILE_USER_INDEX;
	int indexA = ind;
	VDP_loadTileSet(plane_a.tileset, indexA, DMA);
	int indexB = ind + plane_b.tileset->numTile; // AND get next position in VRAM ;
	VDP_loadTileSet(plane_b.tileset, indexB, DMA);

	// SImple image for BG_B.  We're not changing  it during the level
	VDP_drawImageEx(BG_B, &plane_b, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, indexB), 0, 0, FALSE, TRUE);

	// Setup BG_A
	VDP_setTileMapEx(BG_A, plane_a.tilemap, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, indexA),
									 0,							 // Plane X destination
									 0,							 // plane Y destination
									 0,							 // Region X start position
									 0,							 // Region Y start position
									 PLANE_MAX_HORIZONTAL_TILE, // 
									 PLANE_MAX_VERTICAL_TILE, // 
									 CPU);
 

	// setup sprite
	SPR_init();

	shipSprite.pos_x = 144;
	shipSprite.pos_y = 160;
	shipSprite.vel_x = 0;
	shipSprite.vel_y = 0;
	shipSprite.active = TRUE;
	shipSprite.hitbox_x1 = 0;
	shipSprite.hitbox_y1 = 0;
	shipSprite.hitbox_x2 = 32;
	shipSprite.hitbox_y2 = 32;
	shipAnim = 5;
	shipSprite.sprite = SPR_addSprite(&ships, shipSprite.pos_x, shipSprite.pos_y, TILE_ATTR(PAL2, 1, FALSE, FALSE));
	SPR_setAnim(shipSprite.sprite, shipAnim);

	SPR_update();

	JOY_init();
	JOY_setEventHandler(&myJoyHandler);

	for (int row = 0; row < 224; ++row)
	{
		hScrollA[row] = -40;
	}

	// Setup interrupt handlers
	SYS_disableInts();
	{
		SYS_setVBlankCallback(VBlankHandler);
		SYS_setHIntCallback(HIntHandler);
		VDP_setHIntCounter(0);
		VDP_setHInterrupt(1);
	}
	SYS_enableInts();


	// pre calc angles to simplify logic later
	u16 upperAngles[80];
	u16 upperAnglePos = 0;
	for( s16 angle = 20; angle > 0; --angle)  {
		upperAngles[upperAnglePos] = angle;
		upperAngles[79-upperAnglePos] = angle;
		++upperAnglePos;
	}
	for( s16 angle = 1023; angle > 1003; --angle)  {
		upperAngles[upperAnglePos] = angle;
		upperAngles[79-upperAnglePos] = angle;
		++upperAnglePos;
	}
	upperAnglePos = 0;

	u16 lowerAngles[40];
	u16 lowerAnglePos = 0;
	for( s16 angle = 10; angle > 0; --angle)  {
		lowerAngles[lowerAnglePos] = angle;
		lowerAngles[39-lowerAnglePos] = angle;
		++lowerAnglePos;
	}
	for( s16 angle = 1023; angle > 1013; --angle)  {
		lowerAngles[lowerAnglePos] = angle;
		lowerAngles[39-lowerAnglePos] = angle;
		++lowerAnglePos;
	}
	lowerAnglePos = 20;


  u16 delay = 0;
	while (TRUE)
	{
		if (delay !=1 )
		{

			setAngle(upperAngles[upperAnglePos], 0, 80, 40, vScrollUpperA);
			++upperAnglePos;
			if (upperAnglePos == 80)
			{
				upperAnglePos = 0;
			}
		}
		else 
		{
			setAngle(lowerAngles[lowerAnglePos], 144, 224, 200, vScrollLowerA);
			++lowerAnglePos;
			if (lowerAnglePos == 40)
			{
				lowerAnglePos = 0;
			}
		}
		++delay;
		if( delay > 1 ) {
			delay = 0;
		}
		for (int i = 0; i < 20; i++)
		{
			vScrollB[i] -= planeBDeltas[i];
		}

		update();
		SPR_setAnim(shipSprite.sprite, shipAnim);

		VDP_setHorizontalScrollLine(BG_A, 0, hScrollA, 224, CPU);
		VDP_setVerticalScrollTile(BG_B, 0, vScrollB, 20, DMA); // use array to set plane offsets

		SPR_update();

		SYS_doVBlankProcess();
	}
}
