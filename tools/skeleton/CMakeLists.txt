file(GLOB source_files 
	*.h
	*.cpp
)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.41.0 COMPONENTS iostreams)

if(Boost_FOUND)
	include_directories(
		${Boost_INCLUDE_DIRS}
		${OGRE_INCLUDE_DIRS}
		/home/foobat/projects/sotc/tools/xff/
	)
	add_executable(skeleton ${source_files})

	target_link_libraries(skeleton
		${OGRE_LIBRARIES}
		${Boost_LIBRARIES}
	)
endif()
