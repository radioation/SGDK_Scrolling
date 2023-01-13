
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
  rotStep = 1.0   # angle step size in degrees
  if args.angle_increment:
    rotStep =  args.angle_increment 

  # default to center 18 columns
  colStart = 1
  if args.column_start:
    colStart =  args.column_start
  colEnd = 18 
  if args.column_end:
    colEnd =  args.column_end_
  totalCols = colEnd - colStart +1 # inclusive total+1
  centerX = 10    # in columns not pixels
  if args.center_x:
    centerX =  args.center_x 

  # default to all rows.
  rowStart = 0
  if args.row_start:
    rowStart =  args.row_start
  rowEnd = 223
  if args.row_end:
    rowEnd =  args.row_end
  totalRows =  rowEnd - rowStart +1 # inclusive total +1
  centerY = 112   # rows are pixels/lines.  Center is 112 for NTSC
  if args.center_y:
    centerY =  args.center_y 

  outputFilename = "rotation.h"
  if args.output_filename:
    outputFilename =  args.output_filename

  prefix = ""
  if args.prefix:
    prefix =  args.prefix

  # Sega specific
  COL_WIDTH = 16   # 16 pixel width
  ROW_HEIGHT = 1   # rows are 1 pixel in height, might be plausible to do 8 for tiled?

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
  outfile.write("\n\n%shScroll = {" % (prefix + "_") )
  # horizontal scrolling values
  offset = 0
  for deg in np.arange( minRot, maxRot, rotStep ):
    rad = deg * math.pi/180; 
    outfile.write("\n  // rotation values for angle %f starts at %d\n" %( deg, offset) )
    for row in range( rowStart, rowEnd+1, ROW_HEIGHT ):
      rowShift = (row-centerY) * math.sin( rad ) #  - rowStart 
      logging.debug("HSCROLL deg:%f row: %d scroll value:%d", deg, row, rowShift)
      outfile.write( str(round(rowShift)) )
      outfile.write( ", " )
      offset +=1
  outfile.write("0 }")
    
  outfile.write("\n\n%svScroll = {" % (prefix + "_"))
  offset = 0
  # vertical scrolling values
  for deg in np.arange( minRot, maxRot, rotStep ):
    rad = deg * math.pi/180; 
    outfile.write("\n // rotation values for angle %f starts at %d\n" % (deg,offset))
    for col in range( colStart, colEnd+1, 1 ):
      colShift =  16 * (col - centerX) * math.sin( rad )
      logging.debug("VSCROLL deg:%f col: %d scroll value:%d", deg, col, colShift)
      outfile.write( str(round(colShift)) )
      outfile.write( ", " )
      offset +=1

  outfile.write("0 }")

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
                      "--column_start",
                      type=int,
                      help = "First column to rotate",
                      metavar = "ARG")
  parser.add_argument( "-C",
                      "--column_end",
                      type=int,
                      help = "Last column to rotate",
                      metavar = "ARG")
  parser.add_argument( "-x",
                      "--center_x",
                      type=int,
                      help = "Which column is the center of rotation",
                      metavar = "ARG")


  parser.add_argument( "-r",
                      "--row_start",
                      type=int,
                      help = "First row to rotate",
                      metavar = "ARG")
  parser.add_argument( "-R",
                      "--row_end",
                      type=int,
                      help = "Last row to rotate",
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

  parser.add_argument( "-p",
                      "--prefix",
                      help = "Add a prefix to array names",
                      metavar = "ARG")
  args = parser.parse_args()
  
  # Setup logging
  if args.verbose:
    loglevel = logging.DEBUG
  else:
    loglevel = logging.INFO
  
  main(args, loglevel)


