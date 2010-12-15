file(GLOB source_files 
	*.h
	*.cpp
)

find_package(TinyXML REQUIRED)

if(Boost_FOUND)
	include_directories(
		${OGRE_INCLUDE_DIRS}
		${TinyXML_INCLUDE_DIR}
		${CMAKE_HOME_DIRECTORY}/tools/xff/
	)
	add_executable(animation ${source_files})

	target_link_libraries(animation
		${OGRE_LIBRARIES}
		${TinyXML_LIBRARY}
	)
endif()
