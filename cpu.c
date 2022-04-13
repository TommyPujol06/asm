#include "cpu.h"

void init_cpu(cpu_t* cpu, reg_t* reg, mem_t* mem)
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

uint32_t exec(cpu_t* cpu)
{
    if (cpu == NULL) {
        return 0;
    }

    uint8_t opcode = imm_ds(cpu);
    switch (opcode) {
    case 0x08:
    case 0x10:
    case 0x18:
    case 0x20:
    case 0x28:
    case 0x30:
    case 0x38:
        opcode = 0x00;
        break;

    case 0xcb:
        opcode = 0xc3;
        break;

    case 0xd9:
        opcode = 0xc9;
        break;

    case 0xdd:
    case 0xed:
    case 0xfd:
        opcode = 0xcd;
        break;

    default:
        break;
    }

#ifdef DEBUG
#include <stdio.h>

    fprintf(stderr, "%04X PC=%04X SP=%04X A=%02X F=%02X B=%02X C=%02X D=%02X E=%02X H=%02X L=%02X\n",
        opcode,
        cpu->reg->pc - 1,
        cpu->reg->sp,
        cpu->reg->a,
        cpu->reg->f,
        cpu->reg->b,
        cpu->reg->c,
        cpu->reg->d,
        cpu->reg->e,
        cpu->reg->h,
        cpu->reg->l);
#endif

    uint32_t cycle = 0;
    switch (opcode) {
    // Carry bit instructions
    case 0x3f:
        set_reg_flag(cpu->reg, C, !get_reg_flag(cpu->reg, C));
        break;
    case 0x37:
        set_reg_flag(cpu->reg, C, true);
        break;

    // INR
    case 0x04:
        cpu->reg->b = alu_inr(cpu, cpu->reg->b);
        break;
    case 0x0c:
        cpu->reg->c = alu_inr(cpu, cpu->reg->c);
        break;
    case 0x14:
        cpu->reg->d = alu_inr(cpu, cpu->reg->d);
        break;
    case 0x1c:
        cpu->reg->e = alu_inr(cpu, cpu->reg->e);
        break;
    case 0x24:
        cpu->reg->h = alu_inr(cpu, cpu->reg->h);
        break;
    case 0x2c:
        cpu->reg->l = alu_inr(cpu, cpu->reg->l);
        break;

    case 0x34: {
        uint8_t m = get_m(cpu);
        uint8_t r = alu_inr(cpu, m);
        set_m(cpu, r);
        break;
    }
    case 0x3c:
        cpu->reg->a = alu_inr(cpu, cpu->reg->a);
        break;

    // DCR
    case 0x05:
        cpu->reg->b = alu_dcr(cpu, cpu->reg->b);
        break;
    case 0x0d:
        cpu->reg->c = alu_dcr(cpu, cpu->reg->c);
        break;
    case 0x15:
        cpu->reg->d = alu_dcr(cpu, cpu->reg->d);
        break;
    case 0x1d:
        cpu->reg->e = alu_dcr(cpu, cpu->reg->e);
        break;
    case 0x25:
        cpu->reg->h = alu_dcr(cpu, cpu->reg->h);
        break;
    case 0x2d:
        cpu->reg->l = alu_dcr(cpu, cpu->reg->l);
        break;
    case 0x35: {
        uint8_t m = get_m(cpu);
        uint8_t r = alu_dcr(cpu, m);
        set_m(cpu, r);
        break;
    }

    case 0x3d:
        cpu->reg->a = alu_dcr(cpu, cpu->reg->a);
        break;

    // CMA
    case 0x2f:
        cpu->reg->a = !cpu->reg->a;
        break;

    // DAA
    case 0x27:
        alu_daa(cpu);
        break;

    // NOP: undocumented NOPs from 0x08 to 0x38
    case 0x08:
    case 0x10:
    case 0x18:
    case 0x20:
    case 0x28:
    case 0x30:
    case 0x38:
    case 0x00:
        break;

    // MOV
    case 0x40:
        break;
    case 0x41:
        cpu->reg->b = cpu->reg->c;
        break;
    case 0x42:
        cpu->reg->b = cpu->reg->d;
        break;
    case 0x43:
        cpu->reg->b = cpu->reg->e;
        break;
    case 0x44:
        cpu->reg->b = cpu->reg->h;
        break;
    case 0x45:
        cpu->reg->b = cpu->reg->l;
        break;
    case 0x46:
        cpu->reg->b = get_m(cpu);
        break;
    case 0x47:
        cpu->reg->b = cpu->reg->a;
        break;
    case 0x48:
        cpu->reg->c = cpu->reg->b;
        break;
    case 0x49:
        break;
    case 0x4a:
        cpu->reg->c = cpu->reg->d;
        break;
    case 0x4b:
        cpu->reg->c = cpu->reg->e;
        break;
    case 0x4c:
        cpu->reg->c = cpu->reg->h;
        break;
    case 0x4d:
        cpu->reg->c = cpu->reg->l;
        break;
    case 0x4e:
        cpu->reg->c = get_m(cpu);
        break;
    case 0x4f:
        cpu->reg->c = cpu->reg->a;
        break;
    case 0x50:
        cpu->reg->d = cpu->reg->b;
        break;
    case 0x51:
        cpu->reg->d = cpu->reg->c;
        break;
    case 0x52:
        break;
    case 0x53:
        cpu->reg->d = cpu->reg->e;
        break;
    case 0x54:
        cpu->reg->d = cpu->reg->h;
        break;
    case 0x55:
        cpu->reg->d = cpu->reg->l;
        break;
    case 0x56:
        cpu->reg->d = get_m(cpu);
        break;
    case 0x57:
        cpu->reg->d = cpu->reg->a;
        break;
    case 0x58:
        cpu->reg->e = cpu->reg->b;
        break;
    case 0x59:
        cpu->reg->e = cpu->reg->c;
        break;
    case 0x5a:
        cpu->reg->e = cpu->reg->d;
        break;
    case 0x5b:
        break;
    case 0x5c:
        cpu->reg->e = cpu->reg->h;
        break;
    case 0x5d:
        cpu->reg->e = cpu->reg->l;
        break;
    case 0x5e:
        cpu->reg->e = get_m(cpu);
        break;
    case 0x5f:
        cpu->reg->e = cpu->reg->a;
        break;
    case 0x60:
        cpu->reg->h = cpu->reg->b;
        break;
    case 0x61:
        cpu->reg->h = cpu->reg->c;
        break;
    case 0x62:
        cpu->reg->h = cpu->reg->d;
        break;
    case 0x63:
        cpu->reg->h = cpu->reg->e;
        break;
    case 0x64:
        break;
    case 0x65:
        cpu->reg->h = cpu->reg->l;
        break;
    case 0x66:
        cpu->reg->h = get_m(cpu);
        break;
    case 0x67:
        cpu->reg->h = cpu->reg->a;
        break;
    case 0x68:
        cpu->reg->l = cpu->reg->b;
        break;
    case 0x69:
        cpu->reg->l = cpu->reg->c;
        break;
    case 0x6a:
        cpu->reg->l = cpu->reg->d;
        break;
    case 0x6b:
        cpu->reg->l = cpu->reg->e;
        break;
    case 0x6c:
        cpu->reg->l = cpu->reg->h;
        break;
    case 0x6d:
        break;
    case 0x6e:
        cpu->reg->l = get_m(cpu);
        break;
    case 0x6f:
        cpu->reg->l = cpu->reg->a;
        break;
    case 0x70:
        set_m(cpu, cpu->reg->b);
        break;
    case 0x71:
        set_m(cpu, cpu->reg->c);
        break;
    case 0x72:
        set_m(cpu, cpu->reg->d);
        break;
    case 0x73:
        set_m(cpu, cpu->reg->e);
        break;
    case 0x74:
        set_m(cpu, cpu->reg->h);
        break;
    case 0x75:
        set_m(cpu, cpu->reg->l);
        break;
    case 0x77:
        set_m(cpu, cpu->reg->a);
        break;
    case 0x78:
        cpu->reg->a = cpu->reg->b;
        break;
    case 0x79:
        cpu->reg->a = cpu->reg->c;
        break;
    case 0x7a:
        cpu->reg->a = cpu->reg->d;
        break;
    case 0x7b:
        cpu->reg->a = cpu->reg->e;
        break;
    case 0x7c:
        cpu->reg->a = cpu->reg->h;
        break;
    case 0x7d:
        cpu->reg->a = cpu->reg->l;
        break;
    case 0x7e:
        cpu->reg->a = get_m(cpu);
        break;
    case 0x7f:
        break;

    // STAX
    case 0x02:
        set_mem(cpu->mem, get_reg_bc(cpu->reg), cpu->reg->a);
        break;
    case 0x12:
        set_mem(cpu->mem, get_reg_de(cpu->reg), cpu->reg->a);
        break;

    // LDAX
    case 0x0a:
        cpu->reg->a = get_mem(cpu->mem, get_reg_bc(cpu->reg));
        break;
    case 0x1a:
        cpu->reg->a = get_mem(cpu->mem, get_reg_de(cpu->reg));
        break;

    // ADD
    case 0x80:
        alu_add(cpu, cpu->reg->b);
        break;
    case 0x81:
        alu_add(cpu, cpu->reg->c);
        break;
    case 0x82:
        alu_add(cpu, cpu->reg->d);
        break;
    case 0x83:
        alu_add(cpu, cpu->reg->e);
        break;
    case 0x84:
        alu_add(cpu, cpu->reg->h);
        break;
    case 0x85:
        alu_add(cpu, cpu->reg->l);
        break;
    case 0x86:
        alu_add(cpu, get_m(cpu));
        break;
    case 0x87:
        alu_add(cpu, cpu->reg->a);
        break;

    // ADC
    case 0x88:
        alu_adc(cpu, cpu->reg->b);
        break;
    case 0x89:
        alu_adc(cpu, cpu->reg->c);
        break;
    case 0x8a:
        alu_adc(cpu, cpu->reg->d);
        break;
    case 0x8b:
        alu_adc(cpu, cpu->reg->e);
        break;
    case 0x8c:
        alu_adc(cpu, cpu->reg->h);
        break;
    case 0x8d:
        alu_adc(cpu, cpu->reg->l);
        break;
    case 0x8e:
        alu_adc(cpu, get_m(cpu));
        break;
    case 0x8f:
        alu_adc(cpu, cpu->reg->a);
        break;

    // SUB
    case 0x90:
        alu_sub(cpu, cpu->reg->b);
        break;
    case 0x91:
        alu_sub(cpu, cpu->reg->c);
        break;
    case 0x92:
        alu_sub(cpu, cpu->reg->d);
        break;
    case 0x93:
        alu_sub(cpu, cpu->reg->e);
        break;
    case 0x94:
        alu_sub(cpu, cpu->reg->h);
        break;
    case 0x95:
        alu_sub(cpu, cpu->reg->l);
        break;
    case 0x96:
        alu_sub(cpu, get_m(cpu));
        break;
    case 0x97:
        alu_sub(cpu, cpu->reg->a);
        break;

    // SBB
    case 0x98:
        alu_sbb(cpu, cpu->reg->b);
        break;
    case 0x99:
        alu_sbb(cpu, cpu->reg->c);
        break;
    case 0x9a:
        alu_sbb(cpu, cpu->reg->d);
        break;
    case 0x9b:
        alu_sbb(cpu, cpu->reg->e);
        break;
    case 0x9c:
        alu_sbb(cpu, cpu->reg->h);
        break;
    case 0x9d:
        alu_sbb(cpu, cpu->reg->l);
        break;
    case 0x9e:
        alu_sbb(cpu, get_m(cpu));
        break;
    case 0x9f:
        alu_sbb(cpu, cpu->reg->a);
        break;

    // ANA
    case 0xa0:
        alu_ana(cpu, cpu->reg->b);
        break;
    case 0xa1:
        alu_ana(cpu, cpu->reg->c);
        break;
    case 0xa2:
        alu_ana(cpu, cpu->reg->d);
        break;
    case 0xa3:
        alu_ana(cpu, cpu->reg->e);
        break;
    case 0xa4:
        alu_ana(cpu, cpu->reg->h);
        break;
    case 0xa5:
        alu_ana(cpu, cpu->reg->l);
        break;
    case 0xa6:
        alu_ana(cpu, get_m(cpu));
        break;
    case 0xa7:
        alu_ana(cpu, cpu->reg->a);
        break;

    // XRA
    case 0xa8:
        alu_xra(cpu, cpu->reg->b);
        break;
    case 0xa9:
        alu_xra(cpu, cpu->reg->c);
        break;
    case 0xaa:
        alu_xra(cpu, cpu->reg->d);
        break;
    case 0xab:
        alu_xra(cpu, cpu->reg->e);
        break;
    case 0xac:
        alu_xra(cpu, cpu->reg->h);
        break;
    case 0xad:
        alu_xra(cpu, cpu->reg->l);
        break;
    case 0xae:
        alu_xra(cpu, get_m(cpu));
        break;
    case 0xaf:
        alu_xra(cpu, cpu->reg->a);
        break;

    // ORA
    case 0xb0:
        alu_ora(cpu, cpu->reg->b);
        break;
    case 0xb1:
        alu_ora(cpu, cpu->reg->c);
        break;
    case 0xb2:
        alu_ora(cpu, cpu->reg->d);
        break;
    case 0xb3:
        alu_ora(cpu, cpu->reg->e);
        break;
    case 0xb4:
        alu_ora(cpu, cpu->reg->h);
        break;
    case 0xb5:
        alu_ora(cpu, cpu->reg->l);
        break;
    case 0xb6:
        alu_ora(cpu, get_m(cpu));
        break;
    case 0xb7:
        alu_ora(cpu, cpu->reg->a);
        break;

    // CMP
    case 0xb8:
        alu_cmp(cpu, cpu->reg->b);
        break;
    case 0xb9:
        alu_cmp(cpu, cpu->reg->c);
        break;
    case 0xba:
        alu_cmp(cpu, cpu->reg->d);
        break;
    case 0xbb:
        alu_cmp(cpu, cpu->reg->e);
        break;
    case 0xbc:
        alu_cmp(cpu, cpu->reg->h);
        break;
    case 0xbd:
        alu_cmp(cpu, cpu->reg->l);
        break;
    case 0xbe:
        alu_cmp(cpu, get_m(cpu));
        break;
    case 0xbf:
        alu_cmp(cpu, cpu->reg->a);
        break;

    // RLC
    case 0x07:
        alu_rlc(cpu);
        break;

    // RRC
    case 0x0f:
        alu_rrc(cpu);
        break;

    // RAL
    case 0x17:
        alu_ral(cpu);
        break;

    // RAR
    case 0x1f:
        alu_rar(cpu);
        break;

    // PUSH
    case 0xc5:
        stack_add(cpu, get_reg_bc(cpu->reg));
        break;
    case 0xd5:
        stack_add(cpu, get_reg_de(cpu->reg));
        break;
    case 0xe5:
        stack_add(cpu, get_reg_hl(cpu->reg));
        break;
    case 0xf5:
        stack_add(cpu, get_reg_af(cpu->reg));
        break;

    // POP Pop Data Off Stack
    case 0xc1: {
        uint16_t val = stack_pop(cpu);
        set_reg_bc(cpu->reg, val);
        break;
    }
    case 0xd1: {
        uint16_t val = stack_pop(cpu);
        set_reg_de(cpu->reg, val);
        break;
    }
    case 0xe1: {
        uint16_t val = stack_pop(cpu);
        set_reg_hl(cpu->reg, val);
        break;
    }
    case 0xf1: {
        uint16_t val = stack_pop(cpu);
        set_reg_af(cpu->reg, val);
        break;
    }

    // DAD
    case 0x09:
        alu_dad(cpu, get_reg_bc(cpu->reg));
        break;
    case 0x19:
        alu_dad(cpu, get_reg_de(cpu->reg));
        break;
    case 0x29:
        alu_dad(cpu, get_reg_hl(cpu->reg));
        break;
    case 0x39:
        alu_dad(cpu, cpu->reg->sp);
        break;

    // INX
    case 0x03:
        set_reg_bc(cpu->reg, get_reg_bc(cpu->reg) + 1);
        break;
    case 0x13:
        set_reg_de(cpu->reg, get_reg_de(cpu->reg) + 1);
        break;
    case 0x23:
        set_reg_hl(cpu->reg, get_reg_hl(cpu->reg) + 1);
        break;
    case 0x33:
        cpu->reg->sp = cpu->reg->sp + 1;
        break;

    // DCX
    case 0x0b:
        set_reg_bc(cpu->reg, get_reg_bc(cpu->reg) - 1);
        break;
    case 0x1b:
        set_reg_de(cpu->reg, get_reg_de(cpu->reg) - 1);
        break;
    case 0x2b:
        set_reg_hl(cpu->reg, get_reg_hl(cpu->reg) - 1);
        break;
    case 0x3b:
        cpu->reg->sp = cpu->reg->sp - 1;
        break;

    // XCHG
    case 0xeb: {
        swap_mem(&cpu->reg->h, &cpu->reg->d);
        swap_mem(&cpu->reg->l, &cpu->reg->e);
        break;
    }

    // XTHL
    case 0xe3: {
        uint16_t val = get_mem_word(cpu->mem, cpu->reg->sp);
        uint16_t b = get_reg_hl(cpu->reg);
        set_reg_hl(cpu->reg, val);
        set_mem_word(cpu->mem, cpu->reg->sp, b);
        break;
    }

    // SPHL
    case 0xf9:
        cpu->reg->sp = get_reg_hl(cpu->reg);
        break;

    // LXI
    case 0x01: {
        uint16_t val = imm_dw(cpu);
        set_reg_bc(cpu->reg, val);
        break;
    }
    case 0x11: {
        uint16_t val = imm_dw(cpu);
        set_reg_de(cpu->reg, val);
        break;
    }
    case 0x21: {
        uint16_t val = imm_dw(cpu);
        set_reg_hl(cpu->reg, val);
        break;
    }
    case 0x31: {
        uint16_t val = imm_dw(cpu);
        cpu->reg->sp = val;
        break;
    }

    // MVI
    case 0x06:
        cpu->reg->b = imm_ds(cpu);
        break;
    case 0x0e:
        cpu->reg->c = imm_ds(cpu);
        break;
    case 0x16:
        cpu->reg->d = imm_ds(cpu);
        break;
    case 0x1e:
        cpu->reg->e = imm_ds(cpu);
        break;
    case 0x26:
        cpu->reg->h = imm_ds(cpu);
        break;
    case 0x2e:
        cpu->reg->l = imm_ds(cpu);
        break;
    case 0x36: {
        uint8_t val = imm_ds(cpu);
        set_m(cpu, val);
        break;
    }
    case 0x3e:
        cpu->reg->a = imm_ds(cpu);
        break;
    }

    return OPCODES_CYCLES[opcode] + cycle;
}
