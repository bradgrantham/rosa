cmake_minimum_required(VERSION 3.12)

project(rosa-emu)

add_executable(rosa-emu rosa-emu.cpp rosa-apps/apple2e/apple2e.cpp rosa-apps/apple2e/dis6502.cpp rosa-apps/apple2e/interface_rosa.cpp rosa-apps/coleco/coleco_platform_rosa.cpp rosa-apps/coleco/emulator.cpp rosa-apps/coleco/tms9918.cpp rosa-apps/launcher/launcher.cpp rosa-apps/mp3player/mp3player.cpp rosa-apps/coleco/z80emu.c rosa-kit/8x16.cpp rosa-kit/key-repeat.cpp rosa-kit/text-mode.cpp rosa-kit/ui.cpp)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

target_include_directories(rosa-emu PRIVATE rosa-kit)

target_link_libraries(rosa-emu ${SDL2_LIBRARIES})

add_compile_definitions(ROSA=1)

find_package(Threads REQUIRED)

set_property(TARGET rosa-emu PROPERTY CXX_STANDARD 17)

# if(MSVC)
    # target_compile_options(rosa-emu PRIVATE /W4 /WX)
# else()
    # target_compile_options(rosa-emu PRIVATE -Wall -Wextra -pedantic -Werror)
# endif()
