cmake_minimum_required(VERSION 3.10)
project(VisualNLP VERSION 1.0 LANGUAGES CXX)

# Don't run JSON build tests
set(JSON_BuildTests OFF CACHE INTERNAL "")

add_subdirectory(include/pybind11)
add_subdirectory(include/imgui)
add_subdirectory(include/json)

set_property(GLOBAL PROPERTY USE_FOLDERS ON) # Hide extra projects in solution under a folder.

file (GLOB NLP_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

file (GLOB NLP_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_executable(VisualNLP ${NLP_SOURCES} ${NLP_HEADERS} ${VerifyCXX})

target_link_libraries(VisualNLP PRIVATE d3d9)
target_link_libraries(VisualNLP PRIVATE pybind11::embed)
target_link_libraries(VisualNLP PRIVATE imgui)
target_link_libraries(VisualNLP PRIVATE nlohmann_json::nlohmann_json)

target_include_directories(VisualNLP PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_CURRENT_SOURCE_DIR}/include)

# Copy contents of scripts folder to binary directory.
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/scripts
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

message(STATUS "Copied scripts to binary directory")