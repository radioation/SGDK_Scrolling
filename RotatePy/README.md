
# Scroll Rotation Values Script
`sgdk_scroll_rotate.py` is a script that pre-calculates horizontal and vertical scrolling
values to simulate full screen rotation with SGDK's scrolling functions.  It will calculate
scrolling offsets for a range of rotation values specified by the user.  The values 
are stored in s16 arrays for use with `VDP_setHorizontalScrollLine()` and 
`VDP_setVerticalScrollTile()`.   


# TODO
* Check input values for invalid combinations.
* Give the option to get scrolling functions instead of pre-calculated arrays.
* Move `rotation.h` generation code to a template file.

# Dependencies
The script was written for [Python3](https://www.python.org/downloads/) and requires 
[NumPy](https://numpy.org/) and [Jinja](https://palletsprojects.com/p/jinja/).


I'm using WSL and Ubuntu 22.02 on my development machine, but I imagine 
most people will be using Windows.  I'm going to keep on using WSL, but 
I have installed the windows versions to test the script. 
1. Get Python3. I used the Microsoft Store version.

2. Install the dependencies.  I opened a command line window and typed:
```cmd
pip3 install numpy
pip3 install jinja2
```
3. Run the script with:
```cmd
python3 sgdk_scroll_rotate.py
```


# Basic Usage
```txt
usage: sgdk_scroll_rotate.py [-h] [-v] [-s ARG] [-e ARG] [-i ARG] [-c ARG] [-C ARG] [-x ARG] [-w ARG] [-r ARG] [-R ARG] [-y ARG] [-o ARG] [-P ARG] [-p ARG] [-b ARG] [-t ARG] [-S ARG]

Generates rotation arrays for SGDK.

options:
  -h, --help            show this help message and exit
  -v, --verbose         Print debug messages
  -s ARG, --start_angle ARG
                        Starting rotation in degrees
  -e ARG, --end_angle ARG
                        End rotation angle in degrees
  -i ARG, --angle_increment ARG
                        Rotation step size
  -c ARG, --column_start ARG
                        First column to rotate (default 0)
  -C ARG, --column_end ARG
                        Last column to rotate (default 19)
  -x ARG, --center_x ARG
                        Which column is the center of rotation
  -w ARG, --image_width ARG
                        Width of image to rotate
  -r ARG, --row_start ARG
                        First row to rotate
  -R ARG, --row_end ARG
                        Last row to rotate
  -y ARG, --center_y ARG
                        Which row is the center of rotation
  -o ARG, --output_filename ARG
                        Output filename
  -P ARG, --prefix ARG  Add a prefix to array names
  -p ARG, --project_directory ARG
                        Create project directory with resource files and simple SGDK code.
  -b ARG, --background_filename ARG
                        Specify background image
  -t ARG, --points_filename ARG
                        Specify CSV file with points to rotate along with bg
  -S ARG, --sprite_filename ARG
                        Specify target sprite image
```



 
# Examples

## Rotate 2 Degrees
The simplest use case is to generate a single set of rotation offsets.  This is not really useful.
If just want your background image rotated by a fixed angle, you're better off drawing it that way.  
I'm using it here for simplicity


Suppose you want to have a simple image of a green bar. 
![green bar](https://raw.githubusercontent.com/radioation/SGDK_Scrolling/main/RotatePy/green_bar.png)

This image is 320 x 224 pixels.  The green bar's upper left pixel is at `x = 48` and `y = 96`.  The 
lower right corner of the green bar is at `x = 271` and `y = 111`.   
 

To make things easier to read I'll just do rotate the image by 2 degrees.  We only have to horizontally scroll the lines with pixels in them.  Using a small rotation in this example limits the size of the output files.  To limit the computed range to  2 degrees use:
```c
--start_angle 2
--end_angle 2
```


For this example, I want the center of rotation to be near the center of the bar.  This is approximately 
at (x = 159, y = 103).   Recall the Genesis scrolling columns are 16 pixels wide and 159/16 is ~9.9375.
I'm going to set the center X value to 9 ( arrays indices start with 0 in C)
```c
--center_x  9 
--center_y 103 
```


By default, my script will generate offsets for all 223 rows of the genesis screen.  Calculating offsets
for every row is be pretty wasteful for this image.  We can limit the number of rows with 
```c
--row_start 93 
--row_end 113
```

```bash
python3 sgdk_scroll_rotate.py --start_angle 2 --end_angle 2 --center_x  9  --center_y 103   --row_start 93 --row_end 113
```
The output will look something like:

```cmd
C:\Dev\Rotate> python3 sgdk_scroll_rotate.py --start_angle 2 --end_angle 2 --center_x  9  --center_y 103   --row_start 93 --row_end 113
INFO: Parameters
INFO: Start angle: 2.000000 Stop angle: 2.000000 Step size: 1.000000
INFO: Columns to rotate: 20 Center column: 9
INFO: Rows to rotate: 21 Center row: 103
#define ROWS_A 21
#define START_ROW_A 93
#define END_ROW_A 113
#define COLS_A 20
#define START_COL_A 0
#define END_COL_A 19
```
The output tells you what parameters were used.


The script also  generates a file called `rotation.h`.  It should look something like this:
```c
#ifndef _ROTATION_H_
#define _ROTATION_H_


#define _SCROLL_COUNT 1
#define ROWS_A 21
#define START_ROW_A 93
#define END_ROW_A 113
#define COLS_A 20
#define START_COL_A 0
#define END_COL_A 19


s16 _hScroll[] = {
  // rotation values for angle 2.000000 starts at 0
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


s16 _vScroll[] = {
 // rotation values for angle 2.000000 starts at 0
-5, -4, -4, -3, -3, -2, -2, -1, -1, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 6
};


#endif // _ROTATION_H_
```
*`_SCROLL_COUNT` lets you know how many sets of offsets are in your file.  In this case, it's 1 as we only generated values for one angle (2 degrees).
* The `#define` rows can be used by your code when applying the scrolling offset arrays.
* `_hScroll[]`  is the set of horizontal scrolling offsets.
*`_vScroll[]` is the set of vertical scrolling offsets.


"rotation.h" can be included directly in your source code.  Depending on your project, you may want to define the scrolling arrays in .c file instead of a header.  YMMV.

```C
#include "rotation.h"
```

You'll want to make sure the scrolling mode is set to `HSCROLL_LINE` and `VSCROLL_2TILE`.
```c
  // set scrolling mode to LINE for horizontal and TILE for vertical
  VDP_setScrollingMode(HSCROLL_LINE, VSCROLL_2TILE);
```

You tell SGDK to apply the scrolling values with `VDP_setHorizontalScrollLine()` and 
`VDP_setVerticalScrollTile()`
```c
  VDP_setHorizontalScrollLine(BG_A, START_ROW_A, _hScroll, ROWS_A, CPU);
  VDP_setVerticalScrollTile(BG_A, START_COL_A, _vScroll, COLS_A, CPU);
```

Project files can be found [here](https://github.com/radioation/SGDK_Scrolling/tree/main/RotatePy/rotate_2_degrees)


The rotated image should look like:
![green bar rotated 2 degrees](https://raw.githubusercontent.com/radioation/SGDK_Scrolling/main/RotatePy/green_bar_rotated_2.png)
 


The start/stop rows and columns will vary with your application.  You'll need to adjust 
rows and columns to fit your image and the amount you want to rotate it.
 If you have too few rows, the rotation effect will stop too soon.  5 degrees with 
`--row_start 93 --row_end 113` looks like this:
![green bar rotated 5 degrees with too few start and end rows](https://raw.githubusercontent.com/radioation/SGDK_Scrolling/main/RotatePy/green_bar_rotated_5_too_few_rows.png)



Increasing the number of computed rows  lets you use larger rotations.   5 degrees using 
`--row_start 63 --row_end 143`  looks like this:
![green bar rotated 5 degrees with more start and end rows](https://raw.githubusercontent.com/radioation/SGDK_Scrolling/main/RotatePy/green_bar_rotated_5_MOAR_ROWS.png)
-15 degrees

And 15 degrees looks like this.
![green bar rotated 15 degrees with more start and end rows](https://raw.githubusercontent.com/radioation/SGDK_Scrolling/main/RotatePy/green_bar_rotated_15.png)

Note the green pixels on the left portion of the display.  You can around this by using a wider image for the rotating background.  The actual width of your background image depends on what your doing.  



## Generate a Range of Rotations.

It's a waste of  memory  and CPU to do a single rotation on a image.  You'd be better off creating the 
background image pre-rotated.      In this example, I want to rotate the image 
from  -5 degrees to 5 degrees in increments of 1 degree. To do this, I used the following values for the angles:
```c
--start_angle -5 
--end_angle 5 
--angle_increment 1
```
The command and output looks something like this:
```cmd
python3 sgdk_scroll_rotate.py --start_angle -5 --end_angle 5 --angle_increment 1  --center_x  9  --center_y 103   --row_start 73 --row_end 133 
INFO: Parameters
INFO: Start angle: -5.000000 Stop angle: 5.000000 Step size: 1.000000
INFO: Columns to rotate: 20 Center column: 9
INFO: Rows to rotate: 61 Center row: 103
#define ROWS_A 61
#define START_ROW_A 73
#define END_ROW_A 133
#define COLS_A 20
#define START_COL_A 0
#define END_COL_A 19
```

and the `rotation.h` offset file now look like:
```c
#ifndef _ROTATION_H_
#define _ROTATION_H_


#define _SCROLL_COUNT 11
#define ROWS_A 61
#define START_ROW_A 73
#define END_ROW_A 133
#define COLS_A 20
#define START_COL_A 0
#define END_COL_A 19


s16 _hScroll[] = {
  // rotation values for angle -5.000000 starts at 0
3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -3, -3
  // rotation values for angle -4.000000 starts at 61
, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -2, -2, -2, -2, -2, -2, -2, -2, -2
  // rotation values for angle -3.000000 starts at 122
, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -2, -2
  // rotation values for angle -2.000000 starts at 183
, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
  // rotation values for angle -1.000000 starts at 244
, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1
  // rotation values for angle 0.000000 starts at 305
, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  // rotation values for angle 1.000000 starts at 366
, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1
  // rotation values for angle 2.000000 starts at 427
, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
  // rotation values for angle 3.000000 starts at 488
, -2, -2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2
  // rotation values for angle 4.000000 starts at 549
, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2
  // rotation values for angle 5.000000 starts at 610
, -3, -3, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3};


s16 _vScroll[] = {
 // rotation values for angle -5.000000 starts at 0
13, 11, 10, 8, 7, 6, 4, 3, 1, 0, -1, -3, -4, -6, -7, -8, -10, -11, -13, -14
 // rotation values for angle -4.000000 starts at 20
, 10, 9, 8, 7, 6, 4, 3, 2, 1, 0, -1, -2, -3, -4, -6, -7, -8, -9, -10, -11
 // rotation values for angle -3.000000 starts at 40
, 8, 7, 6, 5, 4, 3, 3, 2, 1, 0, -1, -2, -3, -3, -4, -5, -6, -7, -8, -8
 // rotation values for angle -2.000000 starts at 60
, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0, -1, -1, -2, -2, -3, -3, -4, -4, -5, -6
 // rotation values for angle -1.000000 starts at 80
, 3, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, -1, -1, -1, -1, -2, -2, -2, -3, -3
 // rotation values for angle 0.000000 starts at 100
, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
 // rotation values for angle 1.000000 starts at 120
, -3, -2, -2, -2, -1, -1, -1, -1, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 3, 3
 // rotation values for angle 2.000000 starts at 140
, -5, -4, -4, -3, -3, -2, -2, -1, -1, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 6
 // rotation values for angle 3.000000 starts at 160
, -8, -7, -6, -5, -4, -3, -3, -2, -1, 0, 1, 2, 3, 3, 4, 5, 6, 7, 8, 8
 // rotation values for angle 4.000000 starts at 180
, -10, -9, -8, -7, -6, -4, -3, -2, -1, 0, 1, 2, 3, 4, 6, 7, 8, 9, 10, 11
 // rotation values for angle 5.000000 starts at 200
, -13, -11, -10, -8, -7, -6, -4, -3, -1, 0, 1, 3, 4, 6, 7, 8, 10, 11, 13, 14
};


#endif // _ROTATION_H_
```

It's a much larger file now because it's calculating 11 different sets of scrolling 
values.   To help keep track of the data in the arrays
`_hScroll` and `_vScroll` have comments that indicate where each set of angles
begins.






Each set of offsets for `_hScroll`  uses an array element for reach row to be scrolled.  In this example
`ROWS_A` is defined as 61.
```c
`#define ROWS_A 61`
```
The comments for `_hScroll` show the starting position for each angle:    -5.0  degrees at 0, -4.0 degrees at 61,  -3.0 degrees at 122, etc.
```c
s16 _hScroll[] = {
  // rotation values for angle -5.000000 starts at 0
...
  // rotation values for angle -4.000000 starts at 61
...
  // rotation values for angle -3.000000 starts at 122

...

```

 `_vScroll` uses an array element for for each column in the set.  `COLS_A` for this 
example is defined as 20
```c
#define COLS_A 20
```
The comments for `_vScroll` show the starting position for each angle:  -5.0  degrees at 0, -4.0 degrees at 20, -3.0 degrees at 40,  etc.
```c
s16 _vScroll[] = {
 // rotation values for angle -5.000000 starts at 0
13, 11, 10, 8, 7, 6, 4, 3, 1, 0, -1, -3, -4, -6, -7, -8, -10, -11, -13, -14
 // rotation values for angle -4.000000 starts at 20
, 10, 9, 8, 7, 6, 4, 3, 2, 1, 0, -1, -2, -3, -4, -6, -7, -8, -9, -10, -11
 // rotation values for angle -3.000000 starts at 40
, 8, 7, 6, 5, 4, 3, 3, 2, 1, 0, -1, -2, -3, -3, -4, -5, -6, -7, -8, -8

...

```

You can quickly rotate a background by copying the 
right position of the array to SGDK's scrolling functions.  
If we want to rotate by -5 degreees, it's the set of the array,  So we just need to copy 
scroll values from the start of the arrays.   It could be done with code like this:
```c
    // Copy first set of angle offsets to local arrays
    memcpy(hScrollA, _hScroll, ROWS_A*sizeof(s16));
    memcpy(vScrollA, _vScroll, COLS_A*sizeof(s16));

    // set SGDK scrolling functions with local arrays to fake the rotaiton.
    VDP_setHorizontalScrollLine(BG_A, START_ROW_A, hScrollA, ROWS_A, CPU);
    VDP_setVerticalScrollTile(BG_A, START_COL_A, vScrollA, COLS_A, CPU);
```
If you want the rotation to be 0 degrees,  use position 305 of  `_hScroll`.
```c
  // rotation values for angle 0.000000 starts at 305
```
and position 100 of `_vScroll`.
```c
 // rotation values for angle 0.000000 starts at 100
```
So you could set the scrolling lines and tiles with code like this:
```c
    // Copy sixth set of angle offsets to local arrays
    memcpy(hScrollA, _hScroll + 305, ROWS_A*sizeof(s16));
    memcpy(vScrollA, _vScroll + 100, COLS_A*sizeof(s16));
 
    // set SGDK scrolling functions with local arrays to fake the rotaiton.
    VDP_setHorizontalScrollLine(BG_A, START_ROW_A, hScrollA, ROWS_A, CPU);
    VDP_setVerticalScrollTile(BG_A, START_COL_A, vScrollA, COLS_A, CPU);
```

Of course, using hard-coded offsets is not very flexible.  A better way to use the scrolling 
arrays is to keep track of the current angle index in a variable and 
add it to `_hScroll` and `_vScroll` whenever you want to change the rotation.
```c
    // Copy current angle offsets into local arrays
    memcpy(hScrollA, _hScroll + currAngle * ROWS_A, ROWS_A*sizeof(s16));
    memcpy(vScrollA, _vScroll + currAngle * COLS_A, COLS_A*sizeof(s16));
```

To rock the image back and forth though the entire range of scroll values you just need to 
move currAngle from 0 to 10 and from 10 to 0.
```c
  s16 currAngle = 5;  // start at angle 0
  s16 stepDir = 1;    // increase the angle 
  u8 angleDelay = 0;
  while (TRUE)
  {
    // handle rotation
    ++angleDelay;
    if (angleDelay % 6 == 0)
    {
      currAngle += stepDir;
      if (currAngle >= _SCROLL_COUNT)
      {
        stepDir = -1;
        currAngle = 10;
      }
      else if (currAngle < 0)
      {
        stepDir = 1;
        currAngle = 0;
      }

      // Copy current angle offsets into local arrays
      memcpy(hScrollA, _hScroll + currAngle * ROWS_A, ROWS_A * sizeof(s16));
      memcpy(vScrollA, _vScroll + currAngle * COLS_A, COLS_A * sizeof(s16));
    }

    // set SGDK scrolling functions with local arrays to fake the rotation.
    VDP_setHorizontalScrollLine(BG_A, START_ROW_A, hScrollA, ROWS_A, CPU);
    VDP_setVerticalScrollTile(BG_A, START_COL_A, vScrollA, COLS_A, CPU);

    // let SGDK do its thing
    SYS_doVBlankProcess();
  }
```

Project files can be found [here](https://github.com/radioation/SGDK_Scrolling/tree/main/RotatePy/rotate_-5_to_5)

 

* the (angleDelay %6 == 0) just slows down the rate of changin `currAngle`
* still getting  scrolling artifacts on the left.  A simple way around this is to use a wider image (nextexample)
* Rotating around a fixed point may work in some cases, but you might want a more dynamic background




## Rotation and Translation
The above example rotates a background image, but what if you want to move the background while 
its rotating?  If we pass the array values directly to SGDK's scrolling functions, the rotation will always be
around the origin we specified to the script.     We *could* have the script calculate rotations at different positions 
on the screen, but I think this would get very large, very quickly.  



Instead of precalculating every possibility, I'll handle scrolling translation at runtime. We can add or 
subtract from the `_hScroll` values to move the background left and right.  We can also add or subtract 
from the `_vScroll` values to move up and down.   The code could use something like this:

```c
    for(int i=0; i < ROWS_A; ++i ) {
      hScrollA[ i ] = ship_hScroll[ currAngle * ROWS_A + i] + xOffset;
    }
    for (int i = 0; i < COLS_A; ++i)
    {
      vScrollA[i] = ship_vScroll[currAngle * COLS_A + i] + yOffset;
    }
```
This will be slower than a straight `memcpy()` call, but faster than calculating everything at runtime.


The code is not very different from the previous example.  The main difference are 
1. I've added some variables to keep track of the current translation offsets.
```c
  s16 xOffset = 0;
  s16 yOffset = 0;
  s16 yOffsetDir = 1;
  u8 offsetDelay = 0;
```
2. The main loop has some code to move the image up and down by setting `yOffset` to values from 0 to 40. It also moves the image left and right by incrementing or decrementing `xOffset` by 1 pixel based on the value of  `currAngle`
```c
    ++offsetDelay;
    if( offsetDelay % 3 == 0 ) {
      yOffset += yOffsetDir;
      if( yOffset > 40) {
        yOffsetDir = -1;
      }else if( yOffset < 0 ) {
        yOffsetDir = 1;
      }
      if( currAngle < 4) {
        xOffset+=1;
      } else if ( currAngle > 6) {
        xOffset-=1;
      }
    }
```
3. Instead of using `memcpy()` to set the angle offsets, it loops through the relevant `_hScroll` and `_vScroll` 
values and adds `xOffset` or `yOffset`.
```c
    for(int i=0; i < ROWS_A; ++i ) {
      hScrollA[ i ] = _hScroll[ currAngle * ROWS_A + i] + xOffset;
    }
    for (int i = 0; i < COLS_A; ++i)
    {
      vScrollA[i] = _vScroll[currAngle * COLS_A + i] + yOffset;
    }
```
4.  It does a sanity check on the current value of  `yOffset` before setting the current horizontal scroll values.
```c
    s16 startHorizontalScroll = START_ROW_A - yOffset;
    s16 totalRows = ROWS_A ;
    if( startHorizontalScroll < 0 ) {
      totalRows = ROWS_A + startHorizontalScroll;
      startHorizontalScroll = 0;
    }
    VDP_setHorizontalScrollLine(BG_A, startHorizontalScroll, hScrollA, totalRows, DMA);
```
This is done to make sure we don't start the horizontal scroll at a negative row.   

Project files can be found [here](https://github.com/radioation/SGDK_Scrolling/tree/main/RotatePy/rotation_and_translation)




