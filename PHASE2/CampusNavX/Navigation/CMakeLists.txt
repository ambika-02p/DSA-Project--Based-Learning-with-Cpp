cmake_minimum_required(VERSION 3.10)
project(CampusNavX LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CAMPUSNAVX_CORE_SOURCES
    "Data Structures/Graph.cpp"
    "Data Structures/IndoorGraph.cpp"
    algorithms/BFS.cpp
    algorithms/DFS.cpp
    algorithms/Dijkstra.cpp
    algorithms/MinHeap.cpp
    "File Handling/FileManager.cpp"
    navigation/Navigation.cpp
    navigation/IndoorNavigation.cpp
    navigation/AccessibleNavigation.cpp
    navigation/EmergencyNavigation.cpp)

# Original console backend remains available.
add_executable(CampusNavX main.cpp ${CAMPUSNAVX_CORE_SOURCES})

# Browser GUI uses a local HTTP bridge to the same C++ graph/algorithm classes.
add_executable(CampusNavX_GUI_Server server.cpp ${CAMPUSNAVX_CORE_SOURCES})
if(WIN32)
    target_link_libraries(CampusNavX_GUI_Server ws2_32)
endif()
