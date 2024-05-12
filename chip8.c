#include "chip8.h"

int8_t readInstructionsFromFile(char* filename, uint16_t *programMem)
{
    FILE* fptr = fopen(filename, "r");
    if (fptr == NULL)
    {
        printf("Failed to open input file: %s\n", filename);
        return -1;
    }

    fseek(fptr, sizeof(uint16_t), SEEK_END);
    uint16_t numInstructionsInFile = ftell(fptr) / 2;
    fseek(fptr, 0, SEEK_SET);

    if(numInstructionsInFile <= (PROGRAM_SIZE_BYTES / 2))
    {
        uint16_t instruction;
        uint16_t instructionIndex = 0;
        while (fread(&instruction, sizeof(instruction), 1, fptr) == 1)
        {
            // swap for endianness
            uint16_t swapped = (instruction >> 8) | (instruction << 8);
            programMem[instructionIndex] = swapped;
            instructionIndex++;
        }
        printf("\n");
    }
    else
    {
        printf("Program exceeds allowed size of %d bytes.\n", PROGRAM_SIZE_BYTES);
        return -1;
    }

    fclose(fptr);
    return 0;
}

void clearDisplay(uint8_t *display)
{
    memset(display, 0x00, DISPLAY_SIZE_BYTES);
}
