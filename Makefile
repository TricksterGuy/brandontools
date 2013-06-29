# -*- mode: Makefile; -*-
# -----------------------------------------
# project brandontools-Linux

# MAKE_DEP = -MMD -MT $@ -MF $(@:.o=.d)

CFLAGS = -Wall 
INCLUDES = -I/usr/local/include/ImageMagick -I/usr/include/ImageMagick
LDFLAGS = -lMagick++ -lMagickWand -lMagickCore  -s
RCFLAGS = 
LDLIBS = $(T_LDLIBS) -lfreetype -lpng -lz -ltiff -lXt -lxml2 -lXext -lfontconfig -lgomp -ljpeg  -lstdc++

LINK_exe = gcc -o $@ $^ $(LDFLAGS) $(LDLIBS)
COMPILE_cpp = g++ $(CFLAGS) -o $@ -c $< $(MAKEDEP) $(INCLUDES)

%.o : %.cpp ; $(COMPILE_cpp)
.SUFFIXES: .o .d .c .cpp .cxx .rc

all: all.before all.targets all.after

all.before :
	-
all.after : $(FIRST_TARGET)
	
all.targets : Release_target

clean :
	rm -fv $(clean.OBJ)
	rm -fv $(DEP_FILES)

.PHONY: all clean distclean

# -----------------------------------------
# Release_target

Release_target.BIN = brandontools
Release_target.OBJ = main.o cpercep.o mediancut.o mode3batchexporter.o mode3exporter.o mode4batchexporter.o mode4exporter.o shared.o fortunes.o fortunegen.o
DEP_FILES += main.d mediancut.d mode3batchexporter.d mode3exporter.d mode4batchexporter.d mode4exporter.d shared.d fortunes.d fortunegen.d
clean.OBJ += $(Release_target.BIN) $(Release_target.OBJ)

Release_target : Release_target.before $(Release_target.BIN) Release_target.after_always
Release_target : CFLAGS += -O3  -Os -pipe
Release_target : INCLUDES += 
Release_target : RCFLAGS += 
Release_target : LDFLAGS += -s   
Release_target : T_LDLIBS = 

Release_target.before :
	
	
Release_target.after_always : $(Release_target.BIN)
	
$(Release_target.BIN) : $(Release_target.OBJ)
	$(LINK_exe)
	

install :
	cp brandontools /usr/bin/brandontools
	

uninstall :
	rm /usr/bin/brandontools
	

install-script :
	cp makeindexed.scm ~/.gimp-2.6/scripts
	
	
ifdef MAKE_DEP
-include $(DEP_FILES)
endif
