#!/bin/bash

# Ensure program is built
make
echo "Now running a couple of sample commands if you see any assertion failed errors please let me know immediately"

# Simple export
cli/brandontools -mode3 simple test_images/simple.png

# Simple export from them internets
cli/brandontools -mode3 tehinternets http://i1.kym-cdn.com/photos/images/original/000/581/567/bab.jpg

# Animated things are nice too
cli/brandontools -mode3 animated1 http://media.idownloadblog.com/wp-content/uploads/2012/05/Sonic-Animated.gif

# Lets use the -animated flag
cli/brandontools -mode3 -animated animated2 http://media.idownloadblog.com/wp-content/uploads/2012/05/Sonic-Animated.gif

# Lets try resizing something
cli/brandontools -mode3 -resize=240,160 resized test_images/simple.png

# Setting a color key
cli/brandontools -mode3 -transparent=255,255,255 colorkeyd test_images/simple.png

# An odd thing to do here, but yes it works with pdfs (because of imagemagick)
cli/brandontools -mode3 lolpdfs test_images/lol.pdf

# Multiple images
cli/brandontools -mode3 threeimages test_images/simple.png test_images/simple2.png test_images/simple3.png

# Resizing Multiple images
cli/brandontools -mode3 -resize=240,160 threeimages test_images/simple.png test_images/simple2.png test_images/simple3.png

# Multiple images + animated
cli/brandontools -mode3 -animated threeimages test_images/simple.png test_images/simple2.png test_images/simple3.png

# Multiple images + animated + animated gif
cli/brandontools -mode3 -animated threeimages_animated test_images/simple.png test_images/simple2.png test_images/simple3.png http://media.idownloadblog.com/wp-content/uploads/2012/05/Sonic-Animated.gif
