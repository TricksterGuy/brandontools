# File intended only to be ran with GNU make. Use any other make program at your own risk!
PREFIX := /usr/local
export PREFIX
MASTER_CPP_FLAGS := -O3 -s
export MASTER_CPP_FLAGS
INSTALL := install

all : brandontools-cli brandontools-gui

brandontools-cli :
	$(MAKE) -C cli all -f Makefile

brandontools-gui :
	$(MAKE) -C gui all -f Makefile

clean :
	$(MAKE) -C cli clean -f Makefile
	$(MAKE) -C gui clean -f Makefile

install :
	mkdir -p $(PREFIX)
	mkdir -p $(PREFIX)/bin
	$(MAKE) -C cli install -f Makefile
	$(MAKE) -C gui install -f Makefile

uninstall :
	$(MAKE) -C cli uninstall -f Makefile
	$(MAKE) -C gui uninstall -f Makefile

install-script :
	cp makeindexed.scm ~/.gimp-2.6/scripts
