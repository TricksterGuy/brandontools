#!/bin/bash

# Ensure program is built
make
echo -e "Now running a couple of sample commands if you see any assertion failed errors please let me know immediately.\n Note some of these may spit out warnings I do not care about the warnings, just if the program crashes or something.\n"

# Simple export
echo -e "Simple export\n"
cli/brandontools -mode3 simple test_images/simple.png

# Simple export from them internets
echo -e "\n\nSimple export from internet\n"
cli/brandontools -mode3 tehinternets http://i1.kym-cdn.com/photos/images/original/000/581/567/bab.jpg

# Animated things are nice too
echo -e "\n\nSimple export from internet with animated gif\n"
cli/brandontools -mode3 animated1 http://media.idownloadblog.com/wp-content/uploads/2012/05/Sonic-Animated.gif

# Lets use the -animated flag
echo -e "\n\nSimple export from internet with animated gif and export with animation frames array\n"
cli/brandontools -mode3 -animated animated2 http://media.idownloadblog.com/wp-content/uploads/2012/05/Sonic-Animated.gif

# Lets try resizing something
echo -e "\n\nResizing\n"
cli/brandontools -mode3 -resize=240,160 resized test_images/simple.png

# Setting a color key
echo -e "\n\nColor key\n"
cli/brandontools -mode3 -transparent=255,255,255 colorkeyd test_images/simple.png

# An odd thing to do here, but yes it works with pdfs (because of imagemagick)
echo -e "\n\nExporting a pdf file\n"
cli/brandontools -mode3 lolpdfs test_images/lol.pdf

# Multiple images
echo -e "\n\nBatch processing\n"
cli/brandontools -mode3 threeimages test_images/simple.png test_images/simple2.png test_images/simple3.png

# Resizing Multiple images
echo -e "\n\nResizing multiple images\n"
cli/brandontools -mode3 -resize=240,160 threeimages_resized test_images/simple.png test_images/simple2.png test_images/simple3.png

# Multiple images + animated
echo -e "\n\nBatch processing but give me the animation frames array too.\n"
cli/brandontools -mode3 -animated threeimages_animated test_images/simple.png test_images/simple2.png test_images/simple3.png

# Multiple images + animated + animated gif
echo -e "\n\nMultiple images with an animated gif and ask for the animation frames array\n"
cli/brandontools -mode3 -animated multianimated test_images/simple.png test_images/simple2.png test_images/simple3.png http://media.idownloadblog.com/wp-content/uploads/2012/05/Sonic-Animated.gif

# Double export 1
echo -e "\n\nDouble image export\n"
cli/brandontools -mode3 double1 test_images/simple.png test_images/simple.png

# Double export 2
echo -e "\n\nDouble image export 2\n"
cli/brandontools -mode3 double2 test_images/simple.png test_images/simple.png test_images/simple.png test_images/simple2.png test_images/simple2.png

# Double export 3
echo -e "\n\nDouble image export 3\n"
cli/brandontools -mode3 double3 http://media.idownloadblog.com/wp-content/uploads/2012/05/Sonic-Animated.gif http://media.idownloadblog.com/wp-content/uploads/2012/05/Sonic-Animated.gif

# Double export 4
echo -e "\n\nDouble image export 4\n"
cli/brandontools -mode4 double4 test_images/simple.png  test_images/simple3.png test_images/simple.png test_images/simple.png test_images/simple2.png test_images/simple2.png test_images/simple3.png test_images/simple.png 
