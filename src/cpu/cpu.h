#ifndef CPU_H
#define CPU_H
#include <stdint.h>

#include "../memory/memorymap.h"

typedef struct {
        unsigned char z;
        unsigned char n;
        unsigned char h;
        unsigned char c;
} Flags;

typedef struct {
        unsigned char A;
        unsigned char B;
        unsigned char C;
        unsigned char D;
        unsigned char E;
        unsigned char H;
        unsigned char L;
} EightBitRegisters;

typedef struct {
        unsigned short BC;
        unsigned short DE;
        unsigned short HL;
} SixteenBitRegisters;

typedef struct {
        EightBitRegisters ebregisters;
        SixteenBitRegisters sbregisters;
        Flags flags;
        uint64_t cycles;
} Cpu;
// carry rules:
// if 8bit addition higher than 0xFF
// if 16bit addition higher than 0xFFFF

#define EIGHT_BIT_ADD_MAX 0xFF
#define SIXTEEN_BIT_ADD_MAX 0xFFF
#define HALF_CARRY_BITMASK 0x0F
#define CARRY_BITMASK 0xFF
#define NIBBLE_CARRY_MAX 0x0F
#define LS_12_BIT_BITMASK 0x0FFF

uint8_t mem_read(int location, MemMap* mmap);

void adc_a_r8(Cpu* c, uint8_t r8);
void adc_a_hl(Cpu* c, MemMap* mmap);
void adc_a_n8(Cpu* c, uint8_t n8);

void add_a_r8(Cpu* c, uint8_t r8);
void add_a_hl(Cpu* c, MemMap* mmap);
void add_a_n8(Cpu* c, uint8_t n8);

void add_hl_r16(Cpu* c, uint16_t r16);

#endif
