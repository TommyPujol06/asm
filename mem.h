#ifndef __MEM_H__
#define __MEM_H__

#include "common.h"

#define MEM_SIZE 65536 // 64K of memory

typedef struct {
    uint8_t data[MEM_SIZE];
} mem_t;

uint8_t get_mem(mem_t* mem, uint16_t addr);
uint8_t get_mem_word(mem_t* mem, uint16_t addr);

void set_mem(mem_t* mem, uint16_t addr, uint8_t val);
void set_mem_word(mem_t* mem, uint16_t addr, uint8_t val);
#endif
