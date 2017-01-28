set(COMP_NAME SynthLib)
#project(${COMP_NAME} C CXX)

# Set the build type if it isn't already
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# Cmake module path
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules
                          ${CMAKE_MODULE_PATH})

# gtest needs pthreads
find_package(Threads REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(LIB_INCLUDES ${PROJECT_SOURCE_DIR}/Data_Fifo)


#***************************
# Include Directories
#***************************

include_directories( "${PROJECT_SOURCE_DIR}/Audio" 
                     "${PROJECT_SOURCE_DIR}/Data_Fifo" 
                    )

#***************************
# Source files
#***************************                    

set( SOURCE
        ${SOURCE} 
        ${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioDriver.cpp
        PARENT_SCOPE
        )

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
# Build the Component
#*********************
# get all of our sub-libaries together

#add_library(COMP_NAME ${COMP_SRCS} )
#target_link_libraries(COMP_NAME ${CMAKE_THREAD_LIBS_INIT})
