#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define GET_BIT(n, b) (n & (1 << b))
#define SET_BIT(n, b) n | (1 << b)
#define CLR_BIT(n, b) (n & ~(1 << b))

static inline bool parity(uint8_t val)
{
    uint8_t count = 0;
    for (int i = 0; i < 8; i++) {
        count += ((val >> i) & 1);
    }

    return (count & 1) == 0;
}

#define ZSP_UPDATE(cpu, val, res)                              \
    {                                                          \
        set_reg_flag(cpu->reg, S, GET_BIT(res, 7));            \
        set_reg_flag(cpu->reg, Z, res == 0x00);                \
        set_reg_flag(cpu->reg, A, (val & 0x0f) + 0x01 > 0x0f); \
        set_reg_flag(cpu->reg, P, parity(res));                \
    }

#endif
