# File intended only to be ran with GNU make. Use any other make program at your own risk!
OUTPUT := brandontools-gui
WXCPPFLAGS := $(shell wx-config --cppflags)
WXCPPLIBS := $(shell wx-config --libs all)
CPPFLAGS := $(MASTER_CPP_FLAGS) -Wall -I. -I/usr/include/ImageMagick $(WXCPPFLAGS)
LDFLAGS := $(MASTER_CPP_FLAGS) -Wall
LDLIBS := $(WXCPPLIBS) -lMagick++ -lMagickWand -lMagickCore -ldl
CC := g++
CXX := $(CC)
RM := rm
INSTALL := install
FILES := BrandonApp.o BrandonFrame.o BrandonFrameGUI.o BrandonToolsEditorGUI.o

all : $(OUTPUT)

$(OUTPUT) : $(FILES)
	$(CC) $(LDFLAGS) $^ -o $(OUTPUT) $(LDLIBS)

clean : 
	$(RM) -rf *.o $(OUTPUT)

install : $(OUTPUT)
	$(INSTALL) -m 0755 $(OUTPUT) $(PREFIX)/bin

uninstall :
	$(RM) -rf $(PREFIX)/bin/$(OUTPUTx
