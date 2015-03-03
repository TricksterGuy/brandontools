brandontools
============

# **If you are taking 2110 currently (Spring 2015) You are in the wrong place go here instead This repo is no longer being worked on! - https://github.com/TricksterGuy/nin10kit**

This is a little nifty program I wrote back in Spring 2010, because I was unhappy with grit, jgrit, gfx2gba, etc.  My aim for this program is a simple command line interface that converts images, videos, animated images into a format suitable for the gba.

* Supports all GBA Modes (except for affine backgrounds)
* Can export animated images and videos (with an array of pointers to each image)
* Can load images from a URL
* Supports batch mode operations (Generating a global palette for Mode 4, a global palette+tileset for Mode 0)
* Supports Sprites in bitmap modes and tiled modes and 1D or 2D export (with 2d mode building the spritesheet for you).
* Includes a GUI version which allows you to see images before they are exported (for modes 3 and 4 only).
