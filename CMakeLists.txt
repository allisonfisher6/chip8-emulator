cmake_minimum_required(VERSION 3.5)

project(chip8-emulator LANGUAGES C)

add_executable(chip8-emulator main.c
    chip8.h chip8.c)

include(GNUInstallDirs)
install(TARGETS chip8-emulator
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
