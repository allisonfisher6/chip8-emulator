#include "chip8.h"

void printDisplayBits(uint8_t *data, uint8_t rows, uint8_t cols);

int main(int argc, char* argv[])
{
    // struct chip8 chip8Mem;

    // copy input program into memory
    uint16_t numInstructions = readInstructionsFromFile(argv[1], chip8Mem.program);

    // begin with a clear display
    clearDisplay(chip8Mem.display);
    printDisplayBits(chip8Mem.display, 32, 64);

    // print disassembly
    for (uint16_t i = 0; i < numInstructions; i++)
    {
        uint8_t *processorAddress = &chip8Mem;
        uint8_t *programMemAddress = &chip8Mem.program;
        uint16_t instructionAddress = programMemAddress + (sizeof(i) * i) - processorAddress;
        printf("%X %X ", chip8Mem.program[i], instructionAddress);
        parseOpcode(chip8Mem.program[i]);
    }

    // TODO begin processing instructions
    // TODO find library to handle graphics
    // TODO handle keyboard input

    return 0;
}
