cmake_minimum_required(VERSION 3.12)

project(rosa-emu)

add_executable(rosa-emu rosa-emu.cpp events.cpp apps/apple2e/apple2e.cpp apps/apple2e/dis6502.cpp apps/apple2e/interface_rosa.cpp apps/coleco/coleco_platform_rosa.cpp apps/coleco/emulator.cpp apps/coleco/tms9918.cpp apps/launcher/launcher.cpp apps/mp3player/mp3player.cpp apps/coleco/z80emu.c apps/trs80/trs80.cpp apps/trs80/fonts.cpp apps/trs80/z80emu.c api/8x16.cpp api/key-repeat.cpp api/text-mode.cpp api/ui.cpp)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

target_include_directories(rosa-emu PRIVATE api)

target_link_libraries(rosa-emu ${SDL2_LIBRARIES})

add_compile_definitions(ROSA=1)

find_package(Threads REQUIRED)

set_property(TARGET rosa-emu PROPERTY CXX_STANDARD 17)

if(MSVC)
    target_compile_options(rosa-emu PRIVATE /W4 /WX)
else()
    target_compile_options(rosa-emu PRIVATE -Wall -Wextra -pedantic -Werror)
endif()
