cmake_minimum_required(VERSION 3.12)

project(rosa-emu CXX)

add_executable(rosa-emu rosa-emu.cpp)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

target_link_libraries(rosa-emu ${SDL2_LIBRARIES})

find_package(Threads REQUIRED)

set_property(TARGET rosa-emu PROPERTY CXX_STANDARD 17)

if(MSVC)
    target_compile_options(rosa-emu PRIVATE /W4 /WX)
else()
    target_compile_options(rosa-emu PRIVATE -Wall -Wextra -pedantic -Werror)
endif()
