#-------------------------------------------------------------------
# This file is part of the CMake build system for Houyi
#-------------------------------------------------------------------

# define header files for the library
add_header_group(include "include" FRAMEWORK_INC)
set(HEADER_FILES ${FRAMEWORK_INC})

# define source files for the library
add_source_group(\\ "${CMAKE_CURRENT_SOURCE_DIR}" FRAMEWORK_SRC)

# target library
add_library(Houyi ${HEADER_FILES} ${FRAMEWORK_SRC})

include_directories("include")

install (TARGETS Houyi DESTINATION bin)
install (FILES ${HEADER_FILES} DESTINATION include)