#ifndef CHIP8_H
#define CHIP8_H

class Chip8 {
public:
    unsigned short opcode;
    unsigned char memory[4096];
    unsigned char V[16];
    unsigned short I;
    unsigned short pc;
    unsigned char gfx[2048];
    unsigned char delay_timer;
    unsigned char sound_timer;
    unsigned short stack[16];
    unsigned short sp;
    unsigned char key[16];

    Chip8();
    void loadProgram(const char* path);
    bool emulateCycle();
    void printScreen();
};

#endif