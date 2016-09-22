cmake_minimum_required(VERSION 2.6)

set(GRAPH_TARGET gmk_midi)

# midi module will need some conditional/cross platform compilation
# just doing with mac now


set(graph_source
    Graph_Common.hpp
    Node.hpp
    Node.cpp
    Edge.hpp
    Edge.cpp
    Graph.hpp
    Graph.cpp
    Engine.hpp
    Engine.cpp
    )

add_library(${GRAPH_TARGET} SHARED
            ${graph_source})

target_include_directories(${GRAPH_TARGET} PUBLIC
            ${CMAKE_SOURCE_DIR}/Graph/
            ${CMAKE_SOURCE_DIR}/build/
            )

# set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--export-all-symbols")
