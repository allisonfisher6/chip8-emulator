#include "chip8.h"

int main(int argc, char* argv[])
{
    uint8_t exitCode = 0;

    SDL_Window *window = NULL;
    SDL_Renderer *renderer;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(64 * 10, 32 * 10, 0, &window, &renderer);
    SDL_RenderSetScale(renderer, 10, 10);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Event event;
    while(1)
    {
        SDL_PollEvent(&event);

        if(event.type == SDL_QUIT)
            break;
        else if(event.type == SDL_KEYDOWN)
        {
            SDL_KeyboardEvent *keyEvent = &event.key;
            printf("%c\n", keyEvent->keysym.sym);

            // test drawing a pixel
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawPoint(renderer, 5, 5);
            SDL_RenderPresent(renderer);
        }
    }


    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;

    // copy input program into memory
    exitCode = !readInstructionsFromFile(argv[1]);
    if (!exitCode)
    {
        return exitCode;
    }

    // begin with a clear display
    clearDisplay();
    testDrawing();
    printDisplayBits(); // TODO remove when graphics are implemented

    // print disassembly
    printDisassembly();


    // TODO begin processing instructions
    runProgram();

    // TODO find library to handle graphics
    // TODO handle keyboard input
    // TODO timers need to count down at a rate of 60hz

    return 0;
}
