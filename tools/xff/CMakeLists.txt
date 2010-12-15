include_directories(${Boost_INCLUDE_DIRS})
add_library(Xff Xff.hpp)
set_target_properties(Xff PROPERTIES LINKER_LANGUAGE CXX)

