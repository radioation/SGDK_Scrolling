# Parallax Image Generation Script

`sgdk_parallax_iamge.py` is a simple script that creates endless scrolling image 
backgrounds for use with Sega Genesis / SGDK.  You must supply at least one 
16-color indexed image to be warped into a repeating floor pattern.  The generated
image can optionally have a ceiling pattern.  The ceiling pattern can use the 
same pattern as the floor or a separate image can supplied for the ceiling pattern.





python3 sgdk_parallax_image.py -i test_wood.png   -f 6 -n 4   -S 1 -E 40  -I test_tile.png  -p ../TESTP
