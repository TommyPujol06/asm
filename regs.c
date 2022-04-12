#include "regs.h"

void init_reg(reg_t* reg)
{
    if (reg == NULL) {
        return;
    }

    reg->a = 0;
    reg->f = 0x10;
    reg->b = 0;
    reg->c = 0;
    reg->d = 0;
    reg->e = 0;
    reg->h = 0;
    reg->l = 0;
    reg->sp = 0;
    reg->pc = 0;
}

uint16_t get_reg_af(reg_t* reg)
{
    if (reg == NULL) {
        return 0;
    }

    return (reg->a << 8) | reg->f;
}

uint16_t get_reg_bc(reg_t* reg)
{
    if (reg == NULL) {
        return 0;
    }

    return (reg->b << 8) | reg->c;
}

uint16_t get_reg_de(reg_t* reg)
{
    if (reg == NULL) {
        return 0;
    }

    return (reg->d << 8) | reg->e;
}

uint16_t get_reg_hl(reg_t* reg)
{
    if (reg == NULL) {
        return 0;
    }

    return (reg->h << 8) | reg->l;
}

void set_reg_af(reg_t* reg, uint16_t val)
{
    if (reg == NULL) {
        return;
    }

    reg->a = val >> 8;
    reg->f = ((val & 0x00d5) | 0x0002);
}

void set_reg_bc(reg_t* reg, uint16_t val)
{
    if (reg == NULL) {
        return;
    }

    reg->b = val >> 8;
    reg->c = ((val & 0x00d5) | 0x0002);
}

void set_reg_de(reg_t* reg, uint16_t val)
{
    if (reg == NULL) {
        return;
    }

    reg->d = val >> 8;
    reg->e = ((val & 0x00d5) | 0x0002);
}

void set_reg_hl(reg_t* reg, uint16_t val)
{
    if (reg == NULL) {
        return;
    }

    reg->h = val >> 8;
    reg->l = ((val & 0x00d5) | 0x0002);
}

bool get_reg_flag(reg_t* reg, flag_t flag)
{
    if (reg == NULL) {
        return NULL;
    }

    return GET_BIT(reg->f, flag) != 0;
}

void set_reg_flag(reg_t* reg, flag_t flag, bool val)
{
    if (reg == NULL) {
        return;
    }

    if (val == true) {
        reg->f = SET_BIT(reg->f, flag);
    } else {
        reg->f = CLR_BIT(reg->f, flag);
    }
}
