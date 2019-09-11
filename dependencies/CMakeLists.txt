add_subdirectory(sdl)

set_target_properties(SDL2
	PROPERTIES
	FOLDER "${AUDEO_DEPENDENCIES_FOLDER_NAME}"
)

if (MSVC)
	set_target_properties(SDL2
		PROPERTIES
		COMPILE_OPTIONS "/W0"
	)
elseif("${CMAKE_CXX_COMPILER_ID}" matches "GCC" or "${CMAKE_CXX_COMPILER_ID}" matches "Clang")
	set_target_properties(SDL2
		PROPERTIES
		COMPILE_OPTIONS "-Wno-everything"
	)
endif()