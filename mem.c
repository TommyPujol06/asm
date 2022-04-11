#include "mem.h"

uint8_t get_mem(mem_t* mem, uint16_t addr)
{
    if (mem == NULL) {
        return 0;
    }

    return mem->data[addr];
}

uint8_t get_mem_word(mem_t* mem, uint16_t addr)
{
    if (mem == NULL) {
        return 0;
    }

    return get_mem(mem, addr) | (get_mem(mem, addr + 1) << 8);
}

void set_mem(mem_t* mem, uint16_t addr, uint8_t val)
{
    if (mem == NULL) {
        return;
    }

    mem->data[addr] = val;
}

void set_mem_word(mem_t* mem, uint16_t addr, uint8_t val)
{
    if (mem == NULL) {
        return;
    }

    set_mem(mem, addr, (val & 0xFF));
    set_mem(mem, addr + 1, (val >> 8));
}
