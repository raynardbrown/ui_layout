cmake_minimum_required(VERSION 3.15.5)

# Set the name of the project and store it in a variable PROJECT_NAME
project(ui_layout)

# Require at least this version of boost
find_package(Boost 1.71.0)
if(NOT Boost_FOUND)
  message(FATAL_ERROR "boost not found")
endif()

set(Headers "")
list(APPEND Headers ${CMAKE_CURRENT_SOURCE_DIR}/inc)
list(APPEND Headers ${CMAKE_CURRENT_SOURCE_DIR}/src)  # private headers
list(APPEND Headers ${Boost_INCLUDE_DIRS})

set(Sources src/ui/layout/RowLayout.cpp)

if (MSVC)
    add_compile_options(-Zi 
                        -W4
                        -EHsc
                        -D_CRT_SECURE_NO_WARNINGS
                        -DSTRICT
                        -DUNICODE
                        -D_UNICODE
                        -DNOMINMAX)

    set(Sources_Windows src/ui/layout/windows/UiComponentSize.cpp)
    
    list(APPEND Sources ${Sources_Windows})
else()
    # fail until we target more platforms
    message(FATAL_ERROR "compiler not supported")
endif()

find_path(UI_INCLUDE NAMES ui/Window.h PATH_SUFFIXES ui/inc)

if(NOT UI_INCLUDE)
  message(FATAL_ERROR "ui headers not found")
endif()

list(APPEND Headers ${UI_INCLUDE})

find_path(GRAPHICS_INCLUDE NAMES graphics/Font.h PATH_SUFFIXES graphics/inc)

if(NOT GRAPHICS_INCLUDE)
  message(FATAL_ERROR "graphics headers not found")
endif()

list(APPEND Headers ${GRAPHICS_INCLUDE})

add_library(${PROJECT_NAME} ${Sources})

target_include_directories(${PROJECT_NAME} PRIVATE ${Headers})