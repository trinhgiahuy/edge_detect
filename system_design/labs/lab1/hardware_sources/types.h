
#ifndef __TYPES_H_INCLUDED__
#define __TYPES_H_INCLUDED__

#define MEMORY_WIDTH 64
#define MEMORY_BITS (22)
#define MEM_SIZE (1 << MEMORY_BITS)

typedef ac_int<MEMORY_BITS, false> mem_addr_type; 
typedef ac_int<8, false> burst_type;
typedef ac_int<MEMORY_WIDTH, false> dtype;

#endif
