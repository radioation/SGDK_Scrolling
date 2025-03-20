#!/usr/bin/env python

import os,  argparse, logging
import numpy as np
import math
from PIL import Image, ImageDraw
import cv2
import shutil
from jinja2 import Template
from pathlib import Path

# 320 x 224  
COLS = 320  
rows = 224


def makeProjectFiles( destDir, imageFilename, endRow, startRow, nearPolyWidth, farPolyWidth, endCeilingRow, startCeilingRow, imageWidth ):
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

  # copy out image to backtround folder
  shutil.copy( imageFilename, bgFolder )

  # Make resource file
  fname = Path( imageFilename ).stem
  # Make resources file from template
  with open('resources.res.jinja') as resFile:
    resTemp = Template( resFile.read() )
    with open( resFolder + "/resources.res", 'w') as outRes:
      outRes.write( resTemp.render(
        bg_name = fname
        ))

  # Make main.c file from template
  scrollLeftList=[]
  scrollCeilingRows = endCeilingRow - startCeilingRow
  scrollCeilingRatio = nearPolyWidth / farPolyWidth 
  scrollCeilingRowStep = 0
  scrollCeilingIncrement = 1.0

  if startCeilingRow > -1  and  endCeilingRow > startCeilingRow :
    scrollCeilingRowStep = ( scrollCeilingRatio - 1.0 ) / scrollCeilingRows
    for r in range( endCeilingRow, startCeilingRow - 1, -1):
      #srcfile.write( "    fscroll[%d] = fix32Sub( fscroll[%d], FIX32(%.3f));\n" % ( r, r, scrollCeilingIncrement ) )
      scrollLeftList.append( (r, scrollCeilingIncrement ) )
      scrollCeilingIncrement += scrollCeilingRowStep


  scrollRows = endRow - startRow
  scrollRatio = nearPolyWidth / farPolyWidth 
  scrollRowStep = ( scrollRatio - 1.0 ) / scrollRows
  scrollIncrement = 1.0;
  for r in range( startRow, endRow + 1, 1):
    ##srcfile.write( "    fscroll[%d] = fix32Sub( fscroll[%d], FIX32(%.3f));\n" % ( r, r, scrollIncrement ) )
    scrollLeftList.append( (r, scrollIncrement ) )
    scrollIncrement += scrollRowStep
  

  scrollRightList = []
  if startCeilingRow > -1  and  endCeilingRow > startCeilingRow :
    scrollCeilingIncrement = 1.0
    scrollCeilingRowStep = ( scrollCeilingRatio - 1.0 ) / scrollCeilingRows
    for r in range( endCeilingRow, startCeilingRow - 1, -1):
      #srcfile.write( "    fscroll[%d] = fix32Add( fscroll[%d], FIX32(%.3f));\n" % ( r, r, scrollCeilingIncrement ) )
      scrollRightList.append( (r, scrollCeilingIncrement ) )
      scrollCeilingIncrement += scrollCeilingRowStep
  scrollIncrement = 1.0;
  for r in range( startRow, endRow + 1, 1):
    #srcfile.write( "    fscroll[%d] = fix32Add( fscroll[%d], FIX32(%.3f));\n" % ( r, r, scrollIncrement ) )
    scrollRightList.append( (r, scrollIncrement ) )
    scrollIncrement += scrollRowStep

  scrollRightResetList = []

  if startCeilingRow > -1  and  endCeilingRow > startCeilingRow :
    scroll = - int( farPolyWidth)
    scrollStep =  (nearPolyWidth - farPolyWidth) / scrollCeilingRows
    for r in range( endCeilingRow, startCeilingRow , -1):
      #srcfile.write( "    fscroll[%d] = FIX32(%.3f);\n" % ( r, scroll ) )
      scrollRightResetList.append( (r, scroll ) )
      scroll -= scrollStep

  scroll = - int( farPolyWidth)
  finalScrollStep =  (nearPolyWidth - farPolyWidth) / scrollRows
  for r in range( startRow, endRow + 1, 1):
    #srcfile.write( "    fscroll[%d] = FIX32(%.3f);\n" % ( r, scroll ) )
    scrollRightResetList.append( (r, scroll ) )
    scroll -= finalScrollStep

  if imageWidth <= 512:
    with open('main.c.jinja') as srcFile:
      srcTemp = Template( srcFile.read() )
      with open( srcFolder + "/main.c", 'w') as outSrc:
        outSrc.write( srcTemp.render(
          bg_name = fname,
          far_width = farPolyWidth,
          scroll_left_list = scrollLeftList,
          scroll_right_list = scrollRightList,
          scroll_right_reset_list = scrollRightResetList,
 
          ))
  else:
    with open('main_wide.c.jinja') as srcFile:
      srcTemp = Template( srcFile.read() )
      with open( srcFolder + "/main.c", 'w') as outSrc:
        outSrc.write( srcTemp.render(
          bg_name = fname,
          far_width = farPolyWidth,
          scroll_left_list = scrollLeftList,
          image_width = imageWidth,
          ))

def createImages( floorImgFilename, ceilImgFilename, rows, outputCols, bottomTotalWidth, farImageReps, farPolyWidth, nearPolyWidth, startRow, endRow, startCeilingRow, endCeilingRow, outputFilename, imageAFilename ):
  logging.info("WORKING ON:" + floorImgFilename);
  print( outputFilename )
  with Image.open( floorImgFilename ) as im:
    inputImg = im.convert('RGB')
    inputWidth, inputHeight = im.size
    inputCv = np.array(inputImg)
    transitionFilename = ''
    warpImgs =[]
    if len( imageAFilename ) > 0 :
      with Image.open( imageAFilename ) as imgA:
        inputImgA = imgA.convert('RGB')
        tmpImgA = np.array( inputImgA )
        transitionFilename = os.path.splitext(outputFilename)[0]
  
    pal = im.getpalette() # must have same palettes.
    # get source points
    srcTopLeft = ( 0, 0 )
    srcTopRight = ( inputWidth-1, 0 )
    srcBottomLeft = ( 0, inputHeight-1 ) 
    srcBottomRight = ( inputWidth-1, inputHeight-1 )
    srcPts = np.array( [ srcBottomLeft, srcBottomRight, srcTopRight, srcTopLeft] )    
    logging.info("srcPts:");
    logging.info(srcPts);

    # work image
    tmpImg = Image.new('RGB', (outputCols, rows))
    tmpCv = np.array(tmpImg)
    bottomLeftStart = COLS / 2 - bottomTotalWidth / 2
    for rep in range( 0, farImageReps +1, 1 ):
      # detination points
      dstTopLeft = ( rep * farPolyWidth, startRow )
      dstTopRight = ( rep * farPolyWidth + farPolyWidth-0.5, startRow )
      dstBottomLeft = ( bottomLeftStart + rep * nearPolyWidth, endRow ) 
      dstBottomRight = ( bottomLeftStart + rep * nearPolyWidth + nearPolyWidth-0.5, endRow   )
      dstPts = np.array( [ dstBottomLeft, dstBottomRight, dstTopRight, dstTopLeft] )    
      dstPoly = np.array( [ dstBottomLeft, dstBottomRight, dstTopRight, dstTopLeft], dtype=np.int32 )

      logging.info("rep %d dstPts:" % ( rep ) );
      logging.info(dstPts);

      gridTopLeft = ( max( min( dstTopLeft[0] // 8, dstBottomLeft[0]//8), 0 ) , dstTopLeft[1]//8 )
      gridBottomRight = ( min( max( dstBottomRight[0] // 8, dstTopRight[0]//8), (outputCols//8) -1 ) , dstBottomRight[1]//8 )
      logging.info(gridTopLeft);
      logging.info(gridBottomRight);
      # Get Perspective Transform Algorithm
      srcPtsList = np.float32( srcPts.tolist() )
      dstPtsList = np.float32( dstPts.tolist() )
      xfrmMatrix = cv2.getPerspectiveTransform(srcPtsList, dstPtsList)

      # warp it
      image_size = (tmpCv.shape[1], tmpCv.shape[0])
      warpCv = cv2.warpPerspective(inputCv, xfrmMatrix, dsize=image_size)
      warpImg = Image.fromarray( warpCv )
      if args.output_warped_images:
        warpImg.save( "warped_floor_%d.png" %(rep) )


      ## create a copy mask 
      maskCv = np.zeros_like(tmpCv)
      maskCv = cv2.fillPoly(maskCv, pts = [dstPoly], color = (255, 255, 255) )
      maskCv = maskCv.all(axis=2)
      # copy warped image
      tmpCv[maskCv, :] = warpCv[maskCv, :]
      warpImgs.append( ( maskCv, warpCv[maskCv, :] ) ) 

    if len(transitionFilename) > 0:
      for rep in range( farImageReps , -1, -1 ):
        tmpImgA[warpImgs[rep][0], :] = warpImgs[rep][1]
        maskImgA = Image.fromarray( tmpImgA )
        ImageDraw.floodfill( maskImgA, ( outputCols-1, rows/2), ( pal[0], pal[1], pal[2]) )
        outImgA = maskImgA.quantize( palette = im )
        outImgA.save( "%s_%d.png" %(transitionFilename, rep ) )



    # check if ceilng was set.
    if startCeilingRow >= 0 and endCeilingRow > 0:
      ceilFilename = ceilImgFilename if len(ceilImgFilename) > 0 else floorImgFilename
      with Image.open( ceilFilename ) as ceil:
        inputCeilingImg = ceil.convert('RGB')
        inputWidth, inputHeight = ceil.size
        inputCeilingCv = np.array(inputCeilingImg)
        # assume old pal
        # get source points
        srcTopLeft = ( 0, 0 )
        srcTopRight = ( inputWidth-1, 0 )
        srcBottomLeft = ( 0, inputHeight-1 ) 
        srcBottomRight = ( inputWidth-1, inputHeight-1 )
        srcPts = np.array( [ srcBottomLeft, srcBottomRight, srcTopRight, srcTopLeft] )    
        logging.info("srcPts:");
        logging.info(srcPts);
        topLeftStart = COLS / 2 - bottomTotalWidth / 2
        for rep in range( 0, farImageReps +1, 1 ):
          # detination points
          dstTopLeft = ( topLeftStart + rep * nearPolyWidth, startCeilingRow )
          dstTopRight = ( topLeftStart + rep * nearPolyWidth + nearPolyWidth,  startCeilingRow )
          dstBottomLeft = (  rep * farPolyWidth, endCeilingRow  ) 
          dstBottomRight = (  rep * farPolyWidth + farPolyWidth, endCeilingRow   )
          dstPts = np.array( [ dstBottomLeft, dstBottomRight, dstTopRight, dstTopLeft] )    
          logging.info("dstPts:");
          logging.info(dstPts);
          dstPoly = np.array( [ dstBottomLeft, dstBottomRight, dstTopRight, dstTopLeft], dtype=np.int32 )
          # Get Perspective Transform Algorithm
          srcPtsList = np.float32( srcPts.tolist() )
          dstPtsList = np.float32( dstPts.tolist() )
          xfrmMatrix = cv2.getPerspectiveTransform(srcPtsList, dstPtsList)
          
          # warp it
          image_size = (tmpCv.shape[1], tmpCv.shape[0])
          warpCv = cv2.warpPerspective(inputCeilingCv, xfrmMatrix, dsize=image_size)
          warpImg = Image.fromarray( warpCv )
          if args.output_warped_images:
            warpImg.save( "warped_ceil_%d.png" %(rep) )
            
          ## create a copy mask 
          maskCv = np.zeros_like(tmpCv)
          maskCv = cv2.fillPoly(maskCv, pts = [dstPoly], color = (255, 255, 255) )
          maskCv = maskCv.all(axis=2)
          # copy warped image
          tmpCv[maskCv, :] = warpCv[maskCv, :]

    
    # convert backto PIL 
    maskImg = Image.fromarray( tmpCv )
    ImageDraw.floodfill( maskImg, ( outputCols-1, rows/2), ( pal[0], pal[1], pal[2]) )
    ##maskImg.save( "mask.png")
    outImg = maskImg.quantize( palette = im )
    #outImg = Image.new('P', (outputCols, rows))
    #outImg.putpalette(pal)
    outImg.save( outputFilename )

def main(args, loglevel):
  logging.basicConfig(format="%(levelname)s: %(message)s", level=loglevel)
  startRow = args.start_row
  endRow = args.end_row
  if startRow == endRow: 
    print("Start and end row of floor must differ")
    return
  elif startRow > endRow:
    temp = endRow
    endRow = startRow
    startRow = temp

    
  startCeilingRow = args.start_ceiling_row
  endCeilingRow =  args.end_ceiling_row
  if startCeilingRow == endCeilingRow and startCeilingRow > 0: 
    print("Start and end row of ceiling must differ")
    return
  elif startCeilingRow > endCeilingRow:
    temp = endCeilingRow
    endCeilingRow = startCeilingRow
    startCeilingRow = temp

  farImageReps = args.far_image_reps
  nearImageReps = args.near_image_reps
  if farImageReps <= nearImageReps:
    print("Far image reps must be larger than near image reps")
    return


  imageFilename = 'image.png'
  imageFilenameB = ''
  if len(args.input_filename) > 0 :
    imageFilename =  args.input_filename[0]
    if len(args.input_filename) > 1 :
      imageFilenameB =  args.input_filename[1]


  imageCeilingFilename = ''
  imageCeilingFilenameB = ''
  if len(args.input_ceiling_filename) > 0 :
    imageCeilingFilename =  args.input_ceiling_filename[0]
    if len(args.input_ceiling_filename) > 1 :
      imageCeilingFilenameB =  args.input_ceiling_filename[1]

  outputFilename = 'bg.png'
  outputFilenameB = ''
  if len(args.output_filename) > 0 :
    outputFilename =  args.output_filename[0]
    if len(args.output_filename) > 1 :
      outputFilenameB =  args.output_filename[1]



  projectDir =  args.project_directory


  farPolyWidth = COLS / farImageReps
  nearPolyWidth = COLS / nearImageReps

  logging.info("Image reps far: %d", farImageReps)
  logging.info("Image reps near: %d", nearImageReps)
  # always put out image repetition 
  print("Scroll width far: %.3f" % farPolyWidth)
  print("Scroll width near: %.3f" % nearPolyWidth)
  print("Starting row floor: %d" % startRow)
  print("Ending row floor: %d" % endRow)
  scrollRows = endRow - startRow
  scrollRatio = nearPolyWidth / farPolyWidth 
  scrollRowStep = ( scrollRatio - 1.0 ) / scrollRows
  print("* Scroll increment floor: %.4f" % scrollRowStep)
  finalScrollStep =  (nearPolyWidth - farPolyWidth) / scrollRows
  print("* Final Scroll increment floor: %.4f" % finalScrollStep)


  if startCeilingRow > -1  and  endCeilingRow > startCeilingRow :
    logging.info("Starting row ceiling: %d", startCeilingRow)
    logging.info("Ending row ceiling: %d", endCeilingRow)
    scrollCeilingRows = endCeilingRow - startCeilingRow
    scrollCeilingRatio = nearPolyWidth / farPolyWidth 
    scrollCeilingRowStep = ( scrollCeilingRatio - 1.0 ) / scrollCeilingRows
    print("* Scroll increment ceiling: %.4f" % scrollCeilingRowStep)


  bottomTotalWidth = nearPolyWidth * farImageReps
  offset = farImageReps % 2 != nearImageReps % 2
  outputCols = int(COLS + nearPolyWidth)
  # must be multiple of 8 for tile breakdown
  if outputCols % 8 > 0:
    outputCols = (int( outputCols/8) +1) * 8

  print("Image size %d x %d" % ( outputCols,rows ) )

  createImages( imageFilename, imageCeilingFilename, rows, outputCols, bottomTotalWidth, farImageReps, farPolyWidth, nearPolyWidth, startRow, endRow, startCeilingRow, endCeilingRow, outputFilename, '' )

  if imageFilenameB:
    print("DO SECOND IMAGE")
    print(imageFilenameB)
    createImages( imageFilenameB, imageCeilingFilenameB, rows, outputCols, bottomTotalWidth, farImageReps, farPolyWidth, nearPolyWidth, startRow, endRow, startCeilingRow, endCeilingRow, outputFilenameB, outputFilename )


  if len(projectDir) > 0 :
    makeProjectFiles( projectDir, outputFilename, endRow, startRow, nearPolyWidth, farPolyWidth, endCeilingRow, startCeilingRow, outputCols )

# the program.
if __name__ == '__main__':
  parser = argparse.ArgumentParser( 
      description = "Create endless scrolling background for SGDK",
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
      default = 4,
      type=int,
      help = "How many times to repeat image at far side of the floor/ceiling",
      metavar = "ARG")

  parser.add_argument( "-n",
      "--near_image_reps",
      default = 2,
      type=int,
      help = "How many times to repeat image at near side of the floor/ceiling",
      metavar = "ARG")

  parser.add_argument( "-s",
      "--start_row",
      default = 180,
      type=int,
      help = "Which row starts the floor",
      metavar = "ARG")

  parser.add_argument( "-e",
      "--end_row",
      default = 223,
      type=int,
      help = "Which row ends the floor",
      metavar = "ARG")



  parser.add_argument( "-i",
      "--input_filename",
      action = 'append',
      default = [],
      help = "input image filename",
      metavar = "ARG")



  parser.add_argument( "-S",
      "--start_ceiling_row",
      default = -1,
      type=int,
      help = "Which row starts the ceiling",
      metavar = "ARG")

  parser.add_argument( "-E",
      "--end_ceiling_row",
      default = -1,
      type=int,
      help = "Which row ends the ceiling",
      metavar = "ARG")

  parser.add_argument( "-I",
      "--input_ceiling_filename",
      default = [],
      help = "input ceiling image filename",
      metavar = "ARG")

  parser.add_argument( "-o",
      "--output_filename",
      action = 'append',
      default = [],
      help = "Output filename",
      metavar = "ARG")

  parser.add_argument( "-p",
      "--project_directory",
      default = '',
      help = "Create project directory with resource files and simple SGDK code.",
      metavar = "ARG")

  parser.add_argument(
      "-w",
      "--output_warped_images",
      help="Output warped images used to create final image.",
      action="store_true")

  args = parser.parse_args()

  # Setup logging
  if args.verbose:
    loglevel = logging.INFO
  else:
    loglevel = logging.WARNING

  main(args, loglevel)
