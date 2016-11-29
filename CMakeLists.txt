cmake_minimum_required (VERSION 3.0)
project (sfb)
 
include_directories (include)
include_directories (serial/include)
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
target_link_libraries(sfb serial)
