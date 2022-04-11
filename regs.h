#ifndef __REGS_H__
#define __REGS_H__

#include "common.h"

typedef struct {
    uint8_t a;
    uint8_t f; // Not available to the programmer.
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t h;
    uint8_t l;
    uint8_t sp;
    uint8_t pc;
} reg_t;

typedef enum {
    S = 7, // Sign Flag
    Z = 6, // Zero Flag
    A = 4, // Auxiliary Carry Flag
    P = 2, // Parity Flag
    C = 0, // Carry Flag
} flag_t;

// 16bit registers by pairing:
// af,bc,de,hl
uint16_t get_reg_af(reg_t* reg);
uint16_t get_reg_bc(reg_t* reg);
uint16_t get_reg_de(reg_t* reg);
uint16_t get_reg_hl(reg_t* reg);

void set_reg_af(reg_t* reg, uint16_t val);
void set_reg_bc(reg_t* reg, uint16_t val);
void set_reg_de(reg_t* reg, uint16_t val);
void set_reg_hl(reg_t* reg, uint16_t val);

bool get_reg_flag(reg_t* reg, flag_t flag);
void set_reg_flag(reg_t* reg, flag_t flag, bool val);

#endif
