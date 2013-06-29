cmake_minimum_required (VERSION 2.6)
project (brandontools)

SET(CPACK_BINARY_DEB "ON")
SET(CPACK_BINARY_RPM "OFF")
SET(CPACK_BINARY_STGZ "OFF")
SET(CPACK_BINARY_TBZ2 "OFF")
SET(CPACK_BINARY_TGZ "OFF")
SET(CPACK_BINARY_TZ "OFF")
SET(CPACK_BINARY_ZIP "OFF")
SET(CPACK_GENERATOR "DEB")
SET(CPACK_NSIS_DISPLAY_NAME "brandontools 0.9.3")
SET(CPACK_NSIS_PACKAGE_NAME "brandontools 0.9.3")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Image to GBA Exporter")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "brandontools 0.9.3")
SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "brandontools 0.9.3")
SET(CPACK_PACKAGE_VERSION "0.9.3")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "9")
SET(CPACK_PACKAGE_VERSION_PATCH "3")
SET(CPACK_SOURCE_GENERATOR "")
SET(CPACK_SOURCE_TBZ2 "OFF")
SET(CPACK_SOURCE_TGZ "OFF")
SET(CPACK_SOURCE_TZ "OFF")

set( CPACK_DEBIAN_PACKAGE_NAME "brandontools" )
set( CPACK_DEBIAN_PACKAGE_VERSION "0.9" )
set( CPACK_DEBIAN_PACKAGE_DEPENDS "")
set( CPACK_DEBIAN_PACKAGE_MAINTAINER "Brandon Whitehead <brandon.whitehead@gatech.edu>")
SET( CPACK_DEBIAN_PACKAGE_DESCRIPTION "This package contains brandontools a Image to GBA C file exporter")

SET(CPACK_PACKAGE_FILE_NAME "brandontools-0.9.3-i386")
set( CPACK_DEBIAN_PACKAGE_ARCHITECTURE "i386" )

INCLUDE(CPack)

ADD_DEFINITIONS (-DLINUX)

# Source files definition
SET(SRC_BRANDONTOOLS
	cpercep.cpp
	fortunegen.cpp
	fortunes.cpp
	main.cpp
	mediancut.cpp
	mode3batchexporter.cpp
	mode3exporter.cpp
	mode4batchexporter.cpp
	mode4exporter.cpp
	palettegen.cpp
	shared.cpp
	Tile.cpp
)

ADD_EXECUTABLE(
	brandontools
        ${SRC_BRANDONTOOLS}
)

FIND_PACKAGE(ImageMagick COMPONENTS Magick++ MagickWand MagickCore)

SET( CMAKE_CXX_FLAGS "-O3 -s -Wall")
SET( CMAKE_SKIP_RPATH TRUE )

TARGET_LINK_LIBRARIES(
	brandontools
	Magick++
	MagickWand
	MagickCore
)

include_directories(${ImageMagick_INCLUDE_DIRS})

INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/brandontools DESTINATION bin)
#INSTALL(PROGRAMS ${brandontools_SOURCE_DIR}/makeindexed.scm DESTINATION ~/.gimp-2.6/scripts)
