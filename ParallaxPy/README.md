# Parallax Image Generation Script

`sgdk_parallax_iamge.py` is a simple Python script that creates endless
scrolling images for use with Sega Genesis / SGDK.  You must supply at least
one 16-color indexed image to be warped into a repeating floor pattern.  The
generated image can optionally have a ceiling pattern.  The ceiling  can use
the same image as the floor or a separate image can supplied for the ceiling
pattern.

## TODO
The script has some rough edges, but does what I needed it to.  There are 
some obvious things that could be improved:

* Check input values for invalid combinations
* Handle odd numbers for repeatting pattern
* Blend pixels at edges of warp pattern (or sample images instead of 
  using OpenCV's warp)
* Generate code with user configurable scrolling step sizes 
* Generate code with lookup tables for scrolling values

I'll likely add code to check input values and handle odd numbers, but this
script is very low priority.  I may never get around to the rest of it.


## Dependencies
The script was written for [Python3](https://www.python.org/downloads/) and requires 
[numpy](https://numpy.org/), [PILLOW](https://python-pillow.org/), and 
[OpenCV](https://opencv.org/).

I'm using WSL and Ubuntu 22.02 on my development machine, but I imagine 
most people will be using Windows.  I'm going to keep on using WSL, but 
I have installed the windows versions to test the script. 
1. Get Python3. I used the Microsoft Store version.
![Python3 Microsoft Store Download](https://raw.githubusercontent.com/radioation/SGDK_Scrolling/main/ParallaxPy/python_win_install.png)

2. Install the dependencies.  I opened a command line window and typed:
```cmd
pip3 install numpy
pip3 install pillow
pip3 install opency-python
```
3. Run the script with:
```cmd
python3 sgdk_parallax_image.py
```

# Basic usage

```bash
usage: sgdk_parallax_image.py [-h] [-v] [-f ARG] [-n ARG] [-s ARG] [-e ARG]
                              [-i ARG] [-S ARG] [-E ARG] [-I ARG] [-o ARG]
                              [-p ARG]

Create parallax scrolling background for SGDK

options:
  -h, --help            show this help message and exit
  -v, --verbose         increase output verbosity
  -f ARG, --far_image_reps ARG
                        How many times to repeat image at far side of floor
  -n ARG, --near_image_reps ARG
                        How many times to repeat image at near side of floor
  -s ARG, --start_row ARG
                        Which row starts the floor
  -e ARG, --end_row ARG
                        Which row ends the floor
  -i ARG, --input_filename ARG
                        input image filename
  -S ARG, --start_ceiling_row ARG
                        Which row starts the ceiling
  -E ARG, --end_ceiling_row ARG
                        Which row ends the ceiling
  -I ARG, --input_ceiling_filename ARG
                        input ceiling image filename
  -o ARG, --output_filename ARG
                        Output filename
  -p ARG, --project_directory ARG
                        Create project directory with resource files and simple SGDK code.
```

# Examples

```bash
python3 sgdk_parallax_image.py -i test_wood.png   -f 6 -n 4   -S 1 -E 40  -I test_tile.png  -p ../TESTP
```



