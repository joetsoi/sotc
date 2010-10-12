file(GLOB source_files 
	*.h
	*.cpp
)

#set(SOURCE_FILES
#	src/test.cpp
#)
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} " ${CMAKE_CURRENT_SOURCE_DIR}/cmake/")
message(${CMAKE_MODULE_PATH})

find_package(TinyXML REQUIRED)
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${OGRE_INCLUDE_DIRS}
	${TinyXML_INCLUDE_DIR}
)
add_executable(xff2ogrexml ${source_files})

target_link_libraries(xff2ogrexml
	${OGRE_LIBRARIES}
	${TinyXML_LIBRARY}
)
