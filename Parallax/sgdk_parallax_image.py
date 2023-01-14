#!/usr/bin/env python

import os,  argparse, logging
import numpy as np
import math
from PIL import Image, ImageDraw
import cv2
import shutil

def makeProjectFiles( destDir, imageFilename, endRow, startRow, bottomPolyWidth, topPolyWidth ):
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

  srcfile.write('static void scrollRight() {\n')
  srcfile.write('  ++scrollStep;\n')
  srcfile.write('  if (scrollStep < %d) {\n' % topPolyWidth )

  scrollRows = endRow - startRow
  scrollRatio = bottomPolyWidth / topPolyWidth 
  scrollRowStep = ( scrollRatio - 1.0 ) / scrollRows
  scrollIncrement = 1.0;
  for r in range( startRow, endRow + 1, 1):
    srcfile.write( "    fscroll[%d] = fix32Sub( fscroll[%d], FIX32(%.3f));\n" % ( r, r, scrollIncrement ) )
    scrollIncrement += scrollRowStep
  srcfile.write('  } else {\n')
  srcfile.write('    scrollStep = 0;\nmemset(fscroll, 0, sizeof(fscroll));\n')
  srcfile.write('  }\n}\n\n')


  srcfile.write('static void scrollLeft() {\n')
  srcfile.write('  --scrollStep;\n')
  srcfile.write('  if (scrollStep >= 0) {')
  scrollIncrement = 1.0;
  for r in range( startRow, endRow + 1, 1):
    srcfile.write( "    fscroll[%d] = fix32Add( fscroll[%d], FIX32(%.3f));\n" % ( r, r, scrollIncrement ) )
    scrollIncrement += scrollRowStep
  srcfile.write('  } else {\n')
  srcfile.write('    scrollStep = %d;'% int(topPolyWidth) )
  scroll = - int( topPolyWidth)
  scrollStep =  (bottomPolyWidth - topPolyWidth) / scrollRows
  for r in range( startRow, endRow + 1, 1):
    srcfile.write( "    fscroll[%d] = FIX32(%.3f);\n" % ( r, scroll ) )
    scroll -= scrollStep
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
    scrollRight();
    for (int i = 160; i < 224; i++)
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

  projectDir = ""
  if args.project_directory:
    projectDir =  args.project_directory

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
    for rep in range( 0, farImageReps +1, 1 ):
      # detination points
      dstTopLeft = ( rep * topPolyWidth, startRow )
      dstTopRight = ( rep * topPolyWidth + topPolyWidth, startRow )
      dstBottomLeft = ( bottomLeftStart + rep * bottomPolyWidth, endRow +1 ) 
      dstBottomRight = ( bottomLeftStart + rep * bottomPolyWidth + bottomPolyWidth, endRow +1  )
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

    # convert backto PIL 
    maskImg = Image.fromarray( tmpCv )
    ImageDraw.floodfill( maskImg, ( outputCols-1, rows/2), ( pal[0], pal[1], pal[2]) )
    #maskImg.save( "mask.png")
    outImg = maskImg.quantize( palette = im )
    #outImg = Image.new('P', (outputCols, rows))
    #outImg.putpalette(pal)
    outImg.save( outputFilename )

    if len(projectDir) > 0 :
      makeProjectFiles( projectDir, outputFilename, endRow, startRow, bottomPolyWidth, topPolyWidth )

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

  parser.add_argument( "-p",
      "--project_directory",
      help = "Create Project Directory",
      metavar = "ARG")

  args = parser.parse_args()

  # Setup logging
  if args.verbose:
    loglevel = logging.DEBUG
  else:
    loglevel = logging.INFO

  main(args, loglevel)
