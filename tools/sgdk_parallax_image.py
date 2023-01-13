#!/usr/bin/env python

import os,  argparse, logging
import numpy as np
import math
from PIL import Image
import cv2

#def makeProjectFiles( destDir, imageFilename ):
#  # make resource dir and rescomp file
#  print('todo')
#  # make bare source file with some scrolling


def main(args, loglevel):
  logging.basicConfig(format="%(levelname)s: %(message)s", level=loglevel)
  startRow = 160
  if args.start_row:
    startRow =  args.start_row
  endRow = 223
  if args.end_row:
    endRow =  args.end_row

  farImageReps = 4
  if args.far_image_reps:
    farImageReps =  args.far_image_reps
  nearImageReps = 2
  if args.near_image_reps:
    nearImageReps =  args.near_image_reps

  imageFilename = "image.png" 
  if args.input_filename:
    imageFilename =  args.input_filename

  outputFilename = "bg.png"
  if args.output_filename:
    outputFilename =  args.output_filename

  COLS = 320  
  rows = 224

  topPolyWidth = COLS / farImageReps
  bottomPolyWidth = COLS / nearImageReps
  bottomTotalWidth = bottomPolyWidth * farImageReps
  offset = farImageReps % 2 != nearImageReps % 2
  outputCols = int(COLS + bottomPolyWidth)

  with Image.open( imageFilename ) as im:
    inputImg = im.convert('RGB')
    inputWidth, inputHeight = im.size
    #indexed = np.array(im) 
    inputCv = np.array(inputImg)

    pal = im.getpalette()

    # get source points
    srcTopLeft = ( 0, 0 )
    srcTopRight = ( inputWidth, 0 )
    srcBottomLeft = ( 0, inputHeight ) 
    srcBottomRight = ( inputWidth, inputHeight )
    srcPts = np.array( [ srcBottomLeft, srcBottomRight, srcTopRight, srcTopLeft] )    

    # work image
    tmpImg = Image.new('RGB', (outputCols, rows))
    tmpCv = np.array(tmpImg)
    bottomLeftStart = COLS / 2 - bottomTotalWidth / 2
    for rep in range( 0, farImageReps, 1 ):
      # detination points
      dstTopLeft = ( rep * topPolyWidth, startRow )
      dstTopRight = ( rep * topPolyWidth + topPolyWidth, startRow )
      dstBottomLeft = ( bottomLeftStart + rep * bottomPolyWidth, endRow ) 
      dstBottomRight = ( bottomLeftStart + rep * bottomPolyWidth + bottomPolyWidth, endRow  )
      dstPts = np.array( [ dstBottomLeft, dstBottomRight, dstTopRight, dstTopLeft] )    
      dstPoly = np.array( [ dstBottomLeft, dstBottomRight, dstTopRight, dstTopLeft], dtype=np.int32 )
      #print( dstPoly )
      #dstPoly = np.array([[160.0, 130.0], [350, 130], [250, 300]], dtype=np.int32)
        

      # Get Perspective Transform Algorithm
      srcPtsList = np.float32( srcPts.tolist() )
      dstPtsList = np.float32( dstPts.tolist() )
      xfrmMatrix = cv2.getPerspectiveTransform(srcPtsList, dstPtsList)
  
      # warp it
      image_size = (tmpCv.shape[1], tmpCv.shape[0])
      warpCv = cv2.warpPerspective(inputCv, xfrmMatrix, dsize=image_size)
      warpImg = Image.fromarray( warpCv )
      warpImg.save( "warp_%d.png" %(rep) )
  
      ## create a copy mask 
      print( dstPoly )
      maskCv = np.zeros_like(tmpCv)
      maskCv = cv2.fillPoly(maskCv, pts = [dstPoly], color = (255, 255, 255) )
      maskCv = maskCv.all(axis=2)
      # copy warped image
      tmpCv[maskCv, :] = warpCv[maskCv, :]
      #maskImg = Image.fromarray( tmpCv )
      #maskImg.save( "mask_%d.png"%(rep) )
  
    # convert backto PIL 
    maskImg = Image.fromarray( tmpCv )
    maskImg.save( "mask.png")
    outImg = Image.new('P', (outputCols, rows))
    outImg.putpalette(pal)
    outImg.save( outputFilename )



# the program.
if __name__ == '__main__':
  parser = argparse.ArgumentParser( 
      description = "Create parallax scrolling background for SGDK",
      epilog = "As an alternative to the commandline, params can be placed in a file, one per line, and specified on the commandline like '%(prog)s @params.conf'.",
      fromfile_prefix_chars = '@' )
  # parameter list
  parser.add_argument(
      "-v",
      "--verbose",
      help="increase output verbosity",
      action="store_true")

  parser.add_argument( "-f",
      "--far_image_reps",
      type=int,
      help = "How many times to repeat image at start of floor",
      metavar = "ARG")

  parser.add_argument( "-n",
      "--near_image_reps",
      type=int,
      help = "How many times to repeat image at end of floor",
      metavar = "ARG")

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
