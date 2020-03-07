include_directories(${CMAKE_CURRENT_SOURCE_DIR})
project(imgui)

file (GLOB IMGUI_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

file (GLOB IMGUI_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(imgui ${IMGUI_SOURCES} ${IMGUI_HEADERS} )