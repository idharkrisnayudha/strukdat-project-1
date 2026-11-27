cmake_minimum_required(VERSION 3.15)

project(tubes LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ==================================================
# Core Library
# ==================================================

file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Exclude application entry point from the library.
list(FILTER CORE_SOURCES EXCLUDE REGEX ".*/main\\.cpp$")

add_library(project_core STATIC ${CORE_SOURCES})
target_include_directories(project_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")


# ==================================================
# Main Executable
# ==================================================
add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE project_core)

# ==================================================
# Test Executables
# ==================================================
file(GLOB TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp")

foreach(TEST_FILE IN LISTS TEST_SOURCES)
  get_filename_component(TEST_NAME "${TEST_FILE}" NAME_WE)
  add_executable(${TEST_NAME} "${TEST_FILE}")
  target_link_libraries(${TEST_NAME} PRIVATE project_core)
  set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test")
endforeach()
