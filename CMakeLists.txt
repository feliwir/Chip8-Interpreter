cmake_minimum_required (VERSION 3.0.0)

# set a default build type if none was provided
# this has to be done before the project() instruction!
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

project (chip8)

set (EMU chip8)
set (VERSION_MAJOR 0)
set (VERSION_MINOR 1)

set (SOURCES_DIR "src")
set (OTHER_LIBRARIES "")
set (LINKER_FLAGS "")

if (MSVC)
    if (${MSVC_VERSION} VERSION_LESS 1700)
        message(FATAL_ERROR "You need to compile this project with at least Visual Studio 2013")
    endif ()

	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
elseif (UNIX)
    if (${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
        execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
        if (${GCC_VERSION} VERSION_GREATER 4.9 OR ${GCC_VERSION} VERSION_EQUAL 4.9)
            set(C11 "-std=c++1y")
        elseif (${GCC_VERSION} VERSION_GREATER 4.7 OR ${GCC_VERSION} VERSION_EQUAL 4.7)
            set(C11 "-std=c++11")
        elseif (${GCC_VERSION} VERSION_GREATER 4.3 OR ${GCC_VERSION} VERSION_EQUAL 4.3)
            set(C11 "-std=c++0x")
        else ()
            message(FATAL_ERROR "You need at least GCC 4.3 to compile this.")
        endif ()
    elseif (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
        set(C11 "-std=c++11 -stdlib=libc++")
    endif ()

    set(CMAKE_CXX_FLAGS                "-Wall -Wextra ${C11} -pthread")
    set(CMAKE_CXX_FLAGS_DEBUG          "-O0 -g")
    set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
endif ()

set (CMAKE_USE_RELATIVE_PATHS false)
set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")

# Detect the OS
if(${CMAKE_HOST_WIN32})
    set(WINDOWS 1)
elseif(${CMAKE_HOST_APPLE})
    set(MACOSX 1)
elseif(${CMAKE_HOST_UNIX})
    set(LINUX 1)
else()
    message(FATAL_ERROR "Unsupported operating system")
    return()
endif()

if (WINDOWS)
    set (BUILD_STATIC FALSE CACHE BOOL "TRUE to build static version of sfeMovie")
endif()

#################################################################################################################
# ================================================ SFML SETUP ================================================= #
#################################################################################################################
if (BUILD_STATIC)
    set (SFML_STATIC_LIBRARIES TRUE)
endif()

find_package (SFML 2 COMPONENTS graphics window system audio REQUIRED)
message(STATUS "SFML libraries: ${SFML_LIBRARIES}")
message(STATUS "Additional dependencies: ${SFML_DEPENDENCIES}")

#################################################################################################################
# ============================================== OpenGL SETUP ================================================= #
#################################################################################################################
find_package (GL REQUIRED)
message(STATUS "OpenGL libraries: ${OPENGL_LIBRARIES}")
message(STATUS "OpenGL include dir: ${OPENGL_INCLUDE_DIR_GL}")

#################################################################################################################
# ================================================= EMU SETUP ================================================= #
#################################################################################################################

file(GLOB_RECURSE SRC_FILES ${SOURCES_DIR}/[a-z,A-Z]*.[c,h]*)

set(EMU_INCLUDE_DIRS "${SFML_INCLUDE_DIR}")
include_directories (${EMU_INCLUDE_DIRS})

source_group(Source FILES ${SRC_FILES})

if(MSVC)
	add_definitions(-D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS /D_CRT_SECURE_NO_WARNINGS /wd4099 /wd4098 /wd4458 /nologo)
endif()

add_executable(${EMU} ${SRC_FILES})

# Define the libraries game should link against
if (BUILD_STATIC)
    set_target_properties(${EMU} PROPERTIES
        COMPILE_DEFINITIONS SFML_STATIC)		
		
	target_link_libraries(${EMU} PRIVATE ${SFML_LIBRARIES} ${SFML_DEPENDENCIES} ${OPENGL_LIBRARIES})
else()
	target_link_libraries(${EMU} PRIVATE ${SFML_LIBRARIES}  -ldl ${OPENGL_LIBRARIES})
endif()

include (${CMAKE_SOURCE_DIR}/cmake/Install.cmake)
		
# add an option for building the documentation
set(BUILD_DOC FALSE CACHE BOOL "Set to true to build the documentation")
if(BUILD_DOC)
    add_subdirectory(doxygen)
endif()
