#include <stdio.h>
#include <stdlib.h>

#define DEBUG 1

#include "cpu.h"
#include "mem.h"
#include "regs.h"

int main()
{
    cpu_t* cpu = malloc(sizeof(cpu_t));
    mem_t* mem = malloc(sizeof(mem_t));
    reg_t* reg = malloc(sizeof(reg_t));

    if (cpu == NULL || mem == NULL || reg == NULL) {
        fprintf(stderr, "[ERROR:%s:%d] Could not allocate enough space for CPU + RAM + REGISTERS.", __FILE__, __LINE__);
    }

    init_reg(reg);
    init_cpu(cpu, reg, mem);
}
