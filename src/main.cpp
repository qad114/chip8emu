#include <iostream>
#include <vector>
#include "chip8.h"
extern "C" {
    #include <SDL2/SDL.h>
    #include <unistd.h>
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <path to ROM>" << std::endl;
        return 1;
    }

    // Init graphics
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow("chip8emu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 64, 32, SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderSetLogicalSize(renderer, 640, 320);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
    
    // Create pixels array
    std::vector<unsigned char> pixels(64 * 32 * 4, 0);

    Chip8 chip8;
    chip8.loadProgram(argv[1]);

    SDL_Event event;
    while (true) {
        if (!chip8.emulateCycle()) {
            std::cout << "Interpreter exited due to error" << std::endl;
            break;
        }

        if (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    return 0;

                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_1: chip8.key[0] = 1; break;
                        case SDLK_2: chip8.key[1] = 1; break;
                        case SDLK_3: chip8.key[2] = 1; break;
                        case SDLK_4: chip8.key[3] = 1; break;
                        case SDLK_5: chip8.key[4] = 1; break;
                        case SDLK_6: chip8.key[5] = 1; break;
                        case SDLK_7: chip8.key[6] = 1; break;
                        case SDLK_8: chip8.key[7] = 1; break;
                        case SDLK_9: chip8.key[8] = 1; break;
                        case SDLK_q: chip8.key[9] = 1; break;
                        case SDLK_w: chip8.key[10] = 1; break;
                        case SDLK_e: chip8.key[11] = 1; break;
                        case SDLK_r: chip8.key[12] = 1; break;
                        case SDLK_t: chip8.key[13] = 1; break;
                        case SDLK_y: chip8.key[14] = 1; break;
                    }
                    break;

                case SDL_KEYUP:
                    switch (event.key.keysym.sym) {
                        case SDLK_1: chip8.key[0] = 0; break;
                        case SDLK_2: chip8.key[1] = 0; break;
                        case SDLK_3: chip8.key[2] = 0; break;
                        case SDLK_4: chip8.key[3] = 0; break;
                        case SDLK_5: chip8.key[4] = 0; break;
                        case SDLK_6: chip8.key[5] = 0; break;
                        case SDLK_7: chip8.key[6] = 0; break;
                        case SDLK_8: chip8.key[7] = 0; break;
                        case SDLK_9: chip8.key[8] = 0; break;
                        case SDLK_q: chip8.key[9] = 0; break;
                        case SDLK_w: chip8.key[10] = 0; break;
                        case SDLK_e: chip8.key[11] = 0; break;
                        case SDLK_r: chip8.key[12] = 0; break;
                        case SDLK_t: chip8.key[13] = 0; break;
                        case SDLK_y: chip8.key[14] = 0; break;
                    }
                    break;
            }
        }

        for (int i = 0; i < pixels.size(); i += 4) {
            pixels[i] = ((chip8.gfx[i/4] == 1) ? 255 : 0);
            pixels[i+1] = ((chip8.gfx[i/4] == 1) ? 255 : 0);
            pixels[i+2] = ((chip8.gfx[i/4] == 1) ? 255 : 0);
            pixels[i+3] = 255;
        }

        SDL_UpdateTexture(texture, NULL, pixels.data(), 64 * 4);

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        usleep(1200);
    }
    return 0;
}