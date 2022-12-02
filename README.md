# SGDK_Scrolling
This repo contains code I've written while trying out different ways of scrolling with [SGDK](https://github.com/Stephane-D/SGDK).

THis code has been tested with [SGDK 1.65](https://github.com/Stephane-D/SGDK/releases/tag/v1.80).  

## MAP for Large Images
The folders *ScrollingWithCamera* and *SimpleScrolling* are scrolling examples using the MAP type introduced in version [1.60](https://github.com/Stephane-D/SGDK/releases/tag/v1.60).  The code has been updated to compile with SGDK 1.80.  So you must have version 1.80 on your system to use this code.

## Image Scrolling with `VDP_setTileMap()`
* *HScroll* uses `VDP_setTileMapEx()` to scroll a relatively large image over the default SGDK tilespace.  
* *HScrollDec* uses `VDP_setTileMapEx()` and `HSCROLL_PLANE` to scroll two planes.  
* *HScrollTiled* uses `VDP_setTileMapEx()` and  `HSCROLL_TILE` to demonstrate parallax scrolling with two planes.

