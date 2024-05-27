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

    // print disassembly
    // printDisassembly();

    printf("--------------------------------------\n");
    // TODO begin processing instructions
    runProgram();

    // TODO timers need to count down at a rate of 60hz

    return 0;
}
