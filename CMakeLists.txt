cmake_minimum_required(VERSION 3.15.5)

# Set the name of the project and store it in a variable PROJECT_NAME
project(ui_layout)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)

add_library(ui_layout src/RowLayout.cpp)

if (MSVC)
    add_compile_options(-Zi -W4)
else()
    # fail until we target more platforms
    message(FATAL_ERROR "compiler not supported")
endif()