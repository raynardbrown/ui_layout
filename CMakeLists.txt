cmake_minimum_required(VERSION 3.15.5)

# Set the name of the project and store it in a variable PROJECT_NAME
project(ui_layout)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)

add_library(${PROJECT_NAME} src/RowLayout.cpp)

if (MSVC)
    add_compile_options(-Zi 
                        -W4
                        -EHsc
                        -D_CRT_SECURE_NO_WARNINGS
                        -DSTRICT
                        -DUNICODE
                        -D_UNICODE)
else()
    # fail until we target more platforms
    message(FATAL_ERROR "compiler not supported")
endif()