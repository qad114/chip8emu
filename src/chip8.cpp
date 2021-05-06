#include <iomanip>
#include <iostream>
#include <stdio.h>
#include "chip8.h"

unsigned char xorChars(unsigned char a, unsigned char b) {
    return (unsigned char) (a && !b) || (!a && b);
}

void Chip8::printScreen() {
    for (int row = 0; row < 2048; row += 64) {
        for (int col = row; col < row + 64; ++col) {
            //std::cout << (((int)gfx[col] == 1) ? "*" : " ");
            std::cout << (((int)gfx[col]) ? "█" : " ");
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    /*for (int i = 0; i < 2048; ++i) {
        std::cout << (int)gfx[i];
    }*/
}

// Initialize registers and memory
Chip8::Chip8() {
    pc = 0x200;
    opcode = 0;
    I = 0;
    sp = 0;

    for (int i = 0; i < 2048; ++i) gfx[i] = 0;
    for (int i = 0; i < 15; ++i) {V[i] = 0; key[i] = 0; stack[i] = 0;}

    unsigned char fontset[80] = { 
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
    for (int i = 0; i < 80; ++i) {
        memory[i] = fontset[i];
    }
}

// Load program into memory
void Chip8::loadProgram(const char* path) {
    FILE* file = fopen(path, "rb");
    // std::ifstream fin(path);
    unsigned char c;
    for (int i = 0; i < 3584;/*(c = fgetc(file)) != EOF;*/ ++i) {
        c = fgetc(file);
        memory[0x200 + i] = c;
    }
}

// FDE cycle
bool Chip8::emulateCycle() {
    //printScreen();
    /*std::cout
            << "I: " << (int)I << '\n'
            << "V0: " << (int)V[0] << '\n'
            << "V1: " << (int)V[1] << '\n';*/
    std::cout << std::hex;
    opcode = memory[pc] << 8 | memory[pc + 1];
    //std::cout << opcode << std::endl;

    // Handle opcode
    switch (opcode & 0xF000) {
        case 0x0000: // could be 0x00E0 or 0x00EE
            switch (opcode & 0x000F) {
                case 0x0000: // 00E0: clear screen
                    for (int i = 0; i < 2048; ++i) gfx[i] = 0;
                    pc += 2;
                    break;

                case 0x000E: // 00EE: return from subroutine
                    pc = stack[--sp] + 2;
                    break;

                default:
                    std::cout << "unimpl";
                    return false;
            }
            break;

        case 0x1000: // 1NNN: goto NNN
            pc = opcode & 0x0FFF;
            break;

        case 0x2000: // 2NNN: call subroutine at NNN
            stack[sp++] = pc;
            pc = opcode & 0x0FFF;
            break; 

        case 0x3000: // 3XNN: skip next if VX == NN
            if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
                pc += 4;
            } else {
                pc += 2;
            }
            break;

        case 0x4000: // 4XNN: skip next if VX != NN
            if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
                pc += 4;
            } else {
                pc += 2;
            }
            break;

        case 0x5000: // 5XY0: skip next if VX == VY
            if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) {
                pc += 4;
            } else {
                pc += 2;
            }
            break;

        case 0x6000: // 6XNN: set VX to NN
            V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
            pc += 2;
            break;

        case 0x7000: // 7XNN: add NN to VX
            V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
            pc += 2;
            break;

        case 0x8000: // could be 0x8XY0 through 0x8XYE
            switch (opcode & 0x000F) {
                case 0x0000: // 8XY0: set VX = VY
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;

                case 0x0001: // 8XY1: set VX = (VX | VY)
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] | V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;

                case 0x0002: // 8XY2: set VX = (VX & VY)
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] & V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;

                case 0x0003: // 8XY3: set VX = (VX ^ VY)
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] ^ V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;

                case 0x0004: // 8XY4: add VY to VX
                {
                    unsigned char oldVX = V[(opcode & 0x0F00) >> 8];
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] + V[(opcode & 0x00F0) >> 4];
                    V[0xF] = (V[(opcode & 0x0F00) >> 8] < oldVX);
                    pc += 2;
                    break;
                }

                case 0x0005: // 8XY5: subtract VY from VX
                {
                    unsigned char oldVX = V[(opcode & 0x0F00) >> 8];
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] - V[(opcode & 0x00F0) >> 4];
                    V[0xF] = (V[(opcode & 0x0F00) >> 8] < oldVX);
                    pc += 2;
                    break;
                }

                case 0x0006: // 8XY6: store LSB of VX in VF and rightshift VX by 1
                    V[0xF] = V[(opcode & 0x0F00) >> 8] & 0b00000001;
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] >> 1;
                    pc += 2;
                    break;

                case 0x0007: // 8XY7: set VX = (VY - VX)
                    V[0xF] = (V[(opcode & 0x00F0) >> 4] <= V[(opcode & 0x0F00) >> 8]);
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;

                case 0x000E: // 8XYE: store MSB of VX in VF and leftshift VX by 1
                    V[0xF] = (V[(opcode & 0x0F00) >> 8] & 0b10000000) >> 7;
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] << 1;
                    pc += 2;
                    break;
            }
            break;

        case 0x9000: // 9XY0: skip next if VX != VY
            if (V[(opcode & 0x0FF0) >> 4] != V[(opcode & 0x0FF0) >> 4]) {
                pc += 4;
            } else {
                pc += 2;
            }
            break;

        case 0xA000: // ANNN: set I to NNN
            I = opcode & 0x0FFF;
            pc += 2;
            break;

        case 0xB000: // BNNN: set pc to V0 + NNN
            pc = V[0] + (opcode & 0x0FFF);
            break;

        case 0xC000: // CXNN: set VX to rand(255) & NN
            V[(opcode & 0x0F00) >> 8] = (rand() % 256) & (opcode & 0x00FF);
            pc += 2;
            break;

        case 0xD000: // DXYN: draw sprite at (VX, VY), width 8, height N
        {
            unsigned short x = V[(opcode & 0x0F00) >> 8];
            unsigned short y = V[(opcode & 0x00F0) >> 4];
            unsigned short height = opcode & 0x000F;
            unsigned short pixel;

            V[0xF] = 0;
            for (int yline = 0; yline < height; yline++)
            {;
                pixel = memory[I + yline];
                for(int xline = 0; xline < 8; xline++)
                {
                    if((pixel & (0x80 >> xline)) != 0)
                    {
                        if(gfx[(x + xline + ((y + yline) * 64))] == 1)
                            V[0xF] = 1;
                        gfx[x + xline + ((y + yline) * 64)] ^= 1;
                    }
                }
            }

            pc += 2;
        }
        break;

        case 0xE000: // could be EX9E or EXA1
            switch (opcode & 0x000F) {
                case 0x000E: // EX9E: skip next instruction if key(VX) is pressed
                    if (key[V[(opcode & 0x0F00) >> 8]]) {
                        pc += 4;
                    } else {
                        pc += 2;
                    }
                    break;

                case 0x0001: // EXA1: skip next instruction if key(VX) is not pressed
                    if (!(key[V[(opcode & 0x0F00) >> 8]])) {
                        pc += 4;
                    } else {
                        pc += 2;
                    }
                    break;
            }
            break;

        case 0xF000: // could be multiple codes
            switch (opcode & 0x00FF) {
                case 0x0007: // FX07: set VX to delay timer
                    V[(opcode & 0x0F00) >> 8] = delay_timer;
                    pc += 2;
                    break;

                //case 0x000A: // FX0A: get keypress

                case 0x0015: // FX15: set delay timer to VX
                    delay_timer = V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;

                case 0x0018: // FX18: set sound timer to VX
                    sound_timer = V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;

                case 0x001E: // FX1E: add VX to I
                    I += V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;

                case 0x0029: // FX29: set I to the sprite address for the character in VX
                {
                    char c = V[(opcode & 0x0F00) >> 8];
                    if (c < 10) {
                        I = c * 5;
                    } else {
                        I = 10 + ((c - 65) * 5);
                    }
                    pc += 2;
                    break;
                }

                case 0x0033: // FX33: store BCD representation of VX
                    memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
                    memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
                    memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
                    pc += 2;
                    break;

                case 0x0055: // FX55: store V0 to VX inclusive in memory after I
                    for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i) {
                        memory[I + i] = V[i];
                    }
                    pc += 2;
                    break;

                case 0x0065: // FX65: fill V0 to VX with memory values starting at I
                    for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i) {
                        V[i] = memory[I + i];
                    }
                    pc += 2;
                    break;
            }
            break;
        
        default:
            std::cout << "Unimplemented opcode: " << opcode << std::endl;
            return false;
    }

    // Update timers
    if (delay_timer > 0) --delay_timer;
    if (sound_timer > 0) --sound_timer; //TODO: Handle beeping

    return true;
}