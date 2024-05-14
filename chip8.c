#include "chip8.h"

struct chip8 chip8Mem;
uint16_t numInstructions;
uint16_t currentInstruction;
int8_t run;

int8_t readInstructionsFromFile(char* filename)
{
    FILE* fptr = fopen(filename, "r");
    if (fptr == NULL)
    {
        printf("Failed to open input file: %s\n", filename);
        return -1;
    }

    fseek(fptr, sizeof(uint16_t), SEEK_END);
    numInstructions = (ftell(fptr) / 2) - 1;
    fseek(fptr, 0, SEEK_SET);

    if(numInstructions <= (PROGRAM_SIZE_BYTES / 2))
    {
        uint16_t instruction;
        uint16_t instructionIndex = 0;
        while (fread(&instruction, sizeof(instruction), 1, fptr) == 1)
        {
            // swap for endianness
            uint16_t swapped = (instruction >> 8) | (instruction << 8);
            chip8Mem.program[instructionIndex] = swapped;
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

void clearDisplay()
{
    memset(chip8Mem.display, 0x00, DISPLAY_SIZE_BYTES);
}

void printDisplayBits(uint8_t rows, uint8_t cols)
{
    printf("----------------\n");
    for(uint8_t row = 0; row < rows; row++)
    {
        for(int i = (cols - 1); i >= 0; i--)
        {
            printf("%d ", (chip8Mem.display[row] >> i) & 1);
        }
        printf("\n");
    }
    printf("---------------\n");
}

void drawSprite(uint8_t xpos, uint8_t ypos, uint8_t height)
{
    printf("sprite v%x v%x %d\n", xpos, ypos, height);
    // TODO implement
    currentInstruction+=2;
}

void parseOpcode(uint16_t instruction)
{
    switch(instruction)
    {
    case 0x00e0: printf("%X clear\n", instruction); clearDisplay(); break;
    case 0x00ee: returnFromSubroutine(); break;
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
    case 0x01: jump(address); break;
    case 0x02: callSubroutine(address); break;
    case 0x03: ifNumEqualThenSkip(registerX, twoDigitNumber); break;
    case 0x04: ifNumNotEqualThenSkip(registerX, twoDigitNumber); break;
    case 0x05: ifVyEqualThenSkip(registerX, registerY); break;
    case 0x06: setVx(registerX, twoDigitNumber); break;
    case 0x07: addNumToVx(registerX, twoDigitNumber); break;
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
        doVxVyOperation(registerX, registerY, operand);
        break;
    }
    case 0x09: ifVyNotEqualThenSkip(registerX, registerY); break;
    case 0x0a: setAddressRegister(address); break;
    case 0x0b: jump0(address); break;
    case 0x0c: setVxRandom(registerX, twoDigitNumber; break;
    case 0x0d: drawSprite(registerX, registerY, oneDigitNumber); break;
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
    default: printf("Invalid opcode\n"); currentInstruction++; break;
    }
}

uint16_t getMemOffset(uint8_t* address)
{
    uint16_t diff = address - (uint8_t*) &chip8Mem;
    return diff;
}

void printDisassembly()
{
    run = 0;
    for (uint16_t i = 0; i < numInstructions; i++)
    {
        uint16_t instructionAddress = getMemOffset((uint8_t*) &chip8Mem.program + (sizeof(i) * i));
        printf("%X %X ", chip8Mem.program[i], instructionAddress);
        parseOpcode(chip8Mem.program[i]);
    }
}

void jump(uint16_t address)
{
    printf("jump %X\n", address);

    printf("Current Instruction: %X\n", currentInstruction);
    printf("Jumping to %X\n", address);
    currentInstruction = address;
}

void setAddressRegister(uint16_t address)
{
    printf("i := %X\n", address);
    currentInstruction+=2;

}

void setVx(uint8_t registerNum, uint8_t valueToSet)
{
    printf("v%x := %X\n", registerNum, valueToSet);
    chip8Mem.genPurposeRegisters[registerNum] = valueToSet;
    currentInstruction += 2;
}

void ifNumEqualThenSkip(uint8_t vx, uint8_t numToCompare)
{
    printf("if v%x == %X then skip next\n", vx, numToCompare);
    currentInstruction+=2;
    if (chip8Mem.genPurposeRegisters[vx] == numToCompare)
    {
        currentInstruction+=2;
    }
}

void ifNumNotEqualThenSkip(uint8_t vx, uint8_t numToCompare)
{
    printf("if v%x != %X then skip next\n", vx, numToCompare);
    currentInstruction+=2;
    if (chip8Mem.genPurposeRegisters[vx] != numToCompare)
    {
        currentInstruction+=2;
    }
}

void runProgram()
{
    run = 1;

    currentInstruction = 0x200;
    int count = 0;
    // TODO switch to a different flag in loop - currently implementing opcode
    // functions one at a time from an example .ch8 file
    while (count < 17)
    {
        printf("--------------------------------------------\n");
        uint16_t opcodeAtInstruction = chip8Mem.program[(currentInstruction - 0x200) / 2];
        printf("current instruction: %X\n", currentInstruction);
        printf("Opcode At instruction: %X\n", opcodeAtInstruction);
        parseOpcode(opcodeAtInstruction);
        printf("next instruction: %X\n", currentInstruction);
        count++;
    }

}

void returnFromSubroutine()
{
    printf("%X return\n", instruction);

    // TODO implement
    printf("**************** TODO implement\n");
    currentInstruction+=2; // will be from stack
}


void callSubroutine(uint16_t address)
{
    printf("call subroutine at %X\n", address);

    // TODO implement
    printf("**************** TODO implement\n");
    currentInstruction+=2; // will be based on the address of subroutine
}

void ifVyEqualThenSkip(uint8_t vx, uint8_t vy)
{
    printf("if v%x == v%x then skip next\n", vx, vy);
    currentInstruction+=2;
    if (chip8Mem.genPurposeRegisters[vx] == chip8Mem.genPurposeRegisters[vy])
    {
        currentInstruction+=2;
    }
}

void ifVyNotEqualThenSkip(uint8_t vx, uint8_t vy)
{
    printf("if v%x != v%x then skip next\n", vx, vy);
    currentInstruction+=2;
    if (chip8Mem.genPurposeRegisters[vx] != chip8Mem.genPurposeRegisters[vy])
    {
        currentInstruction+=2;
    }
}

void addNumToVx(uint8_t vx, uint8_t num)
{
    printf("v%x += %X\n", vx, num);
    chip8Mem.genPurposeRegisters[vx] += num;
    currentInstruction+=2;
}

void doVxVyOperation(uint8_t vx, uint8_t vy, char *operand)
{
    // TODO implement
    printf("v%x %s v%x\n", vx, operand, vy);

    printf("**************** TODO implement\n");
    currentInstruction+=2;
}

void jump0(uint16_t address)
{
    // TODO implement
    printf("jump0 %X\n", address);
    printf("**************** TODO implement\n");
    currentInstruction+=2; // will be the jump'd addr
}

void setVxRandom(uint8_t vx, uint8_t num)
{
    // TODO implement
    printf("v%x := random %X\n", registerX, twoDigitNumber);
    currentInstruction+=2;
}
