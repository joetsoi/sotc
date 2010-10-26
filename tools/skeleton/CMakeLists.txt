file(GLOB source_files 
	*.h
	*.cpp
)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.41.0 COMPONENTS iostreams)
find_package(TinyXML REQUIRED)

if(Boost_FOUND)
	include_directories(
		${Boost_INCLUDE_DIRS}
		${OGRE_INCLUDE_DIRS}
		${TinyXML_INCLUDE_DIR}
		${CMAKE_HOME_DIRECTORY}/tools/xff/
	)
	add_executable(skeleton ${source_files})

	target_link_libraries(skeleton
		${OGRE_LIBRARIES}
		${Boost_LIBRARIES}
		${TinyXML_LIBRARY}
	)
endif()
