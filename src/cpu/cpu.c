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

void mem_write(int location, uint8_t data, MemMap* mmap) {
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
                // no-op
                return;
        } else if (location <= 0xFE9F) {
                region = mmap->oam;
                region_start = 0xFE00;
        } else if (location <= 0xFEFF) {
                // no-op
                return;
        } else if (location <= 0xFF7F) {
                region = mmap->ioreg;
                region_start = 0xFF00;
        } else if (location <= 0xFFFE) {
                region = mmap->hram;
                region_start = 0xFF80;
        } else {
                // no-op
                return;
        }

        region[location - region_start] = data;
}

void adc_a_r8(Cpu* c, uint8_t r8) {
        // add value r8 + carry flag to A plus carry
        // 1 cycle
        unsigned int temp_result = c->registers.A + r8 + c->flags.c;
        unsigned char a_mask = c->registers.A & HALF_CARRY_BITMASK;
        unsigned char r8_mask = r8 & HALF_CARRY_BITMASK;
        unsigned char cast_result = temp_result;
        uint8_t input_carry = c->flags.c;

        c->registers.A = cast_result;
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
        unsigned char mem_value = mem_read(c->registers.HL, mmap);
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
        unsigned int temp_result = c->registers.A + r8;
        unsigned char a_mask = c->registers.A & HALF_CARRY_BITMASK;
        unsigned char r8_mask = r8 & HALF_CARRY_BITMASK;
        unsigned char cast_result = temp_result;

        c->registers.A = cast_result;
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
        uint8_t mem_value = mem_read(c->registers.HL, mmap);
        add_a_r8(c, mem_value);
        ++c->cycles;
}

void add_a_n8(Cpu* c, uint8_t n8) {
        add_a_r8(c, n8);
        ++c->cycles;
}

void add_hl_r16(Cpu* c, uint16_t r16) {
        int temp_result = c->registers.HL + r16;
        uint16_t hl_masked = c->registers.HL & LS_12_BIT_BITMASK;
        uint16_t r16_masked = r16 & LS_12_BIT_BITMASK;
        uint16_t cast_result = temp_result;
        c->registers.HL = cast_result;
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

void add_hl_sp(Cpu* c) {
        // 2 cycles, incremented in called fn
        add_hl_r16(c, c->registers.SP);
}

void add_sp_e8(Cpu* c, int8_t e8) {
        int temp_result = c->registers.SP + e8;
        int8_t sp_low_masked = c->registers.SP & 0x00FF;
        int8_t sp_low_low_masked = sp_low_masked & 0x0F;
        int8_t e8_low_masked = e8 & 0x0F;

        c->registers.SP = temp_result;
        c->flags.z = 0;
        c->flags.n = 0;
        c->flags.h = 0;
        c->flags.c = 0;
        if (sp_low_low_masked + e8_low_masked > NIBBLE_CARRY_MAX) {
                c->flags.h = 1;
        }
        if (sp_low_masked + e8 > EIGHT_BIT_ADD_MAX) {
                c->flags.c = 1;
        }
        c->cycles += 4;
}

void and_a_r8(Cpu* c, int8_t r8) {
        uint8_t temp_result = c->registers.A & r8;

        c->flags.z = 0;
        c->flags.n = 0;
        c->flags.h = 1;
        c->flags.c = 0;
        if (temp_result == 0) {
                c->flags.z = 1;
        }
        ++c->cycles;
}

void and_a_hl(Cpu* c, MemMap* mmap) {
        uint8_t mem_value = mem_read(c->registers.HL, mmap);
        and_a_r8(c, mem_value);
        ++c->cycles;
}

void and_a_n8(Cpu* c, int8_t n8) {
        add_a_r8(c, n8);
        ++c->cycles;
}

void bit_u3_r8(Cpu* c, uint8_t r8) {
        uint8_t temp_val = r8;
        uint8_t shifted_val = temp_val >> 3;

        c->flags.n = 0;
        c->flags.h = 1;
        c->flags.z = 0;

        if (shifted_val == 0) {
                c->flags.z = 1;
        }
        c->cycles += 2;
}

void bit_u3_hl(Cpu* c, MemMap* mmap) {
        uint8_t mem_value = mem_read(c->registers.HL, mmap);
        bit_u3_r8(c, mem_value);
        ++c->cycles;
}

void call_n16(Cpu* c, MemMap* mmap, int16_t n16) {
        // need to implement a mem_write function
        uint8_t high_byte = (c->registers.PC >> 8) & 0xFF;
        uint8_t low_byte = c->registers.PC & 0x00FF;

        --c->registers.SP;
        mem_write(c->registers.SP, high_byte, mmap);

        --c->registers.SP;
        mem_write(c->registers.SP, low_byte, mmap);

        c->registers.PC = n16;

        c->cycles += 6;
}

void call_cc_n16(Cpu* c, MemMap* mmap, int16_t n16, uint8_t cc) {
        switch (cc) {
                case (0x0):
                        if (c->flags.z == 0) {
                                call_n16(c, mmap, n16);
                        } else {
                                c->cycles += 3;
                        }
                        break;
                case (0x1):
                        if (c->flags.z == 1) {
                                call_n16(c, mmap, n16);
                        } else {
                                c->cycles += 3;
                        }
                        break;
                case (0x2):
                        if (c->flags.c == 0) {
                                call_n16(c, mmap, n16);
                        } else {
                                c->cycles += 3;
                        }

                        break;
                case (0x3):
                        if (c->flags.c == 1) {
                                call_n16(c, mmap, n16);
                        } else {
                                c->cycles += 3;
                        }
                        break;
                default:
                        break;
        }
}

void ccf(Cpu* c) {
        switch (c->flags.c) {
                case 0x0:
                        c->flags.c = 1;
                        break;
                case 0x1:
                        c->flags.c = 0;
                        break;
                default:
                        break;
        }
}

void ret(Cpu* c, MemMap* mmap) {
        uint8_t low_bytes = mem_read(c->registers.SP, mmap);
        ++c->registers.SP;

        uint8_t high_bytes = mem_read(c->registers.SP, mmap);
        ++c->registers.SP;

        uint16_t pc_result = (high_bytes << 8) | low_bytes;
        c->registers.PC = pc_result;

        c->cycles += 4;
}
