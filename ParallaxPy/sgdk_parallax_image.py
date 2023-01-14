#!/usr/bin/env python

import os,  argparse, logging
import numpy as np
import math
from PIL import Image, ImageDraw
import cv2
import shutil

def makeProjectFiles( destDir, imageFilename, endRow, startRow, nearPolyWidth, farPolyWidth, endCeilingRow, startCeilingRow ):
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
  resfile = open( resFolder + "/resources.res", 'w')
  fname = os.path.basename( imageFilename )
  resfile.write('image plane_b "bg/%s" 0\n'% fname )
  resfile.write('PALETTE plane_b_pal "bg/%s"\n' % fname )
  resfile.close()

  # make bare source file with some scrolling
  srcfile = open( srcFolder + "/main.c", 'w')
  srcfile.write('#include <genesis.h>\n')
  srcfile.write('#include "resources.h"\n\n')
  srcfile.write('s16 hScrollB[224];\n')
  srcfile.write('fix32 fscroll[224];\n')
  srcfile.write('s16 scrollStep = 0;\n\n')

  srcfile.write('static void scrollLeft() {\n')
  srcfile.write('  ++scrollStep;\n')
  srcfile.write('  if (scrollStep < %d) {\n' % farPolyWidth )

  scrollCeilingRows = endCeilingRow - startCeilingRow
  scrollCeilingRatio = nearPolyWidth / farPolyWidth 
  scrollCeilingRowStep = 0
  scrollCeilingIncrement = 1.0
  if startCeilingRow > -1  and  endCeilingRow > startCeilingRow :
    scrollCeilingRowStep = ( scrollCeilingRatio - 1.0 ) / scrollCeilingRows
    for r in range( endCeilingRow, startCeilingRow - 1, -1):
      srcfile.write( "    fscroll[%d] = fix32Sub( fscroll[%d], FIX32(%.3f));\n" % ( r, r, scrollCeilingIncrement ) )
      scrollCeilingIncrement += scrollCeilingRowStep


  scrollRows = endRow - startRow
  scrollRatio = nearPolyWidth / farPolyWidth 
  scrollRowStep = ( scrollRatio - 1.0 ) / scrollRows
  scrollIncrement = 1.0;
  for r in range( startRow, endRow + 1, 1):
    srcfile.write( "    fscroll[%d] = fix32Sub( fscroll[%d], FIX32(%.3f));\n" % ( r, r, scrollIncrement ) )
    scrollIncrement += scrollRowStep
  srcfile.write('  } else {\n')
  srcfile.write('    scrollStep = 0;\n    memset(fscroll, 0, sizeof(fscroll));\n')
  srcfile.write('  }\n}\n\n')


  srcfile.write('static void scrollRight() {\n')
  srcfile.write('  --scrollStep;\n')
  srcfile.write('  if (scrollStep >= 0) {\n')

  if startCeilingRow > -1  and  endCeilingRow > startCeilingRow :
    scrollCeilingIncrement = 1.0
    scrollCeilingRowStep = ( scrollCeilingRatio - 1.0 ) / scrollCeilingRows
    for r in range( endCeilingRow, startCeilingRow - 1, -1):
      srcfile.write( "    fscroll[%d] = fix32Add( fscroll[%d], FIX32(%.3f));\n" % ( r, r, scrollCeilingIncrement ) )
      scrollCeilingIncrement += scrollCeilingRowStep


  scrollIncrement = 1.0;
  for r in range( startRow, endRow + 1, 1):
    srcfile.write( "    fscroll[%d] = fix32Add( fscroll[%d], FIX32(%.3f));\n" % ( r, r, scrollIncrement ) )
    scrollIncrement += scrollRowStep
  srcfile.write('  } else {\n')
  srcfile.write('    scrollStep = %d;\n'% int(farPolyWidth) )

  if startCeilingRow > -1  and  endCeilingRow > startCeilingRow :
    scroll = - int( farPolyWidth)
    scrollStep =  (nearPolyWidth - farPolyWidth) / scrollCeilingRows
    for r in range( endCeilingRow, startCeilingRow , -1):
      srcfile.write( "    fscroll[%d] = FIX32(%.3f);\n" % ( r, scroll ) )
      scroll -= scrollStep

  scroll = - int( farPolyWidth)
  finalScrollStep =  (nearPolyWidth - farPolyWidth) / scrollRows
  for r in range( startRow, endRow + 1, 1):
    srcfile.write( "    fscroll[%d] = FIX32(%.3f);\n" % ( r, scroll ) )
    scroll -= finalScrollStep
  srcfile.write('  }\n}\n\n')

  srcfile.write("""int main(bool hard)
{
  memset(hScrollB, 0, sizeof(hScrollB));
  memset(fscroll, 0, sizeof(fscroll));
  VDP_setScreenWidth320();

  PAL_setPalette(PAL0, plane_b_pal.data, CPU);
  PAL_setColor(0, 0x0000);
  // set scrolling modes to support fake rotation
  VDP_setScrollingMode(HSCROLL_LINE, VSCROLL_PLANE);

  // get initial tile position in VRAM
  int ind = TILE_USER_INDEX;
  int indexB = ind;
  VDP_loadTileSet(plane_b.tileset, indexB, CPU);
  VDP_drawImageEx(BG_B, &plane_b, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, indexB), 0, 0, FALSE, TRUE);

  while (TRUE)
  {
    scrollLeft();
    for (int i = 0; i < 224; i++) // Not very efficient.  
    {
      hScrollB[i] = fix32ToInt(fscroll[i]);
    }
    VDP_setHorizontalScrollLine(BG_B, 0, hScrollB, 224, DMA);
    SYS_doVBlankProcess();
  }
}""")
  srcfile.close()

def main(args, loglevel):
  logging.basicConfig(format="%(levelname)s: %(message)s", level=loglevel)
  startRow = 180
  if args.start_row:
    startRow =  args.start_row
  endRow = 223
  if args.end_row:
    endRow =  args.end_row

  startCeilingRow = -1 
  if args.start_ceiling_row:
    startCeilingRow = args.start_ceiling_row
  endCeilingRow =-1 
  if args.end_ceiling_row:
    endCeilingRow =  args.end_ceiling_row

  farImageReps = 4
  if args.far_image_reps:
    farImageReps =  args.far_image_reps
  nearImageReps = 2
  if args.near_image_reps:
    nearImageReps =  args.near_image_reps

  imageFilename = "image.png" 
  if args.input_filename:
    imageFilename =  args.input_filename

  imageCeilingFilename = ""
  if args.input_ceiling_filename:
    imageCeilingFilename =  args.input_ceiling_filename

  outputFilename = "bg.png"
  if args.output_filename:
    outputFilename =  args.output_filename

  projectDir = ""
  if args.project_directory:
    projectDir =  args.project_directory

  # 320 x 224  
  COLS = 320  
  rows = 224

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
    print("Scroll increment ceiling: %.4f" % scrollCeilingRowStep)


  bottomTotalWidth = nearPolyWidth * farImageReps
  offset = farImageReps % 2 != nearImageReps % 2
  outputCols = int(COLS + nearPolyWidth)

  print("Image size %d x %d" % ( outputCols,rows ) )

  with Image.open( imageFilename ) as im:
    inputImg = im.convert('RGB')
    inputWidth, inputHeight = im.size
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
    for rep in range( 0, farImageReps +1, 1 ):
      # detination points
      dstTopLeft = ( rep * farPolyWidth, startRow )
      dstTopRight = ( rep * farPolyWidth + farPolyWidth +1, startRow )
      dstBottomLeft = ( bottomLeftStart + rep * nearPolyWidth, endRow +1 ) 
      dstBottomRight = ( bottomLeftStart + rep * nearPolyWidth + nearPolyWidth +1, endRow +1  )
      dstPts = np.array( [ dstBottomLeft, dstBottomRight, dstTopRight, dstTopLeft] )    
      dstPoly = np.array( [ dstBottomLeft, dstBottomRight, dstTopRight, dstTopLeft], dtype=np.int32 )

      # Get Perspective Transform Algorithm
      srcPtsList = np.float32( srcPts.tolist() )
      dstPtsList = np.float32( dstPts.tolist() )
      xfrmMatrix = cv2.getPerspectiveTransform(srcPtsList, dstPtsList)

      # warp it
      image_size = (tmpCv.shape[1], tmpCv.shape[0])
      warpCv = cv2.warpPerspective(inputCv, xfrmMatrix, dsize=image_size)
      warpImg = Image.fromarray( warpCv )
      #warpImg.save( "warp_%d.png" %(rep) )

      ## create a copy mask 
      maskCv = np.zeros_like(tmpCv)
      maskCv = cv2.fillPoly(maskCv, pts = [dstPoly], color = (255, 255, 255) )
      maskCv = maskCv.all(axis=2)
      # copy warped image
      tmpCv[maskCv, :] = warpCv[maskCv, :]
   
    # check if ceilng was set.
    if startCeilingRow > 0 and endCeilingRow > 0:
      ceilFilename = imageCeilingFilename if len(imageCeilingFilename) > 0 else imageFilename
      with Image.open( ceilFilename ) as ceil:
        inputCeilingImg = ceil.convert('RGB')
        inputWidth, inputHeight = ceil.size
        inputCeilingCv = np.array(inputCeilingImg)
        # assume old pal
        # get source points
        srcTopLeft = ( 0, 0 )
        srcTopRight = ( inputWidth, 0 )
        srcBottomLeft = ( 0, inputHeight ) 
        srcBottomRight = ( inputWidth, inputHeight )
        srcPts = np.array( [ srcBottomLeft, srcBottomRight, srcTopRight, srcTopLeft] )    
        topLeftStart = COLS / 2 - bottomTotalWidth / 2
        for rep in range( 0, farImageReps +1, 1 ):
          # detination points
          dstTopLeft = ( topLeftStart + rep * nearPolyWidth, startCeilingRow )
          dstTopRight = ( topLeftStart + rep * nearPolyWidth + nearPolyWidth + 1, startCeilingRow )
          dstBottomLeft = (  rep * farPolyWidth, endCeilingRow +1 ) 
          dstBottomRight = (  rep * farPolyWidth + farPolyWidth + 1, endCeilingRow +1  )
          dstPts = np.array( [ dstBottomLeft, dstBottomRight, dstTopRight, dstTopLeft] )    
          dstPoly = np.array( [ dstBottomLeft, dstBottomRight, dstTopRight, dstTopLeft], dtype=np.int32 )
          # Get Perspective Transform Algorithm
          srcPtsList = np.float32( srcPts.tolist() )
          dstPtsList = np.float32( dstPts.tolist() )
          xfrmMatrix = cv2.getPerspectiveTransform(srcPtsList, dstPtsList)
          
          # warp it
          image_size = (tmpCv.shape[1], tmpCv.shape[0])
          warpCv = cv2.warpPerspective(inputCeilingCv, xfrmMatrix, dsize=image_size)
          warpImg = Image.fromarray( warpCv )
          #warpImg.save( "warp_ceil_%d.png" %(rep) )
          
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

    if len(projectDir) > 0 :
      makeProjectFiles( projectDir, outputFilename, endRow, startRow, nearPolyWidth, farPolyWidth, endCeilingRow, startCeilingRow )

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
      help = "How many times to repeat image at far side of floor",
      metavar = "ARG")

  parser.add_argument( "-n",
      "--near_image_reps",
      type=int,
      help = "How many times to repeat image at near side of floor",
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

  parser.add_argument( "-S",
      "--start_ceiling_row",
      type=int,
      help = "Which row starts the ceiling",
      metavar = "ARG")

  parser.add_argument( "-E",
      "--end_ceiling_row",
      type=int,
      help = "Which row ends the ceiling",
      metavar = "ARG")

  parser.add_argument( "-I",
      "--input_ceiling_filename",
      help = "input ceiling image filename",
      metavar = "ARG")

  parser.add_argument( "-o",
      "--output_filename",
      help = "Output filename",
      metavar = "ARG")

  parser.add_argument( "-p",
      "--project_directory",
      help = "Create project directory with resource files and simple SGDK code.",
      metavar = "ARG")

  args = parser.parse_args()

  # Setup logging
  if args.verbose:
    loglevel = logging.INFO
  else:
    loglevel = logging.WARNING

  main(args, loglevel)
