file(GLOB source_files 
	*.h
	*.cpp
)

#set(SOURCE_FILES
#	src/test.cpp
#)
message(${source_files})

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${OGRE_INCLUDE_DIRS}
)
add_executable(xff2ogrexml ${source_files})

target_link_libraries(xff2ogrexml
	${OGRE_LIBRARIES}
)
