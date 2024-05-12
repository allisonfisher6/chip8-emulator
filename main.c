#include "chip8.h"

int main(int argc, char* argv[])
{
    uint8_t exitCode = 0;

    // copy input program into memory
    exitCode = !readInstructionsFromFile(argv[1]);
    if (!exitCode)
    {
        return exitCode;
    }

    // begin with a clear display
    clearDisplay();
    // printDisplayBits(32, 64);

    // print disassembly
    printDisassembly();


    // TODO begin processing instructions
    runProgram();
    // TODO find library to handle graphics
    // TODO handle keyboard input

    return 0;
}
