# File intended only to be ran with GNU make. Use any other make program at your own risk!
OUTPUT := brandontools
WXCPPFLAGS := $(shell wx-config --cppflags)
WXCPPLIBS := $(shell wx-config --libs)
CPPFLAGS := $(MASTER_CPP_FLAGS) -Wall -I. -I/usr/include/ImageMagick $(WXCPPFLAGS)
LDFLAGS := $(MASTER_CPP_FLAGS) -Wall
LDLIBS := $(WXCPPLIBS) -lMagick++ -lMagickWand -lMagickCore
CC := g++ -g -std=c++0x -Wall
CXX := $(CC)
RM := rm
INSTALL := install
FILES := color.o \
	cpercep.o \
	dither.o \
	exportfile.o \
	fileutils.o \
	fortunegen.o \
	fortunes.o \
	headerfile.o \
	histogram.o \
	implementationfile.o \
	main.o \
	mapexporter.o \
	mediancut.o \
	mode0exporter.o \
	mode3exporter.o \
	mode4exporter.o \
	reductionhelper.o \
	shared.o \
	spritesexporter.o \
	tilesexporter.o

all : $(OUTPUT)

$(OUTPUT) : $(FILES)
	$(CC) $(LDFLAGS) $^ -o $(OUTPUT) $(LDLIBS)

clean :
	$(RM) -rf *.o $(OUTPUT)

install : $(OUTPUT)
	$(INSTALL) -m 0755 $(OUTPUT) $(PREFIX)/bin

uninstall :
	$(RM) -rf $(PREFIX)/bin/$(OUTPUT)

color.o : color.cpp color.hpp cpercep.hpp
cpercep.o : cpercep.cpp cpercep.hpp
dither.o : dither.cpp dither.hpp color.hpp reductionhelper.hpp
exportfile.o : exportfile.cpp exportfile.hpp reductionhelper.hpp fortunegen.hpp color.hpp shared.hpp
fileutils.o : fileutils.cpp fileutils.hpp color.hpp
fortunegen.o : fortunegen.cpp fortunegen.hpp shared.hpp fortunes.hpp
fortunes.o : fortunes.cpp fortunes.hpp
headerfile.o : headerfile.cpp headerfile.hpp exportfile.hpp fileutils.hpp shared.hpp
histogram.o : histogram.cpp histogram.hpp color.hpp fileutils.hpp
implementationfile.o : implementationfile.cpp implementationfile.hpp exportfile.hpp fileutils.hpp shared.hpp
main.o : cpercep.hpp reductionhelper.hpp color.hpp headerfile.hpp implementationfile.hpp shared.hpp version.h
mapexporter.o : mapexporter.cpp reductionhelper.hpp color.hpp shared.hpp
mediancut.o : mediancut.cpp mediancut.hpp color.hpp histogram.hpp
mode0exporter.o : mode0exporter.cpp reductionhelper.hpp shared.hpp color.hpp
mode3exporter.o : mode3exporter.cpp reductionhelper.hpp shared.hpp color.hpp
mode4exporter.o : mode4exporter.cpp reductionhelper.hpp shared.hpp color.hpp
reductionhelper.o : reductionhelper.cpp reductionhelper.hpp fileutils.hpp color.hpp cpercep.hpp mediancut.hpp shared.hpp
shared.o : shared.cpp shared.hpp reductionhelper.hpp
spritesexporter.o : spritesexporter.cpp reductionhelper.hpp color.hpp shared.hpp
tilesexporter.o : tilesexporter.cpp reductionhelper.hpp color.hpp shared.hpp
