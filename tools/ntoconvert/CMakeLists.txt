file(GLOB source_files 
	*.h
	*.cpp
)

#set(SOURCE_FILES
#	src/test.cpp
#)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

find_package(FreeImage REQUIRED)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${OGRE_INCLUDE_DIRS}
	${FreeImage_INCLUDE_DIRS}
)
add_executable(ntoconvert ${source_files})

target_link_libraries(ntoconvert
	${OGRE_LIBRARIES}
	${FreeImage_LIBRARIES}
)
