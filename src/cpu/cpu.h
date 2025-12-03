#ifndef CPU_H
#define CPU_H
#include <stdint.h>

#include "../memory/memorymap.h"

typedef struct {
        uint8_t z;
        uint8_t n;
        uint8_t h;
        uint8_t c;
} Flags;

typedef struct {
        uint8_t A;
        union {
                uint16_t BC;
                struct {
                        uint8_t C;
                        uint8_t B;
                };
        };
        union {
                uint16_t DE;
                struct {
                        uint8_t E;
                        uint8_t D;
                };
        };
        union {
                uint16_t HL;
                struct {
                        uint8_t L;
                        uint8_t H;
                };
        };
        uint16_t SP;
        uint16_t PC;
} Registers;

typedef struct {
        Registers registers;
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
void mem_write(int location, uint8_t data, MemMap* mmap);

void adc_a_r8(Cpu* c, uint8_t r8);
void adc_a_hl(Cpu* c, MemMap* mmap);
void adc_a_n8(Cpu* c, uint8_t n8);

void add_a_r8(Cpu* c, uint8_t r8);
void add_a_hl(Cpu* c, MemMap* mmap);
void add_a_n8(Cpu* c, uint8_t n8);

void add_hl_r16(Cpu* c, uint16_t r16);
void add_hl_sp(Cpu* c);
void add_sp_e8(Cpu* c, int8_t e8);

void and_a_r8(Cpu* c, int8_t r8);
void and_a_hl(Cpu* c, MemMap* mmap);
void and_a_n8(Cpu* c, int8_t n8);

void bit_u3_r8(Cpu* c, uint8_t r8);
void bit_u3_hl(Cpu* c, MemMap* mmap);

void call_n16(Cpu* c, MemMap* mmap, int16_t n16);
void call_cc_n16(Cpu* c, MemMap* mmap, int16_t n16, uint8_t cc);
void ccf(Cpu* c);

void ret(Cpu* c, MemMap* mmap);

#endif
