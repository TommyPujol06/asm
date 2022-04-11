#include "cpu.h"

void init(cpu_t* cpu, reg_t* reg, mem_t* mem)
{
    if (cpu == NULL || reg == NULL || mem == NULL) {
        return;
    }

    cpu->reg = reg;
    cpu->mem = mem;
    cpu->halted = false;
    cpu->interrupt = false;
    cpu->tick_cycles = 0;
    cpu->start_tick = time(0);
}

uint8_t imm_ds(cpu_t* cpu)
{
    if (cpu == NULL) {
        return 0;
    }

    uint8_t val = get_mem(cpu->mem, cpu->reg->pc);

    cpu->reg->pc += 1;

    return val;
}

uint16_t imm_dw(cpu_t* cpu)
{
    if (cpu == NULL) {
        return 0;
    }

    uint8_t val = get_mem(cpu->mem, cpu->reg->pc++);

    cpu->reg->pc += 2;

    return val;
}

uint8_t get_m(cpu_t* cpu)
{
    if (cpu == NULL) {
        return 0;
    }

    return get_mem(cpu->mem, get_reg_hl(cpu->reg));
}

void set_m(cpu_t* cpu, uint8_t val)
{
    if (cpu == NULL) {
        return;
    }

    set_mem(cpu->mem, get_reg_hl(cpu->reg), val);
}

void stack_add(cpu_t* cpu, uint16_t val)
{
    if (cpu == NULL) {
        return;
    }

    cpu->reg->sp -= 2;
    set_mem_word(cpu->mem, cpu->reg->sp, val);
}

uint16_t stack_pop(cpu_t* cpu)
{
    if (cpu == NULL) {
        return 0;
    }

    uint16_t val = get_mem_word(cpu->mem, cpu->reg->sp);

    cpu->reg->sp += 2;

    return val;
}

uint8_t alu_inr(cpu_t* cpu, uint8_t val)
{
    if (cpu == NULL) {
        return 0;
    }

    uint8_t res = val + 1;

    ZSP_UPDATE(cpu, val, res);

    return res;
}

uint8_t alu_dcr(cpu_t* cpu, uint8_t val)
{
    if (cpu == NULL) {
        return 0;
    }

    uint8_t res = val - 1;

    ZSP_UPDATE(cpu, val, res);

    return res;
}

void alu_daa(cpu_t* cpu)
{
    if (cpu == NULL) {
        return;
    }

    uint8_t correction = 0;
    bool c = get_reg_flag(cpu->reg, C);

    uint8_t lsb = cpu->reg->a & 0x0f;
    uint8_t msb = cpu->reg->a >> 4;

    if (lsb > 9 || get_reg_flag(cpu->reg, A)) {
        correction += 0x06;
    }

    if (msb > 9 || get_reg_flag(cpu->reg, C) || (msb >= 9 && lsb > 9)) {
        correction += 0x06;
        c = true;
    }

    alu_add(cpu, correction);

    set_reg_flag(cpu->reg, C, c);
}

void alu_add(cpu_t* cpu, uint8_t val)
{
    if (cpu == NULL) {
        return;
    }

    uint8_t a = cpu->reg->a;
    uint8_t res = a + val;

    ZSP_UPDATE(cpu, val, res);

    set_reg_flag(cpu->reg, C, a + res > 0xff);

    cpu->reg->a = res;
}

void alu_adc(cpu_t* cpu, uint8_t val)
{
    if (cpu == NULL) {
        return;
    }

    uint8_t a = cpu->reg->a;
    uint8_t c = get_reg_flag(cpu->reg, C);

    uint8_t res = a + val + c;

    ZSP_UPDATE(cpu, val, res);

    set_reg_flag(cpu->reg, C, a + val + res > 0xff);

    cpu->reg->a = res;
}

void alu_sub(cpu_t* cpu, uint8_t val)
{
    if (cpu == NULL) {
        return;
    }

    uint8_t a = cpu->reg->a;
    uint8_t res = a - val;

    ZSP_UPDATE(cpu, val, res);

    set_reg_flag(cpu->reg, A, (a & 0x0f) - (val & 0x0f) >= 0x00);
    set_reg_flag(cpu->reg, C, a < val);

    cpu->reg->a = res;
}

void alu_sbb(cpu_t* cpu, uint8_t val)
{
    if (cpu == NULL) {
        return;
    }

    uint8_t a = cpu->reg->a;
    uint8_t c = get_reg_flag(cpu->reg, C);

    uint8_t res = a - val - c;

    ZSP_UPDATE(cpu, val, res);

    set_reg_flag(cpu->reg, A, (a & 0x0f) - (val & 0x0f) - (c & 0x0f) >= 0x00);
    set_reg_flag(cpu->reg, C, a + val + res > 0xff);

    cpu->reg->a = res;
}

void alu_ana(cpu_t* cpu, uint8_t val)
{
    if (cpu == NULL) {
        return;
    }

    uint8_t a = cpu->reg->a;
    uint8_t res = a & val;

    ZSP_UPDATE(cpu, val, res);

    set_reg_flag(cpu->reg, C, false);

    cpu->reg->a = res;
}

void alu_xra(cpu_t* cpu, uint8_t val)
{
    if (cpu == NULL) {
        return;
    }

    uint8_t a = cpu->reg->a;
    uint8_t res = a ^ val;

    ZSP_UPDATE(cpu, val, res);

    set_reg_flag(cpu->reg, C, false);

    cpu->reg->a = res;
}

void alu_ora(cpu_t* cpu, uint8_t val)
{
    if (cpu == NULL) {
        return;
    }

    uint8_t a = cpu->reg->a;
    uint8_t res = a | val;

    ZSP_UPDATE(cpu, val, res);

    set_reg_flag(cpu->reg, C, false);

    cpu->reg->a = res;
}

void alu_cmp(cpu_t* cpu, uint8_t val)
{
    if (cpu == NULL) {
        return;
    }

    uint8_t res = cpu->reg->a;

    alu_sub(cpu, val);

    cpu->reg->a = res;
}

void alu_rlc(cpu_t* cpu)
{
    if (cpu == NULL) {
        return;
    }

    uint8_t c = GET_BIT(cpu->reg->a, 7);
    uint8_t res = (cpu->reg->a << 1) | c;

    set_reg_flag(cpu->reg, C, c);

    cpu->reg->a = res;
}

void alu_rrc(cpu_t* cpu)
{
    if (cpu == NULL) {
        return;
    }

    uint8_t c = GET_BIT(cpu->reg->a, 0);
    uint8_t res = 0;

    if (c) {
        res = 0x80 | (cpu->reg->a << 1);
    } else {
        res = cpu->reg->a >> 1;
    }

    set_reg_flag(cpu->reg, C, c);

    cpu->reg->a = res;
}

void alu_ral(cpu_t* cpu)
{
    if (cpu == NULL) {
        return;
    }

    uint8_t c = GET_BIT(cpu->reg->a, 7);
    uint8_t res = (cpu->reg->a << 1) | get_reg_flag(cpu->reg, C);

    set_reg_flag(cpu->reg, C, c);

    cpu->reg->a = res;
}

void alu_rar(cpu_t* cpu)
{
    if (cpu == NULL) {
        return;
    }

    uint8_t c = GET_BIT(cpu->reg->a, 0);
    uint8_t res = 0;

    if (get_reg_flag(cpu->reg, C)) {
        res = 0x80 | (cpu->reg->a >> 1);
    } else {
        res = cpu->reg->a >> 1;
    }

    set_reg_flag(cpu->reg, C, c);

    cpu->reg->a = res;
}

void alu_dad(cpu_t* cpu, uint16_t val)
{
    if (cpu == NULL) {
        return;
    }

    uint16_t a = get_reg_hl(cpu->reg);
    uint16_t res = a + val;

    set_reg_flag(cpu->reg, C, a > 0xffff - val);
    set_reg_hl(cpu->reg, res);
}
