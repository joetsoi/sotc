# defines
# TinyXML_FOUND
# TinyXML_INCLUDE_DIR
# TinyXML_LIBRARY

find_path(TinyXML_INCLUDE_DIR 
	NAMES "tinyxml.h"
	PATH_SUFFIXES include
	PATHS 
	/usr/include
	/usr/local/include
)

find_library(TinyXML_LIBRARY
	NAMES "tinyxml"
	PATH_SUFFIXES lib lib64
	PATHS
	/usr
	/usr/local
)
set(TinyXML_FOUND "NO")
if(TinyXML_LIBRARY)
	set(TinyXML_FOUND "YES")
	message("Found TinyXML : ${TinyXML_LIBRARY}")
endif(TinyXML_LIBRARY)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(TinyXML TinyXML_INCLUDE_DIR TinyXML_LIBRARY)
mark_as_advanced(TinyXML_INCLUDE_DIR TinyXML_LIBRARY)
