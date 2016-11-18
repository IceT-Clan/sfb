cmake_minimum_required (VERSION 3.0)
project (sfb)

# Version number
set (SFB_VERSION_MAJOR 0)
set (SFB_VERSION_MINOR 1)
set (SFB_VERSION_PATCH 0)
set (SFB_VERSION "\"${SFB_VERSION_MAJOR}.${SFB_VERSION_MINOR}.${SFB_VERSION_PATCH}\"")
 
# Configure a header file to pass some of the CMake settings to the source code
configure_file (
	"${PROJECT_SOURCE_DIR}/include/sfb.h.in"
	"${PROJECT_SOURCE_DIR}/include/sfb.h"
)

include_directories (include)
add_subdirectory (serial)

if(WIN32)
	include_directories (dirent/include)
	add_subdirectory (dirent)
endif()

set (SOURCES src/sfb.cpp include/sfb.h src/command.cpp include/command.h src/network.cpp
	include/network.h)
  
# Add the executable
add_executable (sfb ${SOURCES})

# Link libraries
if(WIN32)
	target_link_libraries(sfb serial dirent)
else()
	target_link_libraries(sfb serial)
endif()
