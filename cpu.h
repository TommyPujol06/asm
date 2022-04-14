#ifndef __CPU_H__
#define __CPU_H__

#include "common.h"

#include "mem.h"
#include "regs.h"

static const uint32_t CLOCK_FREQUENCY = 2000000;
static const uint32_t TICK_TIME = 16;
static const uint32_t TICK_CYCLES = (uint32_t)(TICK_TIME / (double)(1000 / (double)CLOCK_FREQUENCY));

// clang-format off
static const uint8_t OPCODES_CYCLES[256] = {
	//  0  1   2   3   4   5   6   7   8  9   A   B   C   D   E  F
    4, 10, 7,  5,  5,  5,  7,  4,  4, 10, 7,  5,  5,  5,  7, 4,  // 0
    4, 10, 7,  5,  5,  5,  7,  4,  4, 10, 7,  5,  5,  5,  7, 4,  // 1
    4, 10, 16, 5,  5,  5,  7,  4,  4, 10, 16, 5,  5,  5,  7, 4,  // 2
    4, 10, 13, 5,  10, 10, 10, 4,  4, 10, 13, 5,  5,  5,  7, 4,  // 3
    5, 5,  5,  5,  5,  5,  7,  5,  5, 5,  5,  5,  5,  5,  7, 5,  // 4
    5, 5,  5,  5,  5,  5,  7,  5,  5, 5,  5,  5,  5,  5,  7, 5,  // 5
    5, 5,  5,  5,  5,  5,  7,  5,  5, 5,  5,  5,  5,  5,  7, 5,  // 6
    7, 7,  7,  7,  7,  7,  7,  7,  5, 5,  5,  5,  5,  5,  7, 5,  // 7
    4, 4,  4,  4,  4,  4,  7,  4,  4, 4,  4,  4,  4,  4,  7, 4,  // 8
    4, 4,  4,  4,  4,  4,  7,  4,  4, 4,  4,  4,  4,  4,  7, 4,  // 9
    4, 4,  4,  4,  4,  4,  7,  4,  4, 4,  4,  4,  4,  4,  7, 4,  // A
    4, 4,  4,  4,  4,  4,  7,  4,  4, 4,  4,  4,  4,  4,  7, 4,  // B
    5, 10, 10, 10, 11, 11, 7,  11, 5, 10, 10, 10, 11, 17, 7, 11, // C
    5, 10, 10, 10, 11, 11, 7,  11, 5, 10, 10, 10, 11, 17, 7, 11, // D
    5, 10, 10, 18, 11, 11, 7,  11, 5, 5,  10, 4,  11, 17, 7, 11, // E
    5, 10, 10, 4,  11, 11, 7,  11, 5, 5,  10, 4,  11, 17, 7, 11  // F
};
// clang-format on

typedef struct {
    reg_t* reg;
    mem_t* mem;
    bool halted;
    bool interrupt;
    uint32_t tick_cycles;
} cpu_t;

void init_cpu(cpu_t* cpu, reg_t* reg, mem_t* mem);

uint32_t exec(cpu_t* cpu);
uint32_t step(cpu_t* cpu);
void handle_interrupt(cpu_t* cpu, uint16_t addr);

uint8_t imm_ds(cpu_t* cpu);
uint16_t imm_dw(cpu_t* cpu);

uint8_t get_m(cpu_t* cpu);
void set_m(cpu_t* cpu, uint8_t val);

void stack_add(cpu_t* cpu, uint16_t val);
uint16_t stack_pop(cpu_t* cpu);

uint8_t alu_inr(cpu_t* cpu, uint8_t val);
uint8_t alu_dcr(cpu_t* cpu, uint8_t val);

void alu_daa(cpu_t* cpu);
void alu_add(cpu_t* cpu, uint8_t val);
void alu_adc(cpu_t* cpu, uint8_t val);
void alu_sub(cpu_t* cpu, uint8_t val);
void alu_sbb(cpu_t* cpu, uint8_t val);
void alu_ana(cpu_t* cpu, uint8_t val);
void alu_xra(cpu_t* cpu, uint8_t val);
void alu_ora(cpu_t* cpu, uint8_t val);
void alu_cmp(cpu_t* cpu, uint8_t val);

void alu_rlc(cpu_t* cpu);
void alu_rrc(cpu_t* cpu);
void alu_ral(cpu_t* cpu);
void alu_rar(cpu_t* cpu);
void alu_dad(cpu_t* cpu, uint16_t val);

#endif
