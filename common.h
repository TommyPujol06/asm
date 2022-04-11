#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define GET_BIT(n, b) (n & (1 << b))
#define SET_BIT(n, b) n | (1 << b)
#define CLR_BIT(n, b) (n & ~(1 << b))

#endif
