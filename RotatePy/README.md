
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
usage: sgdk_scroll_rotate.py [-h] [-v] [-s ARG] [-e ARG] [-i ARG] [-c ARG] [-C ARG] [-x ARG] [-w ARG] [-r ARG] [-R ARG] [-y ARG] [-o ARG] [-P ARG] [-p ARG]
                             [-b ARG] [-t ARG] [-S ARG]

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

# Example Usage
```bash
 python3 sgdk_scroll_rotate.py -s -10 -e 10 -i 2 -x 9 -r 60 -R 180 -y 112 -p ship -w 384 -x 9
```



