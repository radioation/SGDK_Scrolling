
#!/usr/bin/env python
#

import os, argparse, logging, csv
import math
import numpy as np
from jinja2 import Template
import shutil
from pathlib import Path
from PIL import Image


def makeProjectFiles(destDir, rowStart, rowEnd, totalRows, colStart, colEnd, totalCols, centerY, prefixStr, backgroundName, spriteName, targetList, outputFilename ):
  # make resource dir and rescomp file
  srcFolder = destDir + "/src"
  if not os.path.exists(srcFolder):
    os.makedirs(srcFolder)
  resFolder = destDir + "/res"
  if not os.path.exists(resFolder):
    os.makedirs(resFolder)
  bgFolder = resFolder + "/bg"
  if not os.path.exists(bgFolder):
    os.makedirs(bgFolder)
  spritesFolder = resFolder + "/sprites"
  if not os.path.exists(spritesFolder):
    os.makedirs(spritesFolder)

  # Copy files to resource folders
  shutil.copy( backgroundName + str(".png"), bgFolder )
  shutil.copy( spriteName + str(".png"), spritesFolder )
  shutil.copy( outputFilename, srcFolder + "/" + outputFilename )

  # Make resources file from template
  with open('resources.res.jinja') as resFile:
    resTemp = Template( resFile.read() )
    with open( resFolder + "/resources.res", 'w') as outRes:
      outRes.write( resTemp.render(
        bg_name = backgroundName,
        sprite_name = spriteName
        ))

  # Make main.c file from template
  with open('main.c.jinja') as srcFile:
    srcTemp = Template( srcFile.read() )
    with open( srcFolder + "/main.c", 'w') as outSrc:
      outSrc.write( srcTemp.render(
        rotation_filename = outputFilename,
        start_row_a = rowStart,
        end_row_a = rowEnd,
        rows_a = totalRows,
        start_col_a = colStart,
        end_col_a = colEnd,
        cols_a = totalCols,
        prefix = prefixStr,
        bg_name = backgroundName,
        sprite_name = spriteName,
        target_list = targetList
        ))

   
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

  # check background image size if not specified and we're making a project dir
  projectDir = args.project_directory
  backgroundFilename = args.background_filename
  backgroundName = ''
  if os.path.isfile( backgroundFilename ):
    backgroundName = Path( backgroundFilename ).stem
  else:
    print('Unable to find ' + backgroundFilename)
    return

  if args.image_width:
    imageWidth = args.image_width
  else:
    # if we're creating a project, get the width from the image
    if len(projectDir) > 0 and len(backgroundFilename) > 0 :
      im = Image.open( backgroundFilename ) 
      imWidth, imHeight = im.size
      if( imWidth > 0 ):
        imageWidth = imWidth
    else:
      # if we're not, assume 320
      imageWidth = 320



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


  # (optional) points to rotate 
  pointsFilename = args.points_filename
  pointsToRotate = { }
  isFile = os.path.isfile( pointsFilename )
  if isFile:
    with open( pointsFilename ) as csvFile:
      csvReader = csv.reader( csvFile, delimiter=',')
      for row in csvReader:
        if len(row) >= 3 and row[1].strip().isnumeric() and row[2].strip().isnumeric():
          newKey = row[0]
          while newKey in  pointsToRotate:
            newKey = row[0] + str(sn)
            sn += 1
          pointsToRotate[newKey] = ( float(row[1].strip()), float(row[2].strip()) )
      print('found points to rotate:')
      print( pointsToRotate )

  
  spriteFilename = args.sprite_filename
  spriteName = ''
  if os.path.isfile( spriteFilename ):
    spriteName = Path( spriteFilename ).stem
  else:
    print('Unable to find ' + spriteFilename)
    return
  

  # Sega specific
  COL_WIDTH = 16   # 16 pixel width
  ROW_HEIGHT = 1   # rows are 1 pixel in height, might be plausible to do 8 for tiled?

  
  logging.info("Parameters")
  logging.info("Start angle: %f Stop angle: %f Step size: %f", minRot, maxRot, rotStep)
  logging.info("Columns to rotate: %d Center column: %d", totalCols, centerX)
  logging.info("Rows to rotate: %d Center row: %d", totalRows, centerY)

  # output C defines for colums and rows in 
  print("#define ROWS_A %d" % totalRows )
  print("#define START_ROW_A %d" % rowStart )
  print("#define END_ROW_A %d" % rowEnd )
  print("#define COLS_A %d" % totalCols )
  print("#define START_COL_A %d" % colStart )
  print("#define END_COL_A %d" % colEnd )

  # Check if file exists, exit to force user to make decision to delete isntead of just blowing it away.
  isFile = os.path.isfile( outputFilename )
  if isFile:
    print("File: %s exists! exiting" % outputFilename)
    return

  # open file
  outfile = open( outputFilename, 'w')
  outfile.write("#ifndef _%s_\n" % outputFilename.upper().replace(".","_") )
  outfile.write("#define _%s_\n" % outputFilename.upper().replace(".","_") )
  outfile.write("\n\n#define %s_SCROLL_COUNT %d\n" % ( prefix.replace(".","_"), int(1+ (maxRot - minRot)/rotStep) )) 
  outfile.write("#define %s_ROWS_A %d\n" % (prefix, totalRows ))
  outfile.write("#define %s_START_ROW_A %d\n" % (prefix, rowStart ))
  outfile.write("#define %s_END_ROW_A %d\n" % (prefix, rowEnd ))
  outfile.write("#define %s_COLS_A %d\n" % (prefix, totalCols ))
  outfile.write("#define %s_START_COL_A %d\n" % (prefix, colStart ))
  outfile.write("#define %s_END_COL_A %d\n" % (prefix, colEnd ))
  outfile.write("\n\ns16 %s_hScroll[] = {" % (prefix ) )
  # horizontal scrolling values
  offset = 0
  for deg in np.arange( minRot, maxRot + rotStep, rotStep ): # include end rot
    rad = deg * math.pi/180; 
    outfile.write("\n  // rotation values for angle %f starts at %d\n" %( deg, offset) )
    for row in range( rowStart, rowEnd+1, ROW_HEIGHT ):
      rowShift = (row-centerY) * math.sin( rad ) + imageShift
      logging.debug("HSCROLL deg:%f row: %d scroll value:%d", deg, row, rowShift)
      if offset > 0: 
        outfile.write( ", " )
      outfile.write( str(round(rowShift)) )
      offset +=1
  outfile.write("};\n")
    
  outfile.write("\n\ns16 %svScroll[] = {" % (prefix + "_"))
  offset = 0
  # vertical scrolling values
  for deg in np.arange( minRot, maxRot + rotStep, rotStep ): # include end rot
    rad = deg * math.pi/180; 
    outfile.write("\n // rotation values for angle %f starts at %d\n" % (deg,offset))
    for col in range( colStart, colEnd+1, 1 ):
      colShift =  16 * (col - centerX) * math.sin( rad )
      logging.debug("VSCROLL deg:%f col: %d scroll value:%d", deg, col, colShift)
      if offset > 0: 
        outfile.write( ", " )
      outfile.write( str(round(colShift)) )
      offset +=1
  outfile.write("\n};\n")

  if len(pointsToRotate) > 0:
    # loop over again
    for key, val in pointsToRotate.items():
      first = True
      outfile.write("\n\ns16 %s[] = {" % (key))
      for deg in np.arange( minRot, maxRot + rotStep, rotStep ): # include end rot
        rad = deg * math.pi/180; 
        # Using real rotation but Y-flipped due to genesis coordinate system.
        newX = centerX* 16 + ( val[0] + imageShift - centerX * 16) * math.cos(rad) - (centerY - val[1])  * math.sin(rad)
        # find the column of current point
        newY =  224 - ( centerY +   (val[0] + imageShift - centerX * 16) * math.sin(rad) + (centerY - val[1]) * math.cos(rad) )
        if not first:
          outfile.write( ", " )
        else:
          first = False;

        outfile.write("\n %d, %d" % (round(newX), round(newY)))
      outfile.write("\n};")


  outfile.write("\n\n#endif // _%s_\n" % outputFilename.upper().replace(".","_") )
  outfile.close()

  # if defined, create an SGDK project skeleton
  if len(projectDir) > 0:
    makeProjectFiles(projectDir, rowStart, rowEnd, totalRows, colStart, colEnd, totalCols, centerY, prefix, backgroundName, spriteName, pointsToRotate.keys(), outputFilename )
  
 
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
                      default=0,
                      type=int,
                      help = "First column to rotate (default 0)",
                      metavar = "ARG")
  parser.add_argument( "-C",
                      "--column_end",
                      default=19,
                      type=int,
                      help = "Last column to rotate (default 19)",
                      metavar = "ARG")
  parser.add_argument( "-x",
                      "--center_x",
                      default=9,
                      type=int,
                      help = "Which column is the center of rotation",
                      metavar = "ARG")

  parser.add_argument( "-w",
                      "--image_width",
                      #default=320,
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

  parser.add_argument( "-P",
                      "--prefix",
                      default="",
                      help = "Add a prefix to array names",
                      metavar = "ARG")

  parser.add_argument( "-p",
                      "--project_directory",
                      default="",
                      help = "Create project directory with resource files and simple SGDK code.",
                      metavar = "ARG")

  parser.add_argument( "-b",
                      "--background_filename",
                      default="ship.png",
                      help = "Specify background image",
                      metavar = "ARG")

  parser.add_argument( "-t",
                      "--points_filename",
                      default="",
                      help = "Specify CSV file with points to rotate along with bg",
                      metavar = "ARG")

  parser.add_argument( "-S",
                      "--sprite_filename",
                      default="crosshairs.png",
                      help = "Specify target sprite image",
                      metavar = "ARG")

  args = parser.parse_args()
  
  # Setup logging
  if args.verbose:
    loglevel = logging.DEBUG
  else:
    loglevel = logging.INFO
  
  main(args, loglevel)


