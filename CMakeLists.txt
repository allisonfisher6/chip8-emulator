cmake_minimum_required(VERSION 3.5)

project(chip8-emulator LANGUAGES C)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(chip8-emulator main.c
    chip8.h chip8.c)
target_link_libraries(chip8-emulator ${SDL2_LIBRARIES})

include(GNUInstallDirs)
install(TARGETS chip8-emulator
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
