cmake_minimum_required(VERSION 2.6)

set(BIN_NAME KlingSynth)
project(${BIN_NAME} C CXX)

#set(LIB_NAME libBP)
#project(${LIB_NAME} C CXX)

# Set the build type if it isn't already
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# Cmake module path
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules
                          ${CMAKE_MODULE_PATH})

# gtest needs pthreads
find_package(Threads REQUIRED)

# audio options - choose appropriate pkg for platform
# prefer portaudio, jack in that order

find_package(Jack)
find_package(Portaudio)

if(NOT PORTAUDIO_FOUND)
    if(NOT JACK_FOUND)
        message(FATAL_ERROR "Audio API files not found for Jack and Portaudio")
    endif()
endif()

#if(PORTAUDIO_FOUND)
#    set(AUDIO_LIBRARY ${PORTAUDIO_LIBRARIES})
#    set(AUDIO_DRIVER "${PROJECT_SOURCE_DIR}/Audio/AudioDriver_PA.cpp")
#endif()

# if no portaudio, use Jack
if(JACK_FOUND)
    set(AUDIO_LIBRARY ${JACK_LIBRARIES})
    set(AUDIO_DRIVER "${PROJECT_SOURCE_DIR}/Audio/AudioDriver_Jack.cpp")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# versioning
set (libBP_VERSION_MAJOR 0)
set (libBP_VERSION_MINOR 1)

# config header for passing build info into compile code
configure_file(
    "${PROJECT_SOURCE_DIR}/libBP.config"
    "${PROJECT_BINARY_DIR}/libBPConfig.h"
    )

set(LIB_INCLUDES ${PROJECT_SOURCE_DIR}/include)
set(EXT_PROJECTS_DIR ${PROJECT_SOURCE_DIR}/ext)

# modules
add_subdirectory(${EXT_PROJECTS_DIR}/gtest)
#add_subdirectory(tests)
add_subdirectory(Audio)
#add_subdirectory(T_Fifo)
#add_subdirectory(Lf_Fifo)
#add_subdirectory(Graphs)
add_subdirectory(Data_Fifo)

#***************************
# Include Directories
#***************************

include_directories( "${PROJECT_SOURCE_DIR}/Audio" 
                     "${PROJECT_SOURCE_DIR}/Data_Fifo" 
                    )

set(SOURCE main.cpp "${PROJECT_SOURCE_DIR}/Audio/SynthContext.cpp" ${AUDIO_DRIVER})

#***************************
# some global compile flags
#***************************

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++17 -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}  -std=c++17 -Wall")

if(UNIX)

    # this may belong somewhere else
    set(STATIC_LINKER_FLAGS "-Wl,--export-all-symbols -lpthread")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -pthread")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pthread")
    if(APPLE)
        # enable the handy CFSTR() macro for initializers
        add_definitions("-fconstant-cfstrings")
    endif()
endif()

#*********************
# Build libraries
#*********************
# get all of our sub-libaries together

#add_library(libBP INTERFACE )
#target_include_directories(libBP INTERFACE ${PROJECT_BINARY_DIR})
#target_link_libraries(libBP INTERFACE ${CMAKE_THREAD_LIBS_INIT} ${JACK_LIBRARIES})

#*********************
# Build the thing
#*********************

add_executable(${BIN_NAME} ${SOURCE} )
target_link_libraries(${BIN_NAME} ${COMP_NAME} ${AUDIO_LIBRARY})