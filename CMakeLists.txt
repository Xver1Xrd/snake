cmake_minimum_required(VERSION 3.16)
project(TimeEchoSnake CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(TimeEchoSnake
    src/main.cpp
    src/Game.cpp
    src/Game.h
    src/Snake.cpp
    src/Snake.h
)

if(EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    set_target_properties(TimeEchoSnake PROPERTIES SUFFIX ".html")
    set_target_properties(TimeEchoSnake PROPERTIES LINK_FLAGS "-s USE_GLFW=3 -s ASYNCIFY --shell-file ${CMAKE_SOURCE_DIR}/web/shell.html")
    target_include_directories(TimeEchoSnake PRIVATE ${CMAKE_SOURCE_DIR}/src)
    
    find_package(raylib QUIET)
    if(NOT raylib_FOUND)
        include(FetchContent)
        FetchContent_Declare(
            raylib
            GIT_REPOSITORY https://github.com/raysan5/raylib.git
            GIT_TAG 5.0
        )
        FetchContent_MakeAvailable(raylib)
    endif()
    
    target_link_libraries(TimeEchoSnake raylib)
else()
    find_package(raylib 5.0 QUIET)
    if(NOT raylib_FOUND)
        include(FetchContent)
        FetchContent_Declare(
            raylib
            GIT_REPOSITORY https://github.com/raysan5/raylib.git
            GIT_TAG 5.0
        )
        FetchContent_MakeAvailable(raylib)
    endif()
    
    target_link_libraries(TimeEchoSnake raylib)
    target_include_directories(TimeEchoSnake PRIVATE ${CMAKE_SOURCE_DIR}/src)
endif()
