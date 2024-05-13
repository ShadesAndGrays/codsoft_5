cmake_minimum_required(VERSION 3.14)

project(bookKeep CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

set(src
src/main.cpp
src/database.cpp
src/util.cpp
)

add_compile_definitions(
    RAYGUI_IMPLEMENTATION
    GUI_COUNTERPAGE_IMPLEMENTATION
    GUI_SEARCHPAGE_IMPLEMENTATION
    GUI_BOOKELEMENT_IMPLEMENTATION
    GUI_RETURNBOOK_IMPLEMENTATION
    SIZE_X=700
    SIZE_Y=500
    )

include_directories(include layouts)

find_package(raylib REQUIRED)

add_executable(${PROJECT_NAME} ${src})

target_link_libraries(${PROJECT_NAME} mariadbcpp raylib)
