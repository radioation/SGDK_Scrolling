
# Scroll Rotation Values Script
`sgdk_scroll_rotate.py` is a script that pre-calculates horizontal and vertical scrolling
values to simulate full screen rotation with SGDK's scrolling functions.  It will calculate
scrolling offsets for a range of rotation values specified by the user.  The values 
are stored in s16 arrays for use with `VDP_setHorizontalScrollLine()` and 
`VDP_setVerticalScrollTile()`.   


# TODO
* Write instructions (basic usage, examples)
* Check input values for invalid combinations.
* Give the option to get scrolling functions instead of pre-calculated arrays.
* Move `rotation.h` generation code to a template file.

# Dependencies
THis script was written for Python3 and requires numpy and jinja2.



# Basic Usage


# Example Usage
```bash
 python3 sgdk_scroll_rotate.py -s -10 -e 10 -i 2 -x 9 -r 60 -R 180 -y 112 -p ship -w 384 -x 9
```



