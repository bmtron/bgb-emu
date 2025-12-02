#ifndef MMAP_H
#define MMAP_H
#define ROM_SIZE 0x4000
#define VRAM_SIZE 0x2000
#define EXRAM_SIZE 0x2000
#define WRAM_SIZE 0x1000
#define OAM_SIZE 160
#define NONUSE_SIZE 96  // probably don't need this
#define IOREG_SIZE 128
#define HRAM_SIZE 127

#define CART_HEADER_START 0x0100
#define CART_HEADER_END 0x14F

typedef struct {
        char rom00[ROM_SIZE];    // 0000 - 3FFF
        char rom01[ROM_SIZE];    // 4000 - 7FFF
        char vram[VRAM_SIZE];    // 8000 - 9FFF
        char exram[EXRAM_SIZE];  // A000 - BFFF
        char wram1[WRAM_SIZE];   // C000 - CFFF
        char wram2[WRAM_SIZE];   // D000 - DFFF
        // char echoram[0x2000]; // E000 - FDFF PROHIBITED, maps to wram, not
        // separate
        char oam[OAM_SIZE];        // FE00 - FE9F
        char nonuse[NONUSE_SIZE];  // FEA0 - FEFF NOT USED
        char ioreg[IOREG_SIZE];    // FF00 - FF7F
        char hram[HRAM_SIZE];      // FF80 - FFFE
        char ie;                   // FFFF - FFFF
} MemMap;
#endif
