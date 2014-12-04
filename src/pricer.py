#!/usr/bin/python

import argparse, sys, os, subprocess

BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE = range(8)

#following from Python cookbook, #475186
def has_colours(stream):
    if not hasattr(stream, "isatty"):
        return False
    if not stream.isatty():
        return False # auto color only on TTYs
    try:
        import curses
        curses.setupterm()
        return curses.tigetnum("colors") > 2
    except:
        # guess false in case of error
        return False
has_colours = has_colours(sys.stdout)


def printout(text, colour=WHITE):
    if has_colours:
        seq = "\x1b[1;%dm" % (30+colour) + text + "\x1b[0m"
        sys.stdout.write(seq)
    else:
        sys.stdout.write(text)

if __name__ == '__main__':

    parser = argparse.ArgumentParser(description='Pricer 2014')

    # Add options for general parser
    parser.add_argument('-c', '-C', action='store_true', help = "Compute P&L");
    parser.add_argument('file', type=argparse.FileType('r'), help = "Input file containing option data");

    args = parser.parse_args()

    if (not (os.path.exists("price-option") and os.path.exists("profit-and-loss")) ):
        printout('Could not find binaries, build the project before using this script\n', RED);
    else:
      if (args.c):
          subprocess.call(['./profit-and-loss',args.file.name])
      else:
          subprocess.call(['./price-option',args.file.name, "-1"]);