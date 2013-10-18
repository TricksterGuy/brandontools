# File intended only to be ran with GNU make. Use any other make program at your own risk!
OUTPUT := brandontools
WXCPPFLAGS := $(shell wx-config --cppflags)
WXCPPLIBS := $(shell wx-config --libs)
CPPFLAGS := $(MASTER_CPP_FLAGS) -Wall -I. -I/usr/include/ImageMagick $(WXCPPFLAGS)
LDFLAGS := $(MASTER_CPP_FLAGS) -Wall
LDLIBS := $(WXCPPLIBS) -lMagick++ -lMagickWand -lMagickCore
TEST := -lfreetype -lpng -lz -ltiff -lXt -lxml2 -lXext -lfontconfig -lgomp -ljpeg  -lstdc++
CC := g++
CXX := $(CC)
RM := rm
INSTALL := install
FILES := 

all : $(OUTPUT)

$(OUTPUT) : $(FILES)


clean : 


install : $(OUTPUT)


uninstall :

