include(FindPkgConfig)

find_path(log4cxx_INCLUDE_DIR
	NAMES log4cxx/log4cxx.h
	DOC "The log4cxx include directories."
)

find_library(
	log4cxx_LIBRARY
	NAMES log4cxx
	DOC "The log4cxx libraries."
)

set(log4cxx_INCLUDE_DIRS ${log4cxx_INCLUDE_DIR})
set(log4cxx_LIBRARIES    ${log4cxx_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(log4cxx
	FOUND_VAR log4cxx_FOUND
	REQUIRED_VARS log4cxx_LIBRARIES log4cxx_INCLUDE_DIRS
)

mark_as_advanced(log4cxx_FOUND)
