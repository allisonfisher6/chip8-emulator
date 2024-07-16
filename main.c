#include <unistd.h>
#include <limits.h>
#include "chip8.h"

void printUsageInfo(char* programName)
{
    printf("Usage: %s [options] path_to_ROM\n", programName);
    printf("  options:\n");
    printf("    -h Print usage info\n");
    printf("    -d Print disassembly output of input ROM\n");
}

void parseInputArgs(int argc, char* argv[], char* filename, int8_t* outputDisassembly)
{
    int32_t opt;
    while ((opt = getopt(argc, argv, "hd")) != -1)
    {
        switch(opt)
        {
        case 'h':
            printUsageInfo(argv[0]);
            return 0;
        case 'd':
            *outputDisassembly = 1;
            break;
        default:
            printf("Error: Unknown input argument.\n");
            printUsageInfo(argv[0]);
            return -1;
        }
    }

    // determine if filename was provided
    if (optind < argc)
    {
        strncpy(filename, argv[optind], PATH_MAX);
    }
    else
    {
        printf("Error: Path to ROM not provided as an input arg.\n");
        printUsageInfo(argv[0]);
        return -1;
    }
}
int main(int argc, char* argv[])
{
    uint8_t exitCode = 0;
    char filename[PATH_MAX + 1];
    int8_t outputDisassembly = 0;

    // parse input args
    parseInputArgs(argc, argv, &filename, &outputDisassembly);

    // copy input program into memory
    exitCode = !readInstructionsFromFile(filename);
    if (!exitCode)
    {
        return exitCode;
    }

    // if -d arg is provided at input, print disassembly
    if (outputDisassembly)
    {
        printDisassembly();
        return 0;
    }

    // begin execution of ROM
    runProgram();

    // TODO timers need to count down at a rate of 60hz

    return 0;
}
