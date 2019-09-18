cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

set(AUDEO_DEPENDENCIES_FOLDER_NAME "dependencies")
set(DEBUG_OUTPUT_DIRECTORY "bin/debug")
set(RELEASE_OUTPUT_DIRECTORY "bin/release")

project(audeo)
enable_language(CXX)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)

set(AUDEO_COMPILE_FLAGS "")

# build Release by default
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
	message(STATUS "Audeo: Build type not specified: Using Release by default.")
endif(NOT CMAKE_BUILD_TYPE)

option(BUILD_EXPORT ON)

add_definitions(
	-DMBCS
	${AUDEO_COMPILE_FLAGS}
)

if (BUILD_EXPORT)
	add_definitions(-DAUDEO_EXPORT)
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/${DEBUG_OUTPUT_DIRECTORY}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/${DEBUG_OUTPUT_DIRECTORY}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/${DEBUG_OUTPUT_DIRECTORY}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/${RELEASE_OUTPUT_DIRECTORY}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/${RELEASE_OUTPUT_DIRECTORY}")
set(CMAKE_EXECUTABLE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/${RELEASE_OUTPUT_DIRECTORY}")

if (MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /W3 /MD /Od /GR /EHsc /permissive- /Za")
   	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /W3 /GL /Od /Oi /GR /Gy /EHsc /permissive- /Za")
else()
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
       		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
   	endif()
endif()

add_subdirectory("dependencies")
add_subdirectory("include")
add_subdirectory("src")
# build examples if requested

option(AUDEO_BUILD_EXAMPLES 
	"Build the audeo example files" ON)

if (AUDEO_BUILD_EXAMPLES)
	add_subdirectory("examples")
endif(AUDEO_BUILD_EXAMPLES)

add_library(audeo SHARED
	${AUDEO_HEADER_FILES}
	${AUDEO_SOURCE_FILES}
)

set_target_properties(audeo PROPERTIES FOLDER "audeo")
source_group(TREE "${CMAKE_CURRENT_LIST_DIR}" FILES ${AUDEO_HEADER_FILES} ${AUDEO_SOURCE_FILES})

target_include_directories(audeo
	PUBLIC ${AUDEO_PUBLIC_INCLUDE_DIRECTORIES}
	"${CMAKE_CURRENT_LIST_DIR}/include"
	"${CMAKE_CURRENT_LIST_DIR}/dependencies/sdl/include"
	"${CMAKE_CURRENT_LIST_DIR}/dependencies/sdl_mixer/include"
)

set(LINK_LIBRARIES 
	${CMAKE_CURRENT_LIST_DIR}/dependencies/sdl/bin/SDL2.lib
	${CMAKE_CURRENT_LIST_DIR}/dependencies/sdl/bin/SDL2main.lib
	${CMAKE_CURRENT_LIST_DIR}/dependencies/sdl_mixer/lib/x64/SDL2_mixer.lib
)

target_link_libraries(audeo ${LINK_LIBRARIES})
