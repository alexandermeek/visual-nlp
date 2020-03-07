cmake_minimum_required(VERSION 3.10)
project(VisualNLP VERSION 1.0 LANGUAGES CXX)

add_subdirectory(include/pybind11)
add_subdirectory(include/imgui)

set_property(GLOBAL PROPERTY USE_FOLDERS ON) # Hide extra projects in solution under a folder.

file (GLOB NLP_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
#${CMAKE_CURRENT_SOURCE_DIR}/include/imgui/*.cpp)

file (GLOB NLP_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
#${CMAKE_CURRENT_SOURCE_DIR}/include/imgui/*.h)

add_executable(VisualNLP ${NLP_SOURCES} ${NLP_HEADERS} ${VerifyCXX})

target_link_libraries(VisualNLP PUBLIC d3d9)
target_link_libraries(VisualNLP PUBLIC pybind11::embed)
target_link_libraries(VisualNLP PUBLIC imgui)

target_include_directories(VisualNLP PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_CURRENT_SOURCE_DIR}/include)