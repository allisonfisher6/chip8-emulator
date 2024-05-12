#ifndef CHIP8_H
#define CHIP8_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define MAX_MEM_BYTES 4096 // max memory of 4K systems (bytes)
#define INTERPRETER_SIZE_BYTES 512 // Memory reserved for interpreter.
#define PROGRAM_SIZE_BYTES 3232 // Memory for user programs.
#define STACK_SIZE_BYTES 48 // Memory reserved for the stack.
#define WORK_AREA_SIZE_BYTES 48 // Memory reserved for interpreter work area.
#define DISPLAY_SIZE_BYTES 256 // Memory reserved for display data.
#define NUM_GEN_PURPOSE_REGISTERS 16 // CHIP-8 has 16 1-byte registers V0-VF.


/**
 * @brief Represents the memory map.
 */
struct chip8{
    uint8_t interpreter[INTERPRETER_SIZE_BYTES]; // 0x0000 - 0x01FF
    uint16_t program[PROGRAM_SIZE_BYTES / 2]; // 0x0200 - 0x0E9F
    uint8_t stack[STACK_SIZE_BYTES]; // 0x0EA0 - 0x0ECF
    uint16_t addressRegister;
    uint8_t delayTimer;
    uint8_t soundTimer;
    uint8_t genPurposeRegisters[NUM_GEN_PURPOSE_REGISTERS];
    uint8_t tbdVars[28]; // @TODO placeholder, what else would go in here?
    uint8_t display[DISPLAY_SIZE_BYTES]; // 0x0F00 - 0xFFF
};

/**
 * @brief Reads input .ch8 file and stores contents in the program
 * portion of the memory map.
 * @param filename File to read.
 * @param programMem Pointer to the program portion of the memory map.
 */
int8_t readInstructionsFromFile(char* filename, uint16_t *programMem);

/**
 * @brief Clears the display contents by setting all pixels to 0.
 * @param display Pointer to the display portion of memory.
 */
void clearDisplay(uint8_t *display);

#endif // CHIP8_H
