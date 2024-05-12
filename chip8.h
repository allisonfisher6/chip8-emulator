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

extern struct chip8 chip8Mem;

/**
 * @brief Reads input .ch8 file and stores contents in the program
 * portion of the memory map.
 * @param filename File to read.
 * @param programMem Pointer to the program portion of the memory map.
 * @return Number of instructions or -1 if there was an error.
 */
int16_t readInstructionsFromFile(char* filename, uint16_t *programMem);

/**
 * @brief Clears the display contents by setting all pixels to 0.
 * @param display Pointer to the display portion of memory.
 */
void clearDisplay(uint8_t *display);

/**
 * @brief Prints the 64x32 bit display for experimentation and debugging
 * purposes until graphics are implemented. @TODO remove when graphics are
 * implemented.
 * @param data Pointer to display data.
 * @param rows Number of rows in display (Y-axis)
 * @param cols Number of columns in display (X-axis)
 */
void printDisplayBits(uint8_t *data, uint8_t rows, uint8_t cols);

/**
 * @brief Draws a sprite at provided x and y coordinates that has a width of 8
 * pixels and passed in height. VF is set to 1 if any sceen pixels are flipped
 * from set to unset when the sprite is draw and 0 if it does not happen.
 * @param display Pointer to display data.
 * @param pixelsToDraw Pointer to pixel data in memory.
 * @param xpos Position on x-axis to draw the sprite.
 * @param ypos Position on y-axis to draw the sprite.
 * @param height Pixel height of the sprite.
 */
void drawSprite(uint8_t *display, uint8_t *pixelsToDraw,
                uint8_t xpos, uint8_t ypos, uint8_t height);

/**
 * @brief Translates instruction into the CHIP-8 opcode it represents.
 * @param instruction Instruction to process.
 */
void parseOpcode(uint16_t instruction);

/**
 * @brief Translates instructions that contain variables for address, VX, VY,
 * or a number.
 * @param instruction Instruction to process.
 */
void parseVariableOpcode(uint16_t instruction);

#endif // CHIP8_H
