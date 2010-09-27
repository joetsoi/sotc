cmake_minimum_required(VERSION 2.6)
project(sotc)

#if you don't want the full compiler output, remove the following line
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_MODULE_PATH /usr/local/lib/OGRE/cmake) 
#add definitions, compiler switches, etc.
add_definitions(-g -Wall -pedantic -Wno-long-long)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

find_package(OGRE REQUIRED)
#find_package(OIS REQUIRED)
#find_package(Boost COMPONENTS program_options REQUIRED)
#subdirs(src tools)
subdirs(tools)

