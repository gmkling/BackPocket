cmake_minimum_required(VERSION 2.6)

#-------------------
# Tests
#-------------------

enable_testing()
set(PROJECT_TEST_NAME test_BackPocket)
include_directories( ${GTEST_INCLUDE_DIRS}
                     ${CMAKE_SOURCE_DIR}/Lf_Fifo/
                     ${CMAKE_SOURCE_DIR}/T_Fifo/
                     ${CMAKE_SOURCE_DIR}/Data_Fifo/
                   )
Message("Gtest include: ${GTEST_INCLUDE_DIRS}")
set(TEST_BACKPOCKET_SRC_FILES
    ../T_Fifo/T_Fifo.hpp
    ../Lf_Fifo/Lf_Fifo.hpp
    ../Data_Fifo/Data_Fifo.hpp
    Test_Lf_Fifo.cpp
    Test_T_Fifo.cpp
    Test_Data_Fifo.cpp
    )

# Add sys specific tests as needed
#if (UNIX)
##  if(APPLE)
##    set(TEST
##        ${TEST_MIDI_SRC_FILES}
##        test_MidiInterfaceOSX.cpp
##        )
##  endif()
#    add_definitions("-pthread")
#endif()

add_executable(${PROJECT_TEST_NAME} ${TEST_BACKPOCKET_SRC_FILES})

# Get gtest cooking
add_dependencies(${PROJECT_TEST_NAME} googletest)
add_definitions("-std=c++11 -pthread")
message("Gtest binary path: ${GTEST_LIBS_DIR}")

if(NOT WIN32)
    target_link_libraries(${PROJECT_TEST_NAME}
        ${GTEST_LIBS_DIR}/libgtest.a
        ${GTEST_LIBS_DIR}/libgtest_main.a
        ${CMAKE_THREAD_LIBS_INIT}
        )
else()
    target_link_libraries(${PROJECT_TEST_NAME}
        debug ${GTEST_LIBS_DIR}/DebugLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${CMAKE_FIND_LIBRARY_SUFFIXES}
        optimized ${GTEST_LIBS_DIR}/ReleaseLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${CMAKE_FIND_LIBRARY_SUFFIXES}
        )
    target_link_libraries(${PROJECT_TEST_NAME}
        debug ${GTEST_LIBS_DIR}/DebugLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main${CMAKE_FIND_LIBRARY_SUFFIXES}
        optimized ${GTEST_LIBS_DIR}/ReleaseLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main${CMAKE_FIND_LIBRARY_SUFFIXES}
        )
endif()

add_test(test1 ${PROJECT_TEST_NAME})
