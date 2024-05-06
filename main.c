#include <stdio.h>
#include <stdint.h>

int main(int argc, char* argv[])
{
    // printHexFileContents(argv[1]);
    parseInstructionsFromFile(argv[1]);

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
    printf("%X ", instruction);
    uint8_t firstHexDigit = instruction >> 12;
        switch(firstHexDigit)
        {
        case 0x01:
        {
            printf("jump %X\n",instruction & 0x0fff);
            break;
        }
        case 0x02:
        {
            printf("call subroutine at %X\n", instruction & 0x0fff);
            break;
        }
        case 0x03:
        {
            printf("if v%x != %X then\n", (instruction & 0x0f00) >> 8, instruction & 0x00ff);
            break;
        }
        case 0x04:
        {
            printf("if v%x == %X then\n", (instruction & 0x0f00) >> 8, instruction & 0x00ff);
            break;
        }
        case 0x05:
        {
            printf("if v%x != v%x then\n", (instruction & 0x0f00) >> 8, (instruction & 0x00f0) >> 4);
            break;
        }
        case 0x06:
        {
            printf("v%x := %X\n", (instruction & 0x0f00) >> 8, instruction & 0x00ff);
            break;
        }
        case 0x07:
        {
            printf("v%x += %X\n", (instruction & 0x0f00) >> 8, instruction & 0x00ff);
            break;
        }
        case 0x08:
        {
            char* operand = "   ";
            switch(instruction & 0x000f)
            {
            case 0x0000: operand = ":="; break;
            case 0x0001: operand = "&="; break;
            case 0x0003: operand = "^="; break;
            case 0x0004: operand = "+="; break;
            case 0x0005: operand = "-="; break;
            case 0x0006: operand = ">>="; break;
            case 0x0007: operand = "=-"; break;
            case 0x000e: operand = "<<="; break;
            default: printf("Invalid opcode %X\n", instruction);
            }

            printf("v%x %s v%x\n", (instruction & 0x0f00) >> 8, operand, (instruction & 0x00f0) >> 4);
            break;
        }
        case 0x09:
        {
            printf("if v%x == v%x then\n", (instruction & 0x0f00) >> 8, (instruction & 0x00f0) >> 4);
            break;
        }
        case 0x0a:
        {
            printf("i := %X\n", instruction & 0x0fff);
            break;
        }
        case 0x0b:
        {
            printf("jump0 %X\n", instruction & 0x0fff);
            break;
        }
        case 0x0c:
        {
            printf("v%x := random %X\n", (instruction & 0x0f00) >> 8, instruction & 0x00ff);
            break;
        }
        case 0x0d:
        {
            printf("sprite v%x v%x %X\n", (instruction & 0x0f00) >> 8, (instruction & 0x00f0) >> 4, instruction & 0x000f);
            break;
        }
        case 0x0e:
        {
            if (instruction & 0x00ff == 0x009e)
            {
                printf("if v%x -key then\n", (instruction & 0x0f00) >> 8);
            }
            else if (instruction & 0x00ff == 0x00A1)
            {
                printf("if v%x key then\n", (instruction & 0x0f00) >> 8);
            }
            else
            {
                printf("Invalid opcode\n");
            }
            break;
        }
        case 0x0f:
        {
            uint8_t registerX = instruction & 0x0f00;
            printf("Register x = %X\n", registerX);
            switch(instruction & 0x00ff)
            {
            case 0x0007: printf("vx := delay\n"); break;
            case 0x000a: printf("vx := key\n"); break;
            case 0x0015: printf("delay := vx\n"); break;
            case 0x0018: printf("buzzer := vx\n"); break;
            case 0x001e: printf("i += vx\n"); break;
            case 0x0029: printf("i := hex vx\n"); break;
            case 0x0033: printf("bcd vx"); break;
            case 0x0055: printf("save vx"); break;
            case 0x0065: printf("load vx"); break;
            default: printf("Invalid opcode %X\n", instruction);
            }
            break;
        }
        default: printf("Invalid opcode\n");
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

    uint16_t instruction;
    while (fread(&instruction, sizeof(instruction), 1, fptr) == 1)
    {
        parseOpcode(instruction);
    }
    printf("\n");

    fclose(fptr);
}

/**
 * @brief Helper function to print the contents of the input ROM file in chunks
 *        of 4 hex digits (e.g. "4E12 ACEA EAAA AEAA A0E0").
 * @param filename Path to the input ROM file.
 */
void printHexFileContents(char* filename)
{
    FILE* fptr = fopen(filename, "r");
    if (fptr == NULL)
    {
        printf("Failed to open input file.\n");
        return -1;
    }

    uint16_t instruction;
    while (fread(&instruction, sizeof(instruction), 1, fptr) == 1)
    {
        printf("%X ", instruction);
    }
    printf("\n");

    fclose(fptr);
}
