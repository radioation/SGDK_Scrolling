# SGDK_Scrolling
This repo contains code I've written while trying out different ways of scrolling with [SGDK](https://github.com/Stephane-D/SGDK).

THis code has been tested with [SGDK 1.65](https://github.com/Stephane-D/SGDK/releases/tag/v1.65).  

## MAP for Large Images
*ScrollingWithCamera* and *SimpleScrolling* are scrolling examples using the MAP type from version [1.60](https://github.com/Stephane-D/SGDK/releases/tag/v1.60).  You will need to have SGDK 1.60 (or higher) setup on your system to use this code.  It requires the new MAP code and won't work with older versions of SGDK

## Image Scrolling with `VDP_setTileMap()`
*Horizontal* uses `VDP_setTileMapEx()` to scroll a relatively large image over the default SGDK tilespace.  

