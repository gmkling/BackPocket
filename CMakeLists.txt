cmake_minimum_required(VERSION 2.6)
set(LIB_NAME libBP)
project(${LIB_NAME} C CXX)

# Set the build type if it isn't already
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# Cmake module path
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules
                          ${CMAKE_MODULE_PATH})

# gtest needs pthreads
find_package(Threads REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# versioning
set (libBP_VERSION_MAJOR 0)
set (libBP_VERSION_MINOR 1)

# config header for passing build info into compile code
configure_file(
    "${PROJECT_SOURCE_DIR}/libBP.config"
    "${PROJECT_BINARY_DIR}/libBPConfig.h"
    )

# directories for ext deps and for client includes
# Q: How does LIB_INCLUDES differ/overlap with INTERFACE includes in target_include_directories below?
# A: subdirectories have their own private includes, LIB_INCLUDES is for public interfaces only(?)
set(LIB_INCLUDES ${PROJECT_SOURCE_DIR}/include)
set(EXT_PROJECTS_DIR ${PROJECT_SOURCE_DIR}/ext)

# modules
add_subdirectory(${EXT_PROJECTS_DIR}/gtest)
add_subdirectory(tests)
add_subdirectory(T_Fifo)
add_subdirectory(Lf_Fifo)
add_subdirectory(Graphs)
add_subdirectory(Data_Fifo)

# some global compile flags

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++11 -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}  -std=c++11 -Wall")

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

add_library(libBP INTERFACE )
target_include_directories(libBP INTERFACE ${PROJECT_BINARY_DIR})
target_link_libraries(libBP INTERFACE ${CMAKE_THREAD_LIBS_INIT} ${JACK_LIBRARIES})
