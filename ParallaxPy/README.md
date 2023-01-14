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
## Floor with two near repetitions and four far repetitions
The default behavior is to take an input image named "image.png" and repeated
it twice at the bottom row of the image (row 223) and four times at the top of
the floor pattern (row 80).  Type 
```bash 
python3 sgdk_parallax_image.py 
```
This genreates an image called `bg.png` and should  output text similar to the
following
```cmd
Scroll width far: 80.000
Scroll width near: 160.000
Starting row floor: 180
Ending row floor: 223
* Scroll increment floor: 0.0233
* Final Scroll increment floor: 1.8605
Image size 480 x 224
```

This lets you know that the tile takes up 80 pixels at the top of the floor
pattern and 160 at the bottom.   It also lets you know where the floor pattern
starts and stops in the image (rows 180 and 223). The scroll increment is the 
amount each successive scroll line should increase if you were scrolling the top
row one pixel at a time.   



If you want the floor rows to scroll right to left, you'd move row 80 left
by one pixel, row 81 left by 1.0233 pixels, row 81 by 1.0466, pixels, and so
on.  The final row (223) would be moved left by 1 +  pixels.
```c
  ++scrollStep;
  fix32 fStep = FIX32(1.0);
  for( u16 row=180; row<224; ++row ) {
    fscroll[row] = fix32Sub( fscroll[row], fStep ); // shift row left
    fStep = fix32Add( fStep, FIX32(0.0233));        // change shift by 0.0233 pixels
  }
```
This works up to 80 pixels for the top row.  This is because floor pattern
repeats every 80 pixles. To handle this, reset the scroll values when the top
row has moved 80 pixels.
```c
    scrollStep = 0;
    memset(fscroll, 0, sizeof(fscroll)); // zero out all scroll values
```
Put together:
```c
static scrollLeftLoop() {
  ++scrollStep;
  fix32 fStep = FIX32(1.0);
  if (scrollStep < 80) {
    for( u16 row=180; row<224; ++row ) {
      fscroll[row] = fix32Sub( fscroll[row], fStep );
      fStep = fix32Add( fStep, FIX32(0.0233));
    }
  } else {
    scrollStep = 0;
    memset(fscroll, 0, sizeof(fscroll));
  }
}
```

Scrolling the floor floor left to right is similar. The main difference is you
move the rows in the opposite direction.  Row 80 moves to the right by one
pixel, row 81 right by 1.0233 pixels, row 81 by 1.0466, pixels, etc.
```c
  --scrollStep;
  fix32 fStep = FIX32(1.0);
  for (u16 row = 180; row < 224; ++row)
  {
    fscroll[row] = fix32Add(fscroll[row], fStep);
    fStep = fix32Add(fStep, FIX32(0.0233));
  }
```
Again, this only works up to 80 pixels for the top row.  Resetting the scroll
lines is a bit more complex here.  You have to put all of the lines at their
Final scroll position. This can be computed with the 'final scroll increment'
value output from the script.  In this case:

`* Final Scroll increment floor: 1.8605`

```c
    scrollStep = 80;
    fix32 scroll = FIX32(-80.0);  // scrolled by 80.
    for (u16 row = 180; row < 224; ++row)
    {
      fscroll[row] = scroll;
      scroll = fix32Sub(scroll, FIX32(1.8605)); // decrement by final value
    }
```
Put together:
```c
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
```



## 
```bash
python3 sgdk_parallax_image.py -i test_wood.png   -f 6 -n 4   -S 1 -E 40  -I test_tile.png  -p ../TESTP
```



