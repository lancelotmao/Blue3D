#-------------------------------------------------------------------
# This file is part of the CMake build system for Houyi
#-------------------------------------------------------------------

set(DIR_CUR_RENDERER "${CMAKE_CURRENT_SOURCE_DIR}")

if (USE_2D)
	set(DIR_CUR_RENDERER "${CMAKE_CURRENT_SOURCE_DIR}/2D")
else ()
	if (WIN32)
		set(DIR_CUR_RENDERER "${CMAKE_CURRENT_SOURCE_DIR}/DirectX")
	endif (WIN32)
	if (APPLE)
		set(DIR_CUR_RENDERER "${CMAKE_CURRENT_SOURCE_DIR}/OpenGL")
	endif(APPLE)
endif (USE_2D)

#message("${DIR_CUR_RENDERER}")

# define header files for the library
add_header_group(include "${DIR_CUR_RENDERER}/include" RENDERER_INC)
set(HEADER_FILES ${RENDERER_INC})

# define source files for the library
add_source_group(\\ "${DIR_CUR_RENDERER}" RENDERER_SRC)

# target library
add_library(Renderer SHARED ${HEADER_FILES} ${RENDERER_SRC})

# link library
target_link_libraries(Renderer Houyi)

# set target directory to plugins folder
set_target_properties(Renderer PROPERTIES
         LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/plugins)
    
include_directories("${DIR_CUR_RENDERER}/include")

install (TARGETS Renderer DESTINATION bin)
install (FILES ${HEADER_FILES} DESTINATION include)