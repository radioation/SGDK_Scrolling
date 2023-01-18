
#!/usr/bin/env python
#

import os, argparse, logging
import math
import numpy as np

def main(args, loglevel):
  logging.basicConfig(format="%(levelname)s: %(message)s", level=loglevel)
  
  logging.debug("Arguments:")
  logging.debug(args)

  minRot = args.start_angle
  maxRot = args.end_angle
  rotStep = args.angle_increment
  if minRot > maxRot:
    print("Starting angle is larger than end angle")
    return
  if rotStep <= 0.0:
    print("Invalid angle increment")
    return

  # default to center 18 columns
  colStart = args.column_start
  colEnd = args.column_end
  totalCols = colEnd - colStart +1 # inclusive total+1

  centerX = args.center_x
  imageWidth = args.image_width
  imageShift = -( imageWidth - 320) / 2;

  # default to all rows.
  rowStart = args.row_start
  rowEnd = args.row_end
  totalRows =  rowEnd - rowStart +1 # inclusive total +1
  centerY = args.center_y

  # where do we put the file at
  outputFilename = args.output_filename

  # naming the variables
  prefix =  args.prefix

  # Sega specific
  COL_WIDTH = 16   # 16 pixel width
  ROW_HEIGHT = 1   # rows are 1 pixel in height, might be plausible to do 8 for tiled?

  logging.info("Parameters")
  logging.info("Start angle: %f Stop angle: %f Step size: %f", minRot, maxRot, rotStep)
  logging.info("Columns to rotate: %d Center column: %d", totalCols, centerX)
  logging.info("Rows to rotate: %d Center row: %d", totalRows, centerY)

  print("#define ROWS_A %d" % totalRows )
  print("#define START_ROW_A %d" % rowStart )
  print("#define END_ROW_A %d" % rowEnd )
  print("#define COLS_A %d" % totalCols )
  print("#define START_COL_A %d" % colStart )
  print("#define END_COL_A %d" % colEnd )

  # Check if file exists
  isFile = os.path.isfile( outputFilename )
  if isFile:
    print("File: %s exists! exiting", outputFilename)
    return

  # open file
  outfile = open( outputFilename, 'w')
  outfile.write("#ifndef _%s_\n" % outputFilename.upper().replace(".","_") )
  outfile.write("#define _%s_\n" % outputFilename.upper().replace(".","_") )
  outfile.write("\n\n#define %s_SCROLL_COUNT %d\n" % ( prefix.upper().replace(".","_"), int(1+ (maxRot - minRot)/rotStep) )) 
  outfile.write("\n\ns16 %shScroll[] = {" % (prefix + "_") )
  # horizontal scrolling values
  offset = 0
  for deg in np.arange( minRot, maxRot + rotStep, rotStep ): # include end rot
    rad = deg * math.pi/180; 
    outfile.write("\n  // rotation values for angle %f starts at %d\n" %( deg, offset) )
    for row in range( rowStart, rowEnd+1, ROW_HEIGHT ):
      rowShift = (row-centerY) * math.sin( rad ) + imageShift
      logging.debug("HSCROLL deg:%f row: %d scroll value:%d", deg, row, rowShift)
      outfile.write( str(round(rowShift)) )
      outfile.write( ", " )
      offset +=1
  outfile.write("0 };")
    
  outfile.write("\n\ns16 %svScroll[] = {" % (prefix + "_"))
  offset = 0
  # vertical scrolling values
  for deg in np.arange( minRot, maxRot + rotStep, rotStep ): # include end rot
    rad = deg * math.pi/180; 
    outfile.write("\n // rotation values for angle %f starts at %d\n" % (deg,offset))
    for col in range( colStart, colEnd+1, 1 ):
      colShift =  16 * (col - centerX) * math.sin( rad )
      logging.debug("VSCROLL deg:%f col: %d scroll value:%d", deg, col, colShift)
      outfile.write( str(round(colShift)) )
      outfile.write( ", " )
      offset +=1

  outfile.write("0 };")

  outfile.write("\n\n#endif // _%s_\n" % outputFilename.upper().replace(".","_") )
  
 
# Standard boilerplate to call the main() function to begin
# the program.
if __name__ == '__main__':
  parser = argparse.ArgumentParser( description = "Generates rotation arrays for SGDK.",
                                    epilog = "As an alternative to the commandline, params can be placed in a file, one per line, and specified on the commandline like '%(prog)s @params.conf'.",
                                    fromfile_prefix_chars = '@' )
  # TODO Specify your real parameters here.
  parser.add_argument( "-v",
                      "--verbose",
                      help="Print debug messages",
                      action="store_true")

  parser.add_argument( "-s",
                      "--start_angle",
                      default=-5.0,
                      type=float,
                      help = "Starting rotation in degrees",
                      metavar = "ARG")
  parser.add_argument( "-e",
                      "--end_angle",
                      default=5.0,
                      type=float,
                      help = "End rotation angle in degrees",
                      metavar = "ARG")
  parser.add_argument( "-i",
                      "--angle_increment",
                      default=1.0,
                      type=float,
                      help = "Rotation step size",
                      metavar = "ARG")
  
  parser.add_argument( "-c",
                      "--column_start",
                      default=1,
                      type=int,
                      help = "First column to rotate",
                      metavar = "ARG")
  parser.add_argument( "-C",
                      "--column_end",
                      default=18,
                      type=int,
                      help = "Last column to rotate",
                      metavar = "ARG")
  parser.add_argument( "-x",
                      "--center_x",
                      default=9,
                      type=int,
                      help = "Which column is the center of rotation",
                      metavar = "ARG")

  parser.add_argument( "-w",
                      "--image_width",
                      default=320,
                      type=int,
                      help = "Width of image to rotate",
                      metavar = "ARG")

  parser.add_argument( "-r",
                      "--row_start",
                      default=0,
                      type=int,
                      help = "First row to rotate",
                      metavar = "ARG")
  parser.add_argument( "-R",
                      "--row_end",
                      default=223,
                      type=int,
                      help = "Last row to rotate",
                      metavar = "ARG")
  parser.add_argument( "-y",
                      "--center_y",
                      default=112,
                      type=int,
                      help = "Which row is the center of rotation",
                      metavar = "ARG")

  parser.add_argument( "-o",
                      "--output_filename",
                      default="rotation.h",
                      help = "Output filename",
                      metavar = "ARG")

  parser.add_argument( "-p",
                      "--prefix",
                      default="",
                      help = "Add a prefix to array names",
                      metavar = "ARG")
  args = parser.parse_args()
  
  # Setup logging
  if args.verbose:
    loglevel = logging.DEBUG
  else:
    loglevel = logging.INFO
  
  main(args, loglevel)


