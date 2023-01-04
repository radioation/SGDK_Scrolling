#!/usr/bin/env python

import os,  argparse, logging
import math
from PIL import Image

# Gather our code in a main() function
def main(args, loglevel):
  logging.basicConfig(format="%(levelname)s: %(message)s", level=loglevel)
  topCount = 8
  bottomCount = 4
  topRow = 160
  bottomRow = 223
  image1 = "" 
  image2 = "" 
  image3 = "" 
  image4 = "" 
  outputFilename = "bg.png"

  # start row


# Standard boilerplate to call the main() function to begin
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

  parser.add_argument( "-t",
      "--top_count",
      type=int,
      help = "Number of image units at top",
      metavar = "ARG")

  parser.add_argument( "-b",
      "--bottom_count",
      type=int,
      help = "number of image units at bottom",
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

  parser.add_argument( "-1",
      "--image_1",
      help = "input image",
      metavar = "ARG")

  parser.add_argument( "-2",
      "--image_2",
      help = "input image",
      metavar = "ARG")

  parser.add_argument( "-3",
      "--image_3",
      help = "input image",
      metavar = "ARG")

  parser.add_argument( "-4",
      "--image_4",
      help = "input image",
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
