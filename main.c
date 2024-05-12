#include "chip8.h"

void printDisplayBits(uint8_t *data, uint8_t rows, uint8_t cols);

int main(int argc, char* argv[])
{
    struct chip8 processor;

    // copy input program into memory
    readInstructionsFromFile(argv[1], processor.program);

    // begin with a clear display
    clearDisplay(processor.display);
    printDisplayBits(processor.display, 32, 64);

    // TODO begin processing instructions
    // TODO find library to handle graphics
    // TODO handle keyboard input

    return 0;
}

void parseOpcode(uint16_t instruction)
{
    switch(instruction)
    {
        case 0x00e0: printf("%X clear\n", instruction); break;
        case 0x00ee: printf("%X return\n", instruction); break;
        default: parseVariableOpcode(instruction);
    }
}

void parseVariableOpcode(uint16_t instruction)
{
    uint8_t oneDigitNumber;  // holds N from opcode (a number between 0 and 15)
    uint8_t twoDigitNumber;  // holds NN from opcode (a number between 0 and 255)
    uint16_t address;        // holds NNN from opcode (an address between 0 and 4095)
    uint8_t registerX;       // holds X from opcode (a register 0-F)
    uint8_t registerY;       // holds Y from opcode (a register 0-F)

    // Parse the opcode into potential register, number, or address variables.
    // Sometimes they will be unused or invalid, but it helps keep the switch
    // statements below concise. NNN will always be the last 3 hex digits,
    // NN will be the last two digits, N will be the last digit, X the second,
    // Y the third.
    oneDigitNumber = instruction & 0x000f;
    twoDigitNumber = instruction & 0x00ff;
    address = instruction & 0x0fff;
    registerX = (instruction & 0x0f00) >> 8;
    registerY = (instruction & 0x00f0) >> 4;

    uint8_t firstHexDigit = instruction >> 12;
    switch(firstHexDigit)
    {
        case 0x01: printf("jump %X\n", address); break;
        case 0x02: printf("call subroutine at %X\n", address); break;
        case 0x03: printf("if v%x != %X then\n", registerX, twoDigitNumber); break;
        case 0x04: printf("if v%x == %X then\n", registerX, twoDigitNumber); break;
        case 0x05: printf("if v%x != v%x then\n", registerX, registerY); break;
        case 0x06: printf("v%x := %X\n", registerX, twoDigitNumber); break;
        case 0x07: printf("v%x += %X\n", registerX, twoDigitNumber); break;
        case 0x08:
        {
            char* operand = "<op>";
            switch(instruction & 0x000f)
            {
                case 0x0000: operand = ":="; break;
                case 0x0001: operand = "|="; break;
                case 0x0002: operand = "&="; break;
                case 0x0003: operand = "^="; break;
                case 0x0004: operand = "+="; break;
                case 0x0005: operand = "-="; break;
                case 0x0006: operand = ">>="; break;
                case 0x0007: operand = "=-"; break;
                case 0x000e: operand = "<<="; break;
                default: printf("Invalid opcode "); break;
            }
            printf("v%x %s v%x\n", registerX, operand, registerY);
            break;
        }
        case 0x09: printf("if v%x == v%x then\n", registerX, registerY); break;
        case 0x0a: printf("i := %X\n", address); break;
        case 0x0b: printf("jump0 %X\n", address); break;
        case 0x0c: printf("v%x := random %X\n", registerX, twoDigitNumber); break;
        case 0x0d: printf("sprite v%x v%x %X\n", registerX, registerY, oneDigitNumber); break;
        case 0x0e:
        {
            switch(instruction & 0x00ff)
            {
                case 0x009e: printf("if v%x -key then\n", registerX); break;
                case 0x00a1: printf("if v%x key then\n", registerX); break;
                default: printf("Invalid opcode\n");
            }
            break;
        }
        case 0x0f:
        {
            switch(instruction & 0x00ff)
            {
                case 0x0007: printf("v%x := delay\n", registerX); break;
                case 0x000a: printf("v%x := key\n", registerX); break;
                case 0x0015: printf("delay := v%x\n", registerX); break;
                case 0x0018: printf("buzzer := v%x\n", registerX); break;
                case 0x001e: printf("i += v%x\n", registerX); break;
                case 0x0029: printf("i := hex v%x\n", registerX); break;
                case 0x0033: printf("bcd v%x\n", registerX); break;
                case 0x0055: printf("save v%x\n", registerX); break;
                case 0x0065: printf("load v%x\n", registerX); break;
                default: printf("Invalid opcode\n"); break;
            }
            break;
        }
        default: printf("Invalid opcode\n"); break;
    }
}

void parseInstructionsFromFile(char* filename)
{
    FILE* fptr = fopen(filename, "r");
    if (fptr == NULL)
    {
        printf("Failed to open input file.\n");
        return -1;
    }

    // instructions start at address 0x200
    uint16_t memAddrOfInstruction = 0x200;

    uint16_t instruction;
    while (fread(&instruction, sizeof(instruction), 1, fptr) == 1)
    {
        // swap for endianness
        uint16_t swapped = (instruction >> 8) | (instruction << 8);

        // instructions start at address 0x200
        printf("%X %X ", memAddrOfInstruction, swapped);
        memAddrOfInstruction += 2;
        parseOpcode(swapped);
    }
    printf("\n");

    fclose(fptr);
}

void drawSprite(struct chip8* processor, uint8_t xpos, uint8_t ypos)
{
    /*
     * From wiki: 	Draws a sprite at coordinate (VX, VY) that has a width of 8
     * pixels and a height of N pixels. Each row of 8 pixels is read as
     * bit-coded starting from memory location I; I value does not change after
     * the execution of this instruction. As described above, VF is set to 1 if
     * any screen pixels are flipped from set to unset when the sprite is drawn,
     * and to 0 if that does not happen.
     */



}
