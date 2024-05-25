#include "chip8.h"

struct chip8 chip8Mem =  {
    .fontData = {0xF0, 0x90, 0x90, 0x90, 0xF0,  //0
                 0x20, 0x60, 0x20, 0x20, 0x70,  //1
                 0xF0, 0x10, 0xF0, 0x80, 0xF0,  //2
                 0xF0, 0x10, 0xF0, 0x10, 0xF0,  //3
                 0x90, 0x90, 0xF0, 0x10, 0x10,  //4
                 0xF0, 0x80, 0xF0, 0x10, 0xF0,  //5
                 0xF0, 0x80, 0xF0, 0x90, 0xF0,  //6
                 0xF0, 0x10, 0x20, 0x40, 0x40,  //7
                 0xF0, 0x90, 0xF0, 0x90, 0xF0,  //8
                 0xF0, 0x90, 0xF0, 0x10, 0xF0,  //9
                 0xF0, 0x90, 0xF0, 0x90, 0x90,  //A
                 0xE0, 0x90, 0xE0, 0x90, 0xE0,  //B
                 0xF0, 0x80, 0x80, 0x80, 0xF0,  //C
                 0xE0, 0x90, 0x90, 0x90, 0xE0,  //D
                 0xF0, 0x80, 0xF0, 0x80, 0xF0,  //E
                 0xF0, 0x80, 0xF0, 0x80, 0x80}  //F
};
uint16_t numInstructions;
uint16_t currentInstruction;
uint8_t pauseExecutionForKeyInput = 0;
uint8_t registerToStoreKeyPress;
SDL_Window *window = NULL;
SDL_Renderer *renderer;
SDL_Event event;

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

void testDrawing()
{
    chip8Mem.display[0] = 0b10101111;
    chip8Mem.display[255] = 0xF0;
    drawSprite(45, 31, 5);
}

void drawSprite(uint8_t xpos, uint8_t ypos, uint8_t height)
{
    printf("sprite v%x v%x %d\n", xpos, ypos, height);
    /*
     * 	Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and
     * a height of N pixels. Each row of 8 pixels is read as bit-coded starting
     * from memory location I; I value does not change after the execution of
     * this instruction. As described above, VF is set to 1 if any screen
     * pixels are flipped from set to unset when the sprite is drawn, and
     * to 0 if that does not happen
     */

    uint16_t spriteStartIndex = (xpos / 8) + (ypos * 8);



    // TODO implement
    currentInstruction+=2;
}

void parseOpcode(uint16_t instruction)
{
    switch(instruction)
    {
    case 0x00e0: printf("%X clear\n", instruction); clearDisplay(); break;
    case 0x00ee: printf("%X return\n", instruction); returnFromSubroutine(); break;
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
    case 0x0b: jumpToAddrPlusV0(address); break;
    case 0x0c: setVxRandom(registerX, twoDigitNumber); break;
    case 0x0d: drawSprite(registerX, registerY, oneDigitNumber); break;
    case 0x0e:
    {
        switch(instruction & 0x00ff)
        {
        case 0x009e: ifKeyIsPressedThenSkip(registerX); break;
        case 0x00a1: ifKeyNotPressedThenSkip(registerX); break;
        default: printf("Invalid opcode\n");
        }
        break;
    }
    case 0x0f:
    {
        switch(instruction & 0x00ff)
        {
        case 0x0007: setVxToDelay(registerX); break;
        case 0x000a: waitForKey(registerX); break;
        case 0x0015: setDelayTimer(registerX); break;
        case 0x0018: setSoundTimer(registerX); break;
        case 0x001e: addVxToI(registerX); break;
        case 0x0029: setAddrToSprite(registerX); break;
        case 0x0033: bcdVx(registerX); break;
        case 0x0055: storeVx(registerX); break;
        case 0x0065: loadVx( registerX); break;
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
    chip8Mem.addressRegister = address;
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
    // create graphics window
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(64 * 10, 32 * 10, 0, &window, &renderer);
    SDL_RenderSetScale(renderer, 10, 10);

    // set all display data bits to 0 and render display
    clearDisplay();
    renderDisplayData();
    SDL_RenderPresent(renderer);

    // TODO try using a timer to control speed at which instructions are run
    uint16_t delayMs = 1000;
    SDL_TimerID instructionTimer = SDL_AddTimer(delayMs, gameLoopTimerCallback, NULL);
    currentInstruction = 0x200;
    int count = 0;

    while(1)
    {
        SDL_PollEvent(&event);

        if(event.type == SDL_QUIT)
            break;
        else if(event.type == SDL_KEYDOWN)
        {
            SDL_KeyboardEvent *keyEvent = &event.key;
            int8_t keyPressed = processKeyPress(keyEvent->keysym.sym);

            if(keyPressed != -1)
            {
                if(!pauseExecutionForKeyInput)
                {
                    // testing rendering display, remove.
                    chip8Mem.display[count] = 0b00001111;
                    renderDisplayData();
                    count+=1;
                    SDL_RenderPresent(renderer);
                }
                else
                {
                    // valid keypress registered while waiting for key
                    chip8Mem.genPurposeRegisters[registerToStoreKeyPress] = keyPressed;

                    // resume processing
                    pauseExecutionForKeyInput = 0;

                    printf("Resuming processing after keypress %x\n", chip8Mem.genPurposeRegisters[registerToStoreKeyPress]);
                }
            }
        }
        else if(event.type == SDL_USEREVENT)
        {
            // if execution is paused awaiting keypress, do not start next instruction
            if(!pauseExecutionForKeyInput)
            {
                // testing rendering display, remove.
                chip8Mem.display[count] = 0b00001111;
                renderDisplayData();
                count+=1;
                SDL_RenderPresent(renderer);
            }
            else
            {
                printf("waiting for keypress...\n");
            }
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return;

    // TODO switch to a different flag in loop - currently implementing opcode
    // functions one at a time from an example .ch8 file thus the count


    while (count < 60)
    {
        printf("--------------------------------------------\n");
        uint16_t opcodeAtInstruction = chip8Mem.program[(currentInstruction - 0x200) / 2];
        printf("current instruction: %X\n", currentInstruction);
        printf("Opcode At instruction: %X\n", opcodeAtInstruction);
        parseOpcode(opcodeAtInstruction);
        printf("next instruction: %X\n", currentInstruction);
        count++;
    }

    // printf("%d  %d\n", sizeof(uint16_t));
}

void returnFromSubroutine()
{
    // TODO implement
    printf("**************** TODO implement\n");
    currentInstruction = chip8Mem.stack[0] += 2; // TODO set based on top of stack and not 0
}

void callSubroutine(uint16_t address)
{
    printf("call subroutine at %X\n", address);

    // TODO implement
    printf("**************** TODO implement\n");

    // todo should adding to the stack instead of always index 0
    chip8Mem.stack[0] = currentInstruction;
    currentInstruction = address;
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

void jumpToAddrPlusV0(uint16_t address)
{
    printf("jump0 %X\n", address);
    currentInstruction = address + chip8Mem.genPurposeRegisters[0];
}

void setVxRandom(uint8_t vx, uint8_t num)
{
    printf("v%x := random %X\n", vx, num);

    // TODO get random number that has more uniformity
    chip8Mem.genPurposeRegisters[vx] = (rand() % 255) & num;
    currentInstruction+=2;
}

void bcdVx(uint8_t vx)
{
    printf("bcd v%x\n", vx);

    // TODO implement
    currentInstruction+=2;
}

void storeVx(uint8_t vx)
{
    printf("save v%x\n", vx);

    // TODO implement
    currentInstruction+=2;
}

void loadVx(uint8_t vx)
{
    printf("load v%x\n", vx);

    // TODO implement
    currentInstruction+=2;
}

void setVxToDelay(uint8_t vx)
{
    // Opcode FX07: Sets VX to the value of the delay timer
    chip8Mem.genPurposeRegisters[vx] = chip8Mem.delayTimer;
    printf("v%x := delay\n", vx);
    currentInstruction+=2;
}

void waitForKey(uint8_t vx)
{
    /*
     * Opcode FX0A: Wait for a keypress and store the result in register vx.
     * Halts all program execution until a key on the keypad is pressed.
     */

    printf("v%x := key\n", vx);

    // stop the game loop until a *valid* key is pressed.
    pauseExecutionForKeyInput = 1;
    registerToStoreKeyPress = vx;

    // the value corresponding to the key pressed will be saved in vx in
    // runProgram()

    currentInstruction+=2;
}

void setDelayTimer(uint8_t vx)
{
    // Opcode FX15: Sets the delay timer to VX.
    printf("delay := v%x\n", vx);
    chip8Mem.delayTimer = chip8Mem.genPurposeRegisters[vx];
    currentInstruction+=2;
}

void setSoundTimer(uint8_t vx)
{
    // Opcode FX18: Sets the sound timer to VX.
    printf("buzzer := v%x\n", vx);
    chip8Mem.soundTimer = chip8Mem.genPurposeRegisters[vx];
    currentInstruction+=2;
}

void addVxToI(uint8_t vx)
{
    // Opcode FX1E: Adds VX to I. VF is not affected.
    printf("i += v%x\n", vx);
    chip8Mem.addressRegister += chip8Mem.genPurposeRegisters[vx];
    currentInstruction+=2;
}

void setAddrToSprite(uint8_t vx)
{
    /*
     * Opcode FX29: Set I to the mem addr of the sprite data corresponding
     * to the hex digit stored in register VX.
     */
    printf("i := hex v%x\n", vx);
    uint8_t fontDigit = chip8Mem.genPurposeRegisters[vx] & 0x0F;
    chip8Mem.addressRegister = getMemOffset(&chip8Mem.fontData[5 * fontDigit]);
    currentInstruction+=2;
}


void ifKeyNotPressedThenSkip(uint8_t vx)
{
    /* Opcode EXA1: Skip the next instruction if the key corresponding to
     * the hex value stored in register VX is not pressed. The interpreter
     * is not halted.
     */

    printf("if v%x key not pressed then skip\n", vx);

    uint8_t key = chip8Mem.genPurposeRegisters[vx];
    if(!chip8Mem.keyStates[key])
    {
        // key is pressed, skip next instruction
        currentInstruction += 2;
    }

    currentInstruction+=2;
}

void ifKeyIsPressedThenSkip(uint8_t vx)
{
    /* Opcode EX9E: Skip the next instruction if the key corresponding to
     * the hex value stored in register VX is pressed. The interpreter is
     * not halted.
     */
    printf("if v%x key pressed then skip\n", vx);

    uint8_t key = chip8Mem.genPurposeRegisters[vx];
    if(chip8Mem.keyStates[key])
    {
        // key is pressed, skip next instruction
        currentInstruction += 2;
    }

    currentInstruction+=2;
}

void renderDisplayData()
{
    for(uint8_t xpos = 0; xpos < DISPLAY_PIXEL_WIDTH; xpos++)
    {
        for(uint8_t ypos = 0; ypos < DISPLAY_PIXEL_WIDTH; ypos++)
        {
            setPixelColor(getDisplayBitValue(xpos, ypos));
            SDL_RenderDrawPoint(renderer, xpos, ypos);
        }
    }

}

void setPixelColor(uint8_t color)
{
    if(color)
    {
        // pixels will be drawn in white
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    }
    else
    {
        // pixels will be drawn in black
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    }
}

uint8_t getDisplayBitValue(uint8_t x, uint8_t y)
{
    uint16_t indexIntoData = (x / 8) + (y * 8);
    uint8_t shift = 7 - (x % 8);
    return chip8Mem.display[indexIntoData] >> shift;
}

uint16_t gameLoopTimerCallback(uint16_t interval, void *param)
{
    // pushes an SDL_USEREVENT to the queue and causes the callback to be
    // called again at the same interval
    // https://stackoverflow.com/questions/27414548/sdl-timers-and-waitevent

    SDL_Event loopTimerEvent;
    loopTimerEvent.type = SDL_USEREVENT;
    SDL_PushEvent(&loopTimerEvent);
    return(interval);
}

int8_t processKeyPress(SDL_Keycode keycode)
{
    // 1 2 3 4 q w e r a s d f z x c v on keybaord map to:
    // 1 2 3 c 4 5 6 d 7 9 9 e a 0 b f chip-8 keys

    uint8_t convertedKey;
    switch(keycode)
    {
        case '1': convertedKey = 0x01; break;
        case '2': convertedKey = 0x02; break;
        case '3': convertedKey = 0x03; break;
        case '4': convertedKey = 0x0c; break;
        case 'q': convertedKey = 0x04; break;
        case 'w': convertedKey = 0x05; break;
        case 'e': convertedKey = 0x06; break;
        case 'r': convertedKey = 0x0d; break;
        case 'a': convertedKey = 0x07; break;
        case 's': convertedKey = 0x08; break;
        case 'd': convertedKey = 0x09; break;
        case 'f': convertedKey = 0x0e; break;
        case 'z': convertedKey = 0x0a; break;
        case 'x': convertedKey = 0x00; break;
        case 'c': convertedKey = 0x0b; break;
        case 'v': convertedKey = 0x0f; break;
        default: printf("Invalid key press %c\n", keycode); return -1;
    }

    chip8Mem.keyStates[convertedKey] = 0x01;
    printf("Converted key %c to chip-8 key %x\n", keycode, convertedKey);
    return convertedKey;
}
