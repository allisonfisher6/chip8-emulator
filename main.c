#include "chip8.h"

void printDisplayBits(uint8_t *data, uint8_t rows, uint8_t cols);

int main(int argc, char* argv[])
{
    struct chip8 processor;

    // copy input program into memory
    uint16_t numInstructions = readInstructionsFromFile(argv[1], processor.program);

    // begin with a clear display
    clearDisplay(processor.display);
    printDisplayBits(processor.display, 32, 64);

    // print disassembly
    for (uint16_t i = 0; i < numInstructions; i++)
    {
        uint8_t *processorAddress = &processor;
        uint8_t *programMemAddress = &processor.program;
        uint16_t instructionAddress = programMemAddress + (sizeof(i) * i) - processorAddress;
        printf("%X %X ", processor.program[i], instructionAddress);
        parseOpcode(processor.program[i]);
    }

    // TODO begin processing instructions
    // TODO find library to handle graphics
    // TODO handle keyboard input

    return 0;
}
