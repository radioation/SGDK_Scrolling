#include <genesis.h>
#include "resources.h"

#define PLAYER_FRAME_COUNT  16
#define MAX_ROTATION_INDEX  FIX32(15)
#define MIN_ROTATION_INDEX  FIX32(0)
#define PLAYER_WIDTH        24
#define PLAYER_HEIGHT       24
#define PLAYER_SHOT_WIDTH   8 
#define PLAYER_SHOT_HEIGHT  8 
#define MAX_PLAYER_SHOTS    4


#define MAX_ROCKS           18 
#define MAX_EXPLOSIONS      6

#define MAP_WIDTH           1280
#define MAP_HEIGHT          896 

#define SCR_WIDTH           320
#define SCR_HEIGHT          224 

#define CAMERA_PADDING      90

// Struct for managing sprites
typedef struct {
	Sprite *sprite;
	fix32 pos_x;
	fix32 pos_y;
	fix32 vel_x;
	fix32 vel_y;

	s32 hitbox_x1;
	s32 hitbox_y1;
	s32 hitbox_x2;
	s32 hitbox_y2;

	bool active;
	u16 ticks;
} CP_SPRITE;


// map variables
Map *map_a;
s32 camPosX;// Camera position is used to move the MAP
s32 camPosY;


// player variables
CP_SPRITE player;
fix32 playerRotation;
bool  updatePlayerPosition;
fix32 deltaX[PLAYER_FRAME_COUNT];
fix32 deltaY[PLAYER_FRAME_COUNT];
CP_SPRITE playerShots[ MAX_PLAYER_SHOTS ];


// Rocks
CP_SPRITE rocks[MAX_ROCKS];

// explosions
CP_SPRITE explosions[MAX_EXPLOSIONS];
u16 nextExplosion = 0;


static void addExplosion(  fix32 pos_x, fix32 pos_y ) {
	// check if the current explosion is free to use
  if( explosions[ nextExplosion ].active == FALSE ){
		// not in use, so place it on screen
    explosions[nextExplosion].pos_x  = pos_x;
    explosions[nextExplosion].pos_y  = pos_y;
    explosions[nextExplosion].active = TRUE;
    explosions[nextExplosion].ticks = 0;
    SPR_setVisibility( explosions[nextExplosion].sprite, VISIBLE);
    SPR_setPosition( explosions[nextExplosion].sprite,F32_toInt(explosions[nextExplosion].pos_x),F32_toInt(explosions[nextExplosion].pos_y));

    // update current explosion index for next request
    ++nextExplosion;
    if( nextExplosion >= MAX_EXPLOSIONS ) {
      nextExplosion = 0;
    }
  }
}



static void updateCameraPos() {
	// figure out where the player is.  
	s32 px = F32_toInt( player.pos_x );
	s32 py = F32_toInt( player.pos_y );

	s32 playerScreenX = px - camPosX;
	s32 playerScreenY = py - camPosY;

	// Adjust new camera X position based on player position
	s32 newCamX;
	// check if the player X position is too close to the right edge of the screen
	if( playerScreenX > SCR_WIDTH - CAMERA_PADDING - PLAYER_WIDTH ) {
		newCamX = px - ( SCR_WIDTH - CAMERA_PADDING - PLAYER_WIDTH );
	} else if( playerScreenX < CAMERA_PADDING ) { // check if the player is too close to the left
		newCamX = px - CAMERA_PADDING;	
	} else {
		newCamX = camPosX; // no change to camera position.
	}

	// Adjust camera Y position based on player position
	s32 newCamY;
	// check if the player Y position is too close to the bottom edge of the screen
	if( playerScreenY > SCR_HEIGHT - CAMERA_PADDING - PLAYER_HEIGHT ) {
		newCamY = py - ( SCR_HEIGHT - CAMERA_PADDING - PLAYER_HEIGHT ) ;
	} else if( playerScreenY < CAMERA_PADDING ) {  // is player too close to the top of the screen?
		newCamY = py - CAMERA_PADDING;	
	} else {
		newCamY = camPosY; // no change to camera position.
	}


	// handle camera position at edges
	if ( newCamX < 0  ) { // don't move past the left edge of the scroll image.
		newCamX = 0;
	} else if ( newCamX > (MAP_WIDTH - SCR_WIDTH )) {  // don't move past the right edge
		newCamX = MAP_WIDTH - SCR_WIDTH ;
	}
	if ( newCamY < 0  ) { // don't move past the top of the scroll image
		newCamY = 0;
	} else if ( newCamY > (MAP_HEIGHT - SCR_HEIGHT )) {  // don't move past the bottom 
		newCamY = MAP_HEIGHT - SCR_HEIGHT ;
	}

	// Store the values
	camPosX = newCamX;
	camPosY = newCamY;
	// Update the MAP position
	MAP_scrollTo( map_a, camPosX, camPosY );
}

static void inputCallback( u16 joy, u16 changed, u16 state ) {
	// create a shot if available
	if( changed & state & BUTTON_A ) {
		for( u16 i = 0; i < MAX_PLAYER_SHOTS; ++i ) {
			if( playerShots[i].active == FALSE ) {
				// create a new one

				u16 rot = F32_toInt( playerRotation);
				playerShots[i].pos_x = player.pos_x + FIX32((PLAYER_WIDTH-PLAYER_SHOT_WIDTH)/2) +  F32_mul( deltaX[rot], FIX32(2.0));
				playerShots[i].pos_y = player.pos_y + FIX32((PLAYER_HEIGHT-PLAYER_SHOT_WIDTH)/2) +  F32_mul( deltaY[rot], FIX32(2.0));
				playerShots[i].vel_x = F32_mul( deltaX[rot], FIX32(2.0));
				playerShots[i].vel_y = F32_mul( deltaY[rot], FIX32(2.0));
				playerShots[i].active = TRUE;
				playerShots[i].ticks = 0;
				break;

			}
		}
	}
}


static void handleInput() {
	u16 value = JOY_readJoypad(JOY_1);

	if( value & BUTTON_LEFT ) {
		playerRotation += FIX32( 0.25 ); // using fixed point to slow down rotation speed 
		if( playerRotation > MAX_ROTATION_INDEX ) {
			playerRotation = MIN_ROTATION_INDEX;
		}
		int rot = F32_toInt( playerRotation);
		SPR_setAnim( player.sprite, rot);
	} else if( value & BUTTON_RIGHT ) {
		playerRotation -= FIX32( 0.25 );
		if( playerRotation < MIN_ROTATION_INDEX ) {
			playerRotation = MAX_ROTATION_INDEX;
		}
		int rot = F32_toInt( playerRotation);
		SPR_setAnim( player.sprite, rot);
	}


	if( value & BUTTON_UP ) {
		// signal update to uplayer position.
		updatePlayerPosition = TRUE;
	} else {
		updatePlayerPosition = FALSE;
	}

}

static void update() {
	// update player
	if( updatePlayerPosition == TRUE ) {
		int rot = F32_toInt( playerRotation);
		player.pos_x = player.pos_x + deltaX[rot];
		player.pos_y = player.pos_y + deltaY[rot];


		if( player.pos_x< FIX32( -6.0 ) ) {
			player.pos_x = 	FIX32( -6.0 );
		} else if( player.pos_x > FIX32( MAP_WIDTH - PLAYER_WIDTH + 6.0 )) {
			player.pos_x = 	FIX32( MAP_WIDTH - PLAYER_WIDTH + 6.0 );
		}

		if( player.pos_y< FIX32( -6.0) ) {
			player.pos_y = 	FIX32( -6.0 );
		} else if( player.pos_y > FIX32( MAP_HEIGHT - PLAYER_HEIGHT + 6.0 )) {
			player.pos_y = 	FIX32( MAP_HEIGHT - PLAYER_HEIGHT + 6.0 );
		}


	}


	// move shots
	for( u16 i=0; i < MAX_PLAYER_SHOTS; ++i ) {
		if( playerShots[i].active == TRUE ) {
			playerShots[i].pos_x +=  playerShots[i].vel_x;
			playerShots[i].pos_y +=  playerShots[i].vel_y;
			s32 x = F32_toInt(playerShots[i].pos_x) - camPosX;	
			s32 y = F32_toInt(playerShots[i].pos_y) - camPosY;	
			if( x >= 0 && x < SCR_WIDTH && y >= 0 && y < SCR_HEIGHT ) {
				SPR_setVisibility( playerShots[i].sprite, VISIBLE);
				SPR_setPosition(playerShots[i].sprite, F32_toInt(playerShots[i].pos_x) - camPosX, F32_toInt(playerShots[i].pos_y) - camPosY );
			} else {
				// shot reached the screen edge, deactivate and hide it.
				playerShots[i].active = FALSE;
				SPR_setVisibility( playerShots[i].sprite, HIDDEN);
			}
		} else {
			// hide inactive shots
			SPR_setVisibility( playerShots[i].sprite, HIDDEN);
		}
	}




	// rocks
	for( u16 i=0; i < MAX_ROCKS; ++i ) {
		if( rocks[i].active == TRUE ) {
			rocks[i].pos_x +=  rocks[i].vel_x;
			if( rocks[i].pos_x < FIX32(-32) ) { rocks[i].pos_x = FIX32(MAP_WIDTH);} 
			else if( rocks[i].pos_x >FIX32(MAP_WIDTH) ) { rocks[i].pos_x = FIX32(-32);} 

			rocks[i].pos_y +=  rocks[i].vel_y;
			if( rocks[i].pos_y < FIX32(-32) ) { rocks[i].pos_y = FIX32(MAP_HEIGHT);} 
			else if( rocks[i].pos_y >FIX32(MAP_HEIGHT)  ) { rocks[i].pos_y = FIX32(-32);} 

			// only show rock if it's visible.
			s32 x = F32_toInt(rocks[i].pos_x) - camPosX;	
			s32 y = F32_toInt(rocks[i].pos_y) - camPosY;	
			if( x >= -32 && x < SCR_WIDTH && y >= -32 && y < SCR_HEIGHT ) {
				SPR_setVisibility( rocks[i].sprite, VISIBLE);
				SPR_setPosition(rocks[i].sprite, F32_toInt(rocks[i].pos_x) - camPosX, F32_toInt(rocks[i].pos_y) - camPosY );
			} else {
				// shot reached the screen edge, hide it.
				SPR_setVisibility( rocks[i].sprite, HIDDEN);
			}
		} else {
			// hide inactive rocks
			SPR_setVisibility( rocks[i].sprite, HIDDEN);
		}
	}

  for( u16 i=0; i < MAX_EXPLOSIONS; ++i ) {
    if( explosions[i].active == TRUE ) {
      explosions[i].ticks += 1;
      if( explosions[i].ticks < 9 ) {
      //  SPR_setFrame( explosions[i].sprite, explosions[i].ticks );
				SPR_setPosition(explosions[i].sprite, F32_toInt(explosions[i].pos_x) - camPosX, F32_toInt(explosions[i].pos_y) - camPosY );
        SPR_setAnimAndFrame( explosions[i].sprite, i%4, explosions[i].ticks );
      }
      else {
        explosions[i].active = FALSE;
        SPR_setVisibility( explosions[i].sprite, HIDDEN);
      }
    }

  }
	// set the player position.
	SPR_setPosition( player.sprite, F32_toInt( player.pos_x) - camPosX, F32_toInt( player.pos_y ) - camPosY );

	// change position of the MAP
	updateCameraPos();
}

static void checkCollisions() {
	// simple collision just checks bad sprites against good.
  for( u16 i=0; i < MAX_ROCKS; ++i ) {
    if( rocks[i].active == TRUE ) {
      // check if ship hits a rock
      if( (rocks[i].pos_x + rocks[i].hitbox_x1) < (player.pos_x + player.hitbox_x2) &&
          (rocks[i].pos_x + rocks[i].hitbox_x2) > (player.pos_x + player.hitbox_x1) &&
          (rocks[i].pos_y + rocks[i].hitbox_y1) < (player.pos_y + player.hitbox_y2) &&
          (rocks[i].pos_y + rocks[i].hitbox_y2) > (player.pos_y + player.hitbox_y1)  )
      {
        rocks[i].active = FALSE;
        SPR_setVisibility( rocks[i].sprite, HIDDEN);
        addExplosion(  rocks[i].pos_x, rocks[i].pos_y );
      }
			
			// check if a player shot hits a rock.
      for( u16 j=0; j < MAX_PLAYER_SHOTS; ++j ) {
        if(
            playerShots[j].active == TRUE &&
            (rocks[i].pos_x + rocks[i].hitbox_x1) < (playerShots[j].pos_x + FIX32(4)) &&
            (rocks[i].pos_x + rocks[i].hitbox_x2) > (playerShots[j].pos_x + FIX32(4)) &&
            (rocks[i].pos_y + rocks[i].hitbox_y1) < (playerShots[j].pos_y + FIX32(4)) &&
            (rocks[i].pos_y + rocks[i].hitbox_y2) > (playerShots[j].pos_y + FIX32(4))  )
        {
          rocks[i].active = FALSE;
          SPR_setVisibility( rocks[i].sprite, HIDDEN);
          playerShots[j].active = FALSE;
          SPR_setVisibility( playerShots[j].sprite, HIDDEN);
          addExplosion(  rocks[i].pos_x, rocks[i].pos_y );
					break; // stop checking once it's been hit
        }
      }
    }
  }
}





static void createPlayerShots() {
	fix32 xpos = FIX32(-16);
	fix32 ypos = FIX32(-16);

	for( u16 i=0; i < MAX_PLAYER_SHOTS; ++i ) {
		playerShots[i].pos_x = xpos;
		playerShots[i].pos_y = ypos;
		playerShots[i].vel_x = FIX32(0.0);
		playerShots[i].vel_y = FIX32(0.0);
		playerShots[i].active = FALSE;
		playerShots[i].hitbox_x1 = FIX32(3);
		playerShots[i].hitbox_y1 = FIX32(3);
		playerShots[i].hitbox_x2 = FIX32(4);
		playerShots[i].hitbox_y2 = FIX32(4);

		playerShots[i].sprite = SPR_addSprite( &shot, xpos, ypos, TILE_ATTR( PAL0, 0, FALSE, FALSE ));
		SPR_setAnim( playerShots[i].sprite, 2 );
	}

}

static void createRocks() {
	fix32 ypos = FIX32(0);
	fix32 xpos = FIX32(0);
	for( u16 i=0; i < MAX_ROCKS; ++i ) {
	  rocks[i].pos_x = FIX32(random()%(MAP_WIDTH-32) + i );  // random starting position for rock sprites
		rocks[i].pos_y = FIX32(random()%(MAP_HEIGHT-32)+ i);

		// use ranodm direction for rock motion
		u16 rot = random() % 16; 
		fix32 vel = FIX32(0.8);
		rocks[i].vel_x = F32_mul( vel, deltaX[rot]  );
		rocks[i].vel_y = F32_mul( vel, deltaY[rot]  );
		rocks[i].active = TRUE;
		rocks[i].hitbox_x1 = FIX32(2);
		rocks[i].hitbox_y1 = FIX32(2);
		rocks[i].hitbox_x2 = FIX32(30);
		rocks[i].hitbox_y2 = FIX32(30);

		//rocks[i].sprite = SPR_addSprite( &rock, -32, -32, TILE_ATTR( PAL3, 0, FALSE, FALSE ));
		rocks[i].sprite = SPR_addSprite( &rock, rocks[i].pos_x, rocks[i].pos_y, TILE_ATTR( PAL3, 0, FALSE, FALSE ));
		SPR_setAnim( rocks[i].sprite, 0 );
	}
}

static void createExplosions() {
  fix32 xpos =  FIX32(0);
  fix32 ypos =  FIX32(264);

  for( u16 i=0; i < MAX_EXPLOSIONS; ++i ) {
    explosions[i].pos_x = xpos;
    explosions[i].pos_y = ypos;
    explosions[i].vel_x = FIX32(0);
    explosions[i].vel_y = FIX32(0);
    explosions[i].active = FALSE;
    explosions[i].hitbox_x1 = FIX32(0);
    explosions[i].hitbox_y1 = FIX32(0);
    explosions[i].hitbox_x2 = FIX32(0);
    explosions[i].hitbox_y2 = FIX32(0);

    explosions[i].sprite = SPR_addSprite( &explosion, F32_toInt(xpos), F32_toInt(ypos), TILE_ATTR( PAL0, 0, FALSE, FALSE ));
    SPR_setAnim( explosions[i].sprite, i % 4 );

    SPR_setVisibility( explosions[i].sprite, HIDDEN );
    SPR_setDepth( explosions[i].sprite, SPR_MIN_DEPTH );
  }

}


int main( bool hard ) {

	// clear 
	memset( playerShots, 0, sizeof(playerShots) );
	memset( rocks, 0, sizeof(rocks) );
	memset( explosions, 0, sizeof(explosions) );


	PAL_setPalette( PAL0, shot_pal.data, CPU);
	PAL_setPalette( PAL1, plane_pal.data, CPU);
	PAL_setPalette( PAL2, ship_pal.data, CPU);
	PAL_setPalette( PAL3, rock_pal.data, CPU);

	// Load the plane tiles into VRAM
	int ind = TILE_USER_INDEX;
	VDP_loadTileSet( &plane_a_tileset, ind, DMA );

	// init plane
	map_a = MAP_create( &plane_a_map, BG_A, TILE_ATTR_FULL( PAL1, FALSE, FALSE, FALSE, ind ) );
	camPosX = MAP_WIDTH/2 - SCR_WIDTH/2;
	camPosY = MAP_HEIGHT/2 - SCR_HEIGHT/2;

	MAP_scrollTo( map_a, camPosX, camPosY );

	// Init sprite engine with defaults
	SPR_init();

	// Setup the player sprite
	player.pos_x = FIX32( MAP_WIDTH/2 - 12 );
	player.pos_y = FIX32( MAP_HEIGHT/2 - 12 );
	player.sprite = SPR_addSprite( &ship, F32_toInt(player.pos_x) - camPosX, F32_toInt(player.pos_y) - camPosY, TILE_ATTR(PAL2, 0, FALSE, FALSE ));
  player.hitbox_x1 = FIX32(5);
  player.hitbox_y1 = FIX32(5);
  player.hitbox_x2 = FIX32(19);
  player.hitbox_y2 = FIX32(19);
	playerRotation = MIN_ROTATION_INDEX;
	SPR_setAnim( player.sprite, F32_toInt(playerRotation ) );


	// setup player motion offset.
	deltaX[0] = FIX32( 0.000000 );
	deltaY[0] = FIX32( -2.000000 );
	deltaX[1] = FIX32( -0.765367 );
	deltaY[1] = FIX32( -1.847759 );
	deltaX[2] = FIX32( -1.414214 );
	deltaY[2] = FIX32( -1.414214 );
	deltaX[3] = FIX32( -1.847759 );
	deltaY[3] = FIX32( -0.765367 );
	deltaX[4] = FIX32( -2.000000 );
	deltaY[4] = FIX32( -0.000000 );
	deltaX[5] = FIX32( -1.847759 );
	deltaY[5] = FIX32( 0.765367 );
	deltaX[6] = FIX32( -1.414214 );
	deltaY[6] = FIX32( 1.414214 );
	deltaX[7] = FIX32( -0.765367 );
	deltaY[7] = FIX32( 1.847759 );
	deltaX[8] = FIX32( -0.000000 );
	deltaY[8] = FIX32( 2.000000 );
	deltaX[9] = FIX32( 0.765367 );
	deltaY[9] = FIX32( 1.847759 );
	deltaX[10] = FIX32( 1.414214 );
	deltaY[10] = FIX32( 1.414214 );
	deltaX[11] = FIX32( 1.847759 );
	deltaY[11] = FIX32( 0.765367 );
	deltaX[12] = FIX32( 2.000000 );
	deltaY[12] = FIX32( 0.000000 );
	deltaX[13] = FIX32( 1.847759 );
	deltaY[13] = FIX32( -0.765367 );
	deltaX[14] = FIX32( 1.414214 );
	deltaY[14] = FIX32( -1.414214 );
	deltaX[15] = FIX32( 0.765367 );
	deltaY[15] = FIX32( -1.847759 );

	createExplosions();
	createPlayerShots();
	createRocks();

	JOY_setEventHandler( &inputCallback );

	while(TRUE) {

		handleInput();

		update();

   	checkCollisions();

		SPR_update();

		SYS_doVBlankProcess();
	}	
	return 0;
}

