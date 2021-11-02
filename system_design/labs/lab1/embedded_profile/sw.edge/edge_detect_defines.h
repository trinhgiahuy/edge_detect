#ifndef __EDGE_DEFS_INCLUDED__
#define __EDGE_DEFS_INCLUDED__

#define KERNEL {1, 0, -1};

#ifdef SMALL
#define IMAGE_WIDTH     12
#define IMAGE_HEIGHT     8
#else
#define IMAGE_WIDTH   1296
#define IMAGE_HEIGHT   864
#endif

#define IMAGE_SIZE   (IMAGE_HEIGHT * IMAGE_WIDTH)

#define MEMORY_IMAGE_FILE "image.mem"
#define SHARED_MEMORY_BASE ((unsigned char *)(0x70000000UL))

#define BUS_WIDTH  128
#define BIT_WIDTH    9

#ifndef HOST
#include "console.h"
#define printf console_out
#endif

void run_sw(unsigned char *data_in, float *data_out_sw);
void run_hw(unsigned char *data_in, float *data_out_hw);

#endif
