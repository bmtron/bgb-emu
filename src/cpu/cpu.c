#include "cpu.h"

uint8_t mem_read(int location, MemMap* mmap) {
        char* region;
        int region_start = 0;

        if (location <= 0x3FFF) {  // ROM bank 00
                region = mmap->rom00;
                region_start = 0x0000;
        } else if (location <= 0x7FFF) {  // ROM bank 01
                region = mmap->rom01;
                region_start = 0x4000;
        } else if (location <= 0x9FFF) {
                region = mmap->vram;
                region_start = 0x8000;
        } else if (location <= 0xBFFF) {
                region = mmap->exram;
                region_start = 0xA000;
        } else if (location <= 0xCFFF) {
                region = mmap->wram1;
                region_start = 0xC000;
        } else if (location <= 0xDFFF) {
                region = mmap->wram2;
                region_start = 0xD000;
        } else if (location <= 0xFDFF) {
                return -1;
        } else if (location <= 0xFE9F) {
                region = mmap->oam;
                region_start = 0xFE00;
        } else if (location <= 0xFEFF) {
                return -1;
        } else if (location <= 0xFF7F) {
                region = mmap->ioreg;
                region_start = 0xFF00;
        } else if (location <= 0xFFFE) {
                region = mmap->hram;
                region_start = 0xFF80;
        } else {
                return mmap->ie;
        }

        return region[location - region_start];
}

void adc_a_r8(Cpu* c, uint8_t r8) {
        // add value r8 + carry flag to A plus carry
        // 1 cycle
        unsigned int temp_result = c->ebregisters.A + r8 + c->flags.c;
        unsigned char a_mask = c->ebregisters.A & HALF_CARRY_BITMASK;
        unsigned char r8_mask = r8 & HALF_CARRY_BITMASK;
        unsigned char cast_result = temp_result;
        uint8_t input_carry = c->flags.c;

        c->ebregisters.A = cast_result;
        c->flags.c = 0;
        c->flags.z = 0;
        c->flags.h = 0;
        c->flags.n = 0;

        if (cast_result == 0) {
                c->flags.z = 1;
        }
        if (temp_result > EIGHT_BIT_ADD_MAX) {
                c->flags.c = 1;
        }

        if (a_mask + r8_mask + input_carry > NIBBLE_CARRY_MAX) {
                c->flags.h = 1;
        }
        ++c->cycles;
}

void adc_a_hl(Cpu* c, MemMap* mmap) {
        // add value from [hl] to A plus carry
        // 2 cycles
        unsigned char mem_value = mem_read(c->sbregisters.HL, mmap);
        adc_a_r8(c, mem_value);
        ++c->cycles;
}

void adc_a_n8(Cpu* c, uint8_t n8) {
        // add value n8 to a plus carry
        // 2 cycles
        adc_a_r8(c, n8);
        ++c->cycles;
}

void add_a_r8(Cpu* c, uint8_t r8) {
        // add value r8 to A
        // 1 cycle
        unsigned int temp_result = c->ebregisters.A + r8;
        unsigned char a_mask = c->ebregisters.A & HALF_CARRY_BITMASK;
        unsigned char r8_mask = r8 & HALF_CARRY_BITMASK;
        unsigned char cast_result = temp_result;

        c->ebregisters.A = cast_result;
        c->flags.c = 0;
        c->flags.z = 0;
        c->flags.h = 0;
        c->flags.n = 0;

        if (cast_result == 0) {
                c->flags.z = 1;
        }
        if (temp_result > EIGHT_BIT_ADD_MAX) {
                c->flags.c = 1;
        }

        if (a_mask + r8_mask > NIBBLE_CARRY_MAX) {
                c->flags.h = 1;
        }
        ++c->cycles;
}

void add_a_hl(Cpu* c, MemMap* mmap) {
        uint8_t mem_value = mem_read(c->sbregisters.HL, mmap);
        add_a_r8(c, mem_value);
        ++c->cycles;
}

void add_a_n8(Cpu* c, uint8_t n8) {
        add_a_r8(c, n8);
        ++c->cycles;
}

void add_hl_r16(Cpu* c, uint16_t r16) {
        int temp_result = c->sbregisters.HL + r16;
        uint16_t hl_masked = c->sbregisters.HL & LS_12_BIT_BITMASK;
        uint16_t r16_masked = r16 & LS_12_BIT_BITMASK;
        uint16_t cast_result = temp_result;
        c->sbregisters.HL = cast_result;
        c->flags.h = 0;
        c->flags.c = 0;
        c->flags.n = 0;

        if (temp_result > SIXTEEN_BIT_ADD_MAX) {
                c->flags.c = 1;
        }
        if (hl_masked + r16_masked > LS_12_BIT_BITMASK) {
                c->flags.h = 1;
        }

        c->cycles += 2;
}
