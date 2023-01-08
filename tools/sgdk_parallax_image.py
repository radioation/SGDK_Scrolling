#!/usr/bin/env python

import os,  argparse, logging
import numpy as np
import math
from PIL import Image
import cv2

# Gather our code in a main() function
def main(args, loglevel):
  logging.basicConfig(format="%(levelname)s: %(message)s", level=loglevel)
  startRow = 160
  if args.start_row:
    startRow =  args.start_row
  endRow = 223
  if args.end_row:
    endRow =  args.end_row

  imageFilename = "image.png" 
  if args.input_filename:
    imageFilename =  args.input_filename

  outputFilename = "bg.png"
  if args.output_filename:
    outputFilename =  args.output_filename

  COLS = 320  
  ROWS = 224
  REPEAT = 5

  with Image.open( imageFilename ) as im:
    inputImg = im.convert('RGB')
    inputWidth, inputHeight = im.size
    #indexed = np.array(im) 
    inputCv = np.array(inputImg)

    pal = im.getpalette()

    # work image
    tmpImg = Image.new('RGB', (COLS, ROWS))
    tmpCv = np.array(tmpImg)

    # get source points
    # srcRect = ((0,0), (inputWidth,inputHeight))
    srcTopLeft = ( 0, 0 )
    srcTopRight = ( inputWidth, 0 )
    srcBottomLeft = ( 0, inputHeight ) 
    srcBottomRight = ( inputWidth, inputHeight )
    srcPts = np.array( [ srcBottomLeft, srcBottomRight, srcTopRight, srcTopLeft] )    


    # detination points
    dstTopLeft = ( 130, startRow )
    dstTopRight = ( 159, startRow )
    dstBottomLeft = ( 100, endRow ) 
    dstBottomRight = ( 159, endRow  )
    dstPts = np.array( [ dstBottomLeft, dstBottomRight, dstTopRight, dstTopLeft] )    
    dstPoly = np.array( [[ dstBottomLeft, dstBottomRight, dstTopRight, dstTopLeft]] )


    # Get Perspective Transform Algorithm
    srcPtsList = np.float32( srcPts.tolist() )
    dstPtsList = np.float32( dstPts.tolist() )
    xfrmMatrix = cv2.getPerspectiveTransform(srcPtsList, dstPtsList)
    
    # warp it
    image_size = (tmpCv.shape[1], tmpCv.shape[0])
    warpCv = cv2.warpPerspective(inputCv, xfrmMatrix, dsize=image_size)
    warpImg = Image.fromarray( warpCv )
    warpImg.save( "warp.png" )

    # copy mask 
    maskCv = np.zeros_like(tmpCv)
    maskCv = cv2.fillPoly(maskCv, pts = dstPoly, color = (255, 255, 255) )
    maskCv = maskCv.all(axis=2)
    tmpCv[maskCv, :] = warpCv[maskCv, :]
    maskImg = Image.fromarray( tmpCv )
    maskImg.save( "mask.png" )





    # convert backto PIL 
    outImg = Image.new('P', (COLS, ROWS))
    outImg.putpalette(pal)
    outImg.save( outputFilename )



# the program.
if __name__ == '__main__':
  parser = argparse.ArgumentParser( 
      description = "Create parallax scrolling images for SGDK",
      epilog = "As an alternative to the commandline, params can be placed in a file, one per line, and specified on the commandline like '%(prog)s @params.conf'.",
      fromfile_prefix_chars = '@' )
  # parameter list
  parser.add_argument(
      "-v",
      "--verbose",
      help="increase output verbosity",
      action="store_true")

  parser.add_argument( "-s",
      "--start_row",
      type=int,
      help = "Which row starts the floor",
      metavar = "ARG")

  parser.add_argument( "-e",
      "--end_row",
      type=int,
      help = "Which row ends the floor",
      metavar = "ARG")

  parser.add_argument( "-i",
      "--input_filename",
      help = "input image filename",
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
