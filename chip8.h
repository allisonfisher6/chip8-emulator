#ifndef CHIP8_H
#define CHIP8_H


#define MAX_MEM_BYTES = 4096 // max memory of 4K systems (bytes)
#define NUM_GEN_PURPOSE_REGISTERS = 16 // CHIP-8 has 16 1-byte registers V0-VF
#define STACK_SIZE_BYTES = 48 // original version allocated 48  bytes #TODO allow more?

struct chip8{
    uint8_t genPurposeRegisters[16];
    uint16_t addressRegister; // this is only 12 bits
    uint8_t stack[48]; // original rca 1802 allocated 48 bytes
    uint8_t display[32];
};


#endif // CHIP8_H
