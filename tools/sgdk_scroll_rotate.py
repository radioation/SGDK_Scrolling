
#!/usr/bin/env python
#

import os, argparse, logging
import math
import numpy as np

def main(args, loglevel):
  logging.basicConfig(format="%(levelname)s: %(message)s", level=loglevel)
  
  logging.debug("Arguments:")
  logging.debug(args)

  minRot = -10.0  # start angle in degrees
  if args.start_angle:
    minRot =  args.start_angle 
  maxRot = 10.0   # end angle in degrees.
  if args.end_angle:
    maxRot =  args.end_angle 
  rotStep = 1.0   # step size.
  if args.angle_increment:
    rotStep =  args.angle_increment 

  totalCols = 18  # only do the center 18 (of 20 cols)
  if args.cols:
    totalCols =  args.cols 
  totalRows = 224 # default to all rows.
  if args.rows:
    totalRows =  args.rows 
  centerX = 10    # in columns not pixels
  if args.center_x:
    centerX =  args.center_x 
  centerY = 112   # in pixels/lines
  if args.center_y:
    centerY =  args.center_y 

  outputFilename = "rotation.h"
  if args.output_filename:
    outputFilename =  args.output_filename


  # Sega specific
  COL_WIDTH = 16   # 16 pixel width
  ROW_HEIGHT = 1   # rows are 1 pixel in height

  logging.info("Parameters")
  logging.info("Start angle: %f Stop angle: %f Step size: %f", minRot, maxRot, rotStep)
  logging.info("Columns to rotate: %d Center column: %d", totalCols, centerX)
  logging.info("Rows to rotate: %d Center row: %d", totalRows, centerY)
 
  # Check if file exists
  isFile = os.path.isfile( outputFilename )
  if isFile:
    print("File: %s exists! exiting", outputFilename)
    return

  # open file
  outfile = open( outputFilename, 'w')
  outfile.write("#ifndef _%s_\n" % outputFilename.upper().replace(".","_") )
  outfile.write("#define _%s_\n" % outputFilename.upper().replace(".","_") )
  outfile.write("\n\nhScroll = {")
  # horizontal scrolling values
  for deg in np.arange( minRot, maxRot, rotStep ):
    rad = deg * math.pi/180; 
    startRow = int(centerY - totalRows / 2 )
    stopRow = int(centerY + totalRows / 2 )
    for row in range( startRow, stopRow, ROW_HEIGHT ):
      rowShift = row-centerY * math.sin( rad ) - startRow - 24
      logging.debug("HSCROLL deg:%f row: %d scroll value:%d", deg, row, rowShift)
      outfile.write( str(round(rowShift)) )
      outfile.write( ", " )
    outfile.write("  // rotation values for angle %f \n" % deg)
  outfile.write("0 }")
    
  outfile.write("\n\nhScroll = {")
  # vertical scrolling values
  for deg in np.arange( minRot, maxRot, rotStep ):
    rad = deg * math.pi/180; 
    startCol = int(centerX - totalCols / 2 )
    stopCol = int(centerX + totalCols / 2 )
    for col in range( startCol, stopCol, 1 ):
      colShift =  16 * (col - centerX) * math.sin( rad )
      logging.debug("VSCROLL deg:%f col: %d scroll value:%d", deg, col, colShift)
      outfile.write( str(round(colShift)) )
      outfile.write( ", " )

    outfile.write("  // rotation values for angle %f \n" % deg)
  outfile.write("0 }")

  outfile.write("#endif // _%s_\n" % outputFilename.upper().replace(".","_") )
  
 
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
                      type=float,
                      help = "Starting rotation",
                      metavar = "ARG")
  parser.add_argument( "-e",
                      "--end_angle",
                      type=float,
                      help = "End rotation angle",
                      metavar = "ARG")
  parser.add_argument( "-i",
                      "--angle_increment",
                      type=float,
                      help = "Rotation step size",
                      metavar = "ARG")
  
  parser.add_argument( "-c",
                      "--cols",
                      type=int,
                      help = "How many columns to rotate",
                      metavar = "ARG")
  parser.add_argument( "-x",
                      "--center_x",
                      type=int,
                      help = "Which column is the center of rotation",
                      metavar = "ARG")

  parser.add_argument( "-r",
                      "--rows",
                      type=int,
                      help = "How many rows to rotate",
                      metavar = "ARG")
  parser.add_argument( "-y",
                      "--center_y",
                      type=int,
                      help = "Which row is the center of rotation",
                      metavar = "ARG")

  parser.add_argument( "-o",
                      "--output_filename",
                      help = "Output filename",
                      metavar = "ARG")

  args = parser.parse_args()
  
  # Setup logging
  if args.verbose:
    loglevel = logging.DEBUG
  else:
    loglevel = logging.INFO
  
  main(args, loglevel)


