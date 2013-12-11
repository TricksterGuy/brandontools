# File intended only to be ran with GNU make. Use any other make program at your own risk!
OUTPUT := brandontools
WXCPPFLAGS := $(shell wx-config --cppflags)
WXCPPLIBS := $(shell wx-config --libs)
CPPFLAGS := $(MASTER_CPP_FLAGS) -Wall -I. -I/usr/include/ImageMagick $(WXCPPFLAGS)
LDFLAGS := $(MASTER_CPP_FLAGS) -Wall
LDLIBS := $(WXCPPLIBS) -lMagick++ -lMagickWand -lMagickCore
CC := g++
CXX := $(CC)
RM := rm
INSTALL := install
FILES := main.o cpercep.o mediancut.o mode3batchexporter.o mode3exporter.o mode4batchexporter.o mode4exporter.o mode0exporter16.o mode0exporter256.o shared.o fileutils.o fortunes.o fortunegen.o tile.o tilesexporter.o reductionhelper.o

all : $(OUTPUT)

$(OUTPUT) : $(FILES)
	$(CC) $(LDFLAGS) $^ -o $(OUTPUT) $(LDLIBS)

clean :
	$(RM) -rf *.o $(OUTPUT)

install : $(OUTPUT)
	$(INSTALL) -m 0755 $(OUTPUT) $(PREFIX)/bin

uninstall :
	$(RM) -rf $(PREFIX)/bin/$(OUTPUT)
